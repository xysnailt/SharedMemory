#ifndef _SV_LOG_H_
#define _SV_LOG_H_

#include <stdarg.h>
#include <stdio.h>
#ifdef __FILENAME__
	#define __BASEFILE__ __FILENAME__
#else
	#include <libgen.h>
	#define __BASEFILE__ basename(__FILE__)
#endif

#include <inttypes.h>

#include <sys/slog.h>
#include <sys/neutrino.h>

#if ( _NTO_VERSION > 650 )
#define LOGGER_USE_SLOG2
#include <sys/slog2.h>
#endif

#include <sys/slogcodes.h>

extern int severity_level() __attribute__((weak));

/* The basename of the program being executed
* This variable isn't defined in any header file.
* If you want to refer to it, you need to add your own extern statement
*/
extern char * __progname;

/**
  @brief: logger_log - Prints debug information to slog2/slog, along with
  the process name, file name, and line number that called this macro.

  @param1: majorid  - Comply with _LOGGER_MAJOR enum from logger_codes.h
  @param2: minorid  - Additional granularity. These are to be definied
                      individually by each technical area.
  @param3: severity - Comply with _LOGGER_SEVERITY from logger_codes.h
  @param4: fmt      - printf styled string

  @return: None

  @note: If slog2f() is invoked with NULL handle, then it logs to the
         buffer that is previously set with the slog2_set_default_buffer()
         call. However if the no default buffer is set then slog2f will
         just silently drop the log, which is unacceptable as QC code can
         run in processes that do not set the default buffer at all.
         Ideally, slog2() should re-direct such messages to legacy slogger.

         Until this behaviour is fixed in slog2f() or everyone moves to
         slog2(), the extra call to figure out the default buffer is
         necessary.
 */
#ifdef LOGGER_USE_SLOG2
#define logger_log(majorid, minorid, severity, fmt, ... ) \
	do{\
		slog2_buffer_t __amss_default_slog2_handle;\
		if(NULL!=(__amss_default_slog2_handle=\
				slog2_set_default_buffer((slog2_buffer_t)-1))){\
			slog2f(__amss_default_slog2_handle,(majorid),(severity),\
					"%s[%s,%s:%d]: " fmt,__progname,__BASEFILE__, __func__,\
					__LINE__, ##__VA_ARGS__);\
			break;\
		}else{\
			slogf( _SLOG_SETCODE((majorid), (minorid)), (severity),\
					"%s[%s:%s():%d]: " fmt,__progname, __BASEFILE__,__func__,\
					__LINE__, ## __VA_ARGS__);\
		}\
	}while(0)
#else
#define logger_log(majorid, minorid, severity, fmt, ... ) \
	do{\
		if(severity_level != NULL) {\
			if(severity <= severity_level()) {\
				slogf( _SLOG_SETCODE((majorid), (minorid)), (severity), "%s[%s:%d]: " fmt,__progname, __BASEFILE__, __LINE__, ## __VA_ARGS__);\
			}\
		}else{\
			slogf( _SLOG_SETCODE((majorid), (minorid)), (severity),\
					"%s[%s:%d]: " fmt,__progname, __BASEFILE__,\
					__LINE__, ## __VA_ARGS__);\
		}\
	}while(0)
#endif




#define LOG_MAJOR_ID  20100
#define LOG(sev, format, ...) \
		logger_log(LOG_MAJOR_ID, 1, sev, format, ##__VA_ARGS__)
#define LOGERR(format, ...) LOG(_SLOG_ERROR, format, ##__VA_ARGS__)
#define LOGWAR(format, ...) LOG(_SLOG_WARNING, format, ##__VA_ARGS__)
#define LOGINF(format, ...) LOG(_SLOG_INFO, format, ##__VA_ARGS__)

void log_data_spi(char *name,int len,char *data);
void log_open(int flag);
void log_close();

#endif
