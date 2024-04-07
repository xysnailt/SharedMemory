#include "ImageReader.hpp"
#include "ParseJson.hpp"
#include "sensedriver_calibration.h"
#include "sensedriver_interface.h"
#include "sstream"

#include <chrono>

struct DistractionCusConfig {
    // config
    std::string image_first;
    std::string image_second;
    std::string config_path;
    std::string calibration_result_file;
    int calibration_run_frames;
    // Json obj
    json11::Json json_config_obj;
    std::string json_config_path;
    // handle
    std::shared_ptr< SDHandle > handle =
        std::shared_ptr< SDHandle >(new SDHandle(nullptr), [](SDHandle* p) {
            SenseDriverDestroy(p);
            delete p;
        });
    // image
    cv::Mat mat_first;
    cv::Mat mat_second;
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

std::string Distraction2Str(const MDistractionLevel& level) {
    switch (level) {
        case MDistractionLevel::M_DISTRACTION_LEVEL_NONE:
            return "Distraction: Focus";
        case MDistractionLevel::M_DISTRACTION_LEVEL_LIGHT:
            return "Distraction: Light";
        case MDistractionLevel::M_DISTRACTION_LEVEL_MEDIUM:
            return "Distraction: Medium";
        case MDistractionLevel::M_DISTRACTION_LEVEL_HEAVY:
            return "Distraction: Heavy";
        default:
            return "Distraction: Invalid";
    }
}

int ParseDistraction(DistractionCusConfig& para) {
    if (0 != ParseFromFilename(para.json_config_path, para.json_config_obj)) {
        return -1;
    }

    const std::string image_first = "image_first";
    const std::string image_second = "image_second";
    const std::string sdk_config_path = "sdk_config_path";
    const std::string calibration_result_file = "calibration_result_file";
    const std::string calibration_run_frames = "calibration_run_frames";

    const std::vector< std::string > config_keys = {image_first,
                                                    image_second,
                                                    sdk_config_path,
                                                    calibration_result_file,
                                                    calibration_run_frames};

    if (0 != CheckJsonKeyExistence(para.json_config_obj, config_keys)) {
        return -1;
    }
    // get value
    para.image_first = para.json_config_obj[image_first].string_value();
    para.image_second = para.json_config_obj[image_second].string_value();
    para.config_path = para.json_config_obj[sdk_config_path].string_value();
    para.calibration_result_file = para.json_config_obj[calibration_result_file].string_value();
    para.calibration_run_frames = para.json_config_obj[calibration_run_frames].int_value();
    return 0;
}

int Init(DistractionCusConfig& para) {
    // parse distraction
    if (0 != ParseDistraction(para)) {
        std::cerr << "parse distraction error" << std::endl;
        return -1;
    }

    // init
    auto ret = SenseDriverInit(
        para.config_path.c_str(), static_cast< int >(para.config_path.length()), para.handle.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error" << std::endl;
        return -1;
    }

    // load image
    para.mat_first = cv::imread(para.image_first);
    para.mat_second = cv::imread(para.image_second);
    if (para.mat_first.empty() || para.mat_second.empty()) {
        std::cout << "load image failed." << std::endl;
        return -1;
    }

    return 0;
}

int RunDistraction(DistractionCusConfig& para) {
    /* ---- ------------------------------
        The process is as follows:
        step 1. Run distraction to start calibration on first image, and check distraction
    calibration status and distraction level, until calibration state is done. Get calibration
    parameters and save to local file

        step 2. Run distraction on second image, and check distraction level, until distraction
    level is heavy

        step 3. Restart distraction calibration, run on second image, and check distraction
    calibration status, until calibration state is done

        step 4. Load calibration result from local file, and set it to handle. Run distraction on
    second image, until distraction level is heavy

        step 5. Run distration on first image, and check distraction level, until distraction level
    will be none
    ---- ------------------------------ */

    // unload all other modules
    SenseDriverModuleBitMask bitmask{0U};
    auto ret = SenseDriverGetEnableModules(*para.handle, &bitmask);
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverGetEnableModules failed" << std::endl;
        return -1;
    }
    ret = SenseDriverLoadModule(*para.handle, ~(bitmask), false);
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverLoadModule failed" << std::endl;
        return -1;
    }

    const int all_frames = para.calibration_run_frames + 300 + para.calibration_run_frames + 600;

    uint32_t step_n = 1; // 1->2->3->4
    for (int i = 0; i < all_frames; ++i) {
        std::cout << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        cv::Mat image;
        if (1 == step_n || 5 == step_n) {
            image = para.mat_first;
        } else {
            image = para.mat_second;
        }

        // get time stamp
        MTimeStamp timestamp;
        GetTimeStamp(timestamp);

#if CONFIG_AD_META_NEURO_PILOT_VERSION_5 == 1
        auto& mat = image;
        auto origin_h = mat.rows;
        auto origin_w = mat.cols;
        cv::cvtColor(mat, mat, cv::COLOR_BGR2YUV_I420);
        YU12_to_YUV420SP(mat.data, origin_h * origin_w, false);
        auto image_handle = CreateSingleImageHandle(
            mat.data, nullptr, timestamp, M_PIX_FMT_NV21, origin_w, origin_h, MEM_ON_CPU);
#else

        // create image handle
        auto image_handle = CreateSingleImageHandle(image.data,
                                                    nullptr,
                                                    timestamp,
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

        // run distraction
        ret = SenseDriverRun(*para.handle, image_handle, result.get());
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
        // check distractin level
        MDistractionLevel level = MDistractionLevel::M_DISTRACTION_LEVEL_INVALID;
        if (result->person_number > 0) {
            const auto& person = result->person_array_result[0];
            const auto& face_result = person.face_result;
            if (MResultValid::RESULT_INVALID == face_result.result_valid) {
                std::cout << "No face or face quality is low." << std::endl;
            } else {
                // distraciton result
                const auto& distraction_result = face_result.distraction_result;
                if (MResultValid::RESULT_VALID == distraction_result.result_valid) {
                    level = distraction_result.level;
                    std::cout << Distraction2Str(distraction_result.level) << std::endl;
                } else {
                    std::cout << "Not support distraction module" << std::endl;
                }
            }
        }
        std::cout << "step " << step_n << ": " << i << std::endl;
        // when in step1, once calibration is done, save to local file
        if (1 == step_n) {
            if (M_DISTRACTION_CALIBRATION_DONE == calibration_state) {
                step_n = 2;
                std::cout << "step1 successully" << std::endl;

                std::ofstream fout(para.calibration_result_file, std::ios::binary);
                if (!fout) {
                    std::cerr << "open file failed: " << para.calibration_result_file << std::endl;
                    return -1;
                }
                // get calibration result
                MDistractionCalibrationParam calibration_param;
                auto ret = SenseDriverGetCalibrationParams(*para.handle, &calibration_param);
                if (MDMS_SUCCESS != ret) {
                    std::cerr << "SenseDriverGetCalibrationParams failed" << std::endl;
                    return -1;
                }
                // save calibration result to local file(todo: now only save headpose)
                fout << calibration_param.head_pose.base_head_pose.yaw << "  "
                     << calibration_param.head_pose.base_head_pose.pitch << "  "
                     << calibration_param.head_pose.base_head_pose.roll << std::endl;
                fout.close();
                std::cout << "**** Save calibration result success! **** " << std::endl;
                std::cout << "yaw, pitch, roll: " << calibration_param.head_pose.base_head_pose.yaw
                          << ", " << calibration_param.head_pose.base_head_pose.pitch << ", "
                          << calibration_param.head_pose.base_head_pose.roll << std::endl;
            }
        }

        if (2 == step_n) {
            if (M_DISTRACTION_LEVEL_HEAVY == level) {
                step_n = 3;
                std::cout << "step2 successully" << std::endl;
                auto ret = SenseDriverRestartCalibration(*para.handle);
                if (MDMS_SUCCESS != ret) {
                    std::cerr << "SenseDriverRestartCalibration failed." << std::endl;
                    return -1;
                }
                calibration_state = M_DISTRACTION_CALIBRATION_INVALID;
                std::cout << "**** Restart calibration success! **** " << std::endl;
            }
        }
        // when in step3, restart calibration
        if (3 == step_n) {
            if (M_DISTRACTION_CALIBRATION_DONE == calibration_state) {
                step_n = 4;
                std::cout << "step3 successully" << std::endl;
                // load calibration result
                MDistractionCalibrationParam calibration_param;
                // open file
                std::ifstream fin(para.calibration_result_file, std::ios::binary);
                if (!fin) {
                    std::cerr << "open file failed: " << para.calibration_result_file << std::endl;
                    return -1;
                }
                std::string line;
                std::getline(fin, line);
                std::stringstream ss(line);
                ss >> calibration_param.head_pose.base_head_pose.yaw;
                ss >> calibration_param.head_pose.base_head_pose.pitch;
                ss >> calibration_param.head_pose.base_head_pose.roll;
                fin.close();

                // set calibration result
                auto ret = SenseDriverSetCalibrationParams(*para.handle, &calibration_param);
                if (MDMS_SUCCESS != ret) {
                    std::cerr << "SenseDriverSetCalibrationParams failed." << std::endl;
                    return -1;
                }
                level = M_DISTRACTION_LEVEL_INVALID;
                std::cout << "**** Load calibration result success! **** " << std::endl;
                std::cout << "yaw, pitch, roll: " << calibration_param.head_pose.base_head_pose.yaw
                          << ", " << calibration_param.head_pose.base_head_pose.pitch << ", "
                          << calibration_param.head_pose.base_head_pose.roll << std::endl;
            }
        }

        // when in step4, load calibration result
        if (4 == step_n) {
            if (M_DISTRACTION_LEVEL_HEAVY == level) {
                std::cout << "step4 successully" << std::endl;
                step_n = 5;
            }
        }

        if (5 == step_n) {
            if (M_DISTRACTION_LEVEL_NONE == level) {
                std::cout << "step5 successully" << std::endl;
                break;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        // use duration time to control frame rate
        long duration =
            std::chrono::duration_cast< std::chrono::microseconds >(end - start).count();

        if (duration < (1000000 / 30)) {
            std::this_thread::sleep_for(std::chrono::microseconds((1000000 / 30) - duration));
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << "  <json_config_path>" << std::endl;
        return -1;
    }

    DistractionCusConfig para;
    para.json_config_path = argv[1];

    // init
    if (0 != Init(para)) {
        return -1;
    }
    // run
    RunDistraction(para);

    return 0;
}