/*
 * Copyright (C) 2019 by SenseTime Group Limited. All rights reserved.
 * Yin Shaopeng <yinshaopeng@sensetime.com>
 */

#ifndef _MATRIXSDK_MCOMMON_MMETA_CAPI_MATRIXSDK_COMMON_H_
#define _MATRIXSDK_MCOMMON_MMETA_CAPI_MATRIXSDK_COMMON_H_

#include "core/common.h"
#include "matrixsdk_type.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------
//! @addtogroup matrix_common  type
//! @{
//! @brief @~English <b>The module contains common types for c and c++.</b>
//! @brief @~Chinese <b>该模块包含了c-interface 和 c++实现通用的数据结构相关信息。</b>
// ----------------------------------
/**MATRIX_API Macro for dynamic api export */
#ifdef _MSC_VER
#ifdef MATRIX_EXPORTS
#define MATRIX_API __declspec(dllexport)
#else
#define MATRIX_API
#endif
#else /* _MSC_VER */
#if defined(MATRIX_EXPORTS) && !defined(__ghs__)
#define MATRIX_API __attribute__((visibility("default")))
#else
#define MATRIX_API
#endif
#endif

// ============================basic meta definition=========================
//! @defgroup basic_meta
//! @brief about basic meta type definition.
//! @{

/** A struct to reprensent transform matrix **/
typedef float LocalDescriptorTransformMatrix[3][4];

/** A struct to reprensent time  */
typedef TimeStamp MTimeStamp;

/** A enum of memory type */
typedef MemoryType MMemoryType;

/** A struct to represent angle */
typedef Oritation MOritation;

/** A struct to represent int rectangle */
typedef Rect MRect;

/** A struct to represent float rectangle */
typedef RectF MRectF;

/** A struct to represent float 2d point */
typedef Point2f MPoint2f;

/** A struct to represent float 2d point + score */
typedef Point2fs MPoint2fs;

/** A struct to represent int 2d point */
typedef Point2i MPoint2i;

/** A struct to represent float 3d point */
typedef Point3f MPoint3f;

/** A enum to represent DataMgrCachePool configuration type */
typedef CachePoolConfigType MCachePoolConfigType;

/** A enum to represent pixel format */
typedef PixelFormat MPixelFormat;
//! @}

// ============================About Face Attribute definition=========================
//! @defgroup faceattribute definition
//! @brief about face attribute type definition.
//! @{

/** A struct to represent EAU nose result*/
typedef struct MFaceEAUNoseResult {
    MFaceEAUNose result; /**< nose status */
    float score;         /**< confidence */
} MFaceEAUNoseResult;

/** A struct to represent EAU mouth result*/
typedef struct MFaceEAUMouthResult {
    MFaceEAUMouth result; /**< mouth status */
    float score;          /**< confidence */
} MFaceEAUMouthResult;

/** A struct to represent EAU mouth chin mix result*/
typedef struct MFaceEAUMouthChinMixResult {
    MFaceEAUMouthChinMix result; /**< mouth status */
    float score;                 /**< confidence */
} MFaceEAUMouthChinMixResult;

/** A struct to represent EAU mouth emotion result*/
typedef struct MFaceEAUMouthEmotionResult {
    MFaceEAUMouthEmotion result; /**< mouth status */
    float score;                 /**< confidence */
} MFaceEAUMouthEmotionResult;

/** A struct to represent EAU eye result*/
typedef struct MFaceEAUEyeResult {
    MFaceEAUEye result; /**< eye status */
    float score;        /**< confidence */
} MFaceEAUEyeResult;

/** A struct to represent EAU eyebrow result*/
typedef struct MFaceEAUEyebrowResult {
    MFaceEAUEyebrow result; /**< eyebrow status */
    float score;            /**< confidence */
} MFaceEAUEyebrowResult;

/** A struct to represent EAU  result*/
typedef struct MFaceEAUResult {
    MFaceEAUEyebrowResult au0_eyebrow;         /**< eyebrow status */
    MFaceEAUEyeResult au1_eye;                 /**< eye status */
    MFaceEAUMouthEmotionResult au2_mouth;      /**< mouth_emotion status */
    MFaceEAUMouthChinMixResult au3_mouth_chin; /**< mouth_cm status */
    MFaceEAUMouthResult au4_mouth_open;        /**< mouth status */
    MFaceEAUNoseResult au5_nose;               /**< nose status */
    MResultValid valid;                        /**< result valid or not */
} MFaceEAUResult;

/** A struct to represent mask result*/
typedef struct MFaceAttrMaskResult {
    MFaceAttrMask mask; /**< mask status */
    float score;        /**< confidence */
    MResultValid valid; /**< result valid or not */
} MFaceAttrMaskResult;

/** A struct to represent race result*/
typedef struct MFaceAttrRaceResult {
    MFaceAttrRace race; /**< race status */
    float score;        /**< confidence */
    MResultValid valid; /**< result valid or not */
} MFaceAttrRaceResult;

/** A struct to represent glasses result*/
typedef struct MFaceAttrGlassesResult {
    MFaceAttrGlasses glasses; /**< glasses status */
    float score;              /**< confidence */
    MResultValid valid;       /**< result valid or not */
} MFaceAttrGlassesResult;

/** A struct to represent hat result */
typedef struct MFaceAttrHatResult {
    MFaceAttrHat hat;       /** hat status */
    float score;            /** confidence */
    MResultValid valid;     /** result valid or not */
} MFaceAttrHatResult;

/** A struct to represent child attr valid result*/
typedef struct MFaceAttrChildResult {
    MFaceAttrChild Child; /**< ChildValid status */
    float score;          /**< confidence */
    MResultValid valid;   /**< result valid or not */
} MFaceAttrChildResult;
typedef struct MFaceAttrQualityResult {
    MFaceAttrQuality quality; /**< ChildValid status */
    float score;              /**< confidence */
    MResultValid valid;       /**< result valid or not */
} MFaceAttrQualityResult;

/** A struct to represent gender result*/
typedef struct MGenderResult {
    MGender gender;     /**< gender */
    float score;        /**< confidence */
    MResultValid valid; /**< result valid or not */
    float valid_score;  /**< score of gender validity */
} MGenderResult;

/** A struct to represent age valid result*/
typedef struct MAgeValidResult {
    MResultValid valid; /**< result valid or not */
    float valid_score;  /**< score of age validity */
} MAgeValidResult;
typedef struct MNormFaceQualityResult {
    float norm_face_quality[192]; /** norm quality result */
    bool valid{false};  /** is data valid */
} MNormFaceQualityResult;

typedef struct MFaceAttrSingleResult {
    MGenderResult gender; /**< gender result, invalid means model not output gender */
    MGenderResult gender_loose; /**< gender_loose result, inference by loose filter,invalid
                                   means model not output gender */
    float age;                  /**< age, age<0 means model not output age */
    MAgeValidResult age_valid;                                  
    float age_loose;            /**< age_loose, age inference by loose filter, age<0 means model not output age */
    MAgeValidResult age_loose_valid;
    MFaceAttrMaskResult mask;       /**< mask result, invalid means model not output mask */
    MFaceAttrRaceResult race;       /**< race result, invalid means model not output race */
    MFaceAttrHatResult hat;         /** hat result, invalid means model not output hat */
    MFaceAttrGlassesResult glasses; /**< glasses result, invalid means model not output glasses
                                     */
    MFaceEAUResult eau_result;      /**< emotion action unit result, invalid means model not output
                                     * EAU
                                     */
    MFaceAttrChildResult child_result; /**< child valid result. */
    float smile;      /**< smile, range [0, 1], smile < 0 means model not output smile */
    float face_value; /**< face_value, range [0, 1], face_value < 0 means model not output
                face_value */
    MFaceAttrQualityResult face_quality; /**< face quality score*/
    MNormFaceQualityResult norm_face_quality;   /** norm face quality result */
    uint32_t track_id;
} MFaceAttrSingleResult;
//! @}

// ============================About Eye Mouth definition=========================
//! @defgroup eyemouth
//! @brief about eye and mouth type definition.
//! @{

struct MMouthResult {
    MMouthStatus mouth;
    float score;
    float valid_score;
    uint32_t track_id;
};
// /** A struct to represent DMS or OMS emotion result*/
typedef struct MEmotionResult {
    MEmotion emotion; /**< emotion */
    float score;      /**< confidence */
    uint32_t track_id;
} MEmotionResult;

enum EyeOpenResultType {
    EyeByInvalid,
    EyeByOpenScore,
    EyeBySquintScore,
    EyeByKpScore,
    EyeByFeatureScore,
    EyeByValidScore
};

/** A struct to represent single face's eye result*/
typedef struct MEyeSingleResult {
    MEyeStatus left_eye;      /**< left eye status */
    float left_open_score;    /**< left confidence */
    float left_kp_score;      /**< left keypoint score of eye */
    float left_squint_score;  /**< left visible score of eye */
    float left_visible_score; /**< left visible score of eye */

    MEyeStatus left_eye_by_ft;        /**< left eye status */
    float left_feature_score;         /**< left feature score of eye */
    float left_feature_kp_score;      /**< left feature keypoint score of eye */
    float left_feature_visible_score; /**< left feature visible score of eye */

    EyeOpenResultType left_result_type;

    MEyeStatus right_eye;      /**< right eye status */
    float right_open_score;    /**< right confidence */
    float right_kp_score;      /**< right keypoint score of eye */
    float right_squint_score;  /**< right visible score of eye */
    float right_visible_score; /**< right visible score of eye */

    MEyeStatus right_eye_by_ft;        /**< left eye status */
    float right_feature_score;         /**< left feature score of eye */
    float right_feature_kp_score;      /**< left feature keypoint score of eye */
    float right_feature_visible_score; /**< left feature visible score of eye */
    EyeOpenResultType right_result_type;

    uint32_t track_id;
} MEyeSingleResult;

//! @}

// ============================About Self Calibrate definition=========================
/** A enum to represent running status of camerapose c-api */

/** A struct to represent self calibrate result*/
typedef struct MSelfCalibrateVecResult {
    LocalDescriptorTransformMatrix T; /**< transform matrix */
    MCameraPoseStatus status;         /**< camera pose status */
} MSelfCalibrateVecResult;

//! @}

// ============================About Face Gaze definition=========================
//! @defgroup gaze
//! @brief about gaze type definition.
//! @{

/** A struct to represent gaze vector result*/
typedef struct MGazeVecResult {
    bool is_valid;                     /** indicates if result is valid*/
    MGzeVecEyeStatus eye_status;       /** two eyes' visibility*/
    MPoint3f gaze_vec;                 /** gaze vector*/
    MPoint3f gaze_origin;              /** gaze vector before calibrate*/
    MGzeVecEyeStatus left_eye_status;  /** left_eye_status */
    MPoint3f gaze_vec_left;            /** left eye gaze vector */
    MGzeVecEyeStatus right_eye_status; /** right_eye_status */
    MPoint3f gaze_vec_right;           /** right eye gaze vector */
    float left_visible_score;          /** left eye's visible score*/
    float right_visible_score;         /** right eye's visible score*/
} MGazeVecResult;

/** A struct to represent head3d raw result*/
typedef struct MHead3DRawResult {
    MResultValid valid;            /**< result valid or not */
    MPoint3f head_position;        /** head position*/
    MPoint3f head_pose;            /** head pose*/
    MPoint3f left_eye_position;    /** left eye position*/
    float left_eye_open_distance;  /** left eye open distance*/
    float left_pupil_distance;     /** left eye pupil distance*/
    MPoint3f right_eye_position;   /** right eye position*/
    float right_eye_open_distance; /** right eye open distance*/
    float right_pupil_distance;    /** right eye pupil distance*/
    float left_eye_open_degree;    /** left eye open degree*/
    float right_eye_open_degree;    /** right eye open degree*/
    float left_eye_corner_distance; /** left eye's left and right corner distance*/
    float right_eye_corner_distance;/** left eye's left and right corner distance*/
    float mouth_horizontal_open_degree; /** mouth horizontal open degree*/
    float mouth_vertical_open_degree;   /** mouth Vertical open degree*/
} MHead3DRawResult;

/** A struct to represent head3d result*/
typedef struct MHead3DResult {
    bool is_valid;        /** indicates if result is valid*/
    MPoint3f gaze_origin; /** origin point of gaze vector*/
    MPoint2f gaze_point;  /** destination point of gaze vector*/
    MHead3DRawResult raw_result;
} MHead3DResult;

/** A struct to represent gazeaoi result*/
typedef struct MGazeAOIResult {
    bool is_valid;      /** indicates if result is valid*/
    unsigned int label; /** index of gaze aoi*/
} MGazeAOIResult;

//! @}

// ============================About Face Feature definition=========================
//! @defgroup facefeature
//! @brief about face feature type definition.
//! @{

/** A struct to reprensent version  */
typedef struct MVersion {
    unsigned char major; /**< Mvision major */
    unsigned char minor; /**< Mvision minor */
    unsigned char patch; /**< Mvision patch */
    unsigned char rsv;   /**< Mvision rsv */
} MVersion;

/** A struct to represent face feature header */
typedef struct MFaceFeatureHeader {
    MVersion version; /**< version of feature */
} MFaceFeatureHeader;
//! @}

// ============================About Face Quality definition=========================
//! @defgroup facequality
//! @brief about face quality type definition.
//! @{

/** A struct to represent filter configuration of face quality */
typedef struct _FaceQualityConfig {
    /**< The filter threshold for yaw of head pose.
     * A image would be filtered if yaw of head pose
     * exceeded the threshold.
     * yaw_threshold < 0 will close the yaw filter.
     */
    float yaw_threshold;
    /**< The filter threshold for pitch of head pose.
     * A image would be filtered if yaw of head pose
     * exceeded the threshold.
     * pitch_threshold < 0 will close the yaw filter.
     */
    float pitch_threshold;
    /**< The filter threshold of bright.
     * A Pixel would be treated as bright if value exceeded the threshold.
     * bright_threshold < 0 will close the bright filter.
     */
    float bright_threshold;
    /**< The filter threshold of dark.
     * A Pixel would be treated as dark if value is below the threshold.
     * dark_threshold < 0 will close the dark filter.
     */
    float dark_threshold;
    /**< The filter threshold of bright ratio.
     * A image would be filtered if bright pixel rate exceeded the threshold.
     * bright_ratio_threshold < 0 will close the bright filter.
     */
    float bright_ratio_threshold;
    /**< The filter threshold of dark ratio.
     * A image would be filtered if dark pixel rate exceeded the threshold.
     * dark_ratio_threshold < 0 will close the dark filter.
     */
    float dark_ratio_threshold;
    /**< The filter threshold of blur.
     * A image would be filtered if blur exceeded the threshold.
     * blur_threshold < 0 will close the blur filter.
     */
    float blur_threshold;
    /**< The filter threshold of detection score.
     * A image would be filtered if detection score is below the threshold.
     * face_detection_score_threshold < 0 will close the face_detect_score filter.
     */
    float face_detection_score_threshold;
    /**< The filter threshold of tracking score.
     * A image would be filtered if tracking score is below the threshold.
     * face_track_score_threshold < 0 will close the face_track_score filter.
     */
    float face_track_score_threshold;
    /**< The filter threshold of acclusion ratio.
     * A image would be filtered if acclusion ratio exceeded the threshold.
     * face_occlusion_ratio_threshold < 0 will close the face_occlusion filter.
     */
    float face_occlusion_ratio_threshold;
    /**< The filter threshold of face size.
     * A image would be filtered if minimum edge of face rect is below the threshold.
     * face_size_threshold < 0 will close the face size filter.
     */
    float face_size_threshold;
} MFaceQualityConfig;

/** Face quality single result */
typedef struct MFaceQualitySingleResult {
    MFaceQualityStatus status; /** FaceQuality status enum */
    float score;               /** FaceQuality score:[0.0, 1.0], higher score higher quality */
    float brightness_score; /** FaceQuality brightness:[0.0, 1.0], 1.0 too bright, 0.0 too dark */
    float blur_score;       /** FaceQuality blur score:[0.0, 1.0], 0.46 too blur */
} MFaceQualitySingleResult;
//! @}

// ============================About MHOI definition=========================
//! @defgroup mhoi
//! @brief about mhoi type definition.
//! @{
/** A struct to represent single action */
typedef struct MDetectedAction {
    MActionType label; /**< the label of action */
    float score;       /**< the score of action */
    // MDetectedObject* obj;  /**< the obj of action */
} MDetectedAction;

/** A struct to represent object tracking attr */
typedef struct MObjectTrackingAttr {
    int id;      /**< the id of object, unified */
    float score; /**< the score of track */
} MObjectTrackingAttr;

//! @}

/** A struct to represent single object detect result */
typedef struct MDetectSingleResult {
    MRect rect;        /**<  rectangle */
    float score;       /**<  confidence */
    MObjectType label; /**< label */
} MDetectSingleResult;

typedef MDetectSingleResult MDetectedObject;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _MATRIXSDK_MCOMMON_MMETA_CAPI_MATRIXSDK_COMMON_H_
