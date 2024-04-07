/*
 * Copyright (C) 2019 by SenseTime Group Limited. All rights reserved.
 * Yin Shaopeng <yinshaopeng@sensetime.com>
 */

#ifndef _ADMETA_INCLUDE_CORE_COMMON_H_
#define _ADMETA_INCLUDE_CORE_COMMON_H_

#include <stdint.h>

using float32_t = float;
using float64_t = double;
using uchar8_t = unsigned char;
using char8_t = char;

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------
//! @addtogroup matrix_common  type
//! @{
//! @brief @~English <b>The module contains common types for c and c++.</b>
//! @brief @~Chinese <b>该模块包含了c-interface 和 c++实现通用的数据结构相关信息。</b>
// ----------------------------------

#ifdef MATRIX_EXPORTS
#ifdef _MSC_VER
#define EXPORT_API __declspec(dllexport)
#elif __ghs__
#define EXPORT_API
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

#else
#define EXPORT_API
#endif

#define CV_SDK_CPP_API EXPORT_API

#if defined(__linux__) || defined(__android__) || defined(__QNX__)
#define PROGRAM_STRING(N) _Pragma(#N)
#define IGNORED(arg) PROGRAM_STRING(GCC diagnostic ignored arg)
#define PUSH_IGNORE(...) _Pragma("GCC diagnostic push") __VA_ARGS__
#define POP_IGNORE _Pragma("GCC diagnostic pop")
#else
#define PROGRAM_STRING(N)
#define IGNORED(arg)
#define PUSH_IGNORE(...)
#define POP_IGNORE
#endif

#ifdef _MSC_VER
#include <Windows.h>
#define USLEEP(microsecend) Sleep((microsecend / 1000) ? (microsecend / 1000) : (1))
#else
#include <unistd.h>
#define USLEEP(microsecend) usleep(microsecend)
#endif

// ============================basic meta definition=========================
//! @defgroup basic_meta
//! @brief about basic meta type definition.
//! @{

/** A struct to reprensent time  */
typedef struct TimeStamp {
    long int tv_sec;  /**< second */
    long int tv_usec; /**< microsecond */
} TimeStamp;

/** A enum of memory type */
typedef enum MemoryType {
    MEM_ON_CPU,                        /**< memory type normal cpu */
    MEM_ON_HISI_NNIE,                  /**< memory type nnie(hisi called MMZ) for hisi chips */
    MEM_ON_AMBA_CVFLOW,                /**< memory type cvflow for amba_cv25 chips */
    MEM_ON_OCL,                        /**< memory type opencl */
    MEM_ON_OCL_SVM,                    /**< memory type opencl */
    MEM_ON_OCL_ION,                    /**< memory type opencl */
    MEM_ON_HEXAGON_DSP,                /**< memory type hexagon dsp */
    MEM_ON_CADENCE_DSP,                /**< memory type cadence dsp */
    MEM_ON_CUDA_HOST,                  /**< memory type normal cuda */
    MEM_ON_CUDA_DEV,                   /**< memory type normal cuda */
    MEM_ON_KESTREL_DEV,                /**< memory type kestrel device */
    MEM_ON_CAMBRICON,                  /**< memory type cambricon */
    MEM_ON_TIDL_CPU_DSP_SHARED,        /**< memory type ppl3ti cpu dsp shared */
    MEM_ON_MTK_CADENCE_CPU_DSP_SHARED, /**< memory type cadence cpu dsp shared */
    MEM_ON_NEURO_PILOT,                /**< memory type neuro pilot dma buf */
    MEM_ON_MEMORY_TYPE_MAX             /**< the max number of memory type */
} MemoryType;

/** A struct to represent angle */
typedef struct Oritation {
    float yaw;   /**< yaw */
    float pitch; /**< pitch */
    float roll;  /**< roll */
} Oritation;

/** A struct to represent int rectangle */
typedef struct Rect {
    int left;   /**< the x coordinate of the top left corner */
    int right;  /**< the x coordinate of the bottom right corner */
    int top;    /**< the y coordinate of the top left corner */
    int bottom; /**< the y coordinate of the bottom right corner */
} Rect;

typedef struct RectS {
    int left;   /**< the x coordinate of the top left corner */
    int right;  /**< the x coordinate of the bottom right corner */
    int top;    /**< the y coordinate of the top left corner */
    int bottom; /**< the y coordinate of the bottom right corner */
    float s;    /**< the score of the rect */
} RectS;
/** A struct to represent float rectangle */
typedef struct RectF {
    float left;   /**< the x coordinate of the top left corner */
    float right;  /**< the x coordinate of the bottom right corner */
    float top;    /**< the y coordinate of the top left corner */
    float bottom; /**< the y coordinate of the bottom right corner */
} RectF;

/** A struct to represent float 2d point */
typedef struct Point2f {
    float x; /**< the x coordinate of the point */
    float y; /**< the y coordinate of the point */
} Point2f;

/** A struct to represent float 2d point + score */
typedef struct Point2fs {
    float x; /**< the x coordinate of the point */
    float y; /**< the y coordinate of the point */
    float s; /**< the score of the point */
} Point2fs;

/** A struct to represent int 2d point */
typedef struct Point2i {
    int x; /**< the x coordinate of the point */
    int y; /**< the y coordinate of the point */
} Point2i;

/** A struct to represent float 3d point */
typedef struct Point3f {
    float x; /**< the x coordinate of the point */
    float y; /**< the y coordinate of the point */
    float z; /**< the z coordinate of the point */
} Point3f;

/** A enum to represent DataMgrCachePool configuration type */
typedef enum CachePoolConfigType {
    CACHE_POOL_CAPACITY,         /* to configure capacity of cache pool */
    CACHE_POOL_MAX_EXPAND_TIMES, /* to configure max expand times of cache pool */
    CACHE_POOL_UNUSED_TIMEOUT    /* to configure unused timeout of cache pool */
} CachePoolConfigType;

/** A enum to represent pixel format */
typedef enum PixelFormat {
    M_PIX_FMT_GRAY8 = 1,             /**< gray8  */
    M_PIX_FMT_RGBA8888 = 2,          /**< rgba8888 */
    M_PIX_FMT_RGB888 = 3,            /**< rgb888 */
    M_PIX_FMT_RGB888_PLANAR = 4,     /**< rgb888 RRRRRR:GGGGGG:BBBBBB */
    M_PIX_FMT_BGRA8888 = 5,          /**< bgra8888 */
    M_PIX_FMT_BGR888 = 6,            /**< bgr888 */
    M_PIX_FMT_BGR888_PLANAR = 7,     /**< bgr888 BBBBBB:GGGGGG:RRRRRR */
    M_PIX_FMT_YUV420P = 8,           /**< yuv420p */
    M_PIX_FMT_NV12 = 9,              /**< YUV  4:2:0 YYYY:UV */
    M_PIX_FMT_NV21 = 10,             /**< YUV  4:2:0 YYYY:VU */
    M_PIX_FMT_GRAY32 = 11,           /**< gray32*/
    M_PIX_FMT_RGB323232 = 12,        /**< rgb323232 fp32*/
    M_PIX_FMT_RGB323232_PLANAR = 13, /**< rgb323232 fp32  RRRRRR:GGGGGG:BBBBBB*/
    M_PIX_FMT_BGR323232 = 14,        /**< bgr323232 fp32*/
    M_PIX_FMT_BGR323232_PLANAR = 15, /**< bgr323232 fp32 BBBBBB:GGGGGG:RRRRRR*/
    M_PIX_FMT_GRAY16 = 16,           /**< gray16*/
    M_PIX_FMT_RGB161616 = 17,        /**< rgb161616 fp16*/
    M_PIX_FMT_RGB161616_PLANAR = 18, /**< rgb161616 fp16  RRRRRR:GGGGGG:BBBBBB*/
    M_PIX_FMT_BGR161616 = 19,        /**< bgr161616 fp16*/
    M_PIX_FMT_BGR161616_PLANAR = 20, /**< bgr161616 fp16 BBBBBB:GGGGGG:RRRRRR*/
    M_PIX_FMT_FLOAT32C4 = 21,        /**< fp32 channel ==4 */
    M_PIX_FMT_NV12_DETACH = 22,      /**< Y/UV not Contiguous memory*/
    M_PIX_FMT_NV21_DETACH = 23,      /**< Y/UV not Contiguous memory*/
    M_PIX_FMT_YUYV = 24,             /**< YUV422 PACKED*/
    M_PIX_FMT_UYVY = 25,             /**< YUV422 PACKED*/
    M_PIX_FMT_YV12 = 26,             /**< YUV  4:2:0 YYYYYYYY:VVUU */
    M_PIX_FMT_YU12 = 27,             /**< YUV  4:2:0 YYYYYYYY:UUVV */
    M_PIX_FMT_MAX = 28               /**< pixel format is invalid */
} PixelFormat;
//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _ADMETA_INCLUDE_CORE_COMMON_H_
