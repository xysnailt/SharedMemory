#ifndef PMGPIO_H
#define PMGPIO_H

/** @file pm_gpio.h
*
*  PMIC-GPIO MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*   the PMIC GPIO module.
*/
/*
*  Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/plt_core.qxa_qa/1.0/pmic/public/amss/core/pm_gpio.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/01/13   kt      Added pm_dev_gpio_get_peripheral_info API.
07/05/12   hs      Updated the interface.
03/01/12   hs      Updated the interface.
04/26/11   wra     Adding more GPIO enumeration needed from PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
Added struct pm_gpio_config_type
Added pm_gpio_config() and pm_gpio_set_interrupt_polarity()
08/11/10   wra     Removed doxygen quotations. They are causing parsing errors
07/26/10   wra     Changed documentation from @example to Examples so Deoxygen 
can parse the file
07/09/10   jtn     Added API to get GPIO status
07/02/10   wra     Changed pm_gpio_perph_index to int
06/23/10   vk      Added pm_gpio_set_mux_ctrl()
03/15/10   fpe     Removed RPC remoting because the application processor can do this directly
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
10/19/09   vk      Removed init API
08/01/09   vk      Modified gpio_digital_input prototype
05/20/09   vk      New file
===========================================================================*/

#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/** @addtogroup pm_gpio
@{ */

/**
Following api's can be used to configure GPIO as Input
  - To configure the mode of GPIO to Input
    pm_gpio_cfg_mode 
    
  - To configure Pull Up or Pull Down 
    pm_gpio_set_cur_src_pull
  
  - To configure input voltage levels for GPIO
    pm_gpio_set_voltage_source

  - To enable GPIO, since gpio's are not enabled by default
    they should be enabled after configuration 
    pm_gpio_enable 
    
  - To get the status of input level high or low 
    pm_gpio_level_status


    
Following api's can be used to configure GPIO as Input
  
  - To configure mode of GPIO to output
    pm_gpio_cfg_mode
    
    - To configure Output buffer type eg. CMOS, Open Drain High, Open Drain Low
    pm_gpio_set_out_buf_cfg
    
  - To configure drive strength of GPIO 
    pm_gpio_set_out_buf_drv_str
    
  - To configure voltage levels for GPIO
    pm_gpio_set_voltage_source
    
  - To drive GPIO level high or low 
    pm_gpio_set_output_level
  
  - To enable GPIO, since gpio's are not enabled by default
    they should be enabled after configuration 
    pm_gpio_enable
  
  -  To check the configuration of output level 
    pm_gpio_level_status
  
  - To check configuration of various parameters 
    pm_gpio_reg_status_get
    
*/



/** Type definition for different GPIOs
*/
typedef enum
{
    PM_GPIO_1,     /**< GPIO 1. */
    PM_GPIO_2,     /**< GPIO 2. */
    PM_GPIO_3,     /**< GPIO 3. */
    PM_GPIO_4,     /**< GPIO 4. */
    PM_GPIO_5,     /**< GPIO 5. */
    PM_GPIO_6,     /**< GPIO 6. */
    PM_GPIO_7,     /**< GPIO 7. */
    PM_GPIO_8,     /**< GPIO 8. */
    PM_GPIO_9,     /**< GPIO 9. */
    PM_GPIO_10,    /**< GPIO 10. */
    PM_GPIO_11,    /**< GPIO 11. */
    PM_GPIO_12,    /**< GPIO 12. */
    PM_GPIO_13,    /**< GPIO 13. */
    PM_GPIO_14,    /**< GPIO 14. */
    PM_GPIO_15,    /**< GPIO 15. */
    PM_GPIO_16,    /**< GPIO 16. */
    PM_GPIO_17,    /**< GPIO 17. */
    PM_GPIO_18,    /**< GPIO 18. */
    PM_GPIO_19,    /**< GPIO 19. */
    PM_GPIO_20,    /**< GPIO 20. */
    PM_GPIO_21,    /**< GPIO 21. */
    PM_GPIO_22,    /**< GPIO 22. */
    PM_GPIO_23,    /**< GPIO 23. */
    PM_GPIO_24,    /**< GPIO 24. */
    PM_GPIO_25,    /**< GPIO 25. */
    PM_GPIO_26,    /**< GPIO 26. */
    PM_GPIO_27,    /**< GPIO 27. */
    PM_GPIO_28,    /**< GPIO 28. */
    PM_GPIO_29,    /**< GPIO 29. */
    PM_GPIO_30,    /**< GPIO 30. */
    PM_GPIO_31,    /**< GPIO 31. */
    PM_GPIO_32,    /**< GPIO 32. */
    PM_GPIO_33,    /**< GPIO 33. */
    PM_GPIO_34,    /**< GPIO 34. */
    PM_GPIO_35,    /**< GPIO 35. */
    PM_GPIO_36,    /**< GPIO 36. */
    PM_GPIO_37,    /**< GPIO 37. */
    PM_GPIO_38,    /**< GPIO 38. */
    PM_GPIO_39,    /**< GPIO 39. */
    PM_GPIO_40,    /**< GPIO 40. */
    PM_GPIO_41,    /**< GPIO 41. */
    PM_GPIO_42,    /**< GPIO 42. */
    PM_GPIO_43,    /**< GPIO 43. */
    PM_GPIO_44     /**< GPIO 44. */ 
}pm_gpio_perph_index;


/** Select GPIO I/O type
*/
typedef enum
{
    PM_GPIO_DIG_IN,
    PM_GPIO_DIG_OUT,
    PM_GPIO_DIG_IN_DIG_OUT,
    PM_GPIO_ANA_PASS_THRU,
    PM_GPIO_TYPE_INVALID
} pm_gpio_config_type;

/** Select voltage source.
*/
typedef enum
{
    PM_GPIO_VIN0,  /**< Voltage input 0. */
    PM_GPIO_VIN1,  /**< Voltage input 1. */
    PM_GPIO_VIN_INVALID
}pm_gpio_volt_src_type;

// /** Select output level.
// */
// typedef enum
// {
    // PM_GPIO_OUT_LEVEL_LOW,             /**< Set Output Level High. */
    // PM_GPIO_OUT_LEVEL_HIGH,            /**< Set Output Level Low.  */
    // PM_GPIO_OUT_LEVEL_INVALID
// }pm_gpio_out_level_type;

/** Input/Output level Status.
*/
typedef enum
{
    PM_GPIO_LEVEL_LOW,             /**< Level of pin is low. */
    PM_GPIO_LEVEL_HIGH,            /**< Level of pin is high.  */
    PM_GPIO_LEVEL_INVALID
}pm_gpio_level_type;


/** Select output buffer configuration.
*/
typedef enum
{
    PM_GPIO_OUT_BUFFER_CONFIG_CMOS,             /**< CMOS output. */
    PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_NMOS,  /**< Open drain NMOS output. */
    PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_PMOS,  /**< Open drain PMOS output. */
    PM_GPIO_OUT_BUFFER_CONFIG_INVALID
}pm_gpio_out_buf_cfg_type;

// /** External pin output inversion type.
// */
// typedef enum
// {
  // PM_GPIO_INVERT_EXT_PIN_OUTPUT_DISABLE, /**< Ext pin output is not inverted. */
  // PM_GPIO_INVERT_EXT_PIN_OUTPUT_ENABLE,  /**< Ext pin output is inverted. */
  // PM_GPIO_INVERT_EXT_PIN_OUTPUT_INVALID 
// }pm_gpio_invert_ext_pin_type;

typedef enum
{
    PM_GPIO_OUT_BUFFER_RESERVED,  /**< Output buffer strength reserved. */
    PM_GPIO_OUT_BUFFER_LOW,       /**< Output buffer strength low. */
    PM_GPIO_OUT_BUFFER_MEDIUM,    /**< Output buffer strength medium. */
    PM_GPIO_OUT_BUFFER_HIGH,      /**< Output buffer strength high. */
    PM_GPIO_OUT_BUFFER_INVALID
}pm_gpio_out_buf_drv_str_type;


/** Select current source pulls type.
*/
typedef enum
{
    PM_GPIO_I_SRC_PULL_UP_30uA,                   /**< Pull up 30 uA. */
    PM_GPIO_I_SRC_PULL_UP_1_5uA,                  /**< Pull up 1.5 uA. */
    PM_GPIO_I_SRC_PULL_UP_31_5uA,                 /**< Pull up 31.5 uA. */
    PM_GPIO_I_SRC_PULL_UP_1_5uA_PLUS_30uA_BOOST,  /**< Pull up 1.5 uA plus 30 uA boost. */
    PM_GPIO_I_SRC_PULL_DOWN_10uA,                 /**< Pull down 10 uA. */
    PM_GPIO_I_SRC_PULL_NO_PULL,                   /**< No pull. */
    PM_GPIO_I_SRC_PULL_INVALID
}pm_gpio_i_src_pull_type;

/** GPIO source select.
*/
typedef enum
{
    PM_GPIO_SOURCE_GND,                  /**< Ground. */
    PM_GPIO_SOURCE_PAIRED_GPIO,          /**< Paired GPIO. */
    PM_GPIO_SOURCE_SPECIAL_FUNCTION1,    /**< Special function 1. */
    PM_GPIO_SOURCE_SPECIAL_FUNCTION2,    /**< Special function 2. */
    PM_GPIO_SOURCE_SPECIAL_FUNCTION3,    /**< Special function 3. */
    PM_GPIO_SOURCE_SPECIAL_FUNCTION4,    /**< Special function 4. */
    PM_GPIO_SOURCE_DTEST1,               /**< D-test 1. */
    PM_GPIO_SOURCE_DTEST2,               /**< D-test 2. */
    PM_GPIO_SOURCE_DTEST3,               /**< D-test 3. */
    PM_GPIO_SOURCE_DTEST4,               /**< D-test 4. */
    PM_GPIO_SOURCE_INVALID
}pm_gpio_src_cfg_type;

// /** D-test buffer enable/disable.
// */
typedef enum
{
    PM_GPIO_DTEST_DISABLE,  /**< GPIO D-test disable. */
    PM_GPIO_DTEST_ENABLE,   /**< GPIO D-test enable. */
    PM_GPIO_DTEST_INVALID
}pm_gpio_dtest_buf_en_type;

/** External pin configuration.
*/
typedef enum
{
    PM_GPIO_EXT_PIN_DISABLE,  /**< Disable EXT_PIN. */
    PM_GPIO_EXT_PIN_ENABLE,   /**< Puts EXT_PIN at high Z state and disables
                            // the block. */
    PM_GPIO_EXT_PIN_CONFIG_TYPE__INVALID    
}pm_gpio_ext_pin_config_type;

/**
* Structure used to return GPIO status. This structure includes all of the
* enums that are used when configuring the GPIOs.
*/
typedef struct 
{
    boolean                          enable;                       /**GPIO Enable Config*/
    pm_gpio_config_type              mode_select;             /**< GPIO mode selection */
    pm_gpio_volt_src_type            volt_src;                /**< GPIO voltage source. */
    pm_gpio_out_buf_cfg_type         out_buf_cfg;             /**< GPIO output buffer configuration. */
    pm_gpio_level_type               level;               /**< GPIO invert EXT_PIN configuration. */
    pm_gpio_out_buf_drv_str_type         out_buf_drv_str;         /**< GPIO output buffer driver strength. */
    pm_gpio_i_src_pull_type          i_src_pulls;             /**< GPIO current source pulls. */
    pm_gpio_src_cfg_type             src_cfg;                 /**< GPIO source configuration. */
    pm_gpio_dtest_buf_en_type        dtest_buffer_on_off;     /**< GPIO D-text buffer on/off. */
} pm_gpio_status_type;

/*=========================================================================== */
/*                     FUNCTION DEFINITIONS                                   */   
/*=========================================================================== */

/*=========================================================================== */
/*                        pm_gpio_enable                                      */
/*=========================================================================== */

/** 
*  Enables GPIO.
* 
* @param[in] pmic_chip Each PMIC device in the system is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure voltage source. See
*                 #pm_gpio_perph_index.
* @param[in] enable Set TRUE to enable GPIO, FALSE to disable.
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  Enable GPIO5:
* @code
*  errFlag = pm_gpio_enable(0, PM_GPIO_5, TRUE); @endcode
*/
pm_err_flag_type 
pm_gpio_enable(uint8 pmic_chip, pm_gpio_perph_index  gpio, boolean enable);


/*=========================================================================== */
/*                        pm_gpio_cfg_mode                                    */
/*=========================================================================== */
/** 
*  Set GPIO Configuration to Input, Output, InOut.
* 
* @param[in] pmic_chip Each PMIC device in the system is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure voltage source. See
*                 #pm_gpio_perph_index.
* @param[in] type Configuration type. See pm_gpio_config_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  Configure GPIO5 to Output:
* @code
*  errFlag = pm_gpio_cfg_in_out(0, PM_GPIO_5, PM_GPIO_DIG_OUT); @endcode
*/
pm_err_flag_type
pm_gpio_cfg_mode(uint8 pmic_chip, 
                 uint8  gpio, 
                 pm_gpio_config_type type);


/*=========================================================================== */
/*                        pm_gpio_level_status                                */
/*=========================================================================== */
/** 
* Get input level or configuration of output level for a given gpio.
* 
* @param[in] pmic_chip Each PMIC device in the system is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure voltage source. See
*                 #pm_gpio_perph_index.
* @param[in] type Configuration type. See pm_gpio_config_type.
* @param[out] level pointer to level 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  Get GPIO5 to Output level:
* @code
*  errFlag = pm_gpio_level_status(0, PM_GPIO_5, 
*                                 PM_GPIO_DIG_OUT, &level); @endcode
*/
pm_err_flag_type 
pm_gpio_level_status(uint8 pmic_chip, 
                     pm_gpio_perph_index gpio,
                     pm_gpio_config_type type,
                     pm_gpio_level_type *level_status);


/*=========================================================================== */
/*                        pm_gpio_set_voltage_source                          */
/*=========================================================================== */

/** 
*  Sets the voltage source.
* 
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure voltage source. See
*                 #pm_gpio_perph_index.
* @param[in] voltage_source GPIO voltage source. See
*                           #pm_gpio_voltage_source_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  Set voltage source to VIN2 for GPIO5:
* @code
*  errFlag = pm_gpio_set_voltage_source(0, PM_GPIO_5, 
*                                       PM_GPIO_VIN2); @endcode
*/
pm_err_flag_type 
pm_gpio_set_voltage_source(uint8 pmic_chip, 
                           pm_gpio_perph_index gpio, 
                           pm_gpio_volt_src_type  voltage_source);

/*=========================================================================== */
/*                        pm_gpio_set_out_buf_cfg                             */
/*=========================================================================== */

/**
*  Sets the output buffer configuration.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure the current source pulls. See
*                 #pm_gpio_perph_index.
* @param[in] out_buffer_config GPIO output buffer configuration: 0 -- CMOS;
*                              1 -- open drain. See 
*                              #pm_gpio_out_buf_cfg_type.
* 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  Set the output buffer configuration to CMOS in bias mode for GPIO5:
* @code
*  errFlag = pm_gpio_set_out_buf_cfg(0, PM_GPIO_5, 
*                                    PM_GPIO_OUT_BUFFER_CONFIG_CMOS); @endcode 
*/
pm_err_flag_type 
pm_gpio_set_out_buf_cfg(uint8 pmic_chip, 
                        pm_gpio_perph_index gpio, 
                        pm_gpio_out_buf_cfg_type out_buffer_config);
                        
                        
/*=========================================================================== */
/*                        pm_gpio_set_output_level()                          */
/*=========================================================================== */

/**
*  Sets level for output.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                      starting with zero.
* @param[in] gpio      GPIO for which to set the inversion configuration. 
*                      See #pm_gpio_perph_index.
* @param[in] level     Output level to be set for GPIO 
*                      see #pm_gpio_output_level_type
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
* 
*  <b>Example </b> \n
*  For  GPIO5, set output high:
* @code
*  errFlag = pm_gpio_set_output_level(0, PM_GPIO_5, 
                                      PM_GPIO_OUTPUT_LEVEL_HIGH); @endcode
*/
pm_err_flag_type 
pm_gpio_set_output_level(uint8 pmic_chip, 
                         pm_gpio_perph_index gpio, 
                         pm_gpio_level_type level);

/*=========================================================================== */
/*                        pm_gpio_set_cur_src_pull                            */
/*=========================================================================== */

/**
*  Sets the current source pulls.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure current source pulls. See
*                 #pm_gpio_perph_index.
* @param[in] i_source_pulls Current source pulls. See
*                           #pm_gpio_current_src_pulls_type.
* 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*  Set the current source pulls to 1.5 uA + 30 uA boost for GPIO5:
* @code
*  errFlag = pm_gpio_set_cur_src_pull(0, PM_GPIO_5, 
*                              PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST); @endcode
*/
pm_err_flag_type 
pm_gpio_set_cur_src_pull(uint8 pmic_chip, 
                         pm_gpio_perph_index gpio, 
                         pm_gpio_i_src_pull_type  i_src_pull);

/*=========================================================================== */
/*                        pm_gpio_set_out_buf_drv_str                         */
/*=========================================================================== */

/**
*  Sets the output buffer drive strength.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to configure current source pulls. See
*                 #pm_gpio_perph_index.
* @param[in] out_buffer_strength GPIO output buffer drive strength.
*                                #See #pm_gpio_out_buf_drv_str_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
* 
*  <b>Example </b> \n
*  Set the output buffer drive strength for GPIO5 to HIGH:
* @code
*  errFlag = pm_gpio_set_out_buf_drv_str(0, PM_GPIO_5, 
*                                        PM_GPIO_OUT_BUFFER_HIGH); @endcode
*/
pm_err_flag_type
pm_gpio_set_out_buf_drv_str(uint8 pmic_chip,
                            pm_gpio_perph_index gpio,
                            pm_gpio_out_buf_drv_str_type out_buffer_strength);

/*======================================================================= */
/*                        pm_gpio_set_out_src_cfg                             */
/*======================================================================= */

/**
*  Sets the source configuration.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated
*                              starting with zero.
* @param[in] gpio GPIO for which to set the source configuration.
*                 See #pm_gpio_perph_index</td>
* @param[in] source Select the source. See #pm_gpio_src_cfg_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
* 
*  <b>Example </b> \n
*  To serve GPIO5 as an output in Level Translator mode, select Pair In as
*  the source:
* @code
* errFlag=pm_gpio_set_src_cfg(0, PM_GPIO_5, 
*                             PM_GPIO_SOURCE_PAIRED_GPIO); @endcode
*/
pm_err_flag_type 
pm_gpio_set_out_src_cfg(uint8 pmic_chip, 
                        pm_gpio_perph_index gpio, 
                        pm_gpio_src_cfg_type  source);

/*======================================================================= */
/*                        pm_gpio_irq_enable                              */
/*======================================================================= */
                    
 /**
 * @brief This function enables or disables gpio irq
 * 
 * @param[in] pmic_chip. Primary: 0. Secondary: 1
 * @param[in] gpio:  
 *                GPIO number. Refer enum pm_gpio_perph_index.
 *  
 * @param[in] enable:  
 *                TRUE: Enable interrupt for the gpio 
 *                FALSE:Disable interrupt for the gpio
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type 
pm_gpio_irq_enable(uint8 pmic_chip, 
                   pm_gpio_perph_index gpio, 
                   boolean enable);


/*======================================================================= */
/*                        pm_gpio_irq_clear                               */
/*======================================================================= */
/**
 *  Clears the GPIO IRQ. 
 *
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] gpio GPIO number. See #pm_gpio_perph_index.  
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_gpio_irq_clear(uint8 pmic_chip, pm_gpio_perph_index gpio);

/*======================================================================= */
/*                        pm_gpio_irq_set_trigger                         */
/*======================================================================= */
/**
 *  Configures the GPIO for an IRQ. 
 *
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] gpio GPIO number. See #pm_gpio_perph_index.
 * @param[in] trigger One of the various IRQ triggers. See
 *                    #pm_irq_trigger_type.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_gpio_irq_set_trigger(uint8 pmic_chip, 
                        pm_gpio_perph_index gpio, 
                        pm_irq_trigger_type trigger);


/*======================================================================= */
/*                        pm_gpio_irq_status                              */
/*======================================================================= */
/**
 *  Gets the GPIO IRQ status. 
 *
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] gpio GPIO number. See #pm_gpio_perph_index.
 * @param[in] type Type of IRQ status to read. See #pm_irq_status_type.
 * @param[out] status IRQ status.
 *
 * @return
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_gpio_irq_status(uint8 pmic_chip, 
                   pm_gpio_perph_index gpio, 
                   pm_irq_status_type irq_type, 
                   boolean *status);

/*======================================================================= */
/*                        pm_gpio_reg_status_get                          */
/*======================================================================= */
 /**
*  Returns the status of one of the PMIC GPIOs.
* 
* @param[in] pmic_chip Each PMIC device in the systems is enumerated,
*                      starting with zero.
* @param[in] gpio GPIO identifier.
* @param[out] gpio_status Pointer to the GPIO status.
* 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type 
pm_gpio_reg_status_get(uint8 pmic_chip, pm_gpio_perph_index gpio, 
                   pm_gpio_status_type *gpio_status);




/*======================================================================= */
/*                        pm_gpio_get_perph_id                            */
/*======================================================================= */
 /**
*  Returns peripheral ID for gpio.
* 
* @param[in] pmic_chip Each PMIC device in the systems is enumerated,
*                      starting with zero.
* @param[in] gpio GPIO identifier.
* @param[out] addr Pointer to the .
* 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*/
                       
pm_err_flag_type 
pm_gpio_get_perph_id(uint8 pmic_chip, 
                     pm_gpio_perph_index gpio, 
                     uint32 *addr);


/*======================================================================= */
/*                        pm_gpio_create_channel                            */
/*======================================================================= */
 /**
*  Associate spmi channel with gpio
*  if *chid = -1 , then a new channel id will be created
*  if *chid !=-1 , then *chid will be considered a valid existing channel and
*  no new channel will be created . The existing *chid will be associated with
*  the gpio.
*  This way we can use same channel for multiple gpios if needed. However it
*  is not required that we use same channel for multiple gpios or vice versa
*  To be used with pmic client only.
*
* @param[in] pmic_chip Each PMIC device in the systems is enumerated,
*                      starting with zero.
* @param[in] gpio GPIO identifier.
* @param[out] channel id.
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type
pm_gpio_create_channel(uint8 pmic_chip , pm_gpio_perph_index gpio , int *chid);

           
/*======================================================================= */
/*                        pm_gpio_wait_for_irq                            */
/*======================================================================= */
 /**
*  Waits for interrupt on gpio. To be used with pmic client only. 
* 
* @param[in] pmic_chip Each PMIC device in the systems is enumerated,
*                      starting with zero.
* @param[in] gpio GPIO identifier.
*  
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*/           
           
pm_err_flag_type
pm_gpio_wait_for_irq(uint8 pmic_chip, pm_gpio_perph_index gpio, int chid);
/** @} */ /* end_addtogroup pm_gpio */

#endif /* PMGPIO_H */
