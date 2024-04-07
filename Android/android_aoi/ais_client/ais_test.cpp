#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "qcarcam.h"

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>

#define TESTUTIL_ALIGN(x, y) (((x) + ((y)-1)) & ~((y)-1))

using namespace std;

#define  SIZE_4KB    0x00001000

#define ION_HEAP(bit)                   bit
#define ION_BIT(nr)                     (1U << (nr))
#define ION_SYSTEM_HEAP_ID              ION_BIT(25)
#define ION_SECURE_CARVEOUT_HEAP_ID     ION_BIT(15)
#define ION_FLAG_SECURE                 ION_BIT(31)
#define ION_QSECOM_HEAP_ID              ION_BIT(7)
#define ION_FLAG_CP_CDSP                ION_BIT(29)
#define ION_DISPLAY_HEAP_ID             ION_BIT(3)

#define ION_NODE "/dev/ion"

/**
 * struct ion_allocation_data - metadata passed from userspace for allocations
 * @len:		size of the allocation
 * @heap_id_mask:	mask of heap ids to allocate from
 * @flags:		flags passed to heap
 * @handle:		pointer that will be populated with a cookie to use to
 *			refer to this allocation
 *
 * Provided by userspace as an argument to the ioctl
 */
struct ion_allocation_data {
	__u64 len;
	__u32 heap_id_mask;
	__u32 flags;
	__u32 fd;
	__u32 unused;
};

#define ION_IOC_MAGIC		'I'

/**
 * DOC: ION_IOC_ALLOC - allocate memory
 *
 * Takes an ion_allocation_data struct and returns it with the handle field
 * populated with the opaque handle for the allocation.
 */
#define ION_IOC_ALLOC		_IOWR(ION_IOC_MAGIC, 0, \
				      struct ion_allocation_data)

qcarcam_hndl_t cam_handle_ = nullptr; // camera handler
static long QCARCAM_TEST_DEFAULT_GET_FRAME_TIMEOUT = 500000000;
qcarcam_buffers_t cam_buffers_;
void** virtual_address_ = nullptr;
void** physical_address_ = nullptr;

static void qcarcam_event_cb(qcarcam_hndl_t hndl,
                             qcarcam_event_t event_id,
                             qcarcam_event_payload_t* p_payload) {
    std::cout << "=== qcarcam_event_cb " << std::endl;

    if (!cam_handle_) {
        cout << "camera handle is null!" << endl;
        return;
    }

    if (event_id == QCARCAM_EVENT_FRAME_READY) {
        std::cout << "event_id: QCARCAM_EVENT_FRAME_READY" << std::endl;
        qcarcam_frame_info_t frame_info = {0};
        qcarcam_ret_t ret =
            qcarcam_get_frame(cam_handle_, &frame_info, QCARCAM_TEST_DEFAULT_GET_FRAME_TIMEOUT, 0);
        if (QCARCAM_RET_OK != ret) {
            cout << "qcarcam_get_frame failed errorcode: " << ret << endl;
            return;
        }
        if (( unsigned int )frame_info.idx >= cam_buffers_.n_buffers) {
            printf("camera buffer overflow frame index:%d frame buffers:%d\n",
                   frame_info.idx,
                   cam_buffers_.n_buffers);
            return;
        }

        std::cout << "=== seq_no: " << frame_info.seq_no << std::endl;
        std::cout << "=== timestamp: " << frame_info.timestamp << std::endl;
        std::cout << "=== timestamp_system: " << frame_info.timestamp_system << std::endl;
        std::cout << "=== sof_qtimestamp: " << frame_info.sof_qtimestamp << std::endl;

#if 1
        std::cout << "cam_buffers_.buffers[frame_info.idx].planes[0].size: " << cam_buffers_.buffers[frame_info.idx].planes[0].size << std::endl;
        int buffer_size = cam_buffers_.buffers[frame_info.idx].planes[0].size;
        unsigned char *buff = (unsigned char *)malloc(buffer_size);
        auto t1 = std::chrono::steady_clock::now();
        memcpy(buff, (const char*)virtual_address_[frame_info.idx], cam_buffers_.buffers[frame_info.idx].planes[0].size);
        auto t2 = std::chrono::steady_clock::now();
        long delta = std::chrono::duration_cast< std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "===memcpy cost time: " << delta << "ms" << std::endl;
        static int n = 1;
        if (n < 20) {
            std::string image_path = std::to_string(n) + ".bin";
            std::ofstream output(image_path, std::ios::out | std::ios::binary);
            if (!output.is_open()) {
                std::cerr << "[ERROR] open image file: " << image_path.c_str() << " failed." << std::endl;
            }

            output.write(reinterpret_cast<const char*>(buff), buffer_size);
            output.close();
            n++;
        }

        free(buff);

#else

        std::cout << "cam_buffers_.buffers[frame_info.idx].planes[0].size: "
                  << cam_buffers_.buffers[frame_info.idx].planes[0].size << std::endl;
        int buffer_size = cam_buffers_.buffers[frame_info.idx].planes[0].size;
        unsigned char* buff = ( unsigned char* )malloc(buffer_size);
        auto t1 = std::chrono::steady_clock::now();
        memcpy(buff,
               ( const char* )virtual_address_[frame_info.idx],
               cam_buffers_.buffers[frame_info.idx].planes[0].size);
        auto t2 = std::chrono::steady_clock::now();
        long delta = std::chrono::duration_cast< std::chrono::milliseconds >(t2 - t1).count();
        std::cout << "===memcpy cost time: " << delta << "ms" << std::endl;
        static int n = 1;
        if (n < 20) {
            std::string image_path = std::to_string(n) + ".bin";
            std::ofstream output(image_path, std::ios::out | std::ios::binary);
            if (!output.is_open()) {
                std::cerr << "[ERROR] open image file: " << image_path.c_str() << " failed."
                          << std::endl;
            }

            output.write(reinterpret_cast< const char* >(buff), buffer_size);
            output.close();
            n++;
        }
#endif

        ret = qcarcam_release_frame(cam_handle_, frame_info.idx);
        if (QCARCAM_RET_OK != ret) {
            cout << "qcarcam_release_frame failed errorcode: " << ret << endl;
            return;
        }
        return;
    }
}

int main(int argc, char* argv[]) {
    // qcarcam init
    qcarcam_init_t qcarcam_init = {0};
    qcarcam_init.version = QCARCAM_VERSION;
    qcarcam_init.debug_tag = ( char* )"qcarcam_test";
    auto ret_init = qcarcam_initialize(&qcarcam_init);
    if (ret_init) {
        cout << "qcar cam init failed, errorcode: " << ret_init << endl;
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
    printf("--- QCarCam Queried Inputs ----\n");
    for (int i = 0; i < queryFilled; i++) {
        printf("%d: input_id=%d, res=%dx%d fmt=0x%08x fps=%.2f flags=0x%x\n",
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
        // cam_handle_ = qcarcam_open(QCARCAM_INPUT_TYPE_EXT_FRONT);
        // cam_handle_ = qcarcam_open(QCARCAM_INPUT_TYPE_EXT_LEFT);
        // cam_handle_ = qcarcam_open(QCARCAM_INPUT_TYPE_EXT_RIGHT);
        if (cam_handle_ != nullptr) {
            cout << "open camera success!" << endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "qcarcam_open() failed, try " << i + 1 << " again..." << std::endl;
    }

    if (cam_handle_ == nullptr) {
        cout << "open camera failed!" << endl;
        return -1;
    }

    // qcarcam_s_buffers
    // color_fmt
    cam_buffers_.color_fmt = QCARCAM_FMT_MIPIRAW_8;
    // n_buffers
    cam_buffers_.n_buffers = 6;
    // buffers
    cam_buffers_.buffers =
        ( qcarcam_buffer_t* )calloc(cam_buffers_.n_buffers, sizeof(*cam_buffers_.buffers));
    if (!cam_buffers_.buffers) {
        cout << "calloc for cam_buffers_ failed, buf num: " << cam_buffers_.n_buffers << endl;
        return -1;
    }
    // flag
    cam_buffers_.flags = QCARCAM_BUFFER_FLAG_SECURE;
    cam_buffers_.flags |= QCARCAM_BUFFER_FLAG_OS_HNDL;
    // planes
    virtual_address_ = new void*[cam_buffers_.n_buffers];
    if (!virtual_address_) {
        printf("camera_id:{%d}, new for virtual_address_ failed, buf num: {%d}",
               0,
               cam_buffers_.n_buffers);
        return -1;
    }
    physical_address_ = new void*[cam_buffers_.n_buffers];
    if (!physical_address_) {
        printf("camera_id:{%d}, new for physical_address_ failed, buf num: {%d}",
               0,
               cam_buffers_.n_buffers);
        return -1;
    }

    // 打开 ION 设备
    int ion_fd = open(ION_NODE, O_RDWR);
    if (ion_fd < 0) {
        perror("Failed to open /dev/ion");
        return -1;
    }

    qcarcam_ret_t rc = QCARCAM_RET_OK;
    for (uint16_t i = 0; i < cam_buffers_.n_buffers; ++i) {
        cam_buffers_.buffers[i].n_planes = 1;
        cam_buffers_.buffers[i].planes[0].width = 1280;
        cam_buffers_.buffers[i].planes[0].height = 722;
        cam_buffers_.buffers[i].planes[0].stride = 1280;
        cam_buffers_.buffers[i].planes[0].size =
            cam_buffers_.buffers[i].planes[0].stride * cam_buffers_.buffers[i].planes[0].height;

        struct ion_allocation_data ion_alloc_data{};
        // align to 4KB
        ion_alloc_data.len = TESTUTIL_ALIGN(cam_buffers_.buffers[i].planes[0].size, SIZE_4KB);
        ion_alloc_data.heap_id_mask = ION_HEAP(ION_SYSTEM_HEAP_ID);
        ion_alloc_data.flags = 0;

        auto ret = ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc_data);
        if (ret == -1) {
            // 获取错误信息并输出
            printf("ioctl failed: %s\n", strerror(errno));
        } else {
            printf("ioctl succeeded\n");
        }

        cam_buffers_.buffers[i].planes[0].p_buf = (void*)(uintptr_t)ion_alloc_data.fd;

        virtual_address_[i] = mmap(NULL, ion_alloc_data.len, PROT_READ | PROT_WRITE, MAP_SHARED, ion_alloc_data.fd, 0);

        printf("Processed buffer %d\n", i);
    }

    printf("qcarcam_s_buffers 1 \n");

    qcarcam_ret_t ret_s_buffers = qcarcam_s_buffers(cam_handle_, &cam_buffers_);
    if (ret_s_buffers != QCARCAM_RET_OK) {
        cout << "qcarcam_s_buffers() failed, errorcode: " << ret_s_buffers << endl;
        return -1;
    }
    printf("qcarcam_s_buffers 2 \n");

    qcarcam_buffers_t *buffers;

    // qcarcam_s_param
    qcarcam_param_value_t param;
    param.ptr_value = ( void* )qcarcam_event_cb;
    qcarcam_ret_t ret = qcarcam_s_param(cam_handle_, QCARCAM_PARAM_EVENT_CB, &param);
    if (ret != QCARCAM_RET_OK) {
        cout << "qcarcam_s_param for qcarcam_event_cb failed!" << endl;
        return -1;
    }

    param.uint_value = QCARCAM_EVENT_FRAME_READY | QCARCAM_EVENT_INPUT_SIGNAL |
                       QCARCAM_EVENT_ERROR | QCARCAM_EVENT_VENDOR | QCARCAM_EVENT_FRAME_FREEZE;
    ret = qcarcam_s_param(cam_handle_, QCARCAM_PARAM_EVENT_MASK, &param);
    if (ret != QCARCAM_RET_OK) {
        cout << "qcarcam_s_param for uint_value failed!" << endl;
        return -1;
    }

    if (cam_handle_ == nullptr) {
        std::cout << "cam_handle_ == nullptr " << std::endl;
    }

    printf("qcarcam_s_param \n");

    // qcarcam_start
    int ret_start = qcarcam_start(cam_handle_);
    if (ret_start != QCARCAM_RET_OK) {
        cout << "qcarcam_start failed, errorcode: " << ret_start << endl;
        return -1;
    }
    cout << "camera start success!" << endl;

    printf("qcarcam_start \n");

    // wait
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // qcarcam_stop
    if (cam_handle_ != nullptr) {
        qcarcam_stop(cam_handle_);
    }

    // qcarcam_close
    if (cam_handle_ != nullptr) {
        qcarcam_close(cam_handle_);
        cam_handle_ = nullptr;
    }

    // // 解除映射
    // if (munmap(file_ptr, FILE_SIZE) == -1) {
    //     perror("Error unmapping file");
    // }

    // 关闭ion设备
    close(ion_fd);

    // uninit
    qcarcam_ret_t ret_uninit = qcarcam_uninitialize();
    if (ret_uninit != QCARCAM_RET_OK) {
        cout << "qcar cam uninit failed, errorcode: " << ret_uninit << endl;
        return ret;
    }

    return 0;
}
