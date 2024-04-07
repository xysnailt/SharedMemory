/*
 * Copyright (C) 2020 by SenseTime Group Limited. All rights reserved.
 *
 * @file sensedriver_calibration.h
 * @brief sensedriver calibration interface for all user
 * @version v0.3.0
 *
 */

#ifndef __SENSEDRIVER_INCLUDE_PRODUCT_COMMON_PUB_CALIBRATION_H__
#define __SENSEDRIVER_INCLUDE_PRODUCT_COMMON_PUB_CALIBRATION_H__

#include "MDmsStatus.h"
#include "matrix_macro.h"
#include "sensedriver_type.h"

MATRIX_DECLARE_C_CODE_BEGIN
/**
 * @brief Get calibration state
 *
 * @param[in] handle, user need to instantiate a handle
 * @param[out] state, calibration state
 * @return MDistractionCalibrationState
 *
 */
MATRIX_API MDmsStatus SenseDriverGetCalibrationState(SDHandle handle,
                                                     MDistractionCalibrationState* const state);

/**
 * @brief Get calibration parameters
 *
 * @param[in] handle, user need to instantiate a handle
 * @param[out] param, calibration parameter pointer, user is responsible for its memory
 * @return MDmsStatus, if handle is invalid, return MDMS_PARAM_INVALID; if distatcion module is
 * not initialized, return MDMS_MODULE_UNINIT; or return MDMS_SUCCESS
 *
 */
MATRIX_API MDmsStatus SenseDriverGetCalibrationParams(SDHandle handle,
                                                      MDistractionCalibrationParam* const param);

/**
 * @brief Set calibration parameters; after call this function, distration is ready to work
 *
 * @param[in] handle, user need to instantiate a handle
 * @param[in] param, calibration parameter pointer, user is responsible for its memory
 * @return MDmsStatus, if handle is invalid or param is nullptr, return MDMS_PARAM_INVALID; if
 * distraction do not supoort calibration, retun MDMS_CONFIG_PARAM_INVALID; if distatcion module is
 * not initialized, return MDMS_MODULE_UNINIT;or return MDMS_SUCCESS
 *
 */
MATRIX_API MDmsStatus
SenseDriverSetCalibrationParams(SDHandle handle, const MDistractionCalibrationParam* const param);

/**
 * @brief restart calibration
 *
 * @param[in] handle, user need to instantiate a handle
 * @return MDmsStatus, if handle is invalid is nullptr, return MDMS_PARAM_INVALID; if
 * distraction do not supoort calibration, retun MDMS_CONFIG_PARAM_INVALID; if distatcion module is
 * not initialized, return MDMS_MODULE_UNINIT;or return MDMS_SUCCESS
 *
 */
MATRIX_API MDmsStatus SenseDriverRestartCalibration(SDHandle handle);

MATRIX_DECLARE_C_CODE_END

#endif // __SENSEDRIVER_INCLUDE_PRODUCT_COMMON_PUB_CALIBRATION_H__