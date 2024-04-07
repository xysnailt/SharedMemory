#ifndef DMSRESULTPRINTER_H
#define DMSRESULTPRINTER_H

#include "sensedriver_interface.h"

#include <iostream>

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

    std::string Hat2Str(const MFaceAttrHat& hat) {
        switch (hat) {
            case MFaceAttrHat::FACE_ATTR_HAT_EXIST:
                return "Exist";
                break;
            case MFaceAttrHat::FACE_ATTR_HAT_NONE:
                return "None";
                break;
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

    std::string AOI2Str(GazeAoi const& aoi) {
        switch (aoi) {
            case GazeAoi::GAZEAOI_FRONT_WINDOWSHILED:
                return "FRONT_WINDOWSHILED";
            case GazeAoi::GAZEAOI_SIDE_FRONT_WINDSHIELD:
                return "SIDE_FRONT_WINDSHIELD";
            case GazeAoi::GAZEAOI_SPEED_INDICATOR:
                return "SPEED_INDICATOR";
            case GazeAoi::GAZEAOI_REAR_MIRROR:
                return "REAR_MIRROR";
            case GazeAoi::GAZEAOI_NAVI_DISPLAY:
                return "NAVI_DISPLAY";
            case GazeAoi::GAZEAOI_LEFT_MIRROR:
                return "LEFT_MIRROR";
            case GazeAoi::GAZEAOI_RIGHT_MIRROR:
                return "RIGHT_MIRROR";
            case GazeAoi::GAZEAOI_SUNVISOR:
                return "SUNVISOR";
            case GazeAoi::GAZEAOI_GEAR_SHIFT:
                return "GEAR_SHIFT";
            case GazeAoi::GAZEAOI_SMART_PHONE:
                return "SMART_PHONE";
            case GazeAoi::GAZEAOI_PASSENGER_SEAT:
                return "PASSENGER_SEAT";
            case GazeAoi::GAZEAOI_INVALID:
            default:
                return "INVALID";
        }
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
                      << face_align_result.head_pose.roll;
            // landmark
            // if (face_align_result.landmark) {
            //     std::cout << "|LAND_MARK: ";
            //     for (int i = 0; i < face_align_result.landmark_size; ++i) {
            //         if (i > 0) {
            //             std::cout << ", ";
            //         }
            //         std::cout << "[x: " << face_align_result.landmark[i].x
            //                   << ", y: " << face_align_result.landmark[i].y << "]";
            //     }
            //     std::cout << std::endl;
            // }

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

            // distraciton result
            const auto& distraction_result = face_result.distraction_result;
            if (MResultValid::RESULT_VALID == distraction_result.result_valid) {
                std::cout << "|DISTRACTION_LEVEL:" << Distraction2Str(distraction_result.level);
            } else {
                std::cout << "|DISTRACTION_LEVEL: Invalid";
            }

            // gazeaoi result
            const auto& gazeaoi_result = face_result.gazeaoi_result;
            if (MResultValid::RESULT_VALID == gazeaoi_result.result_valid) {
                std::cout << "|AOI:" << AOI2Str(gazeaoi_result.gazeaoi);
            } else {
                std::cout << "|AOI: Invalid";
            }
            std::cout << std::endl;
        }
    }
};

#endif