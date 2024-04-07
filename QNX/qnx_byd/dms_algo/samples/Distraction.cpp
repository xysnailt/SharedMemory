#include "ImageReader.hpp"

#include <opencv2/core/version.hpp>
#include <opencv2/opencv.hpp>
#if CV_MAJOR_VERSION >= 4
#include <opencv2/videoio/legacy/constants_c.h>
#endif
#include "sensedriver_calibration.h"
#include "sensedriver_interface.h"

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#ifdef CONFIG_AD_NN_ENABLE_CVFLOW
#include "hardware.hpp"
#endif

struct DistractionConfig {
    // config
    uint32_t fps;
    std::string video_path;
    std::string config_path;
    float yaw = 0;
    float pitch = 0;
    float roll = 0;
    // handle
    std::shared_ptr< SDHandle > handle =
        std::shared_ptr< SDHandle >(new SDHandle(nullptr), [](SDHandle* p) {
            SenseDriverDestroy(p);
            delete p;
        });
};

std::string Distraction2Str(const MDistractionLevel& level) {
    switch (level) {
        case MDistractionLevel::M_DISTRACTION_LEVEL_NONE:
            return "Focus";
        case MDistractionLevel::M_DISTRACTION_LEVEL_LIGHT:
            return "Light";
        case MDistractionLevel::M_DISTRACTION_LEVEL_MEDIUM:
            return "Medium";
        case MDistractionLevel::M_DISTRACTION_LEVEL_HEAVY:
            return "Heavy";
        default:
            return "Invalid";
    }
}

int Init(DistractionConfig& para) {

    // init
    auto ret = SenseDriverInit(
        para.config_path.c_str(), static_cast< int >(para.config_path.length()), para.handle.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error" << std::endl;
        return -1;
    }

    return 0;
}

int RunDistraction(DistractionConfig& para) {
    int count = 0;

    // set calibration result
    MDistractionCalibrationParam calibration_param;
    calibration_param.head_pose.base_head_pose.yaw = para.yaw;
    calibration_param.head_pose.base_head_pose.pitch = para.pitch;
    calibration_param.head_pose.base_head_pose.roll = para.roll;

    auto ret = SenseDriverSetCalibrationParams(*para.handle, &calibration_param);
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverSetCalibrationParams failed." << std::endl;
        return -1;
    }

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

        std::cout << "Frame:" << frame_index;

        // run distraction
        auto ret = SenseDriverRun(*para.handle, image_handle, result.get());
        if (MDMS_SUCCESS != ret) {
            std::cerr << "|SenseDriverRun failed with error: " << static_cast< int >(ret)
                      << std::endl;
            return -1;
        }

        if (result->person_number <= 0) {
            ++frame_index;
            continue;
        }

        // person result:
        const auto& person = result->person_array_result[0];
        // person result ----> face result
        const auto& face_result = person.face_result;
        if (MResultValid::RESULT_INVALID == face_result.result_valid) {
            std::cout << "|No face or face quality is low." << std::endl;
        } else {
            std::cout << "|FACE_RECT:"
                      << "[" << face_result.face_rect.left << "," << face_result.face_rect.top
                      << "][" << face_result.face_rect.right << "," << face_result.face_rect.bottom
                      << "]";
        }

        if (face_result.gazeaoi_result.result_valid == MResultValid::RESULT_VALID) {
            std::cout << "|AOI:" << face_result.gazeaoi_result.gazeaoi;
        }

        // distraciton result
        const auto& distraction_result = face_result.distraction_result;
        if (MResultValid::RESULT_VALID == distraction_result.result_valid) {
            std::cout << "|DISTRACTION_LEVEL:" << Distraction2Str(distraction_result.level)
                      << std::endl;
        } else {
            std::cout << "null distraction result" << std::endl;
        }

        // update frame index
        ++frame_index;
    } // while
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <video path> "
                  << "<fps for video>" << std::endl;
        return -1;
    }

    DistractionConfig para;
    if (argc > 3) {
        if (argc < 6) {
            std::cout << "Usage: " << argv[0] << " <video path> "
                      << "<fps for video>"
                      << " <yaw> <pitch> <roll> " << std::endl;
            return -1;
        }
        std::stringstream ss;
        ss << argv[3];
        ss >> para.yaw;
        ss.clear();

        ss << argv[4];
        ss >> para.pitch;
        ss.clear();

        ss << argv[5];
        ss >> para.roll;
        ss.clear();
    } else {
        para.yaw = .0f;
        para.pitch = .0f;
        para.roll = .0f;
    }

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
    if (0 != RunDistraction(para)) {
        return -1;
    }

    return 0;
}
