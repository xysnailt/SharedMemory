#include "ImageReader.hpp"
#include "ParseJson.hpp"
#include "sensedriver_interface.h"

struct DMSCusConfig {
    // config
    uint32_t fps = 30;
    uint32_t base_frame_count = 100;
    std::string image_list_path;
    std::string video_path;
    std::string config_path;
    // Json obj
    json11::Json json_config_obj;
    std::string json_config_path;
    // handle
    std::shared_ptr< SDHandle > handle =
        std::shared_ptr< SDHandle >(new SDHandle(nullptr), [](SDHandle* p) {
            SenseDriverDestroy(p);
            delete p;
        });
    // image reader
    std::shared_ptr< ImageReader > image_reader = nullptr;
};

class DMSProcessControl {
public:
    void SetFrameCount(const int base_frame_count) {
        this->frames_base = base_frame_count;
        this->frames_stop_drowsiness = frames_base + 30;
        this->frames_start_drowsiness = frames_stop_drowsiness + 30;
        this->frames_unload_drowsiness = frames_start_drowsiness + 30;
    }

    int SetDMSModule(DMSCusConfig& para) {
        if (!is_switch_stop_drowsiness && frames_base < count && count <= frames_stop_drowsiness) {
            // use switch function to turn off the drowsiness module
            auto ret = SenseDriverModuleSwitch(*para.handle, SENSEDRIVER_MODULE_EMO_MOUTH, false);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverModuleSwitch failed with error: "
                          << static_cast< int >(ret) << std::endl;
                return -1;
            }
            ret = SenseDriverModuleSwitch(*para.handle, SENSEDRIVER_MODULE_EYE, false);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverModuleSwitch failed with error: "
                          << static_cast< int >(ret) << std::endl;
                return -1;
            }
            is_switch_stop_drowsiness = true;
            std::cout << "Drowsiness module has been stopped!" << std::endl;
        } else if (!is_switch_restart_drowsiness && frames_stop_drowsiness < count &&
                   count <= frames_start_drowsiness) {
            // use switch function to turn on the drowsiness module
            auto ret = SenseDriverModuleSwitch(*para.handle, SENSEDRIVER_MODULE_EMO_MOUTH, true);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverModuleSwitch failed with error: "
                          << static_cast< int >(ret) << std::endl;
                return -1;
            }
            ret = SenseDriverModuleSwitch(*para.handle, SENSEDRIVER_MODULE_EYE, true);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverModuleSwitch failed with error: "
                          << static_cast< int >(ret) << std::endl;
                return -1;
            }
            is_switch_restart_drowsiness = true;
            std::cout << "Drowsiness module has been started!" << std::endl;
        } else if (!is_unload_drowsiness && frames_start_drowsiness < count &&
                   count <= frames_unload_drowsiness) {
            // use load function to unload the drowsiness module
            auto ret = SenseDriverModuleSwitch(*para.handle, SENSEDRIVER_MODULE_EMO_MOUTH, false);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverModuleSwitch failed with error: "
                          << static_cast< int >(ret) << std::endl;
                return -1;
            }
            ret = SenseDriverModuleSwitch(*para.handle, SENSEDRIVER_MODULE_EYE, false);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverModuleSwitch failed with error: "
                          << static_cast< int >(ret) << std::endl;
                return -1;
            }
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverLoadModule failed with error: " << static_cast< int >(ret)
                          << std::endl;
                return -1;
            }
            is_unload_drowsiness = true;
            std::cout << "Drowsiness module has been unloaded!" << std::endl;
        } else if (!is_reload_drowsiness && count > frames_unload_drowsiness) {
            // use load function to load the drowsiness module
            auto ret = SenseDriverModuleSwitch(*para.handle, SENSEDRIVER_MODULE_EMO_MOUTH, true);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverModuleSwitch failed with error: "
                          << static_cast< int >(ret) << std::endl;
                return -1;
            }
            ret = SenseDriverModuleSwitch(*para.handle, SENSEDRIVER_MODULE_EYE, true);
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverModuleSwitch failed with error: "
                          << static_cast< int >(ret) << std::endl;
                return -1;
            }
            if (MDMS_SUCCESS != ret) {
                std::cerr << "SenseDriverLoadModule failed with error: " << static_cast< int >(ret)
                          << std::endl;
                return -1;
            }
            is_reload_drowsiness = true;
            std::cout << "Drowsiness module has been loaded!" << std::endl;
        }
        // update count
        ++count;
        std::cout << "Frame count: " << count << std::endl;
        return 0;
    }

private:
    int frames_base = 0;
    int frames_stop_drowsiness = 0;
    int frames_start_drowsiness = 0;
    int frames_unload_drowsiness = 0;

    bool is_switch_stop_drowsiness = false;
    bool is_switch_restart_drowsiness = false;
    bool is_unload_drowsiness = false;
    bool is_reload_drowsiness = false;

    int count = 0;
};

class DMSResultPrinter {
public:
    std::string Drowsiness2Str(const MDrowsinessLevel& status) {
        switch (status) {
            case MDrowsinessLevel::M_DROWSINESS_LEVEL_NONE:
                return "Drowsiness: Focus";
            case MDrowsinessLevel::M_DROWSINESS_LEVEL_LIGHT:
                return "Drowsiness: Light";
            case MDrowsinessLevel::M_DROWSINESS_LEVEL_MEDIUM:
                return "Drowsiness: Medium";
            case MDrowsinessLevel::M_DROWSINESS_LEVEL_HEAVY:
                return "Drowsiness: Heavy";
            default:
                return "Drowsiness: Invalid";
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

    std::string Action2Str(const MProductActionType& level) {
        switch (level) {
            case MProductActionType::M_PRODUCT_ACTION_NONE:
                return "Action: None";
            case MProductActionType::M_PRODUCT_ACTION_CALL:
                return "Action: call";
            case MProductActionType::M_PRODUCT_ACTION_DRINK:
                return "Action: drink";
            case MProductActionType::M_PRODUCT_ACTION_SMOKE:
                return "Action: smoke";
            default:
                return "Action: Invalid";
        }
    }

    std::string DriverMissing2Str(const MProductDriverMissingResult& status) {
        switch (status) {
            case MProductDriverMissingResult::M_DRIVER_MISSING:
                return "Driver: Missing";
            default:
                return "Driver: Not missing";
        }
    }

    std::string CameraOcclusion2Str(const MProductCameraOcclusionResult& status) {
        switch (status) {
            case MProductCameraOcclusionResult::M_CAMERA_OCCLUSION:
                return "Camera: Occlusion";
            default:
                return "Camera: Not occlusion";
        }
    }

    void PrintDMSResult(const SDResult& result) {
        // driver missing status
        const auto& driver_miss_result = result.driver_missing_result;
        if (MResultValid::RESULT_VALID == driver_miss_result.result_valid) {
            std::cout << DriverMissing2Str(driver_miss_result.result) << std::endl;
        } else {
            std::cout << "Not support driver missing module." << std::endl;
        }

        // camera occlusion status
        const auto& camera_occlusion_result = result.camera_occlusion_result;
        if (MResultValid::RESULT_VALID == camera_occlusion_result.result_valid) {
            std::cout << CameraOcclusion2Str(camera_occlusion_result.result) << std::endl;
        } else {
            std::cout << "Not support camera occlusion module." << std::endl;
        }

        if (result.person_number <= 0) {
            return;
        }

        // person result:
        const auto& person = result.person_array_result[0];

        // person result ----> action result
        const auto& action_result = person.action_result;
        if (MResultValid::RESULT_VALID == action_result.result_valid) {
            std::cout << Action2Str(action_result.type) << std::endl;
        } else {
            std::cout << "Not support action module." << std::endl;
        }

        // person result ----> face result
        const auto& face_result = person.face_result;
        if (MResultValid::RESULT_INVALID == face_result.result_valid) {
            std::cout << "No face or face quality is low." << std::endl;
        } else {
            // face rect
            std::cout << "face rect: " << face_result.face_rect.left << ", "
                      << face_result.face_rect.top << ", " << face_result.face_rect.right << ", "
                      << face_result.face_rect.bottom << std::endl;

            // drowsiness result
            const auto& drowsiness_result = face_result.drowsiness_result;
            if (MResultValid::RESULT_VALID == drowsiness_result.result_valid) {
                std::cout << Drowsiness2Str(drowsiness_result.level) << std::endl;
            } else {
                std::cout << "Not support drowsiness module" << std::endl;
            }

            // distraciton result
            const auto& distraction_result = face_result.distraction_result;
            if (MResultValid::RESULT_VALID == distraction_result.result_valid) {
                std::cout << Distraction2Str(distraction_result.level) << std::endl;
            } else {
                std::cout << "Not support distraction module" << std::endl;
            }
        }
    }
};

int ParseDMS(DMSCusConfig& para) {
    if (0 != ParseFromFilename(para.json_config_path, para.json_config_obj)) {
        return -1;
    }

    const std::string read_image_fps = "read_image_fps";
    const std::string base_frame_count = "base_frame_count";
    const std::string image_list_path = "image_list_path";
    const std::string video_file = "video_file";
    const std::string sdk_config_path = "sdk_config_path";

    const std::vector< std::string > config_keys = {
        read_image_fps, base_frame_count, image_list_path, video_file, sdk_config_path};

    if (0 != CheckJsonKeyExistence(para.json_config_obj, config_keys)) {
        return -1;
    }
    // get value
    para.fps = para.json_config_obj[read_image_fps].int_value();
    para.base_frame_count = para.json_config_obj[base_frame_count].int_value();
    para.image_list_path = para.json_config_obj[image_list_path].string_value();
    para.video_path = para.json_config_obj[video_file].string_value();
    para.config_path = para.json_config_obj[sdk_config_path].string_value();

    return 0;
}

int Init(DMSCusConfig& para) {
    // parse dms
    if (0 != ParseDMS(para)) {
        std::cerr << "parse DMS error" << std::endl;
        return -1;
    }

    // init image reader
    para.image_reader = std::make_shared< ImageReader >(para.fps);

    // init
    auto ret = SenseDriverInit(
        para.config_path.c_str(), static_cast< int >(para.config_path.length()), para.handle.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error" << std::endl;
        return -1;
    }
    // unload face id module
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
    return 0;
}

int ReadImageThread(DMSCusConfig& para) {
    if (!para.image_list_path.empty()) {
        return para.image_reader->ReadImage(para.image_list_path, false);
    } else {
        return para.image_reader->ReadImage(para.video_path, true);
    }
}

int DMSThread(DMSCusConfig& para) {
    // The process is as follows:
    // 1. Run all modules (para.base_frame_count) frames
    // 2. Use switch function to stop drowsiness, and run other modules 30 frames
    // 3. Use switch function to restart drowsiness, and run all modules 30 frames
    // 4. Unload drowsiness module, and run other modules 30 frames
    // 5. Reload drowsiness module, and run all modules continuously
    DMSProcessControl dms_process_control;
    dms_process_control.SetFrameCount(para.base_frame_count);

    DMSResultPrinter dms_result_printer;

    while (true) {
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
        auto image_handle = CreateSingleImageHandle(mat.data,
                                                    nullptr,
                                                    image_with_timestamp.time_stamp,
                                                    M_PIX_FMT_NV21,
                                                    origin_w,
                                                    origin_h,
                                                    MEM_ON_CPU);

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

        // set dms module by frame count, this is just a sample to show the usage of switch and load
        // function.
        // user may stop/start/load/unload some modules by other reason, such as car speed/gear
        // status/doors open status
        if (0 != dms_process_control.SetDMSModule(para)) {
            std::cerr << "set dms module failed." << std::endl;
            return -1;
        }

        // run
        auto ret = SenseDriverRun(*para.handle, image_handle, result.get());
        if (MDMS_SUCCESS != ret) {
            std::cerr << "SenseDriverRun failed with error: " << static_cast< int >(ret)
                      << std::endl;
            return -1;
        }

        // print result to screen
        dms_result_printer.PrintDMSResult(*result);
    }
    return 0;
}

void Run(DMSCusConfig& para) {
    // read image and do face id
    std::thread image_read_thread(ReadImageThread, std::ref(para));
    std::thread dms_thread(DMSThread, std::ref(para));

    dms_thread.join();
    // stop image queue
    para.image_reader->is_run_.store(false);

    image_read_thread.join();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << "  <json_config_path>" << std::endl;
        return -1;
    }

    DMSCusConfig para;
    para.json_config_path = argv[1];

    // init
    if (0 != Init(para)) {
        return -1;
    }
    // run
    Run(para);

    return 0;
}