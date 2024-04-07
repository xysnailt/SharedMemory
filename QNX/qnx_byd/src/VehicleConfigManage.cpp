#include "VehicleConfigManage.hpp"

#include <iomanip>
#include <iostream>

int FunctionSwitchManage::SetFunctionSwitch(const FunctionType func_type, const bool enable) {
    if (m_func_bitmask_map.count(func_type) > 0) {
        SenseDriverModuleBitMask module_bitmask = m_func_bitmask_map[func_type];
        if (nullptr != m_dms_handle) {
            MDmsStatus ret = SenseDriverGetEnableModules(*m_dms_handle.get(), &module_bitmask);
            if (m_strategy == SwitchStrategy::switch_type) {
                ret = SenseDriverModuleSwitch(*m_dms_handle.get(), module_bitmask, enable);
            } else {
                ret = SenseDriverLoadModule(*m_dms_handle.get(), module_bitmask, enable);
            }
            if (0 == ret) {
                StoreFuncSwitchStatus(func_type, enable);
            }
            return ret;
        } else {
            std::cout << "dms handle is nullptr, please register\n";
            return -1;
        }
    }
    std::cout << "Dont't support func type " << std::hex << int(func_type) << "\n";
    return -1;
}

void FunctionSwitchManage::SetDmsWorkMode(const SDWorkMode work_mode) {
    if (WORK_MODE_INVALID != work_mode) {
        SDVehicleParameters vehicle_param;
        m_dms_workmode = work_mode;
        vehicle_param.work_mode = m_dms_workmode;
        vehicle_param.work_mode_optional = OPTIONAL_VALID;
        SenseDriverSetVehicleParameters(&vehicle_param);
    }
}

void FunctionSwitchManage::SetVehicleSingal(const SPI_REC& vehicle_sig) {
    SDVehicleParameters vehicle_param;
    if (vehicle_sig.hal_sensor_info.SpeedSignal144S <= speed_max) {
        vehicle_param.speed_optional = OPTIONAL_VALID;
        vehicle_param.speed = vehicle_sig.hal_sensor_info.SpeedSignal144S;
    }

    if (vehicle_sig.hal_sensor_info.SteeringWheelAngel11FS >= steering_wheel_angle_min ||
        vehicle_sig.hal_sensor_info.SteeringWheelAngel11FS <= steering_wheel_angle_max) {
        vehicle_param.steering_wheel_angle = OPTIONAL_VALID;
        vehicle_param.steering_wheel_angle = vehicle_sig.hal_sensor_info.SteeringWheelAngel11FS;
    }

    if (vehicle_sig.hal_sensor_info.TurnSignalWorkCondition38AS == turn_sig_left) {
        vehicle_param.turn_signal_optional = OPTIONAL_VALID;
        vehicle_param.turn_signal = TURN_LEFT;
    } else if (vehicle_sig.hal_sensor_info.TurnSignalWorkCondition38AS == turn_sig_right) {
        vehicle_param.turn_signal_optional = OPTIONAL_VALID;
        vehicle_param.turn_signal = TURN_RIGHT;
    }

    if (vehicle_sig.hal_sensor_info.BCMPower_Gear_12D == acc_on) {
        vehicle_param.acc_signal_optional = OPTIONAL_VALID;
        vehicle_param.acc_signal = ACC_ON;
    } else if (vehicle_sig.hal_sensor_info.BCMPower_Gear_12D == acc_off) {
        vehicle_param.acc_signal_optional = OPTIONAL_VALID;
        vehicle_param.acc_signal = ACC_OFF;
    }
    SenseDriverSetVehicleParameters(&vehicle_param);
}
