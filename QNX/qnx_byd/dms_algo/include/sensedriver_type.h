/*
 * Copyright (C) 2019 by SenseTime Group Limited. All rights reserved.
 *
 * @file matrix_type.h
 * @brief matrix basic types define
 * @version v0.1.0
 *
 */

#ifndef _CC_SENSEDRIVER_TYPE_H_
#define _CC_SENSEDRIVER_TYPE_H_

#include "MDmsStatus.h"
#include "MImageHelper.h"
#include "SenseDriverCommon.h"
#include "matrix_macro.h"

MATRIX_DECLARE_C_CODE_BEGIN

/**@name module bit mask define
 * @{
 */
// new sense_driver type
#define SENSEDRIVER_MODULE_DISABLE_ALL (0)             /**< disable all modules */
#define SENSEDRIVER_MODULE_FACEATTR (1u << 0)          /**< enable face attribute module */
#define SENSEDRIVER_MODULE_EMO_MOUTH (1u << 1)         /**< enable emotion_mouth module */
#define SENSEDRIVER_MODULE_EYE (1u << 2)               /**< enable eye module */
#define SENSEDRIVER_MODULE_EYE_FEATURE (1u << 3)       /**< enable eye_feature module */
#define SENSEDRIVER_MODULE_GAZE_AOI (1u << 4)          /**< enable gaze_aoi module */
#define SENSEDRIVER_MODULE_ACTION (1u << 5)            /**< enable action module */
#define SENSEDRIVER_MODULE_SMOKE (1u << 6)             /**< enable smoke module */
#define SENSEDRIVER_MODULE_VERIFY (1u << 7)            /**< enable verify(face_id) module */
#define SENSEDRIVER_MODULE_CAMERA_OCC (1u << 8)        /**< enable camera_occ module */
#define SENSEDRIVER_MODULE_HEAD3D (1u << 9)            /**< enable head3d module */
#define SENSEDRIVER_MODULE_WB_GESTURE (1u << 11)       /**< enable wb_gesture module */
#define SENSEDRIVER_MODULE_GESTURE_DYNAMIC (1u << 12)  /**< enable gesture_dynamic module */
#define SENSEDRIVER_MODULE_DISTRACTION (1u << 13)      /**< enable distraction module */
#define SENSEDRIVER_MODULE_DROWSINESS (1u << 14)       /**< enable drowsiness module */
#define SENSEDRIVER_MODULE_ENABLE_ALL ((1u << 15) - 1) /**< enable all modules */
                                                       /**@} */

/**
 * define bitmask type
 */
typedef unsigned int SenseDriverModuleBitMask;

/**
 * define MatrixHandle to MatrixContext's pointer
 */
typedef struct SDContext* SDHandle;

/** A struct to represent drowsiness result*/
typedef struct SDDrowsinesssResult {
    MDrowsinessLevel level;    /**< drowsiness level */
    MResultValid result_valid; /** current distraction result is valid or not */
    MEyeStatus eye_left_rt;    // 左眼实时值
    MEyeStatus eye_right_rt;   // 右眼实时值
    MEyeStatus eye_left_sm;    // 该值是左右眼状态融合并且平滑后的值
    MEyeStatus eye_right_sm;   // 该值是左右眼状态融合并且平滑后的值
    MMouthStatus mouth_rt;     // mouth 的实时值
    MMouthStatus mouth_sm;     // mouth 平滑后的值
} SDDrowsinessResult;

/** A struct to represent distraction status*/
typedef struct SDDistractionResult {
    MDistractionLevel level;   /**< distraction level */
    float score;               /**< distraction confidence */
    MResultValid result_valid; /** current distraction result is valid or not */
    MDistAlarmType dist_type;  /** current distraction type[long or short] */
} SDDistractionResult;

/** A enum to represent action status */
typedef struct SDActionResult {
    MProductActionType type;   /** the action type of driver */
    float score;               /** the confidence of action type */
    MResultValid result_valid; /** current action result is valid or not */
} SDActionResult;

/** A enum to represent gaze aoi(area of interest)*/
typedef enum GazeAoi {
    GAZEAOI_FRONT_WINDOWSHILED = 1, /** front windshield*/
    GAZEAOI_SIDE_FRONT_WINDSHIELD,  /** side_front windshield*/
    GAZEAOI_SPEED_INDICATOR,        /** speed indicator*/
    GAZEAOI_REAR_MIRROR,            /** rear mirror*/
    GAZEAOI_NAVI_DISPLAY,           /** navi display*/
    GAZEAOI_LEFT_MIRROR,            /** left mirror*/
    GAZEAOI_RIGHT_MIRROR,           /** right mirror*/
    GAZEAOI_SUNVISOR,               /** sunvisor*/
    GAZEAOI_GEAR_SHIFT,             /** gear shift*/
    GAZEAOI_SMART_PHONE,            /** smart phone*/
    GAZEAOI_PASSENGER_SEAT,         /** passenger sea*/
    GAZEAOI_INVALID = 1000,         /** invalid aoi*/
} GazeAoi;

/** A struct to represent gaze area result*/
typedef struct SDGazeAoiResult {
    GazeAoi gazeaoi;           /**< Gaze Area */
    MResultValid result_valid; /** current gazeaoi result is valid or not */
} GazeAoiResult;

/** A struct to represent face attribute result*/
typedef struct SDFaceAttrResult {
    MGenderResult gender_result;    /** the gender result of person */
    float age_result;               /** the age result of person, age<0 means result is invalid */
    MFaceAttrMaskResult mask;       /** the mask result of person */
    MFaceAttrGlassesResult glasses; /** the glasses result of person */
    MFaceAttrHatResult has_hat;     /** the hat result of person */
} SDFaceAttrResult;

/** A enum to represent drivermissing status */
typedef struct SDDriverMissingResult {
    MProductDriverMissingResult result; /** the drivermissing result */
    MResultValid result_valid;          /** current drivermissing result is valid or not */
} SDDriverMissingResult;

/** A enum to represent cameraocclusion status */
typedef struct SDCameraOcclusionResult {
    MProductCameraOcclusionResult result; /** the cameraocclusion result */
    MResultValid result_valid;            /** current cameraocclusion result is valid or not */
} SDCameraOcclusionResult;

/** A struct to represent face align result*/
typedef struct SDFaceAlignResult {
    MOritation head_pose;
    bool headpose_abnormal;
    float headpose_abnormal_score;
    Point2f* landmark;
    int landmark_size;
} SDFaceAlignResult;

/** A struct to represent face result*/
typedef struct SDFaceResult {
    MResultValid result_valid;              /** the face result is valid or not */
    int face_id;                            /** the face id of person*/
    MRect face_rect;                        /** the rectangle of person's face */
    SDFaceAttrResult attr_result;           /** the face attr result of person*/
    MEmotionResult emotion_result;          /** the emotion result of person */
    SDDrowsinesssResult drowsiness_result;  /** the drowsiness result of person */
    SDDistractionResult distraction_result; /** the distraction result of person */
    GazeAoiResult gazeaoi_result;           /** the area result where person gaze in */
    SDFaceAlignResult face_align_result;    /** the face align result of person */
} SDFaceResult;

/** A struct to represent one person result*/
typedef struct SDPersonResult {
    SDFaceResult face_result;     /** the face result of person*/
    SDActionResult action_result; /** the action result of person */
} SDPersonResult;

typedef enum SDHandGesture {
    GESTURE_NONE = 0,
    GESTURE_BASE,
    GESTURE_OTHERS,       /* meaning less or not support*/
    GESTURE_OK,           /* OK: */
    GESTURE_V,            /* V: forefinger and middle finger stretched out upwards*/
    GESTURE_THUMB,        /* Thumbs Up: only thumb stretched out upwards*/
    GESTURE_FIVE,         /* FIVE: five fingers stretched out*/
    GESTURE_GUN,          /* GUN: thumb and forefinger stretched out*/
    GESTURE_FIST,         /* FIST: hand clenched*/
    GESTURE_FOREFINGER,   /* FOREFINGER: forefinger stretched upwards with othersclenched*/
    GESTURE_SIX,          /* SIX: thumb and little finger stretched out with others clenched*/
    GESTURE_THUMB_V,      /* THUMB V: thumb, forefinger and middle finger stretched out*/
    GESTURE_HEART,        /* Heart: thumb and forefinger crossed, the gap facing upwards */
    GESTURE_SILENCE,      /* KEEP SILENCE: forefinger stretched upwards only in front of mouth*/
    GESTURE_MIDDLEFINGER, /* MIDDLEFINGER: middlefinger stretched out with others clenched*/
    GESTURE_RINGFINGER,   /* RINGFINGER: ringfinger stretched out with others clenched*/
    GESTURE_THREE,        /* THREE: forefinger middlefinger ringfinger stretched out*/
    GESTURE_FOUR,         /* FOUR: finger streched out except for thumb*/
    GESTURE_THUMB_DOWN,   /* THUMB_DOWN: thumb streched out and turn down*/
    GESTURE_THUMB_LEFT,   /* THUMB_LEFT: thumb streched out and turn left*/
    GESTURE_THUMB_RIGHT,  /* THUMB_RIGHT: thumb streched out and turn right*/
    GESTURE_PALM,         /* PALM: palm forward or palm back*/
    GESTURE_SIX_DOWN,
    GESTURE_UNION_WIND,
    GESTURE_UNION_RAIN,
    GESTURE_UNION_THUNDER,
    GESTURE_UNION_ELECTRICITY
} SDHandGesture;

typedef enum SDHandTag {
    SC_HAND_INVALID = -1,
    SC_HAND_LEFT = 0,
    SC_HAND_RIGHT,
} SDHandTag;

typedef enum SDSeatPosition {
    SCSEAT_POSITION_INVALID = -1,
    SCSEAT_POSITION_DRIVER = 0, /** the seat where cabin sit*/
    SCSEAT_POSITION_CODRIVER,
    SCSEAT_POSITION_LEFTREAR,
    SCSEAT_POSITION_RIGHTREAR,
    SCSEAT_POSITION_MIDREAR,
} SDSeatPosition;

/** A enum to represent ghand result*/
typedef struct SDHandResult {
    MResultValid result_valid{MResultValid::RESULT_INVALID}; /** whether result is valid*/
    SDHandGesture hand_gesture;                              /** hand gesture*/
    MRect rect;                                              /** hand rect*/
    float score;                                             /** hand's detection coef*/
    int32_t id;
    SDSeatPosition seat_pos;
    SDHandTag hand_tag;
} SDHandResult;

typedef struct SDHandUnionResult {
    MResultValid result_valid{MResultValid::RESULT_INVALID}; /** whether result is valid*/
    SDHandGesture hand_gesture;                              /** hand gesture*/
    MRect rect_base;
    MRect rect_rule; /** hand rect*/
    float score;     /** hand's detection coef*/
    int32_t id;      /** person id*/
    SDSeatPosition seat_pos;
    SDHandTag hand_tag;
} SDHandUnionResult;

typedef struct SDHandResultDynamic {
    MResultValid result_valid{MResultValid::RESULT_INVALID}; /** whether result is valid*/
    GestureDynActType hand_gesture;                          /** hand gesture*/
    MRect rect;                                              /** hand rect*/
    float score;                                             /** hand's detection coef*/
    SDSeatPosition seat_pos;
    SDHandTag hand_tag;
} SDHandResultDynamic;

/** A struct to represent result of person_number persons */
typedef struct SDResult {
    SDPersonResult* person_array_result;                     /** the person's array results */
    int person_number;                                       /** person's number in car */
    SDDriverMissingResult driver_missing_result;             /** driver missing result */
    SDCameraOcclusionResult camera_occlusion_result;         /** camera occlusion result */
    SDHandResult* hand_array_result{nullptr};                /** array of hand result*/
    int hand_number{0};                                      /** number of static hands*/
    SDHandUnionResult* hand_union_array_result{nullptr};     /** array of gesture union reuslt*/
    int hand_union_number{0};                                /** number of union hands*/
    SDHandResultDynamic* hand_dynamic_array_result{nullptr}; /** array of hand result*/
    int hand_dynamic_number{0};                              /** number of dynamic hands*/
} SDResult;

/** A struct to represent generate result of FaceID*/
typedef struct SDFaceIDResult {
    MFaceIDStatus status_code;
    char* feature_str;
} SDFaceIDResult;

/************** vehicle signals define begin **************/
typedef enum SDOptionalValid {
    OPTIONAL_INVALID, /**< the optional is invalid */
    OPTIONAL_VALID    /**< the optional is valid */
} SDOptionalValid;

typedef enum SDWorkMode {
    STANDARD_MODE, /**< the standard mode */
    FACTORY_MODE,  /**< the factory mode */
    WORK_MODE_INVALID
} SDWorkMode;

typedef enum SDDoorSignal {
    DOOR_CLOSE, /**< close door signal */
    DOOR_OPEN   /**< open door signal */
} SDDoorSignal;

typedef enum SDTurnSignal {
    TURN_OFF,  /**< turn OFF signal */
    TURN_LEFT, /**< turn LEFT signal */
    TURN_RIGHT /**< turn RIGHT signal */
} SDTurnSignal;

typedef enum SDAccSignal {
    ACC_OFF, /**< acc off signal */
    ACC_ON   /**< acc on signal */
} SDAccSignal;

/** A struct to represent vehicle parameters*/
typedef struct SDVehicleParameters {
    SDOptionalValid work_mode_optional{OPTIONAL_INVALID};
    SDWorkMode work_mode; /**< work mode */
    SDOptionalValid speed_optional{OPTIONAL_INVALID};
    int32_t speed; /**< vehicle speed */
    SDOptionalValid steering_wheel_angle_optional{OPTIONAL_INVALID};
    int32_t steering_wheel_angle; /**< steering wheel angle */
    SDOptionalValid door_signal_optional{OPTIONAL_INVALID};
    SDDoorSignal door_signal; /**< door signal */
    SDOptionalValid turn_signal_optional{OPTIONAL_INVALID};
    SDTurnSignal turn_signal; /**< turn signal */
    SDOptionalValid acc_signal_optional{OPTIONAL_INVALID};
    SDAccSignal acc_signal; /**< acc signal */
} SDVehicleParameters;
/************** vehicle signals define end **************/

MATRIX_DECLARE_C_CODE_END

#endif // _SENSEDRIVER_TYPE_H_
