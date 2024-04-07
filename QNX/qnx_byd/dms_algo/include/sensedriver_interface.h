/*
 * Copyright (C) 2019 by SenseTime Group Limited. All rights reserved.
 *
 * @file sensedriver_interface.h
 * @brief sensedriver interface for all user
 * @version v0.3.0
 *
 */

#ifndef _CC_MATRIX_CORE_H_
#define _CC_MATRIX_CORE_H_

#include "MDmsStatus.h"
#include "matrix_macro.h"
#include "sensedriver_type.h"

MATRIX_DECLARE_C_CODE_BEGIN
/**
 * @brief Create Single Image Handle, Malloc Memory and return new image
 *
 * @param vir_addr cpu mem address
 * @param phy_addr phy(gpu/vpu-like) mem address,if cpu mem, use nullptr
 * @param timestamp input image time
 * @param format pixel format
 * @param width w of image
 * @param height h og image
 * @param memory_type cpu/gpu/vpu mem
 * @return MATRIX_API
 */
MATRIX_API ADImageHandle SenseDriverCreateSingleImageHandle(unsigned char* vir_addr,
                                                            unsigned char* phy_addr,
                                                            const MTimeStamp timestamp,
                                                            const MPixelFormat format,
                                                            const unsigned int width,
                                                            const unsigned int height,
                                                            MMemoryType memory_type);

/**
 * @brief Destroy Image and free Memory
 *
 * @param handle
 * @return MATRIX_API
 */
MATRIX_API void SenseDriverDestroySingleImageHandle(ADImageHandle* handle);

/**
 * @brief Init SD Handle
 *
 * @param[in] base_path the path to all config files
 * @param[in] path_len the length of path string
 * @param[out] handle user need to instantiate a handle
 * @return MDmsStatus
 * @warning handle is not thread-safe, please do not use it in another thread.
 */
MATRIX_API MDmsStatus SenseDriverInit(const char* base_path, const int path_len, SDHandle* handle);

/**
 * @brief Destroy SD Handle
 *
 * @param[inout] handle matrix initialized handle
 * @return MDmsStatus
 */
MATRIX_API MDmsStatus SenseDriverDestroy(SDHandle* handle);

/**
 * @brief switch some modules
 *
 * @param[inout] handle matrix initialized handle
 * @param[in] module_bitmask bitmask of the modules which would be enable or disable
 * @param[in] enable true: enable the module, false: disable the module
 * @return MDmsStatus
 */
MATRIX_API MDmsStatus SenseDriverModuleSwitch(SDHandle handle,
                                              const SenseDriverModuleBitMask module_bitmask,
                                              const bool enable);

/**
 * @brief SDRun all enabled modules
 *
 * @param[inout] handle matrix initialized handle
 * @param[in] image
 * @param[out] result
 * @return MDmsStatus
 */
MATRIX_API MDmsStatus SenseDriverRun(SDHandle handle, const ADImageHandle image, SDResult* result);

/**
 * @brief SDDestroyResult destroy SDResult
 *
 * @param[inout] result
 * @return MDmsStatus
 */
MATRIX_API MDmsStatus SenseDriverDestroyResult(SDResult* result);

/**
 * @brief load or unload modules
 *
 * @param[in] module_bitmask
 * @param[in] load; true: load modules, false: unload modules
 * @return MDmsStatus
 * in case of load, if return M_OK, all modules requested are loaded, ; otherwise, no module
 * requested is loaded
 * in case of unload, if return M_OK, all modules requsted are unloaded; otherwise, part of modules
 * requested are unloaded
 */
MATRIX_API MDmsStatus SenseDriverLoadModule(SDHandle handle,
                                            const SenseDriverModuleBitMask module_bitmask,
                                            const bool load);

/**
 * @brief get current modules enabled
 *
 * @param[in] handle matrix initialized handle
 * @param[out] module_bitmask
 * @return MDmsStatus
 * if handle  and module_bitmask are valid , return MDMS_SUCCESS and module_bitmask is all modules
 * enabled
 * if handle or module_bitmask is invalid, return MDMS_PARAM_INVALID and module_bitmask mean nothing
 */
MATRIX_API MDmsStatus SenseDriverGetEnableModules(SDHandle handle,
                                                  SenseDriverModuleBitMask* module_bitmask);

/**
 * @brief Set vehicle parameters.
 * @param[in] handle, a handle which has been initialized
 * @param[in] param, an vehicle parameters
 * @return MDmsStatus
 */
MATRIX_API MDmsStatus SenseDriverSetVehicleParameters(const SDVehicleParameters* param);

#ifdef QUALIFICATION_TEST

MATRIX_API MDmsStatus SenseDriverGenerateEyeFeature(SDHandle handle,
                                                    const ADImageHandle image,
                                                    float32_t* left_eye_feature,
                                                    float32_t* right_eye_feature);

MATRIX_API MDmsStatus SenseDriverRunByEyeFeature(SDHandle handle,
                                                 const ADImageHandle image,
                                                 float32_t* left_eye_feature,
                                                 float32_t* right_eye_feature,
                                                 SDResult* result);

#endif

MATRIX_DECLARE_C_CODE_END

#endif // _CC_MATRIX_CORE_H_
