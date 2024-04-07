#ifndef __SV_OEM_PM_CLIENT_LIB_H__
#define __SV_OEM_PM_CLIENT_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void * SV_OEM_PM_HANDLE_T;

struct _sv_oem_pm_client_s ;
typedef struct _sv_oem_pm_client_s* sv_oem_pm_client_t;

struct sv_oem_pm_ops_s {
    int (*suspend)(void *ctxt);
    int (*resume)(void *ctxt);
};

/**
 * @brief 
 * 
 * @param name name of client that is going to be registered to server
 * @param ops pointer to structure which consists function pointer
 * @param ctxt used by  client library
 * @param [out] handle output handle, to be used during deregister time to
 *                       free up resources allocated during registration.
 * @return int return 0 on success, non-zero on failure
 */
int sv_oem_pm_register(const char *name,
                const struct sv_oem_pm_ops_s *ops ,
                void *ctxt,
                sv_oem_pm_client_t *hdl);


/**
 * @brief This function frees up resources that allocated during registration time.
 * 
 * @param hdl Client handle, returned via sv_oem_pm_register()
 * @return int return 0 on success, non-zero on failure
 */
int sv_oem_pm_deregister(SV_OEM_PM_HANDLE_T hdl);


#ifdef __cplusplus
}
#endif

#endif // __SV_OEM_PM_CLIENT_LIB_H__