#include "CameraHandle.h"

#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

int CameraHandle::initialize() {
    // qcarcam init
    qcarcam_init_t qcarcam_init = {0};
    qcarcam_init.version = QCARCAM_VERSION;
    qcarcam_init.debug_tag = ( char* )"qcarcam_test";
    auto ret_init = qcarcam_initialize(&qcarcam_init);
    if (ret_init) {
        std::cout << "qcar cam init failed, errorcode: " << ret_init << std::endl;
        return -1;
    }

    /*query qcarcam*/
    qcarcam_input_t* pInputs;
    unsigned int queryNumInputs = 0, queryFilled = 0;
    auto query_ret1 = qcarcam_query_inputs(NULL, 0, &queryNumInputs);
    if (QCARCAM_RET_OK != query_ret1 || queryNumInputs == 0) {
        std::cout << "qcarcam_query_inputs failed " << std::endl;
    }
    pInputs = ( qcarcam_input_t* )calloc(queryNumInputs, sizeof(*pInputs));
    if (!pInputs) {
        printf("Failed to allocate pInputs");
        exit(-1);
    }
    auto query_ret2 = qcarcam_query_inputs(pInputs, queryNumInputs, &queryFilled);
    if (QCARCAM_RET_OK != query_ret2 || queryFilled != queryNumInputs) {
        printf("Failed qcarcam_query_inputs with ret %d %d %d",
               query_ret2,
               queryFilled,
               queryNumInputs);
        exit(-1);
    }
    printf("--- QCarCam Queried Inputs ----");
    for (int i = 0; i < queryFilled; i++) {
        printf("%d: input_id=%d, res=%dx%d fmt=0x%08x fps=%.2f flags=0x%x",
               i,
               pInputs[i].desc,
               pInputs[i].res[0].width,
               pInputs[i].res[0].height,
               pInputs[i].color_fmt[0],
               pInputs[i].res[0].fps,
               pInputs[i].flags);
    }

    // qcarcam_open
    const int try_count = 10;
    for (int i = 0; i < try_count; i++) {
        cam_handle_ = qcarcam_open(QCARCAM_INPUT_TYPE_EXT_REAR);
        if (cam_handle_ != nullptr) {
            std::cout << "open camera success!" << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "qcarcam_open() failed, try " << i + 1 << " again..." << std::endl;
    }

    if (cam_handle_ == nullptr) {
        std::cout << "open camera failed!" << std::endl;
        return -1;
    }

    // qcarcam_s_buffers
    // color_fmt
    // cam_buffers_.color_fmt = QCARCAM_FMT_UYVY_8;
    cam_buffers_.color_fmt = QCARCAM_FMT_MIPIRAW_8;

    // n_buffers
    cam_buffers_.n_buffers = 6;
    // buffers
    cam_buffers_.buffers =
        ( qcarcam_buffer_t* )calloc(cam_buffers_.n_buffers, sizeof(*cam_buffers_.buffers));
    if (!cam_buffers_.buffers) {
        std::cout << "calloc for cam_buffers_ failed, buf num: " << cam_buffers_.n_buffers
                  << std::endl;
        return -1;
    }
    // flag
    cam_buffers_.flags = QCARCAM_BUFFER_FLAG_SECURE;
    cam_buffers_.flags |= QCARCAM_BUFFER_FLAG_OS_HNDL;
    // planes
    virtual_address_ = new void*[cam_buffers_.n_buffers];
    if (!virtual_address_) {
        printf("camera_id:{}, new for virtual_address_ failed, buf num: {}",
               0,
               cam_buffers_.n_buffers);
        return -1;
    }
    for (int i = 0; i < cam_buffers_.n_buffers; ++i) {
        cam_buffers_.buffers[i].n_planes = 1;
        cam_buffers_.buffers[i].planes[0].width = IMAGE_WIDTH;
        cam_buffers_.buffers[i].planes[0].height = IMAGE_HEIGHT;
        cam_buffers_.buffers[i].planes[0].stride = IMAGE_STRIDE;
        cam_buffers_.buffers[i].planes[0].size =
            cam_buffers_.buffers[i].planes[0].stride * cam_buffers_.buffers[i].planes[0].height;
        pmem_handle_t mem_handle;
        virtual_address_[i] = pmem_malloc_ext_v2(cam_buffers_.buffers[i].planes[0].size,
                                                 PMEM_CAMERA_ID,
                                                 PMEM_FLAGS_SHMEM | PMEM_FLAGS_PHYS_NON_CONTIG |
                                                     PMEM_FLAGS_CACHE_NONE,
                                                 PMEM_ALIGNMENT_4K,
                                                 0,
                                                 &mem_handle,
                                                 NULL);
        cam_buffers_.buffers[i].planes[0].p_buf = mem_handle;
        physical_address_ = ( long long )pmem_get_phys_addr(virtual_address_[i]);
    }

    qcarcam_ret_t ret_s_buffers = qcarcam_s_buffers(cam_handle_, &cam_buffers_);
    if (ret_s_buffers != QCARCAM_RET_OK) {
        std::cout << "qcarcam_s_buffers() failed, errorcode: " << ret_s_buffers << std::endl;
        return -1;
    }

    // qcarcam_s_param
    qcarcam_param_value_t param;
    param.ptr_value = ( void* )qcarcam_event_cb;
    qcarcam_ret_t ret = qcarcam_s_param(cam_handle_, QCARCAM_PARAM_EVENT_CB, &param);
    if (ret != QCARCAM_RET_OK) {
        std::cout << "qcarcam_s_param for qcarcam_event_cb failed!" << std::endl;
        return -1;
    }

    param.uint_value = QCARCAM_EVENT_FRAME_READY | QCARCAM_EVENT_INPUT_SIGNAL |
                       QCARCAM_EVENT_ERROR | QCARCAM_EVENT_VENDOR | QCARCAM_EVENT_FRAME_FREEZE;
    ret = qcarcam_s_param(cam_handle_, QCARCAM_PARAM_EVENT_MASK, &param);
    if (ret != QCARCAM_RET_OK) {
        std::cout << "qcarcam_s_param for uint_value failed!" << std::endl;
        return -1;
    }

    if (cam_handle_ == nullptr) {
        std::cout << "cam_handle_ == nullptr " << std::endl;
    }
}

// 启动摄像头并触发回调
void CameraHandle::start() {
    // qcarcam_start
    int ret_start = qcarcam_start(cam_handle_);
    if (ret_start != QCARCAM_RET_OK) {
        std::cout << "qcarcam_start failed, errorcode: " << ret_start << std::endl;
        return;
    }
    std::cout << "camera start success!" << std::endl;

    // process thread start
    processThread = std::thread(&CameraHandle::process_cb_event_thread, this);
}

// 停止摄像头
void CameraHandle::stop() {
    // qcarcam_stop
    if (cam_handle_ != nullptr) {
        qcarcam_stop(cam_handle_);
    }
}

void CameraHandle::uninitialize() {
    delete[] virtual_address_;
    free(buff);

    // qcarcam_close
    if (cam_handle_ != nullptr) {
        qcarcam_close(cam_handle_);
        cam_handle_ = nullptr;
    }

    // uninit
    qcarcam_ret_t ret_uninit = qcarcam_uninitialize();
    if (ret_uninit != QCARCAM_RET_OK) {
        std::cout << "qcar cam uninit failed, errorcode: " << ret_uninit << std::endl;
        return;
    }
}

void CameraHandle::qcarcam_event_cb(qcarcam_hndl_t hndl,
                                    qcarcam_event_t event_id,
                                    qcarcam_event_payload_t* p_payload) {
    CameraHandle& instance = getInstance();

    if (!instance.cam_handle_) {
        std::cout << "camera handle is null!" << std::endl;
        return;
    }
    // push event data to queue
    qcarcam_event_msg_t event_msg;
    event_msg.event_id = event_id;
    memcpy(&event_msg.payload, p_payload, sizeof(qcarcam_event_payload_t));
    pthread_mutex_lock(&instance.queue_mutex);
    instance.eventqueue.push(event_msg);
    // std::cout << "event_msg.event_id: " << event_msg.event_id << std::endl;
    pthread_mutex_unlock(&instance.queue_mutex);
}

int CameraHandle::process_cb_event_thread() {

    qcarcam_event_msg_t event_msg;

    while (1) {
        pthread_mutex_lock(&queue_mutex);
        if (!eventqueue.empty()) {
            event_msg = eventqueue.front();
            eventqueue.pop();
        } else {
            // std::cout << "event queue is empty!" << std::endl;
            pthread_mutex_unlock(&queue_mutex);
            continue;
        }
        pthread_mutex_unlock(&queue_mutex);

        switch (event_msg.event_id) {
            case QCARCAM_EVENT_FRAME_READY: {
                handle_new_frame();
                break;
            }
            case QCARCAM_EVENT_INPUT_SIGNAL: {
                // TODO
                break;
            }
            case QCARCAM_EVENT_ERROR: {
                switch (event_msg.payload.uint_payload) {
                    case QCARCAM_CONN_ERROR:
                        // TODO
                        break;
                    case QCARCAM_FATAL_ERROR:
                        // TODO
                        break;
                    case QCARCAM_FRAMESYNC_ERROR:
                        // TODO
                        break;
                    case QCARCAM_IFE_OVERFLOW_ERROR:
                        // TODO
                        break;
                    default:
                        break;
                }
                break;
            }
            case QCARCAM_EVENT_VENDOR: {
                // TODO
                break;
            }
            case QCARCAM_EVENT_PROPERTY_NOTIFY:
                // TODO
                break;
            case QCARCAM_EVENT_FRAME_FREEZE: {
                // TODO
                break;
            }
            default:
                // TODO
                break;
        }
    }
    return 0;
}

void CameraHandle::handle_new_frame() {
    qcarcam_frame_info_t frame_info = {0};
    qcarcam_ret_t ret =
        qcarcam_get_frame(cam_handle_, &frame_info, QCARCAM_TEST_DEFAULT_GET_FRAME_TIMEOUT, 0);
    if (QCARCAM_RET_OK != ret) {
        std::cout << "qcarcam_get_frame failed errorcode: " << ret << std::endl;
        return;
    }
    if (( unsigned int )frame_info.idx >= cam_buffers_.n_buffers) {
        printf("camera buffer overflow frame index:%d frame buffers:%d\n",
               frame_info.idx,
               cam_buffers_.n_buffers);
        return;
    }

    std::cout << "frame_info.idx: " << frame_info.idx << std::endl;
    get_frame_buf_idx = frame_info.idx;

    ret = qcarcam_release_frame(cam_handle_, frame_info.idx);
    if (QCARCAM_RET_OK != ret) {
        std::cout << "qcarcam_release_frame failed errorcode: " << ret << std::endl;
        return;
    }
    return;
}

int CameraHandle::getImage(unsigned char* image) {
    vaddr = mmap_device_memory(0,
                               cam_buffers_.buffers[get_frame_buf_idx].planes[0].size,
                               PROT_READ | PROT_WRITE,
                               0,
                               ( uint64_t )physical_address_);

    if ((MAP_FAILED == vaddr) || (nullptr == vaddr)) {
        std::cout << "mmap_device_memory() failed" << std::endl;
        return -1;
    }

    int buffer_size = cam_buffers_.buffers[get_frame_buf_idx].planes[0].size;

    memcpy(image, vaddr, cam_buffers_.buffers[get_frame_buf_idx].planes[0].size);

    munmap_device_memory(vaddr, cam_buffers_.buffers[get_frame_buf_idx].planes[0].size);
    return 0;
}
