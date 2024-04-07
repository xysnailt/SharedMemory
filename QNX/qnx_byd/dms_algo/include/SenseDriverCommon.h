#ifndef _MATRIX_SDK_SENSEDRIVER_COMMON_H_
#define _MATRIX_SDK_SENSEDRIVER_COMMON_H_

#include "MImageHelper.h"
#include "matrixsdk_common.h"

#include <string>

#ifdef __cplusplus
extern "C" {
#endif
/** A enum to represent drowsiness status */
typedef enum MDrowsinessLevel {
    M_DROWSINESS_LEVEL_NONE = 0,         /** not drowsy */
    M_DROWSINESS_LEVEL_LIGHT = 1,        /** driver is drowsy in light level */
    M_DROWSINESS_LEVEL_MEDIUM = 2,       /** driver is drowsy in medium level */
    M_DROWSINESS_LEVEL_HEAVY = 3,        /** driver is drowsy in heavy  level*/
    M_DROWSINESS_LEVEL_MICRO_SLEEP = 4,  /** driver is drowsy in micor sleep level*/
    M_DROWSINESS_LEVEL_SLEEP = 5,        /** driver is drowsy in sleep level*/
    M_DROWSINESS_LEVEL_UNRESPONSIVE = 6, /** driver is drowsy in unresponsive level*/
    M_DROWSINESS_LEVEL_INVALID = 7       /** can not judge if driver is drowsy */
} MDrowsinessLevel;

typedef enum MDistractionLevel {
    M_DISTRACTION_LEVEL_NONE = 0,         /** driver is not distracted*/
    M_DISTRACTION_LEVEL_LIGHT = 1,        /** driver is not distracted in light level*/
    M_DISTRACTION_LEVEL_MEDIUM = 2,       /** driver is not distracted in medium level*/
    M_DISTRACTION_LEVEL_HEAVY = 3,        /** driver is not distracted in heavy level*/
    M_DISTRACTION_LEVEL_UNRESPONSIVE = 4, /** driver is not distracted in unresponsive level*/
    M_DISTRACTION_LEVEL_INVALID = 5       /** can not judge if driver is distracted*/
} MDistractionLevel;

typedef enum MDistractionCalBase {
    M_DISTRACTION_CAL_BASE_HEAD_POSE,    /**base headpose area*/
    M_DISTRACTION_CAL_BASE_GAZE3D_POINT, /**base head3d.gaze_point area*/
} MDistractionCalBase;

typedef enum MDistractionCalibrationState {
    M_DISTRACTION_CALIBRATION_UNCALIBRATED =
        0, /** uncalibration, user need to call the StartCalibration to start calibration*/
    M_DISTRACTION_CALIBRATION_CALIBRATING = 1, /** distraction is calibrating*/
    M_DISTRACTION_CALIBRATION_DONE = 2,        /** calibration done.*/
    M_DISTRACTION_CALIBRATION_INVALID = 3,     /** calibration is unnecessary*/
} MDistractionCalibrationState;

typedef struct MHeadPoseCalibrationParam {
    int is_valid;
    MOritation base_head_pose;
} MHeadPoseCalibrationParam;

typedef struct MGazeVectorCalibrationParam {
    int is_valid;
    MPoint3f base_gaze_vector;
} MGazeVectorCalibrationParam;

typedef struct MDistractionCalibrationParam {
    MHeadPoseCalibrationParam head_pose;
    MGazeVectorCalibrationParam gaze_vector;
} MDistractionCalibrationParam;

typedef enum MFaceIDStatus {
    M_FACEID_OK,      // faceid success to recognize
    M_FACEID_WAITING, // login or register is processing, please wait and continue to input image
    M_FACEID_NO_FACE, // no face in image
    M_FACEID_TRACK_SCORE_LOW,   // face track socre is too low in image
    M_FACEID_TOO_BRIGHT,        // the image of face is too bright
    M_FACEID_TOO_DARK,          // the image of face is too dark
    M_FACEID_BLURRED,           // the image of face is too blurred
    M_FACEID_FACE_TOO_SMALL,    // the face is too small
    M_FACEID_YAW_TOO_BIG,       // the yaw is too big
    M_FACEID_PITCH_TOO_BIG,     // the pitch is too big
    M_FACEID_NON_LIVING,        // the face in image is non-living
    M_FACEID_NOT_FOUND,         // login not found faceid
    M_FACEID_OCCLUSION,         // the face is blocked
    M_FACEID_LANDMARK_OUTBOUND, // face landmark is out of image
    M_FACEID_INVALID            // faceid status result is invalid
} MFaceIDStatus;

typedef struct ADImage* ADImageHandle;

/** A enum to represent action type */
typedef enum MProductActionType {
    M_PRODUCT_ACTION_NONE = 0,   /** driver focus on driving */
    M_PRODUCT_ACTION_SMOKE = 1,  /** driver is smoking */
    M_PRODUCT_ACTION_DRINK = 2,  /** driver is drinking */
    M_PRODUCT_ACTION_CALL = 3,   /** driver is calling */
    M_PRODUCT_ACTION_INVALID = 4 /** action could not work well */
} MProductActionType;

/** A enum to represent camera occlusion status */
typedef enum MProductCameraOcclusionResult {
    M_CAMERA_OCCLUSION = 0,    /** camera occlusion */
    M_CAMERA_NOT_OCCLUSION = 1 /** camera not occlusion */
} MProductCameraOcclusionResult;

/** A enum to represent driver missing status */
typedef enum MProductDriverMissingResult {
    M_DRIVER_MISSING = 0,    /** driver missing */
    M_DRIVER_NOT_MISSING = 1 /** driver not missing */
} MProductDriverMissingResult;

typedef struct MFaceIDQueryResult {
    MFaceIDStatus query_status; // the faceId status of query the unkown person in gallery
    unsigned int result_idx;    // query result index of face in gallery
} MFaceIDQueryResult;

typedef enum MFacePipelineStrategy {
    PIPELINE_STRATEGY_DETECT = 1,        //!< DETECT Strategy
    PIPELINE_STRATEGY_TRACK = 2,         //!< TRACK Strategy
    PIPELINE_STRATEGY_AIO_THEN_TRACK = 3 //!< AIO + TRACK Strategy
} MFacePipelineStrategy;

typedef enum MActionPipelineStrategy {
    M_ACTION_PIPELINE_STRATEGY_DETECT = 1,           //!< DETECT Strategy
    M_ACTION_PIPELINE_STRATEGY_DETECT_TWO_STAGE = 2, //!< DETECT WITH TWO STAGE Strategy
    M_ACTION_PIPELINE_STRATEGY_TRACK = 3             //!< TRACK Strategy
} MActionPipelineStrategy;

typedef enum class MDistAlarmType {
    TDistNone,
    TDistLong,
    TDistShort,
    TDistUnresponsive,
    TDistInvalid
} MDistAlarmType;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
