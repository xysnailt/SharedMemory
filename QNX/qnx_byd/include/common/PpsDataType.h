#ifndef PPSDATATYPE_H
#define PPSDATATYPE_H

#include <array>
#include <iostream>
#include <vector>

typedef struct WheelMovDirect {
    uint8_t RRWheelMovDirect;
    uint8_t RLWheelMovDirect;
    uint8_t FRWheelMovDirect;
    uint8_t FLWheelMovDirect;
} WheelMovDirect;

typedef struct WheelPuls {
    uint8_t FLWheelPulsCountValid;
    uint8_t FRWheelPulsCountValid;
    uint8_t RLWheelPulsCountValid;
    uint8_t RRWheelPulsCountValid;
    uint8_t reserved;
    uint16_t FLWheelPulsCount;
    uint16_t FRWheelPulsCount;
    uint16_t RLWheelPulsCount;
    uint16_t RRWheelPulsCount;
} WheelPuls;

typedef struct RadarProbeState {
    uint8_t RadarProbeStateFL;
    uint8_t RadarProbeStateFML;
    uint8_t RadarProbeStateFMR;
    uint8_t RadarProbeStateFR;
    uint8_t RadarProbeStateMR;
    uint8_t RadarProbeStateRL;
    uint8_t RadarProbeStateRML;
    uint8_t RadarProbeStateRMR;
    uint8_t RadarProbeStateRR;
} RadarProbeState;

typedef struct VehicleInfo {
    WheelMovDirect WheDirect;
    WheelPuls WhePuls;
    uint8_t TurnSignalSwitch;
    uint8_t TurnSignalStatus108;
    uint8_t dragModeState;
    uint8_t TrailerModeConfigR;
    uint8_t AVMAPAAbortReason;
    RadarProbeState RadarProbeSt;
    uint8_t NewReverseRadarSwitchStateR;
    uint8_t ReverseRadarSwitchStateR;
    uint8_t reserved_1;
    uint8_t GearboxAutoModeType;
    uint8_t Left_Front_Door_Status_294;
    uint8_t RightFrontDoorStatus294;
    uint8_t Left_Back_Door_Status_294;
    uint8_t Right_Back_Door_Status_294;
    uint8_t reserved_2;
    uint8_t DoorStateLuggage;
    uint8_t DoorStateHood;
    uint8_t SunRoofStateR;
    uint8_t DoorLockStatusAreaBack;
    uint8_t DoorLockStatusAreaLeftFront;
    uint8_t DoorLockStatusRightFront;
    uint8_t DoorLockStatusLeftRear;
    uint8_t DoorLockStatusAreaRightRear;
    uint8_t reserved_3;
    uint8_t OtaPowerLevel;
    uint16_t VehicleSpeed;
    uint16_t SteeringWheelValueAngel;
    uint8_t AutoExternalRearMirrorFoldActionStateR;
    uint8_t DayTimeLightState;
    uint8_t StopLightStateR;
    uint8_t ReversingLightState;
    uint8_t TurnLightState;
    uint8_t LightSideStatusR;
    uint8_t LightLowBeamStatusR;
    uint8_t LightHighBeamR;
    uint8_t LightRearPositionSignalR;
    uint8_t LightRearFog;
    uint8_t SupportAutoParking;
    uint8_t ValetParkingR;
    uint8_t MemoryParkingR;
    uint8_t reserved_4;
    uint8_t ScreenAngleR;
    uint8_t ScreenRotationActionR;
    uint8_t reserved_5;
    uint8_t Veer;
    uint8_t AcOpenState;
    uint8_t reserved_6;
    uint8_t AcControllerDriverTempR;
    uint8_t AcControllerPassengerTempR;
    uint8_t AcFrontWindLevelR;
    uint8_t AcTempCtrlSeparateState;
    uint8_t AcDefrostR;
    uint8_t AcCircleMode;
    uint8_t AcVentilationStateR;
    uint8_t TemperatureUnit;
    uint8_t AcFaultNumShownState;
    uint8_t AcDefrost;
    uint8_t AutoType;
    uint8_t AcInteraction;
    uint8_t PanoramaBtInfo;
} VehicleInfo;

typedef struct HalSensorInfo {
    uint8_t BCMPower_Gear_12D;
    uint8_t reserve_1;
    uint8_t RightLineTrackStatus;
    uint8_t LeftLineTrackStatus;
    uint8_t SwsMediumRelatedKeysS;
    uint8_t SpeedSignal144S;
    uint16_t SteeringWheelAngel11FS;
    uint8_t SteeringWheelRotationSpeedS;
    uint8_t TurnSignalWorkCondition38AS;
    uint8_t DNP_Stats_SDNP;
    uint8_t reserve_2;
} HalSensorInfo;

typedef struct AvmLens {
    std::array< uint8_t, 8 > lens_fx;
    std::array< uint8_t, 8 > lens_fy;
    std::array< uint8_t, 8 > lens_cx;
    std::array< uint8_t, 8 > lens_cy;
    std::array< uint8_t, 8 > lens_k1;
    std::array< uint8_t, 8 > lens_k2;
    std::array< uint8_t, 8 > lens_k3;
    std::array< uint8_t, 8 > lens_k4;
} AvmLens;

typedef struct AvmInterPara {
    AvmLens avmlens0;
    AvmLens avmlens1;
    AvmLens avmlens2;
    AvmLens avmlens3;
} AvmInterPara;

typedef struct CameraCfg {
    uint8_t AVMEnable;
    uint8_t RVCEnable;
    uint8_t DMSEnable;
    uint8_t Steering_wheel_position;
} CameraCfg;

typedef struct DmsInfo {
    uint8_t index;
    uint8_t DMSSelfCheckStatus;
    uint8_t DistractionStatus;
    uint8_t FatigueStatus;
    uint8_t Country;
    uint8_t DmsMode;
    uint8_t DistractionSwitch;
    uint8_t FatigueSwitch;
} DmsInfo;

typedef struct SPI_REC {
    HalSensorInfo hal_sensor_info;
    VehicleInfo vehicle_info;
    uint8_t str_status;
    uint8_t boot_animation_status;
    AvmInterPara avm_inter_para;
    CameraCfg camera_cfg;
} SPI_REC;

typedef struct SPI_SEND {
    DmsInfo dms_info;
} SPI_SEND;

#endif