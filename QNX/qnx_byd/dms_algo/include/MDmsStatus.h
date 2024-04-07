#ifndef _MATRIX_SDK_MMDMS_STATUS_H_
#define _MATRIX_SDK_MMDMS_STATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

/** A enum to represent running status */
typedef enum MDmsStatus {
    MDMS_SUCCESS = 0,        //!< success
    MDMS_MODULE_UNINIT = 1,  //!< module uninit
    MDMS_PARAM_INVALID = 2,  //!< input parameter is invalid
    MDMS_FILE_OPEN_FAIL = 3, //!< open file failed

    // MDMS config errors
    MDMS_CONFIG_OPEN_FAIL = 100,     //!< config file open failed
    MDMS_CONFIG_PARSE_FAIL = 101,    //!< config parse failed
    MDMS_CONFIG_PARAM_MISSING = 102, //!< the parameter is missing in config file
    MDMS_CONFIG_PARAM_INVALID = 103, //!< the value of parameter is invalid
    MDMS_CONFIG_MISSING_MODEL = 104, //!< config missing model file, or model cannot load

    // MDMS license errors
    MDMS_LICENSE_EXPIRE = 200,                     //!< license expire
    MDMS_LICENSE_UDID_MISMATCH = 201,              //!< uuid is invalid
    MDMS_LICENSE_ONLINE_ACTIVATE_FAIL = 202,       //!< online activate failed
    MDMS_LICENSE_ACTIVATIONS_RUN_OUT = 203,        //!< no need online activate
    MDMS_LICENSE_UNAUTH = 204,                     //! module is unauthorized
    MDMS_LICENSE_HARDWARE_AUTH_INIT_FAIL = 205,    //! hardware auth init fail
    MDMS_LICENSE_HARDWARE_AUTH_FAIL = 206,         //! hardware auth fail
    MDMS_LICENSE_HARDWARE_AUTH_NOTSUPPORTED = 207, //! not support hardware auth
    MDMS_LICENSE_INVALID = 208,                    //!< license file is invalid

    MDMS_FUNC_HANDLE_INVALID = 300,          //!< input invalid handle called api functions
    MDMS_FUNC_PIXEL_FMT_INVALID = 302,       //!< the format of input image is invalid
    MDMS_FUNC_MODEL_FMT_INVALID = 303,       //!< the format of loaded model is invalid
    MDMS_RECOGNITION_FACE_LOW_QUALITY = 400, //!< recongnition face low quality
    MDMS_RECOGNITION_FACE_NOT_FOUND =
        401, //!< recongnition face not found, valid face is not detected.
    MDMS_MODULEMANAGER_MODULE_DISABLE = 600, //!< the module has not been enabled.

    MDMS_INTERNAL_ERROR = 1000 //!< internal error
} MDmsStatus;

#ifdef __cplusplus
} // extern "C"
#endif

#endif