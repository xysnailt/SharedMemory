#include "ImageReader.hpp"

#include <opencv2/core/version.hpp>
#include <opencv2/opencv.hpp>
#if CV_MAJOR_VERSION >= 4
#include <opencv2/videoio/legacy/constants_c.h>
#endif

#include "sensedriver_interface.h"

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

class DMSResultPrinter {
public:
    std::string CameraOcclusion2Str(const MProductCameraOcclusionResult& status) {
        switch (status) {
            case MProductCameraOcclusionResult::M_CAMERA_OCCLUSION:
                return "Occlusion";
            default:
                return "Not_Occlusion";
        }
    }

    void PrintDMSResult(const SDResult& result) {
        if (result.person_number > 0) {
            // person result:
            const auto& person = result.person_array_result[0];
            // person result ----> face result
            const auto& face_result = person.face_result;
            if (MResultValid::RESULT_INVALID == face_result.result_valid) {
                std::cout << "|No face or face quality is low";
            } else {
                // face rect
                std::cout << "|FACE_RECT:"
                          << "[" << face_result.face_rect.left << "," << face_result.face_rect.top
                          << "][" << face_result.face_rect.right << ","
                          << face_result.face_rect.bottom << "]";
            }
        }

        // camera occlusion status
        const auto& camera_occlusion_result = result.camera_occlusion_result;
        if (MResultValid::RESULT_VALID == camera_occlusion_result.result_valid) {
            std::cout << "|CAMERAOCCLUSION:" << CameraOcclusion2Str(camera_occlusion_result.result)
                      << std::endl;
        } else {
            std::cout << "|CAMERAOCCLUSION:Invalid" << std::endl;
        }
    }
};

int Init(DMSConfig& para) {
    // init
    auto ret = SenseDriverInit(
        para.config_path.c_str(), static_cast< int >(para.config_path.length()), para.handle.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error" << std::endl;
        return -1;
    }
    // unload modules
    SenseDriverModuleBitMask bitmask{0U};
    ret = SenseDriverGetEnableModules(*para.handle, &bitmask);
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverGetEnableModules error" << std::endl;
        return -1;
    }
    ret = SenseDriverLoadModule(*para.handle, bitmask & SENSEDRIVER_MODULE_VERIFY, false);
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverLoadModule error" << std::endl;
        return -1;
    }
    ret = SenseDriverLoadModule(*para.handle, bitmask & SENSEDRIVER_MODULE_FACEATTR, false);
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverLoadModule error" << std::endl;
        return -1;
    }

    ret = SenseDriverLoadModule(*para.handle, bitmask & SENSEDRIVER_MODULE_ACTION, false);
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverLoadModule error" << std::endl;
        return -1;
    }
    return 0;
}

int DoDMS(DMSConfig& para) {
    DMSResultPrinter dms_result_printer;

    // open video
    cv::VideoCapture video_capture;
    if (!video_capture.open(para.video_path)) {
        std::cerr << "open video failed: " << para.video_path << std::endl;
        return -1;
    }

    if (para.fps < 1) {
        std::cout << "video fps error!" << std::endl;
        return -1;
    }

    auto tmp_fps = para.fps;
    para.fps = video_capture.get(CV_CAP_PROP_FPS); // 获取视频文件的帧率
    if (para.fps <= 1e-5) {
        para.fps = tmp_fps;
    }

    // image obj
    cv::Mat image;

    // video frame index
    int frame_index = 0;

    while (true) {
        // get image
        video_capture >> image;
        if (image.empty()) {
            std::cout << "Read video finished" << std::endl;
            return 0;
        }

        static MTimeStamp cur_time_stamp;
        if (frame_index == 0) { // first time, get system timestamp
            GetTimeStamp(cur_time_stamp);
        } else { // update timestamp by fps
            UpdateTimeStampByFPS(para.fps, cur_time_stamp);
        }

#if CONFIG_AD_META_NEURO_PILOT_VERSION_5 == 1
        auto& mat = image;
        auto origin_h = mat.rows;
        auto origin_w = mat.cols;
        cv::cvtColor(mat, mat, cv::COLOR_BGR2YUV_I420);
        YU12_to_YUV420SP(mat.data, origin_h * origin_w, false);
        auto image_handle = CreateSingleImageHandle(
            mat.data, nullptr, cur_time_stamp, M_PIX_FMT_NV21, origin_w, origin_h, MEM_ON_CPU);

#else

        // create image handle
        auto image_handle = CreateSingleImageHandle(image.data,
                                                    nullptr,
                                                    cur_time_stamp,
                                                    MPixelFormat::M_PIX_FMT_BGR888,
                                                    image.cols,
                                                    image.rows,
                                                    MMemoryType::MEM_ON_CPU);
#endif

        // image handle deleter
        std::shared_ptr< char > image_handle_deletor(
            nullptr, [&](char* p) { DestroySingleImageHandle(&image_handle); });
        // dms result
        std::shared_ptr< SDResult > result(new SDResult, [](SDResult* p) {
            SenseDriverDestroyResult(p);
            delete p;
        });

        std::cout << "Frame: " << frame_index;

        // run
        auto ret = SenseDriverRun(*para.handle, image_handle, result.get());
        if (MDMS_SUCCESS != ret) {
            std::cerr << "SenseDriverRun failed with error: " << static_cast< int >(ret)
                      << std::endl;
            return -1;
        }

        // print result to screen
        dms_result_printer.PrintDMSResult(*result);

        // update frame index
        ++frame_index;
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << "<video path>"
                  << "<fps for video>" << std::endl;
        return -1;
    }

    DMSConfig para;
    // set value
    para.video_path = argv[1];
    para.config_path = "./data/config/";
    para.fps = std::stoi(std::string(argv[2]));

#ifdef CONFIG_AD_NN_ENABLE_CVFLOW
    hardware* hw = new hardware();
    hw->InitHardWare();
#endif

    // init
    if (0 != Init(para)) {
        return -1;
    }

    // run
    if (0 != DoDMS(para)) {
        return -1;
    }
    return 0;
}