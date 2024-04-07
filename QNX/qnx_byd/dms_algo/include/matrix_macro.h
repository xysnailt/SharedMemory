/*
 * Copyright (C) 2019 by SenseTime Group Limited. All rights reserved.
 *
 */

#ifndef _CC_MATRIX_MACRO_H_
#define _CC_MATRIX_MACRO_H_

#ifdef __cplusplus
#define MATRIX_DECLARE_C_CODE_BEGIN extern "C" {
#else
#define MATRIX_DECLARE_C_CODE_BEGIN
#endif

#ifdef __cplusplus
#define MATRIX_DECLARE_C_CODE_END }
#else
#define MATRIX_DECLARE_C_CODE_END
#endif

#endif // _CC_MATRIX_MACRO_H_
