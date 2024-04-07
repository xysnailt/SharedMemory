// #include "linux/ion.h"

#include <errno.h>
#include <fcntl.h>
// #include <gralloc_priv.h>
// #include <linux/msm_ion.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
// #include <mqueue.h>
// #include "pmem.h"
#include "qcarcam.h"
#include "test_util.h"

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <linux/videodev2.h>
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
#include <ion.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <dlfcn.h>

using namespace std;

#define FILE_SIZE (6 * 1280 * 722)
#define BLOCK_SIZE (1280 * 722)

#define ION_HEAP(bit)                   bit
#define ION_BIT(nr)                     (1U << (nr))
#define ION_SYSTEM_HEAP_ID              ION_BIT(25)
#define ION_SECURE_CARVEOUT_HEAP_ID     ION_BIT(15)
#define ION_FLAG_SECURE                 ION_BIT(31)
#define ION_QSECOM_HEAP_ID              ION_BIT(7)
#define ION_FLAG_CP_CDSP                ION_BIT(29)
#define ION_DISPLAY_HEAP_ID             ION_BIT(3)

#define DEFAULT_HEAP "/dev/dma_heap/qcom,system"
#define ION_NODE "/dev/ion"


/* Valid FD_FLAGS are O_CLOEXEC, O_RDONLY, O_WRONLY, O_RDWR */
#define DMA_HEAP_VALID_FD_FLAGS (O_CLOEXEC | O_ACCMODE)

/* Currently no heap flags */
#define DMA_HEAP_VALID_HEAP_FLAGS (0)

/**
 * struct dma_heap_allocation_data - metadata passed from userspace for
 *                                      allocations
 * @len:		size of the allocation
 * @fd:			will be populated with a fd which provides the
 *			handle to the allocated dma-buf
 * @fd_flags:		file descriptor flags used when allocating
 * @heap_flags:		flags passed to heap
 *
 * Provided by userspace as an argument to the ioctl
 */
struct dma_heap_allocation_data {
	__u64 len;
	__u32 fd;
	__u32 fd_flags;
	__u64 heap_flags;
};

/**
 * struct ion_allocation_data - metadata passed from userspace for allocations
 * @len:		size of the allocation
 * @align:		required alignment of the allocation
 * @heap_id_mask:	mask of heap ids to allocate from
 * @flags:		flags passed to heap
 * @handle:		pointer that will be populated with a cookie to use to
 *			refer to this allocation
 *
 * Provided by userspace as an argument to the ioctl
 */

typedef int ion_user_handle_t;

// struct ion_allocation_data {
// 	size_t len;
// 	size_t align;
// 	unsigned int heap_id_mask;
// 	unsigned int flags;
// 	ion_user_handle_t handle;
//     int fd;
// };

#define ION_IOC_MAGIC		'I'

/**
 * DOC: ION_IOC_ALLOC - allocate memory
 *
 * Takes an ion_allocation_data struct and returns it with the handle field
 * populated with the opaque handle for the allocation.
 */
#define ION_IOC_ALLOC		_IOWR(ION_IOC_MAGIC, 0, \
				      struct ion_allocation_data)

#define DMA_HEAP_IOC_MAGIC		'H'

/**
 * DOC: DMA_HEAP_IOCTL_ALLOC - allocate memory from pool
 *
 * Takes a dma_heap_allocation_data struct and returns it with the fd field
 * populated with the dmabuf handle of the allocation.
 */
#define DMA_HEAP_IOCTL_ALLOC	_IOWR(DMA_HEAP_IOC_MAGIC, 0x0,\
				      struct dma_heap_allocation_data)

qcarcam_hndl_t cam_handle_ = nullptr; // camera handler
static long QCARCAM_TEST_DEFAULT_GET_FRAME_TIMEOUT = 500000000;
qcarcam_buffers_t cam_buffers_;
void** virtual_address_ = nullptr;
void** physical_address_ = nullptr;
static int
alloc_ion_buffer(struct instance *i, size_t size, uint32_t flags)
{
 struct ion_allocation_data ion_alloc = { 0 };
 struct ion_fd_data ion_fd_data = { 0 };
 struct ion_handle_data ion_handle_data = { 0 };
 static int ion_fd = -1;
 int ret;

 if (ion_fd < 0) {
  ion_fd = open("/dev/ion", O_RDONLY);
  if (ion_fd < 0) {
   err("Cannot open ion device: %m");
   return -1;
  }
 }

 ion_alloc.handle = -1;
 ion_alloc.len = size;
 ion_alloc.align = 4096;
 ion_alloc.flags = flags;

 if (flags & ION_SECURE)
  ion_alloc.heap_id_mask = ION_HEAP(ION_SECURE_HEAP_ID);
 else
  ion_alloc.heap_id_mask = ION_HEAP(ION_IOMMU_HEAP_ID);

 if (flags & ION_FLAG_CP_BITSTREAM)
  ion_alloc.heap_id_mask |= ION_HEAP(ION_SECURE_DISPLAY_HEAP_ID);

 if (ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc) < 0) {
  err("Failed to allocate ion buffer: %m");
  return -1;
 }

 dbg("Allocated %zd bytes ION buffer %d",
     ion_alloc.len, ion_alloc.handle);

 ion_fd_data.handle = ion_alloc.handle;
 ion_fd_data.fd = -1;

 if (ioctl(ion_fd, ION_IOC_MAP, &ion_fd_data) < 0) {
  err("Failed to map ion buffer: %m");
  ret = -1;
 } else {
  ret = ion_fd_data.fd;
 }

 ion_handle_data.handle = ion_alloc.handle;
 if (ioctl(ion_fd, ION_IOC_FREE, &ion_handle_data) < 0)
  err("Failed to free ion buffer: %m");

 return ret;
}

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

#if 0
        std::cout << "cam_buffers_.buffers[frame_info.idx].planes[0].size: " << cam_buffers_.buffers[frame_info.idx].planes[0].size << std::endl;
        int buffer_size = cam_buffers_.buffers[frame_info.idx].planes[0].size;
        unsigned char *buff = (unsigned char *)malloc(buffer_size);
        auto t1 = std::chrono::steady_clock::now();
        memcpy(buff, (const char*)physical_address_[frame_info.idx], cam_buffers_.buffers[frame_info.idx].planes[0].size);
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
        printf("camera_id:{}, new for virtual_address_ failed, buf num: {}",
               0,
               cam_buffers_.n_buffers);
        return -1;
    }
    physical_address_ = new void*[cam_buffers_.n_buffers];
    if (!physical_address_) {
        printf("camera_id:{}, new for physical_address_ failed, buf num: {}",
               0,
               cam_buffers_.n_buffers);
        return -1;
    }

    int fd = open("/dev/video51", O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        std::cout << "Error opening file" << std::endl;
    }

    struct v4l2_capability cap;
    int rc = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (rc) {
        std::cout << "fail to querycap" << std::endl;
    } else {
        std::cout << "succeed to querycap" << std::endl;
    }
    int rc = io_ctrl(VIDIOC_QUERYCAP, &cap);
    if (rc) {
        CAM_MSG(ERROR, "fail to querycap");
    } else {
        CAM_MSG(HIGH, "succeed to querycap");
    }

    CAM_MSG(HIGH, "succeed to connect %s fd %d", "/dev/video51", fd);

    调整文件大小
    if (ftruncate(fd, FILE_SIZE) == -1) {
        std::cout << "Error ftruncate file" << std::endl;
        close(fd);
    }

    映射文件到内存
    void* file_ptr = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_ptr == MAP_FAILED) {
        std::cout << "Error mmap" << std::endl;
        close(fd);
    }
    v4l2_buffer device_buffer;
    rc = 0;
    struct v4l2_exportbuffer expbuf = {0};

    // 打开共享库文件
    void *lib_handle = dlopen("/system/lib64/libion.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    typedef int (*mfd)(int, size_t, unsigned int, unsigned int);
    char *error;

    // 获取要调用的函数指针
    mfd ion_alloc_fd = (mfd)dlsym(lib_handle, "ion_alloc_fd");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        dlclose(lib_handle);
        return 1;
    }

    // 打开 ION 设备
    int ion_fd = open("/dev/ion", O_RDWR);
    if (ion_fd < 0) {
        perror("Failed to open /dev/ion");
        dlclose(lib_handle);
        return -1;
    }

    std::cout << "step1 " << std::endl;



    std::cout << "step2 " << std::endl;

    qcarcam_ret_t rc = QCARCAM_RET_OK;
    for (uint16_t i = 0; i < cam_buffers_.n_buffers; ++i) {
        cam_buffers_.buffers[i].n_planes = 1;
        // cam_buffers_.buffers[i].planes[0].width = 1280;
        // cam_buffers_.buffers[i].planes[0].height = 722;
        // cam_buffers_.buffers[i].planes[0].stride = 1280;
        cam_buffers_.buffers[i].planes[0].width = 1280;
        cam_buffers_.buffers[i].planes[0].height = 722;
        cam_buffers_.buffers[i].planes[0].stride = 1280;
        cam_buffers_.buffers[i].planes[0].size =
            cam_buffers_.buffers[i].planes[0].stride * cam_buffers_.buffers[i].planes[0].height;

        // cam_buffers_.buffers[i].planes[1].width = 1280;
        // cam_buffers_.buffers[i].planes[1].height = 722;
        // cam_buffers_.buffers[i].planes[1].stride = 1280;
        // cam_buffers_.buffers[i].planes[1].size =
        //     cam_buffers_.buffers[i].planes[1].stride * cam_buffers_.buffers[i].planes[1].height;

        // 分配 1MB 大小的内存，并指定相应的标志和堆类型
        size_t len = 10000; // 1MB
        unsigned int flags = ION_FLAG_CACHED;
        unsigned int heap_mask = 4;

        int fd = ion_alloc_fd(ion_fd,len,heap_mask,flags);
        if (fd < 0) {
            perror("ION allocation failed");
            close(ion_fd);
            dlclose(lib_handle);
            return -1;
        }
        std::cout << "ddd" << std::endl;

        void *pmmap = mmap(NULL, 10000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (pmmap == MAP_FAILED) {
            perror("mmap failed");
            close(fd); // 关闭文件描述符
            return -1;
        }

        char* buf = (char*)pmmap;

        std::cout << "mmap stop" << std::endl;

        std::cout << (char)(buf)[0] << std::endl;


        std::cout << "pmmap[0] stop" << std::endl;

        std::string str = "a";
        buf[0] = 'A';
        // memset(pmmap, 'A', 1);

        std::cout << (char)(buf[0]) << std::endl;

        std::cout << "dd11111d" << std::endl;

        device_buffer.type = m_type;
        device_buffer.memory = m_mem_type;
        device_buffer.index = idx;

        每个 buffer 的起始位置
        cam_buffers_.buffers[i].planes[0].p_buf = ( void* )(( char* )file_ptr + i * BLOCK_SIZE);
        cam_buffers_.buffers[i].planes[0].p_buf = mmap(NULL,
                                                       cam_buffers_.buffers[i].planes[0].size,
                                                       PROT_READ | PROT_WRITE,
                                                       MAP_SHARED,
                                                       fd,
                                                       i * BLOCK_SIZE);
        int dma_heap_fd;
        dma_heap_fd = open(DEFAULT_HEAP, O_RDONLY | O_CLOEXEC);

        int ion_fd = open(ION_NODE, O_RDONLY | O_CLOEXEC);
        if (ion_fd < 0) {
            perror("open");
            std::cout << "fail ion_fd = " << ion_fd << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "success ion_fd = " << ion_fd << std::endl;
        std::cout << "ion heap" << std::endl;
        struct dma_heap_allocation_data alloc_data;
        unsigned int fd_flags = O_RDWR | O_CLOEXEC;
        unsigned int heap_flags = DMA_HEAP_VALID_HEAP_FLAGS;
        alloc_data.len = cam_buffers_.buffers[i].planes[0].size;

        alloc_data.fd = 0;
        alloc_data.fd_flags = fd_flags;
        alloc_data.heap_flags = heap_flags;

        struct ion_allocation_data ion_alloc_data;
        memset(&ion_alloc_data, 0, sizeof(ion_alloc_data));
        ion_alloc_data.len = cam_buffers_.buffers[i].planes[0].size;
        // ion_alloc_data.align = 2;
        ion_alloc_data.heap_id_mask = ION_HEAP(ION_SYSTEM_HEAP_ID);
        ion_alloc_data.flags = 0;
        auto ret = ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc_data);
        if (ret == -1) {
            // 获取错误信息并输出
            printf("ioctl failed: %s\n", strerror(errno));
        } else {
            printf("ioctl succeeded\n");
        }

        if (ret) {
            QCARCAM_ERRORMSG("failed to allocate memory from ion: %d", ret);
            rc =  QCARCAM_RET_NOMEM;
            break;
        }

        QCARCAM_DBGMSG("Success: map ion_device");
        std::cout << "test 1" << std::endl;

        // auto ret = ioctl(dma_heap_fd, ION_IOC_ALLOC, &alloc_data);

        // if (ret) {
        //     QCARCAM_ERRORMSG("failed to allocate dmabuf from heap  %s %d:\n",
        //                 "dmsssss", ret);
        //     std::cout << "ioctl fail" << ret << std::endl;
        //     rc =  QCARCAM_RET_NOMEM;
        //     break;
        // }
        cam_buffers_.buffers[i].planes[0].p_buf = (void*)(uintptr_t)fd;

        printf("Processed buffer %d\n", i);
    }

    printf("qcarcam_s_buffers 1 \n");

    qcarcam_ret_t ret_s_buffers = qcarcam_s_buffers(cam_handle_, &cam_buffers_);
    if (ret_s_buffers != QCARCAM_RET_OK) {
        cout << "qcarcam_s_buffers() failed, errorcode: " << ret_s_buffers << endl;
        return -1;
    }
    printf("qcarcam_s_buffers 2 \n");

    test_util_ctxt_t *ctxt;
    test_util_window_t *user_ctxt;
    qcarcam_buffers_t *buffers;

    // test_util_init_window_buffers(ctxt, user_ctxt, buffers);

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

    // 关闭文件
    // close(fd);

    // uninit
    qcarcam_ret_t ret_uninit = qcarcam_uninitialize();
    if (ret_uninit != QCARCAM_RET_OK) {
        cout << "qcar cam uninit failed, errorcode: " << ret_uninit << endl;
        return ret;
    }

    return 0;
}


static inline int access_ok(int type, const void *addr, unsigned long size)
/*
type :是VERIFY_READ 或者VERIFY_WRITE用来表明是读用户内存还是写用户内存;
addr:是要操作的用户内存地址;
size:是操作的长度。如果ioctl需要从用户空间读一个整数，那么size参数就等于sizeof(int)；
返回值：Access_ok返回一个布尔值：1，是成功（存取没问题）；0，是失败，ioctl返回-EFAULT；
*/