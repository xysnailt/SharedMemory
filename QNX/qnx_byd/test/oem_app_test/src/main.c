#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include "sv_oem_pm_client_lib.h"

#include "sv_log.h"


sv_oem_pm_client_t hdl;
struct sv_oem_pm_ops_s ops;


int suspend_callback(void * ctx) {
    printf("oem_app_test suspend_callback, pid=%d\n", getpid());
	LOGINF("oem_app_test suspend_callback, pid=%d\n", getpid());
    return 0;
}


int resume_callback(void * ctx) {
    printf("oem_app_test resume_callback, pid=%d\n", getpid());
	LOGINF("oem_app_test resume_callback, pid=%d\n", getpid());
    return 0;
}

static int oem_pm_pulse_register(){
	int ret;
	// memset ops 
	memset((char *)&ops, 0, sizeof(ops));
	
	ops.suspend = suspend_callback;
    ops.resume = resume_callback;
	ret = sv_oem_pm_register("oem_app", &ops, NULL, &hdl);

	if (0 != ret)
	{
		printf("oem_pm register failed: %d\n", ret);
	}

	return ret;
}



int main(int argc, char *argv[ ]) {
   printf("---------------------- main 1\n");
   oem_pm_pulse_register();

   while(1) {
        pause();
   }


    return 0;
}
