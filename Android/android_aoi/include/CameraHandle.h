#ifndef CAMERAHANDLE_H
#define CAMERAHANDLE_H

#include "pmem.h"
#include "qcarcam.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

static long QCARCAM_TEST_DEFAULT_GET_FRAME_TIMEOUT = 500000000;
#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 722
#define IMAGE_STRIDE 1280

class CameraHandle {
private:
    CameraHandle(){};
    ~CameraHandle(){};

    CameraHandle(const CameraHandle& instance) = delete;
    const CameraHandle& operator=(const CameraHandle& instance) = delete;

    static void qcarcam_event_cb(qcarcam_hndl_t hndl,
                                 qcarcam_event_t event_id,
                                 qcarcam_event_payload_t* p_payload);

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

private:
    qcarcam_hndl_t cam_handle_ = nullptr; // camera handler
    qcarcam_buffers_t cam_buffers_;
    void** virtual_address_ = nullptr;
    long long physical_address_;
    void* vaddr;
    unsigned char* buff = ( unsigned char* )malloc(IMAGE_STRIDE * IMAGE_HEIGHT);
};

#endif