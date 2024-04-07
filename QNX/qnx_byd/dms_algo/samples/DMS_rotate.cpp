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
    std::string Drowsiness2Str(const MDrowsinessLevel& status) {
        switch (status) {
            case MDrowsinessLevel::M_DROWSINESS_LEVEL_NONE:
                return "Focus";
            case MDrowsinessLevel::M_DROWSINESS_LEVEL_LIGHT:
                return "Light";
            case MDrowsinessLevel::M_DROWSINESS_LEVEL_MEDIUM:
                return "Medium";
            case MDrowsinessLevel::M_DROWSINESS_LEVEL_HEAVY:
                return "Heavy";
            default:
                return "Invalid";
        }
    }

    std::string Action2Str(const MProductActionType& level) {
        switch (level) {
            case MProductActionType::M_PRODUCT_ACTION_NONE:
                return "None";
            case MProductActionType::M_PRODUCT_ACTION_CALL:
                return "Call";
            case MProductActionType::M_PRODUCT_ACTION_DRINK:
                return "Drink";
            case MProductActionType::M_PRODUCT_ACTION_SMOKE:
                return "Smoke";
            default:
                return "Invalid";
        }
    }

    std::string DriverMissing2Str(const MProductDriverMissingResult& status) {
        switch (status) {
            case MProductDriverMissingResult::M_DRIVER_MISSING:
                return "Missing";
            default:
                return "Not_Missing";
        }
    }

    std::string CameraOcclusion2Str(const MProductCameraOcclusionResult& status) {
        switch (status) {
            case MProductCameraOcclusionResult::M_CAMERA_OCCLUSION:
                return "Occlusion";
            default:
                return "Not_Occlusion";
        }
    }

    std::string Emotion2Str(const MEmotion& emotion) {
        switch (emotion) {
            case MEmotion::ANGRY:
                return "Angry";
            case MEmotion::CALM:
                return "Calm";
            case MEmotion::DISGUST:
                return "Disgust";
            case MEmotion::HAPPY:
                return "Happy";
            case MEmotion::SAD:
                return "Sad";
            case MEmotion::SCARED:
                return "Scared";
            case MEmotion::SURPRISE:
                return "Surprise";
            case MEmotion::UNCLEAR:
                return "Unclear";
            default:
                return "Invalid";
        }
    }

    std::string Gender2Str(const MGender& gender) {
        switch (gender) {
            case MGender::MALE:
                return "Male";
            case MGender::FEMALE:
                return "Female";
            default:
                return "Invalid";
        }
    }

    std::string Mask2Str(const MFaceAttrMask& mask) {
        switch (mask) {
            case MFaceAttrMask::FACE_ATTR_MASK_NONE:
                return "None";
            case MFaceAttrMask::FACE_ATTR_MASK_EXIST:
                return "Exist";
            default:
                return "Invalid";
        }
    }

    std::string Glasses2Str(const MFaceAttrGlasses& glasses) {
        switch (glasses) {
            case MFaceAttrGlasses::FACE_ATTR_GLASSES_NONE:
                return "None";
            case MFaceAttrGlasses::FACE_ATTR_GLASSES_GENERAL:
                return "General";
            case MFaceAttrGlasses::FACE_ATTR_GLASSES_SUNGLASSES:
                return "Sunglasses";
            default:
                return "Invalid";
        }
    }

    std::string EyeStatus2Str(MEyeStatus eye) {
        std::string str;
        switch (eye) {
            case MEyeStatus::EYE_OPEN:
                str = "Open";
                break;
            case MEyeStatus::EYE_CLOSE:
                str = "Close";
                break;
            default:
                str = "Invalid";
        }
        return str;
    }

    std::string MouthStatus2Str(MMouthStatus mouth) {
        std::string str;
        switch (mouth) {
            case MMouthStatus::MOUTH_OPEN:
                str = "Open";
                break;
            case MMouthStatus::MOUTH_CLOSE:
                str = "Close";
                break;
            default:
                str = "Invalid";
        }
        return str;
    }

    void PrintDMSResult(const SDResult& result) {
        // driver missing status
        const auto& driver_miss_result = result.driver_missing_result;
        if (MResultValid::RESULT_VALID == driver_miss_result.result_valid) {
            std::cout << "|DRIVERMISSING:" << DriverMissing2Str(driver_miss_result.result);
        } else {
            std::cout << "|DRIVERMISSING:Invalid";
        }

        // camera occlusion status
        const auto& camera_occlusion_result = result.camera_occlusion_result;
        if (MResultValid::RESULT_VALID == camera_occlusion_result.result_valid) {
            std::cout << "|CAMERAOCCLUSION:" << CameraOcclusion2Str(camera_occlusion_result.result);
        } else {
            std::cout << "|CAMERAOCCLUSION:Invalid";
        }

        if (result.person_number <= 0) {
            return;
        }

        // person result:
        const auto& person = result.person_array_result[0];

        // person result ----> action result
        const auto& action_result = person.action_result;
        if (MResultValid::RESULT_VALID == action_result.result_valid) {
            std::cout << "|ACTION_TYPE:" << Action2Str(action_result.type);
        } else {
            std::cout << "|ACTION_TYPE:Invalid";
        }

        // person result ----> face result
        const auto& face_result = person.face_result;
        if (MResultValid::RESULT_INVALID == face_result.result_valid) {
            std::cout << "|No face or face quality is low" << std::endl;
        } else {
            // face rect
            std::cout << "|FACE_RECT:"
                      << "[" << face_result.face_rect.left << "," << face_result.face_rect.top
                      << "][" << face_result.face_rect.right << "," << face_result.face_rect.bottom
                      << "]";

            const auto& face_align_result = person.face_result.face_align_result;
            std::cout << ", |FACE_HEADPOSE: " << face_align_result.head_pose.yaw << ", "
                      << face_align_result.head_pose.pitch << ", "
                      << face_align_result.head_pose.roll << std::endl;
            // landmark
            if (face_align_result.landmark) {
                std::cout << "|LAND_MARK: ";
                for (int i = 0; i < face_align_result.landmark_size; ++i) {
                    if (i > 0) {
                        std::cout << ", ";
                    }
                    std::cout << "[x: " << face_align_result.landmark[i].x
                              << ", y: " << face_align_result.landmark[i].y << "]";
                }
                std::cout << std::endl;
            }

            // drowsiness result
            const auto& drowsiness_result = face_result.drowsiness_result;
            if (MResultValid::RESULT_VALID == drowsiness_result.result_valid) {
                std::cout << "|DROWSINESS_LEVEL: " << Drowsiness2Str(drowsiness_result.level);
                std::cout << ", |EYE_LEFT_RT: " << EyeStatus2Str(drowsiness_result.eye_left_rt);
                std::cout << ", |EYE_RIGHT_RT: " << EyeStatus2Str(drowsiness_result.eye_right_rt);
                std::cout << ", |EYE_LEFT_SM: " << EyeStatus2Str(drowsiness_result.eye_left_sm);
                std::cout << ", |EYE_RIGHT_SM: " << EyeStatus2Str(drowsiness_result.eye_right_sm);
                std::cout << ", |MOUTH_RT: " << MouthStatus2Str(drowsiness_result.mouth_rt);
                std::cout << ", |MOUTH_SM: " << MouthStatus2Str(drowsiness_result.mouth_sm)
                          << std::endl;
            } else {
                std::cout << "|DROWSINESS_LEVEL: Invalid" << std::endl;
            }

            // emotion result
            const auto& emotion_result = face_result.emotion_result;
            if (MEmotion::EMOTION_INVALID != emotion_result.emotion) {
                std::cout << "|EMOTION:" << Emotion2Str(emotion_result.emotion);
            } else {
                std::cout << "|EMOTION: Invalid";
            }

            // gender result
            const auto& gender_result = face_result.attr_result.gender_result;
            if (MResultValid::RESULT_VALID == gender_result.valid) {
                std::cout << ", |GENDER: " << Gender2Str(gender_result.gender);
            } else {
                std::cout << ", |GENDER: Invalid";
            }
            // age result
            auto& age = face_result.attr_result.age_result;
            if (age >= 0) {
                std::cout << ", |AGE: " << std::to_string(age);
            } else {
                std::cout << ", |AGE: Invalid";
            }
            const auto& mask = face_result.attr_result.mask;
            if (mask.valid == MResultValid::RESULT_VALID) {
                std::cout << ", |MASK: " << Mask2Str(mask.mask);
            } else {
                std::cout << ", |MASK: Invalid";
            }
            const auto& glasses = face_result.attr_result.glasses;
            if (glasses.valid == MResultValid::RESULT_VALID) {
                std::cout << ", |GLASSES: " << Glasses2Str(glasses.glasses) << std::endl;
            } else {
                std::cout << ", |GLASSES: Invalid" << std::endl;
            }
        }
    }
};

int Init(DMSConfig& para) {
    // init
    auto ret =
        SenseDriverInit(para.config_path.c_str(), para.config_path.length(), para.handle.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error" << std::endl;
        return -1;
    }
    // unload face id module
    SenseDriverModuleBitMask bitmask{0U};
    // ret = SenseDriverGetEnableModules(*para.handle, &bitmask);
    // if (MDMS_SUCCESS != ret) {
    //     std::cerr << "SenseDriverGetEnableModules error" << std::endl;
    //     return -1;
    // }
    // ret = SenseDriverLoadModule(*para.handle, bitmask & SENSEDRIVER_MODULE_VERIFY, false);
    // if (MDMS_SUCCESS != ret) {
    //     std::cerr << "SenseDriverLoadModule error" << std::endl;
    //     return -1;
    // }
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
    cv::Mat image_src;

    // video frame index
    int frame_index = 0;
    MTimeStamp cur_time_stamp;
    GetTimeStamp(cur_time_stamp);

    while (true) {
        // get image
        video_capture >> image_src;
        if (image_src.empty()) {
            std::cout << "Read video finished" << std::endl;
            return 0;
        }
        // rotation for GDC
        cv::transpose(image_src, image);
        cv::flip(image, image, 1); // rotate 270

        UpdateTimeStampByFPS(para.fps, cur_time_stamp);
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

        std::cout << "Frame: " << frame_index << std::endl;

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