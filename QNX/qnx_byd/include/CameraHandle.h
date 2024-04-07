#ifndef CAMERAHANDLE_H
#define CAMERAHANDLE_H

#include "pmem.h"
#include "qcarcam.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 722
#define IMAGE_STRIDE 1280

static long QCARCAM_TEST_DEFAULT_GET_FRAME_TIMEOUT = 500000000;

typedef struct {
    qcarcam_event_t event_id;
    qcarcam_event_payload_t payload;
} qcarcam_event_msg_t;

class CameraHandle {
private:
    CameraHandle(){};
    ~CameraHandle(){};

    CameraHandle(const CameraHandle& instance) = delete;
    const CameraHandle& operator=(const CameraHandle& instance) = delete;

    static void qcarcam_event_cb(qcarcam_hndl_t hndl,
                                 qcarcam_event_t event_id,
                                 qcarcam_event_payload_t* p_payload);
    int process_cb_event_thread();
    void handle_new_frame();

public:
    static CameraHandle& getInstance() {
        static CameraHandle instance;
        return instance;
    }

    int initialize();
    void start();
    void stop();
    void uninitialize();
    unsigned char* get_buf_addr() { return buff; }

    int getImage(unsigned char* image);

private:
    qcarcam_hndl_t cam_handle_ = nullptr; // camera handler
    qcarcam_buffers_t cam_buffers_;
    void** virtual_address_ = nullptr;
    long long physical_address_;
    void* vaddr;
    unsigned char* buff = ( unsigned char* )malloc(IMAGE_STRIDE * IMAGE_HEIGHT);
    std::queue< qcarcam_event_msg_t > eventqueue;
    pthread_mutex_t queue_mutex; // mutex for eventqueue
    std::thread processThread;
    int get_frame_buf_idx;
};

#endif
