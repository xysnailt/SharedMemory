#ifndef _PRODUCT_FRAMEWORK_CORE_ADImageHELPER_HPP_
#define _PRODUCT_FRAMEWORK_CORE_ADImageHELPER_HPP_

#include "MDmsStatus.h"
#include "matrixsdk_common.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct ADImage* ADImageHandle;

// this header file(ADImageHelper.h) is for the user of sensedriver to create image and operate
// image.

MATRIX_API ADImageHandle CreateSingleImageHandle(unsigned char* vir_addr,
                                                 unsigned char* phy_addr,
                                                 const TimeStamp timestamp,
                                                 const MPixelFormat format,
                                                 const unsigned int width,
                                                 const unsigned int height,
                                                 MMemoryType memory_type);

MATRIX_API void DestroySingleImageHandle(ADImageHandle* handle);

MATRIX_API MDmsStatus GetProductImageSize(const ADImageHandle handle,
                                          unsigned int* width,
                                          unsigned int* height);
MATRIX_API MDmsStatus GetProductImageTimeStamp(const ADImageHandle handle, TimeStamp* timestamp);
MATRIX_API MDmsStatus GetProductImageDataVirAddrOnCpu(const ADImageHandle handle,
                                                      unsigned char** data);
MATRIX_API MDmsStatus GetProductImagePixelFormat(const ADImageHandle handle, MPixelFormat* format);

#ifdef __cplusplus
} // extern "C"
#endif

#endif