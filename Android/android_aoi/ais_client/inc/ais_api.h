#ifndef AIS_API_H
#define AIS_API_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/select.h>
#include <thread>
#include <queue>

#ifndef C2D_DISABLED
#include "c2d2.h"
#endif

#ifdef USE_VENDOR_EXT_PARAMS
#include "vendor_ext_properties.h"
#endif

#include "qcarcam.h"

#include "test_util.h"
#include "qcarcam_diag_types.h"

namespace AIS_API {

#define NUM_MAX_CAMERAS 28
#define NUM_MAX_DISP_BUFS 3

/*1sec delay before restart */
#define PAUSE_RESUME_USLEEP 1000000
#define START_STOP_USLEEP 1000000

/*print input state as frozen if start and no frames after 1 sec*/
#define QCARCAMTEST_SOF_FREEZE_TIMEOUT 1.0f

#define QCARCAM_TEST_DEFAULT_GET_FRAME_TIMEOUT 500000000
#define DEFAULT_PRINT_DELAY_SEC 10
#define SIGNAL_CHECK_DELAY 33333;
#define CSI_ERR_CHECK_DELAY 100000;
#define NS_TO_MS 0.000001F

#define QCARCAM_TEST_INPUT_INJECTION 11
#define BUFSIZE 10

#define SIGWAIT_TIMEOUT_MS 100
#define TIMER_THREAD_USLEEP 1000000

#define USR_PROCESS_THREAD_USLEEP 1000
#define USR_PROCESS_WAIT_USLEEP 1000

#if defined(__INTEGRITY)
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC CLOCK_REALTIME
#endif
extern "C" const Value __PosixServerPriority = CAM_POSIXSERVER_PRIORITY;
#endif

#define SET_BIT(num, nbit)   ((num) |=  (0x1<<(nbit)))
#define CHECK_BIT(num, nbit) ((num) & (0x1<<(nbit)))

#define QCARCAMTEST_BUFFER_STALE_BIT  4

typedef enum
{
    QCARCAMTEST_BUFFER_STATE_INIT,
    QCARCAMTEST_BUFFER_STATE_QCARCAM,
    QCARCAMTEST_BUFFER_STATE_GET_FRAME,
    QCARCAMTEST_BUFFER_STATE_USR_PROCESS,
    QCARCAMTEST_BUFFER_STATE_USR_PROCESS_DONE,
    QCARCAMTEST_BUFFER_STATE_POST_DISPLAY,
    QCARCAMTEST_BUFFER_STATE_MAX = 0x7FFFFFF
}qcarcam_test_buffer_state_t;

typedef enum
{
    QCARCAM_TEST_BUFFERS_OUTPUT = 0,
    QCARCAM_TEST_BUFFERS_DISPLAY,
    QCARCAM_TEST_BUFFERS_INPUT,
    QCARCAM_TEST_BUFFERS_MAX
} qcarcam_test_buffers_t;

typedef struct
{
    qcarcam_buffers_t p_buffers;

    qcarcam_color_fmt_t format;
    unsigned int n_buffers;
    unsigned int width;
    unsigned int height;
} qcarcam_buffers_param_t;

typedef enum
{
    QCARCAMTEST_STATE_INVALID = 0,
    QCARCAMTEST_STATE_INIT,
    QCARCAMTEST_STATE_OPEN,
    QCARCAMTEST_STATE_START,
    QCARCAMTEST_STATE_STOP,
    QCARCAMTEST_STATE_PAUSE,
    QCARCAMTEST_STATE_PAUSE_STOP_PENDING,
    QCARCAMTEST_STATE_ERROR,
    QCARCAMTEST_STATE_CLOSED,
}qcarcamtest_state_t;

typedef struct
{
    uint32 buf_idx;
    void *p_data;
}timer_usr_data;

typedef struct
{
    timer_usr_data usr_data;
    CameraTimer ptimer;
}qcarcam_test_buf_timer;

typedef struct
{
    qcarcam_event_t event_id;
    qcarcam_event_payload_t payload;
} qcarcam_event_msg_t;

typedef struct
{
    CameraThread thread_handle;
    CameraThread process_cb_event_handle;
    CameraSignal m_eventHandlerSignal;

    unsigned int idx;
    int query_inputs_idx;

    pthread_mutex_t mutex;
    qcarcamtest_state_t state;
    bool is_fatal_error;

    /*qcarcam context*/
    qcarcam_hndl_t qcarcam_context;
    qcarcam_input_desc_t qcarcam_input_id;

    qcarcam_buffers_param_t buffers_param[QCARCAM_TEST_BUFFERS_MAX];

    qcarcam_buffers_t p_buffers_output;
    qcarcam_buffers_t p_buffers_disp;
    qcarcam_buffers_t p_buffers_input;

    qcarcam_res_t resolution;

    unsigned long long int frame_timeout;
    int use_event_callback;
    qcarcam_opmode_type op_mode;

    /* test util objects */
    test_util_ctxt_t *test_util_ctxt;
    test_util_window_t *qcarcam_window;
    test_util_window_t *display_window;
    test_util_window_param_t window_params;

    /* buffer management tracking */
    int get_frame_buf_idx;
    int buf_idx_qcarcam;
    std::list<uint32> release_buf_idx;

    int buf_idx_disp;
    int prev_buf_idx_disp;

    /* diag */
    int frameCnt;
    int releaseframeCnt;
    int prev_frameCnt;
    int prev_releaseframeCnt;
    bool is_first_start;
    bool is_injection;
    unsigned long long t_start; //start command
    unsigned long long t_start_success;
    unsigned long long t_firstFrame; //first frame time
    unsigned long long t_before;
    unsigned long long t_after;
    test_util_diag_t diag;
    bool dumpNextFrame;

    /* Exposure values */
    float exp_time;
    float gain;
    int manual_exposure;

    /* frame rate parameters */
    qcarcam_frame_rate_t frame_rate_config;

    qcarcam_test_buffer_state_t buf_state[QCARCAM_MAX_NUM_BUFFERS];

    bool skip_post_display;
    qcarcam_field_t field_type_previous;

    bool signal_lost;
    int fatal_err_cnt;

#ifdef ENABLE_CL_CONVERTER
    void* g_converter = NULL;
    ClConverter_surface_t source_surface;
    ClConverter_surface_t target_surface;
#endif

    /* subscription for changed setting events notification */
    uint64 subscribe_parameter_change;
    bool is_master;

    /* user process */
    uint32 delay_time;
    qcarcam_test_buf_timer buf_timer[QCARCAM_MAX_NUM_BUFFERS];

    std::list<uint32> usr_process_buf_idx;
    int usr_process_frameCnt;
    CameraThread usr_process_thread_handle;
    std::queue<qcarcam_event_msg_t> eventqueue;
    pthread_mutex_t queue_mutex;
} qcarcam_test_input_t;

typedef struct
{
    int numInputs;
    qcarcam_test_input_t inputs[NUM_MAX_CAMERAS];
    int opened_stream_cnt;

    /* 0 : post buffers directly to screen
     * 1 : blit buffers to display buffers through c2d
     */
    int enable_c2d;
#ifndef C2D_DISABLED
    pthread_mutex_t mutex_c2d;
#endif


    int dumpFrame;
    int enablePauseResume;
    int enableStartStop;
    int multithreaded;
    int enableStats;
    int enableMenuMode;
    int enableBridgeErrorDetect;
    int enableFatalErrorRecover;
    int enableIFEOverflowhandle;
    int enableRetry;
    int gpioNumber;
    int gpioMode;
    int disable_display;
    int enable_csc;

    int exitSeconds;

    int fps_print_delay;
    int vis_value;
    int check_buffer_state;

    /*abort condition*/
    pthread_mutex_t mutex_abort;
    pthread_cond_t cond_abort;
    pthread_mutex_t mutex_csi_err;
    pthread_mutex_t mutex_open_cnt;

    unsigned long long t_start; //program start
    int enable_deinterlace;
} qcarcam_test_ctxt_t;

typedef enum
{
    QCARCAM_TEST_MENU_FIRST_ITEM = 1,
    QCARCAM_TEST_MENU_STREAM_OPEN = QCARCAM_TEST_MENU_FIRST_ITEM,
    QCARCAM_TEST_MENU_STREAM_CLOSE,
    QCARCAM_TEST_MENU_STREAM_STOP,
    QCARCAM_TEST_MENU_STREAM_START,
    QCARCAM_TEST_MENU_STREAM_PAUSE,
    QCARCAM_TEST_MENU_STREAM_RESUME,
    QCARCAM_TEST_MENU_STREAM_STOP_ALL,
    QCARCAM_TEST_MENU_STREAM_START_ALL,
    QCARCAM_TEST_MENU_STREAM_PAUSE_ALL,
    QCARCAM_TEST_MENU_STREAM_RESUME_ALL,
    QCARCAM_TEST_MENU_STREAM_ENABLE_CALLBACK,
    QCARCAM_TEST_MENU_STREAM_DISABLE_CALLBACK,
    QCARCAM_TEST_MENU_STREAM_SET_FRAMERATE,
    QCARCAM_TEST_MENU_STREAM_SET_EXPOSURE,
    QCARCAM_TEST_MENU_STREAM_SET_COLOR_PARAM,
    QCARCAM_TEST_MENU_STREAM_GET_COLOR_PARAM,
    QCARCAM_TEST_MENU_STREAM_SET_GAMMA_PARAM,
    QCARCAM_TEST_MENU_STREAM_GET_GAMMA_PARAM,
    QCARCAM_TEST_MENU_STREAM_SET_ISP_PARAM,
    QCARCAM_TEST_MENU_STREAM_GET_ISP_PARAM,
    QCARCAM_TEST_MENU_DUMP_NEXT_FRAME,
    QCARCAM_TEST_MENU_CHECK_BUFFERS,
    QCARCAM_TEST_MENU_STREAM_SET_VENDOR_PARAM,
    QCARCAM_TEST_MENU_STREAM_GET_VENDOR_PARAM,
    QCARCAM_TEST_MENU_STREAM_SUBSCRIBE_CHANGE_EVENT,
    QCARCAM_TEST_MENU_STREAM_UNSUBSCRIBE_CHANGE_EVENT,
    QCARCAM_TEST_MENU_MASTER,
    QCARCAM_TEST_MENU_GET_SYSTEM_DIAGNOSTICS,
    QCARCAM_TEST_MENU_MAX
}qcarcam_test_menu_option_t;

typedef struct
{
    qcarcam_test_menu_option_t id;
    const char* str;
}qcarcam_test_menu_t;

typedef struct
{
    qcarcam_isp_param_t id;
    const char* str;
}qcarcam_isp_menu_t;

static qcarcam_isp_menu_t g_qcarcam_isp_menu[QCARCAM_ISP_PARAM_NUM] =
{
    {QCARCAM_CONTROL_AE_LOCK,  "AE Lock"},
    {QCARCAM_CONTROL_AE_MODE,  "AE Mode"},
    {QCARCAM_CONTROL_AWB_LOCK,  "AWB Lock"},
    {QCARCAM_CONTROL_AWB_MODE,  "AWB Mode"},
    {QCARCAM_CONTROL_EFFECT_MODE,  "Effect Mode"},
    {QCARCAM_CONTROL_MODE,  "Control Mode"},
    {QCARCAM_CONTROL_SCENE_MODE,  "Scene Mode"},
    {QCARCAM_CONTROL_AE_ANTIBANDING_MODE,  "AE antibanding Mode"},
};

static qcarcam_test_menu_t g_qcarcam_menu[QCARCAM_TEST_MENU_MAX] =
{
    {},
    {QCARCAM_TEST_MENU_STREAM_OPEN,  "Open a stream"},
    {QCARCAM_TEST_MENU_STREAM_CLOSE, "Close a stream"},
    {QCARCAM_TEST_MENU_STREAM_STOP,  "Stop a stream"},
    {QCARCAM_TEST_MENU_STREAM_START, "Start a stream"},
    {QCARCAM_TEST_MENU_STREAM_PAUSE, "Pause a stream"},
    {QCARCAM_TEST_MENU_STREAM_RESUME, "Resume a stream"},
    {QCARCAM_TEST_MENU_STREAM_STOP_ALL, "Stop all streams"},
    {QCARCAM_TEST_MENU_STREAM_START_ALL, "Start all streams"},
    {QCARCAM_TEST_MENU_STREAM_PAUSE_ALL, "Pause all streams"},
    {QCARCAM_TEST_MENU_STREAM_RESUME_ALL, "Resume all streams"},
    {QCARCAM_TEST_MENU_STREAM_ENABLE_CALLBACK, "Enable callback"},
    {QCARCAM_TEST_MENU_STREAM_DISABLE_CALLBACK, "Disable callback"},
    {QCARCAM_TEST_MENU_STREAM_SET_FRAMERATE, "Set frame rate control"},
    {QCARCAM_TEST_MENU_STREAM_SET_EXPOSURE, "Set exposure"},
    {QCARCAM_TEST_MENU_STREAM_SET_COLOR_PARAM, "Set color param"},
    {QCARCAM_TEST_MENU_STREAM_GET_COLOR_PARAM, "Get color param"},
    {QCARCAM_TEST_MENU_STREAM_SET_GAMMA_PARAM, "Set Gamma Table"},
    {QCARCAM_TEST_MENU_STREAM_GET_GAMMA_PARAM, "Get Gamma Table"},
    {QCARCAM_TEST_MENU_STREAM_SET_ISP_PARAM, "Set ISP settings"},
    {QCARCAM_TEST_MENU_STREAM_GET_ISP_PARAM, "Get ISP settings"},
    {QCARCAM_TEST_MENU_DUMP_NEXT_FRAME, "Dump Next Frame"},
    {QCARCAM_TEST_MENU_CHECK_BUFFERS, "Check Buffers"},
    {QCARCAM_TEST_MENU_STREAM_SET_VENDOR_PARAM, "Set Vendor Param"},
    {QCARCAM_TEST_MENU_STREAM_GET_VENDOR_PARAM, "Get Vendor Param"},
    {QCARCAM_TEST_MENU_STREAM_SUBSCRIBE_CHANGE_EVENT, "Subscribe for an event"},
    {QCARCAM_TEST_MENU_STREAM_UNSUBSCRIBE_CHANGE_EVENT, "Unsubscribe for an event"},
    {QCARCAM_TEST_MENU_MASTER, "Set/Release a client as master"},
    {QCARCAM_TEST_MENU_GET_SYSTEM_DIAGNOSTICS, "Get System diagnostic info"},
};

class AISAPI
{
public:
    /*qcarcam init, query inputs, open, and buffer*/
    int ais_initialize();
    
    /*qcarcam start, get frame, and release frame*/
    int ais_read();
    
    /*qcarcam stop and close*/
    int ais_release();

private:
    unsigned long long t_before = 0;
    unsigned long long t_after = 0;

    const char *tok = NULL;
    qcarcam_ret_t ret = QCARCAM_RET_OK;

    int rval = EXIT_FAILURE;
    unsigned int i = 0;
    int rc = 0;
    int input_idx = 0;

    CameraThread timer_thread_handle = NULL;
    CameraThread signal_thread_handle = NULL;
    CameraThread fps_thread_handle = NULL;
    CameraThread signal_loss_thread_handle = NULL;
    CameraThread csi_error_thread_handle = NULL;
    char thread_name[64];
    int bprint_diag = 0;

    test_util_intr_thrd_args_t intr_thrd_args;

    int remote_attach_loop = 0; // no looping for remote process attach

    test_util_ctxt_t* test_util_ctxt = NULL;
    test_util_ctxt_params_t test_util_ctxt_params = {};

    qcarcam_input_t *pInputs;
    unsigned int queryNumInputs = 0, queryFilled = 0;

    test_util_xml_input_t *xml_inputs = (test_util_xml_input_t *)calloc(NUM_MAX_CAMERAS, sizeof(test_util_xml_input_t));

    int fail_process();
};

}

#endif // AIS_API_H