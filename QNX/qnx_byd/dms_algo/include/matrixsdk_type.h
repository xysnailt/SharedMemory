#ifndef _MATRIXSDK_MMETA_CAPI_MATRIXSDK_TYPE_H_
#define _MATRIXSDK_MMETA_CAPI_MATRIXSDK_TYPE_H_

#include "core/common.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------
//! @addtogroup matrix_type  type
//! @{
//! @brief @~English <b>The module contains definition types for c-interface.</b>
//! @brief @~Chinese <b>该模块包含了数据结构相关信息。</b>
// ----------------------------------

typedef enum MObjectType {
    M_OBJECT_TYPE_FACE,
    M_OBJECT_TYPE_BODY,
    M_OBJECT_TYPE_HAND,
    M_OBJECT_TYPE_PHONE,
    M_OBJECT_TYPE_CUP,
    M_OBJECT_TYPE_WALLET,
    M_OBJECT_TYPE_SMOKE,
    M_OBJECT_TYPE_PET,
    M_OBJECT_TYPE_SAFETYSEAT,
    M_OBJECT_TYPE_MAX
} MObjectType;

typedef enum MResultValid {
    RESULT_INVALID, /**< the result is invalid */
    RESULT_VALID    /**< the result is valid */
} MResultValid;

/** A enum to represent mask status */
typedef enum MFaceAttrMask {
    FACE_ATTR_MASK_NONE,   /**< Face has no mask status */
    FACE_ATTR_MASK_EXIST,  /**< Face has mask status */
    FACE_ATTR_MASK_INVALID /**< Face mask invalid status */
} MFaceAttrMask;

/** A enum to represent race status */
typedef enum MFaceAttrHat {
    FACE_ATTR_HAT_NONE,   /** Face has no hat status */
    FACE_ATTR_HAT_EXIST,  /** Face has hat status */
    FACE_ATTR_HAT_INVALID /** Face hat invalid status */
} MFaceAttrHat;

/** A enum to represent race status */
typedef enum MFaceAttrRace {
    FACE_ATTR_RACE_YELLOW, /**< Face yellow race status */
    FACE_ATTR_RACE_BLACK,  /**< Face black race status */
    FACE_ATTR_RACE_WHITE,  /**< Face white race status */
    FACE_ATTR_RACE_INVALID /**< Face race invalid status */
} MFaceAttrRace;

/** A enum to represent glasses status */
typedef enum MFaceAttrGlasses {
    FACE_ATTR_GLASSES_NONE,       /**< Face has no glasses status */
    FACE_ATTR_GLASSES_GENERAL,    /**< Face has general glasses status */
    FACE_ATTR_GLASSES_SUNGLASSES, /**< Face has sun glasses status */
    FACE_ATTR_GLASSES_INVALID     /**< Face glasses invalid status */
} MFaceAttrGlasses;

/** A enum to represent child attr status */
typedef enum MFaceAttrChild {
    FACE_ATTR_CHILD_ISNOT,  /**< Face has no child attr */
    FACE_ATTR_CHILD_IS,     /**< Face has child attr*/
    FACE_ATTR_CHILD_INVALID /**< Face child classify invalid */
} MFaceAttrChild;

typedef enum MFaceAttrQuality {
    FACE_QUALITY_ISNOT,  /**< Face has no quality */
    FACE_QUALITY_IS,     /**< Face has quality */
    FACE_QUALITY_INVALID /**< Face quality classify invalid */
} MFaceAttrQuality;

/** A enum to represent emotion action unit: eyebrow status */
typedef enum MFaceEAUEyebrow {
    FACE_EAU_EYEBROW_OTHER,  /**< Face EAU eyebrow other status */
    FACE_EAU_EYEBROW_FROWN,  /**< Face EAU eyebrow frown status */
    FACE_EAU_EYEBROW_INVALID /**< Face EAU eyebrow invalid status */
} MFaceEAUEyebrow;

/** A enum to represent emotion action unit: eye status */
typedef enum MFaceEAUEye {
    FACE_EAU_EYE_OTHER,  /**< Face EAU eye other status */
    FACE_EAU_EYE_GLARE,  /**< Face EAU eye glare status */
    FACE_EAU_EYE_INVALID /**< Face EAU eye invalid status */
} MFaceEAUEye;

/** A enum to represent emotion action unit: mouth emotion status */
typedef enum MFaceEAUMouthEmotion {
    FACE_EAU_MOUTH_EMOTION_OTHER,  /**< Face EAU mouth emotion other status */
    FACE_EAU_MOUTH_EMOTION_SMILE,  /**< Face EAU mouth emotion smile status */
    FACE_EAU_MOUTH_EMOTION_POUT,   /**< Face EAU mouth emotion pout status */
    FACE_EAU_MOUTH_EMOTION_INVALID /**< Face EAU mouth emotion invalid status */
} MFaceEAUMouthEmotion;

/** A enum to represent emotion action unit: mouth chin mix status */
typedef enum MFaceEAUMouthChinMix {
    FACE_EAU_MCM_OTHER,                          /**< Face EAU MCM other status */
    FACE_EAU_MCM_MOUTH_CORNER_DOWN_CHIN_WRINKLE, /**< Face EAU MCM MCDCW status */
    FACE_EAU_MCM_INVALID                         /**< Face EAU MCM invalid status */
} MFaceEAUMouthChinMix;

/** A enum to represent emotion action unit: nose status*/
typedef enum MFaceEAUNose {
    FACE_EAU_NOSE_OTHER,   /**< Face EAU nose other status */
    FACE_EAU_NOSE_WRINKLE, /**< Face EAU nose wrinkle status */
    FACE_EAU_NOSE_INVALID  /**< Face EAU nose invalid status */
} MFaceEAUNose;

/** A enum to represent emotion action unit: mouth status */
typedef enum MFaceEAUMouth {
    FACE_EAU_MOUTH_CLOSE,  /**< Face EAU mouth close status */
    FACE_EAU_MOUTH_OPEN,   /**< Face EAU mouth open status */
    FACE_EAU_MOUTH_INVALID /**< Face EAU mouth invalid. status */
} MFaceEAUMouth;

/** A enum to represent gender */
typedef enum MGender {
    FEMALE,        /**< Gender female */
    MALE,          /**< Gender male */
    GENDER_INVALID /**< Gender invalid */
} MGender;

/** A enum to represent emotion */
typedef enum MEmotion {
    ANGRY = 1,          /**< Emotion angry */
    CALM = 2,           /**< Emotion calm */
    DISGUST = 3,        /**< Emotion disgust */
    HAPPY = 4,          /**< Emotion happy */
    SAD = 5,            /**< Emotion sad */
    SCARED = 6,         /**< Emotion scared */
    SURPRISE = 7,       /**< Emotion surpprise */
    UNCLEAR = 8,        /**< Emotion unclear */
    EMOTION_INVALID = 9 /**< Emotion invalid */
} MEmotion;

/** A enum to represent mouth status */
typedef enum MMouthStatus {
    MOUTH_OPEN,   /**< Mouth opened status */
    MOUTH_CLOSE,  /**< Mouth Close status */
    MOUTH_INVALID /**< Mouth invalid status */
} MMouthStatus;

/** A enum to represent eye status */
typedef enum MEyeStatus {
    EYE_OPEN,   /**< Eye opened status */
    EYE_CLOSE,  /**< Eye Close status */
    EYE_INVALID /**< Eye invalid status */
} MEyeStatus;

typedef enum MCameraPoseStatus {
    SUCCESS = 0,      /**< success */
    IMG_LOW_QUALITY,  /**< image low quality */
    POSE_LARGE_ERROR, /**< pose large error */
    INVALID_ARG,      /**< args invalid */
    INTERNAL_FAILED,  /**< internal failed */
} MCameraPoseStatus;

/** A struct to represent two  eyes' visibility from gaze model*/
typedef enum MGzeVecEyeStatus {
    GAZEVEC_TWO_INVISIBLE = 0, /** left and right eye are not visible*/
    GAZEVEC_LEFT_VISIBLE = 1,  /** left eye is visible, right eye is not visible*/
    GAZEVEC_RIGHT_VISIBLE = 2, /** left eye is not visible, right eye is visible*/
    GAZEVEC_TWO_VISIBLE = 3    /** left and right eye are visible*/
} MGzeVecEyeStatus;

/** A enum to represent single face's filter result of face quality */
typedef enum MFaceQualityStatus {
    FILTER_OK,                     /** input param pass all facequality filter*/
    FILTER_BY_DETECTION_SCORE,     /** filter the face for detection score is low*/
    FILTER_BY_DETECTION_FACE_SIZE, /** filter the face for face size is too small */
    FILTER_BY_TRACK_SCORE,         /** filter the face for track score */
    FILTER_BY_YAW,                 /** filter the face for face's yaw is too big */
    FILTER_BY_PITCH,               /** filter the face for face's pitch is too big */
    FILTER_BY_BRIGHT,              /** filter the face for image is too bright */
    FILTER_BY_DARK,                /** filter the face for image is too dark */
    FILTER_BY_BLUR,                /** filter the face for image is blurred */
    FILTER_BY_OCCLUSION,           /** filter the face for face is blocked */
    FILTER_BY_LANDMARK_OUTBOUND,   /** filter the face for landmark out of image */
    FILTER_BY_DARK_AND_BLUR,       /** filter the face for image is too dark and too blurred */
    FILTER_INVLAID                 /** filter result is invalid*/
} MFaceQualityStatus;

typedef enum MActionType {
    // Action None
    M_ACTION_TYPE_NONE,
    // General binary classify
    M_BINARY_CLASSIFY_TRUE,
    M_BINARY_CLASSIFY_FALSE,

    // Handup Action for insight
    M_ACTION_TYPE_HANDUP_BASE,
    M_ACTION_TYPE_HANDUP_NOT = M_ACTION_TYPE_HANDUP_BASE,
    M_ACTION_TYPE_HANDUP_EXIST,

    // smoke, drink, call for cabin
    M_ACTION_TYPE_CABIN_BASE,
    M_ACTION_TYPE_PHONE_CALL = M_ACTION_TYPE_CABIN_BASE,
    M_ACTION_TYPE_DRINK,
    M_ACTION_TYPE_SMOKE,

    M_ACTION_TYPE_CHILD_BASE,
    M_ACTION_TYPE_CHILD = M_ACTION_TYPE_CHILD_BASE,
    M_ACTION_TYPE_ADULT,
    M_ACTION_TYPE_CHILD_INVALID,

    M_ACTION_TYPE_HEADOUT_BASE,
    M_ACTION_TYPE_HEADOUT = M_ACTION_TYPE_HEADOUT_BASE,
    M_ACTION_TYPE_NO_HEADOUT,

    M_ACTION_TYPE_HANDOUT_BASE,
    M_ACTION_TYPE_HANDOUT = M_ACTION_TYPE_HANDOUT_BASE,
    M_ACTION_TYPE_NO_HANDOUT,

    M_ACTION_TYPE_BINDING_BASE,
    M_ACTION_TYPE_BINDING = M_ACTION_TYPE_BINDING_BASE,

    M_ACTION_TYPE_SEATBELT_BASE,
    M_ACTION_TYPE_NO_SEATBELT = M_ACTION_TYPE_SEATBELT_BASE,
    M_ACTION_TYPE_SEATBELT,

    M_ACTION_TYPE_INVALID
} MActionType;

typedef enum MTrackerConfigType {
    DETECT_INTERVAL,                      // uint32_t
    DETECTOR_SCORE_THRESHOLD,             // float32_t
    DETECTOR_INTERSECTION_RATE_THRESHOLD, // float32_t, NMS Threshold
    ALIGNER_SCORE_THRESHOLD,              // float32_t

    /* float32_t, default 1.0F, If (area of target) / (area of image) exceed this threshold,
   engine will track only. */
    LARGE_TARGET_THRESHOLD,

    MAX_TARGET_NUM, // uint32_t
    TARGET_ID,      // std::vector<std::vector<uint32_t> >

    /* Tracker Two Thread Only */
    MAIN_THREAD_PATTERNER, // std::shared_ptr<TrackerAligner>
    SAMPLE_STRATEGY,

    /* Tracker OON Only */
    FRAME_INTER_IN_MS, // float32_t, frame interval in ms
    CONFIG_TYPE_MAX,

    /* Motion Model */
    MOTION_MODEL_ENABEL,
    MOTION_MOMENTUM
} MTrackerConfigType;

// ============================About GESTURE definition=========================
//! @defgroup gesture
//! @brief about gesture type definition.
//! @{
typedef enum MGestureType {
    /** gesture is meaning less or not support*/
    MGESTURE_OTHERS = 0,
    /** gesture OK*/
    MGESTURE_OK = 1,
    /** gesture V, forefinger and middle finger stretched out upwards*/
    MGESTURE_V = 2,
    /** gesture Thumbs Up, only thumb stretched out upwards*/
    MGESTURE_THUMB = 3,
    /** gesture FIVE, five fingers stretched out*/
    MGESTURE_FIVE = 4,
    /** gesture GUN, thumb and forefinger stretched out*/
    MGESTURE_GUN = 5,
    /** gesture FIST, hand clenched*/
    MGESTURE_FIST = 6,
    /** gesture FOREFINGER, forefinger stretched upwards with other fingers clenched*/
    MGESTURE_FOREFINGER = 7,
    /** gesture SIX, thumb and little finger stretched out with other fingers clenched*/
    MGESTURE_SIX = 8,
    /** gesture THUMB V, thumb, forefinger and middle finger stretched out*/
    MGESTURE_THUMB_V = 9,
    /** gesture Heart with fingers, thumb and forefinger crossed, the gap facing upwards */
    MGESTURE_HEART = 10,
    /** gesture KEEP SILENCE, forefinger stretched upwards only and put forefinger in front of
       mouth*/
    MGESTURE_SILENCE = 11,
    /** gesture THREE, only forefinger, middlefinger, ringfinger stretched out */
    MGESTURE_THREE = 12,
    /** gesture FOUR, four finger stretched out, except for thumb */
    MGESTURE_FOUR = 13,
    /** gesture THUMB_DOWN, thumb turn down */
    MGESTURE_THUMB_DOWN = 14,
    /** gesture THUMB_LEFT, thumb turn left */
    MGESTURE_THUMB_LEFT = 15,
    /** gesture THUMB_RIGHT, thumb turn right */
    MGESTURE_THUMB_RIGHT = 16,
    /** gesture PALM, the little finger, forefinger, middlefinger and ringfinger is close */
    MGESTURE_PALM = 17,
    /** gesture MIDDLEFINGER, middlefinger stretched out with other fingers clenched*/
    MGESTURE_MIDDLEFINGER = 18,
    /** gesture RINGFINGER, ringfinger stretched out with other fingers clenched*/
    MGESTURE_RINGFINGER = 19,

    MGESTURE_FIVE_DOWN = 20,

    MGESTURE_PALM_LEFT = 21,

    MGESTURE_PALM_RIGHT = 22,

    MGESTURE_PALM_DOWN = 23,

    MGESTURE_TWO = 24,

    MGESTURE_COMBTHREE = 25,

    MGESTURE_SEP_TWO = 26,

    MGESTURE_NULL = 27,

    MGESTURE_SIX_DOWN = 28,

    MGESTURE_UNION_WIND = 29,

    MGESTURE_UNION_RAIN = 30,

    MGESTURE_UNION_THUNDER = 31,

    MGESTURE_UNION_ELECTRICITY = 32,

    MGESTURE_FOREFINGER_LEFT = 33,

    MGESTURE_FOREFINGER_RIGHT = 34,
    /** gesture is invalid*/
    MGESTURE_INVALID = 35
} MGestureType;

typedef enum GestureDynActType {
    GESTURE_DYNAMIC_INVALID = -1,
    GESTURE_FOREFINGER_ROTATION_CLOCKWISE,     /* FOREFINGER ROTATION CLOCKWISE: forefinger
                                                   rotation clockwise with othersclenched*/
    GESTURE_FOREFINGER_ROTATION_ANTICLOCKWISE, /* FOREFINGER ROTATION CLOCKWISE: forefinger
                                                       rotation anticlockwise with others clenched*/
    GESTURE_PALM_MOVE_LEFT,  /* PALM MOVE LEFT: move palm to the left with five fingers
                                     stretched out */
    GESTURE_PALM_MOVE_RIGHT, /* PALM MOVE RIGHT: move palm to the right with five fingers
                                    stretched out */
    GESTURE_PALM_MOVE_UP,    /* PALM MOVE UP: move palm to the up with five fingers
                                       stretched out */
    GESTURE_PALM_MOVE_DOWN,  /* PALM MOVE DOWN: move palm to the down with five fingers
                                     stretched out */
    GESTURE_PALM_WAVE_LEFT,  /* PALM WAVE LEFT: wave palm to the left with five fingers
                                 stretched out */

    GESTURE_PALM_WAVE_UP, /* PALM WAVE UP: wave palm to the up with five fingers
                               stretched out */

    GESTURE_PALM_WAVE_RIGHT, /* PALM WAVE RIGHT: wave palm to the right with five
                              fingers stretched out */

    GESTURE_PALM_WAVE_DOWN, /* PALM WAVE DOWN: wave palm to the down with five fingers
                                stretched out */

    GESTURE_FIST_MOVE_FORWARD,  /* FIST MOVE FORWARD: move fist forward with five fingers
                                     clenched */
    GESTURE_FIST_MOVE_BACKWARD, /* FIST MOVE BACKWARD: move fist backward with five fingers
                                    clenched */
    GESTURE_PALM2FIST,          /* PALM TO FIST: change plam to fist */

    GESTURE_PALM_VERTICAL_SWAY, /* PALM_VERTICAL_SWAY: which means byebye */

    GESTURE_DYNAMIC_OTHERS
} GestureDynActType;

// ============================About LIPS definition=========================
//! @defgroup lips
//! @brief about lips type definition.
typedef enum MLipsType {
    MLIPS_TYPE_INVALID = 0,
    MLIPS_TYPE_MUTE = 1,
    MLIPS_TYPE_SPEAKING = 2
} MLipsType;

typedef enum MLipsReadType {
    MLIPSREAD_TYPE_INVALID = 0,
    MLIPSREAD_TYPE_NOTALK = 1,
    MLIPSREAD_TYPE_UNKNOWN = 2,
    MLIPSREAD_TYPE_CLOSE_WINDOW = 3,           // 关上窗户
    MLIPSREAD_TYPE_NEXT = 4,                   // 下一首
    MLIPSREAD_TYPE_TAKE_A_PHOTO = 5,           // 拍个照
    MLIPSREAD_TYPE_TURN_UP = 6,                // 声音大一点
    MLIPSREAD_TYPE_OPEN_WINDOW = 7,            // 打开窗户
    MLIPSREAD_TYPE_TURN_OFF = 8,               // 把音乐关掉
    MLIPSREAD_TYPE_TURN_ON = 9,                // 把音乐打开
    MLIPSREAD_TYPE_AIR_CONDITIONER_ON = 10,    // 打开空调
    MLIPSREAD_TYPE_AIR_CONDITIONER_OFF = 11,   // 关掉空调
    MLIPSREAD_TYPE_OPEN_MUSIC = 12,            // 打开音乐
    MLIPSREAD_TYPE_CLOSE_MUSIC = 13,           // 关闭音乐
    MLIPSREAD_TYPE_AUDIO_ON = 14,              // 打开收音机
    MLIPSREAD_TYPE_AUDIO_OFF = 15,             // 关闭收音机
    MLIPSREAD_TYPE_OPNE_SUNSHADE = 16,         // 打开遮阳帘
    MLIPSREAD_TYPE_CLOSE_SUNSHADE = 17,        // 关闭遮阳帘
    MLIPSREAD_TYPE_OPEN_GAODE = 18,            // 打开高德地图
    MLIPSREAD_TYPE_OPEN_SEAT_HEAT = 19,        // 打开座椅加热
    MLIPSREAD_TYPE_CLOSE_SEAT_HEAT = 20,       // 关闭座椅加热
    MLIPSREAD_TYPE_OPEN_SEAT_VENTILATION = 21, // 打开座椅通风
    MLIPSREAD_TYPE_OPEN_SOUND = 22,            // 打开声音
    MLIPSREAD_TYPE_CLOSE_SOUND = 23,           // 关闭声音
    MLIPSREAD_TYPE_OPEN_CAR_WINDOW = 24,       // 打开车窗
    MLIPSREAD_TYPE_CLOSE_CAR_WINDOW = 25,      // 关闭车窗
    MLIPSREAD_TYPE_OPEN_SUNROOF = 26,          // 开启天窗
    MLIPSREAD_TYPE_CLOSE_SUNROOF = 27,         // 关掉天窗
    MLIPSREAD_TYPE_ZOOM_IN_MAP = 28,           // 放大地图
    MLIPSREAD_TYPE_ZOOM_OUT_MAP = 29,          // 缩小地图
    MLIPSREAD_TYPE_TRAFFIC_ON = 30,            // 关闭路况
    MLIPSREAD_TYPE_TRAFFIC_OFF = 31,           // 打开路况
    MLIPSREAD_TYPE_BACK_TO_NAVIGATION = 32,    // 返回导航
    MLIPSREAD_TYPE_CLOSE_NAVIGATION = 33,      // 关闭导航
    MLIPSREAD_TYPE_ANSWER_PHONE = 34,          // 接听电话
    MLIPSREAD_TYPE_HANG_UP = 35,               // 挂断电话
    MLIPSREAD_TYPE_OPEN_NAVIGATION = 36,       // 打开导航
    MLIPSREAD_TYPE_USER_DEFINE_1 = 37,         // 小迪小迪
    MLIPSREAD_TYPE_USER_DEFINE_2 = 38,         // Hi,nomi
    MLIPSREAD_TYPE_USER_DEFINE_3 = 39,
    MLIPSREAD_TYPE_USER_DEFINE_4 = 40,
    MLIPSREAD_TYPE_USER_DEFINE_5 = 41,
    MLIPSREAD_TYPE_USER_DEFINE_6 = 42,
    MLIPSREAD_TYPE_USER_DEFINE_7 = 43,
    MLIPSREAD_TYPE_USER_DEFINE_8 = 44,
    MLIPSREAD_TYPE_USER_DEFINE_9 = 45,
    MLIPSREAD_TYPE_USER_DEFINE_10 = 46,
    MLIPSREAD_TYPE_USER_DEFINE_11 = 47,
    MLIPSREAD_TYPE_USER_DEFINE_12 = 48,
    MLIPSREAD_TYPE_USER_DEFINE_13 = 49,
    MLIPSREAD_TYPE_USER_DEFINE_14 = 50,
    MLIPSREAD_TYPE_USER_DEFINE_15 = 51,
    MLIPSREAD_TYPE_USER_DEFINE_16 = 52,
    MLIPSREAD_TYPE_USER_DEFINE_17 = 53,
    MLIPSREAD_TYPE_USER_DEFINE_18 = 54,
    MLIPSREAD_TYPE_USER_DEFINE_19 = 55,
    MLIPSREAD_TYPE_USER_DEFINE_20 = 56,
    MLIPSREAD_TYPE_USER_DEFINE_21 = 57,
    MLIPSREAD_TYPE_USER_DEFINE_22 = 58,
    MLIPSREAD_TYPE_USER_DEFINE_23 = 59,
    MLIPSREAD_TYPE_USER_DEFINE_24 = 60,
    MLIPSREAD_TYPE_USER_DEFINE_25 = 61,
    MLIPSREAD_TYPE_USER_DEFINE_26 = 62,
    MLIPSREAD_TYPE_USER_DEFINE_27 = 63,
    MLIPSREAD_TYPE_USER_DEFINE_28 = 64,
    MLIPSREAD_TYPE_USER_DEFINE_29 = 65,
    MLIPSREAD_TYPE_USER_DEFINE_30 = 66,
    MLIPSREAD_TYPE_USER_DEFINE_31 = 67,
    MLIPSREAD_TYPE_USER_DEFINE_32 = 68,
    MLIPSREAD_TYPE_USER_DEFINE_33 = 69,
    MLIPSREAD_TYPE_USER_DEFINE_34 = 70,
    MLIPSREAD_TYPE_USER_DEFINE_35 = 71,
    MLIPSREAD_TYPE_USER_DEFINE_36 = 72,
    MLIPSREAD_TYPE_USER_DEFINE_37 = 73,
    MLIPSREAD_TYPE_USER_DEFINE_38 = 74,
    MLIPSREAD_TYPE_USER_DEFINE_39 = 75,
    MLIPSREAD_TYPE_USER_DEFINE_40 = 76,
    MLIPSREAD_TYPE_USER_DEFINE_41 = 77,
    MLIPSREAD_TYPE_USER_DEFINE_42 = 78,
    MLIPSREAD_TYPE_USER_DEFINE_43 = 79,
    MLIPSREAD_TYPE_USER_DEFINE_44 = 80,
    MLIPSREAD_TYPE_USER_DEFINE_45 = 81,
    MLIPSREAD_TYPE_USER_DEFINE_46 = 82,
    MLIPSREAD_TYPE_MAX = 83
} MLipsReadType;

/** A enum to config lipsread before lips&&lipsread running */
typedef enum MLipsConfigType {
    LIPS_TALH_THR,            // float32_t
    LIPS_SMOOTH_WINDOW_SIZE,  // uint32_t
    LIPS_IS_NORMAL_FRAMERATE, // bool using lips_test
    LIPSREAD_THRESHOLDS       // std::vector<float>
} MLipsConfigType;
//! @}

typedef enum MLiveType { MLIVE_TYPE_INVALID = 0, MLIVE_TYPE_LIVENESS = 1 } MLiveType;

MObjectType MObjectTypeFromStr(const char* const str);
MActionType MActionTypeFromStr(const char* const str);
MGestureType MGestureTypeFromStr(const char* const str);
const char* MGestureTypeToStr(const MGestureType type);
const char* MLipsReadTypeToStr(const MLipsReadType type);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _MATRIXSDK_MMETA_CAPI_MATRIXSDK_TYPE_H_
