#pragma once
#include "PpsDataType.h"
#include "sensedriver_interface.h"

#include <map>
#include <memory>

class FunctionSwitchManage final {
public:
    enum class SwitchStrategy { switch_type = 0, load_type };
    enum class FunctionType { drowsiness_type = 1, distraction_type, occlusion_type };

public:
    static FunctionSwitchManage& Instance() {
        static FunctionSwitchManage s_func_switch_manage;
        return s_func_switch_manage;
    }

    void SetSwitchStrategy(const SwitchStrategy strategy) { m_strategy = strategy; }
    int SetFunctionSwitch(const FunctionType func_type, const bool enable);
    bool GetFunctionSwitch(const FunctionType func_type, bool& status) const {
        if (m_switch_status.count(func_type) > 0) {
            status = m_switch_status.at(func_type);
            return true;
        }
        return false;
    }

    void SetDmsWorkMode(const SDWorkMode work_mode);
    SDWorkMode GetDmsWorkMode() const { return m_dms_workmode; }

    void SetVehicleSingal(const SPI_REC& vehicle_sig);

    void DmdHandleRegister(std::shared_ptr< SDHandle > dms_handle) { m_dms_handle = dms_handle; }

private:
    FunctionSwitchManage() = default;
    ~FunctionSwitchManage() = default;

    void StoreFuncSwitchStatus(const FunctionType func_type, bool status) {
        if (m_switch_status.count(func_type) > 0) {
            m_switch_status[func_type] = status;
        }
    }

    // 信号定义见《AVM&DMS_PPS协议_231206.xlsm》
    static constexpr uint8_t speed_max{240};
    static constexpr int steering_wheel_angle_min{-7800}; // unit: 0.1°
    static constexpr int steering_wheel_angle_max{7799};  // unit: 0.1°
    static constexpr uint8_t turn_sig_left{0x2};
    static constexpr uint8_t turn_sig_right{0x4};
    static constexpr uint8_t acc_on{0x2};
    static constexpr uint8_t acc_off{0x1};

private:
    std::map< FunctionType, SenseDriverModuleBitMask > m_func_bitmask_map{
        {FunctionType::drowsiness_type, SENSEDRIVER_MODULE_DROWSINESS},
        {FunctionType::distraction_type, SENSEDRIVER_MODULE_DISTRACTION},
        {FunctionType::occlusion_type, SENSEDRIVER_MODULE_CAMERA_OCC}};
    SwitchStrategy m_strategy{SwitchStrategy::switch_type};
    std::map< FunctionType, bool > m_switch_status{{FunctionType::drowsiness_type, true},
                                                   {FunctionType::distraction_type, true},
                                                   {FunctionType::occlusion_type, true}};
    SDWorkMode m_dms_workmode{STANDARD_MODE};
    std::shared_ptr< SDHandle > m_dms_handle{nullptr};
};
