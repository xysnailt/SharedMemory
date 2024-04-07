/* ===========================================================================
 * Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
=========================================================================== */
#ifndef _TEST_UTIL_QNX_H_
#define _TEST_UTIL_QNX_H_

#include "qcarcam.h"
// #include "screen.h"
#ifndef C2D_DISABLED
#include "c2d2.h"
#endif


typedef struct
{
    int display_id;
    int size[2];
} test_util_display_prop_t;

typedef struct
{
    void* mem_handle;

    void*  ptr[2];  //plane va
    uint32 size[2]; //plane size
    long long phys_addr;

    uint32 c2d_surface_id;
} test_util_buffer_t;

struct test_util_ctxt_t
{
    test_util_ctxt_params_t params;

    screen_context_t screen_ctxt;
    screen_display_t *screen_display;
    test_util_display_prop_t *display_property;
    int screen_ndisplays;
};

typedef struct
{
    int size[2];
    int pos[2];
    int ssize[2];
    int spos[2];
    int visibility;
} test_util_window_param_cpy_t;

struct test_util_window_t
{
    screen_window_t screen_win;
    char winid[64];
    test_util_window_param_cpy_t params;

    int is_offscreen;

    /*buffers*/
    screen_buffer_t* screen_bufs;
    test_util_buffer_t* buffers;

    int buffer_size[2]; // width,height
    int n_buffers;
    int n_pointers;
    int stride[2];
    int offset[3];   //offset to each plane from start of buffer
    test_util_color_fmt_t format;
    int screen_format;

    int prev_post_idx; //previously posted buffer idx
};

#endif
