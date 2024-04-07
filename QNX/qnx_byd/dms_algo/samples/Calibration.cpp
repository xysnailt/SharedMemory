#include "ImageReader.hpp"
#include "sensedriver_calibration.h"
#include "sensedriver_interface.h"
#include "sstream"

#include <chrono>

struct CalibrationConfig {
    // config
    uint32_t fps;
    uint32_t base_frame_count = 100;
    std::string image_list_path;
    std::string video_path;
    std::string config_path;
    // handle
    std::shared_ptr< SDHandle > handle =
        std::shared_ptr< SDHandle >(new SDHandle(nullptr), [](SDHandle* p) {
            SenseDriverDestroy(p);
            delete p;
        });
    // image reader
    std::shared_ptr< ImageReader > image_reader = nullptr;
};

std::string CalibrationState2Str(const MDistractionCalibrationState& calibration_state) {
    switch (calibration_state) {
        case MDistractionCalibrationState::M_DISTRACTION_CALIBRATION_UNCALIBRATED:
            return "Calibration state: Uncalibration";
        case MDistractionCalibrationState::M_DISTRACTION_CALIBRATION_CALIBRATING:
            return "Calibration state: Calibrating";
        case MDistractionCalibrationState::M_DISTRACTION_CALIBRATION_DONE:
            return "Calibration state: Done";
        default:
            return "Calibration is unnecessary";
    }
}

int Init(CalibrationConfig& para) {

    // init image reader
    para.image_reader = std::make_shared< ImageReader >(para.fps);

    // init
    auto ret =
        SenseDriverInit(para.config_path.c_str(), static_cast< int >(para.config_path.length()), para.handle.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error" << std::endl;
        return -1;
    }

    return 0;
}

int RunCalibration(CalibrationConfig& para) {

    int count = 0;
    while (true) {
        std::cout << "Frame:" << count << " | ";
        ++count;
        // get image
        auto image_with_timestamp = para.image_reader->image_queue_->pop();
        if (image_with_timestamp.image.empty()) {
            std::cout << " Input image is empty." << std::endl;
            return 0;
        }
#if CONFIG_AD_META_NEURO_PILOT_VERSION_5 == 1
        auto& mat = image_with_timestamp.image;
        auto origin_h = mat.rows;
        auto origin_w = mat.cols;
        cv::cvtColor(mat, mat, cv::COLOR_BGR2YUV_I420);
        YU12_to_YUV420SP(mat.data, origin_h * origin_w, false);
        auto image_handle = CreateSingleImageHandle(
            mat.data, nullptr, image_with_timestamp.time_stamp, M_PIX_FMT_NV21, origin_w, origin_h, MEM_ON_CPU);

#else
        // create image handle
        auto image_handle = CreateSingleImageHandle(image_with_timestamp.image.data,
                                                    nullptr,
                                                    image_with_timestamp.time_stamp,
                                                    MPixelFormat::M_PIX_FMT_BGR888,
                                                    image_with_timestamp.image.cols,
                                                    image_with_timestamp.image.rows,
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

        // run distraction
        auto ret = SenseDriverRun(*para.handle, image_handle, result.get());
        if (MDMS_SUCCESS != ret) {
            std::cerr << "SenseDriverRun failed with error: " << static_cast< int >(ret)
                      << std::endl;
            return -1;
        }

        // check distraction status
        MDistractionCalibrationState calibration_state;
        ret = SenseDriverGetCalibrationState(*para.handle, &calibration_state);
        if (MDMS_SUCCESS != ret) {
            std::cerr << "SenseDriverGetCalibrationState failed with error: "
                      << static_cast< int >(ret) << std::endl;
            return -1;
        }
        std::cout << CalibrationState2Str(calibration_state) << std::endl;

        if (M_DISTRACTION_CALIBRATION_DONE == calibration_state) {
            // get calibration result
            MDistractionCalibrationParam calibration_param;
            auto ret = SenseDriverGetCalibrationParams(*para.handle, &calibration_param);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverGetCalibrationParams failed" << std::endl;
                return -1;
            }
            std::cout << "Frame:" << count << "|Status:success|Params: ";
            std::cout << calibration_param.head_pose.base_head_pose.yaw << ","
                      << calibration_param.head_pose.base_head_pose.pitch << ","
                      << calibration_param.head_pose.base_head_pose.roll << std::endl;
            break;
        }
    } // while
    return 0;
}

int ReadImageThread(CalibrationConfig& para) {
    if (!para.image_list_path.empty()) {
        return para.image_reader->ReadImage(para.image_list_path, false);
    } else {
        return para.image_reader->ReadImage(para.video_path, true);
    }
}

void Run(CalibrationConfig& para) {
    // read image and do face id
    std::thread image_read_thread(ReadImageThread, std::ref(para));
    std::thread dms_thread(RunCalibration, std::ref(para));

    dms_thread.join();
    // stop image queue
    para.image_reader->is_run_.store(false);

    image_read_thread.join();
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <video path> "
                  << " <calibration fps(for example: 30)> " << std::endl;
        return -1;
    }

    CalibrationConfig para;
    // set value
    para.fps = std::stoi(std::string(argv[2]));
    para.video_path = argv[1];
    para.config_path = "./data/config/";

#ifdef CONFIG_AD_NN_ENABLE_CVFLOW
    hardware* hw = new hardware();
    hw->InitHardWare();
#endif

    // init
    if (0 != Init(para)) {
        return -1;
    }
    // run
    Run(para);

    return 0;
}
