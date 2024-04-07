#include "PpsReader.h"

#include <sys/pps.h>
#include <vector>
extern "C" {
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
}

PpsReader::~PpsReader() {}

int PpsReader::init() {
    fdVehicInfo_ = open(ppsNodeVehicInfo_, O_RDONLY);
    fdHalSensorInfo_ = open(ppsNodeHalSensorInfo_, O_RDONLY);
    fdStrSt_ = open(ppsNodeStrSt_, O_RDONLY);
    fdBootAnimationSt_ = open(ppsNodeBootAnimationSt_, O_RDONLY);
    fdAvmLens0_ = open(ppsNodeAvmLens0_, O_RDONLY);
    fdAvmLens1_ = open(ppsNodeAvmLens1_, O_RDONLY);
    fdAvmLens2_ = open(ppsNodeAvmLens2_, O_RDONLY);
    fdAvmLens3_ = open(ppsNodeAvmLens3_, O_RDONLY);
    fdCameraCfg_ = open(ppsNodeCameraCfg_, O_RDONLY);

    if (fdVehicInfo_ < 0) {
        std::cout << "open" << ppsNodeVehicInfo_ << " failed" << std::endl;
    }
    if (fdHalSensorInfo_ < 0) {
        std::cout << "open" << ppsNodeHalSensorInfo_ << " failed" << std::endl;
    }
    if (fdStrSt_ < 0) {
        std::cout << "open" << ppsNodeStrSt_ << " failed" << std::endl;
    }
    if (fdBootAnimationSt_ < 0) {
        std::cout << "open" << fdBootAnimationSt_ << "failed" << std::endl;
    }
    if (fdAvmLens0_ < 0) {
        std::cout << "open" << ppsNodeAvmLens0_ << " failed" << std::endl;
    }
    if (fdAvmLens1_ < 0) {
        std::cout << "open" << ppsNodeAvmLens1_ << " failed" << std::endl;
    }
    if (fdAvmLens2_ < 0) {
        std::cout << "open" << ppsNodeAvmLens2_ << " failed" << std::endl;
    }
    if (fdAvmLens3_ < 0) {
        std::cout << "open" << ppsNodeAvmLens3_ << " failed" << std::endl;
    }
    if (fdCameraCfg_ < 0) {
        std::cout << "open" << ppsNodeCameraCfg_ << " failed" << std::endl;
    }
    return 0;
}

int PpsReader::deInit() {}

int PpsReader::getData(SPI_REC& data_rec) {
    std::lock_guard< std::mutex > lock(mutex_);
    if (dataQueue_.empty()) {
        // std::cout << "dataQueue is empty!" << std::endl;
        return -1;
    }
    data_rec = dataQueue_.front();
    dataQueue_.pop();
    return 0;
}

void PpsReader::start() {
    dataThread_ = std::thread(&PpsReader::ReceiveDataThread, this);
}

void PpsReader::stop() {
    {
        std::lock_guard< std::mutex > lock(mutex_);
        isRunning_ = false;
    }

    dataThread_.join();
}

uint8_t PpsReader::extractBits(uint8_t byte, int n, int m) {
    if (n < 0 || m > 7 || n > m) {
        std::cerr << "Invalid bit range." << std::endl;
        return 0;
    }

    uint8_t mask = (1 << (m - n + 1)) - 1;

    mask <<= n;

    uint8_t result = (byte & mask) >> n;

    return result;
}

void PpsReader::ReadHalSensorInfo(SPI_REC& spi_rec) {

    if (fdHalSensorInfo_ < 0) {
        return;
    }

    pps_decoder_t decoder;
    pps_decoder_error_t ret;

    // data
    int len = 52;
    std::vector< int > data(len, 0);

    // read buffer
    char buffer[1024] = {0};
    int read_len = read(fdHalSensorInfo_, buffer, sizeof(buffer));

    // parse buffer to data
    if (read_len > 0) {
        int ret = pps_decoder_initialize(&decoder, nullptr);
        if (ret == PPS_DECODER_OK) {
            pps_decoder_parse_pps_str(&decoder, buffer);
            pps_decoder_push(&decoder, nullptr);

            int t = pps_decoder_type(&decoder, nullptr);
            switch (t) {
                case PPS_TYPE_OBJECT: {
                    const char* name = pps_decoder_name(&decoder);
                    if (std::string(name) == "data") {
                        if (PPS_DECODER_OK == pps_decoder_push(&decoder, "data")) {
                            pps_decoder_get_int(&decoder, "len", &len);
                            if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "data")) {
                                int length = pps_decoder_length(&decoder);
                                for (int i = 0; i < length; ++i) {
                                    int value = 0;
                                    int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                    if (PPS_DECODER_OK == ret) {
                                        data[i] = value;
                                    } else {
                                        std::cout << "pps_decoder_get_int error " << ret
                                                  << std::endl;
                                    }
                                }
                                pps_decoder_pop(&decoder);
                            }
                            pps_decoder_pop(&decoder);
                        }
                    }
                    break;
                }
                case PPS_TYPE_STRING:
                case PPS_TYPE_NULL:
                default:
                    break;
            }
            pps_decoder_pop(&decoder);
        }
    }
    pps_decoder_cleanup(&decoder);

    // parse data for dms
    auto& sensor_info = spi_rec.hal_sensor_info;
    sensor_info.BCMPower_Gear_12D = (data[0] & 0xe0) >> 5;
    sensor_info.reserve_1 = (data[0] & 0x10) >> 4;
    sensor_info.RightLineTrackStatus = (data[0] & 0x0c) >> 2;
    sensor_info.LeftLineTrackStatus = data[0] & 0x03;
    sensor_info.SwsMediumRelatedKeysS = data[1];
    sensor_info.SpeedSignal144S = data[2];
    sensor_info.SteeringWheelAngel11FS = data[3];
    sensor_info.SteeringWheelRotationSpeedS = data[5];
    sensor_info.TurnSignalWorkCondition38AS = (data[6] & 0xf0) >> 4;
    sensor_info.DNP_Stats_SDNP = data[6] & 0x0f;
    sensor_info.reserve_2 = data[7];
    return;
}
void PpsReader::ReadVehicleInfo(SPI_REC& spi_rec) {

    if (fdVehicInfo_ < 0) {
        return;
    }

    pps_decoder_t decoder;
    pps_decoder_error_t ret;

    // data
    int len = 52;
    std::vector< int > data(len, 0);

    // read buffer
    char buffer[1024] = {0};
    int read_len = read(fdVehicInfo_, buffer, sizeof(buffer));

    // parse buffer to data
    if (read_len > 0) {
        int ret = pps_decoder_initialize(&decoder, nullptr);
        if (ret == PPS_DECODER_OK) {
            pps_decoder_parse_pps_str(&decoder, buffer);
            pps_decoder_push(&decoder, nullptr);

            int t = pps_decoder_type(&decoder, nullptr);
            switch (t) {
                case PPS_TYPE_OBJECT: {
                    const char* name = pps_decoder_name(&decoder);
                    if (std::string(name) == "data") {
                        if (PPS_DECODER_OK == pps_decoder_push(&decoder, "data")) {
                            pps_decoder_get_int(&decoder, "len", &len);
                            if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "data")) {
                                int length = pps_decoder_length(&decoder);
                                for (int i = 0; i < length; ++i) {
                                    int value = 0;
                                    int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                    if (PPS_DECODER_OK == ret) {
                                        data[i] = value;
                                    } else {
                                        std::cout << "pps_decoder_get_int error " << ret
                                                  << std::endl;
                                    }
                                }
                                pps_decoder_pop(&decoder);
                            }
                            pps_decoder_pop(&decoder);
                        }
                    }
                    break;
                }
                case PPS_TYPE_STRING:
                case PPS_TYPE_NULL:
                default:
                    break;
            }
            pps_decoder_pop(&decoder);
        }
    }
    pps_decoder_cleanup(&decoder);

    // parse data for dms
    auto& vehicle_info = spi_rec.vehicle_info;

    vehicle_info.WheDirect.RRWheelMovDirect = extractBits(data[0], 0, 1);
    vehicle_info.WheDirect.RLWheelMovDirect = extractBits(data[0], 2, 3);
    vehicle_info.WheDirect.FRWheelMovDirect = extractBits(data[0], 4, 5);
    vehicle_info.WheDirect.FLWheelMovDirect = extractBits(data[0], 6, 7);

    vehicle_info.WhePuls.FLWheelPulsCountValid = extractBits(data[1], 0, 0);
    vehicle_info.WhePuls.FRWheelPulsCountValid = extractBits(data[1], 1, 1);
    vehicle_info.WhePuls.RLWheelPulsCountValid = extractBits(data[1], 2, 2);
    vehicle_info.WhePuls.RRWheelPulsCountValid = extractBits(data[1], 3, 3);
    vehicle_info.WhePuls.reserved = extractBits(data[1], 4, 7);
    vehicle_info.WhePuls.FLWheelPulsCount = (data[2] << 8) + data[3];
    vehicle_info.WhePuls.FRWheelPulsCount = (data[4] << 8) + data[5];
    vehicle_info.WhePuls.RLWheelPulsCount = (data[6] << 8) + data[7];
    vehicle_info.WhePuls.RRWheelPulsCount = (data[8] << 8) + data[9];

    vehicle_info.TurnSignalSwitch = extractBits(data[10], 0, 3);
    vehicle_info.TurnSignalStatus108 = extractBits(data[10], 4, 7);
    vehicle_info.dragModeState = extractBits(data[11], 0, 3);
    vehicle_info.TrailerModeConfigR = extractBits(data[11], 4, 7);
    vehicle_info.AVMAPAAbortReason = data[12];

    vehicle_info.RadarProbeSt.RadarProbeStateFL = data[13];
    vehicle_info.RadarProbeSt.RadarProbeStateFML = data[14];
    vehicle_info.RadarProbeSt.RadarProbeStateFMR = data[15];
    vehicle_info.RadarProbeSt.RadarProbeStateFR = data[16];
    vehicle_info.RadarProbeSt.RadarProbeStateMR = data[17];
    vehicle_info.RadarProbeSt.RadarProbeStateRL = data[18];
    vehicle_info.RadarProbeSt.RadarProbeStateRML = data[19];
    vehicle_info.RadarProbeSt.RadarProbeStateRMR = data[20];
    vehicle_info.RadarProbeSt.RadarProbeStateRR = data[21];

    vehicle_info.NewReverseRadarSwitchStateR = extractBits(data[22], 0, 0);
    vehicle_info.ReverseRadarSwitchStateR = extractBits(data[22], 1, 1);
    vehicle_info.reserved_1 = extractBits(data[22], 2, 2);
    vehicle_info.GearboxAutoModeType = extractBits(data[22], 3, 7);

    vehicle_info.Left_Front_Door_Status_294 = extractBits(data[23], 0, 1);
    vehicle_info.RightFrontDoorStatus294 = extractBits(data[23], 2, 3);
    vehicle_info.Left_Back_Door_Status_294 = extractBits(data[23], 4, 5);
    vehicle_info.Right_Back_Door_Status_294 = extractBits(data[23], 6, 7);

    vehicle_info.reserved_2 = extractBits(data[24], 0, 0);

    vehicle_info.DoorStateLuggage = extractBits(data[24], 1, 2);
    vehicle_info.DoorStateHood = extractBits(data[24], 3, 4);
    vehicle_info.SunRoofStateR = extractBits(data[24], 5, 7);

    vehicle_info.DoorLockStatusAreaBack = extractBits(data[25], 0, 1);
    vehicle_info.DoorLockStatusAreaLeftFront = extractBits(data[25], 2, 3);
    vehicle_info.DoorLockStatusRightFront = extractBits(data[25], 4, 5);
    vehicle_info.DoorLockStatusLeftRear = extractBits(data[25], 6, 7);

    vehicle_info.DoorLockStatusAreaRightRear = extractBits(data[26], 0, 1);
    vehicle_info.reserved_3 = extractBits(data[26], 2, 3);
    vehicle_info.OtaPowerLevel = extractBits(data[26], 4, 7);

    vehicle_info.VehicleSpeed = (data[27] << 8) + data[28];
    vehicle_info.SteeringWheelValueAngel = (data[29] << 8) + data[30];

    vehicle_info.AutoExternalRearMirrorFoldActionStateR = extractBits(data[31], 0, 1);
    vehicle_info.DayTimeLightState = extractBits(data[31], 2, 3);
    vehicle_info.StopLightStateR = extractBits(data[31], 4, 5);
    vehicle_info.ReversingLightState = extractBits(data[31], 6, 7);

    vehicle_info.TurnLightState = extractBits(data[32], 0, 3);
    vehicle_info.LightSideStatusR = extractBits(data[32], 4, 5);
    vehicle_info.LightLowBeamStatusR = extractBits(data[32], 6, 7);

    vehicle_info.LightHighBeamR = extractBits(data[33], 0, 1);
    vehicle_info.LightRearPositionSignalR = extractBits(data[33], 2, 3);
    vehicle_info.LightRearFog = extractBits(data[33], 4, 5);
    vehicle_info.SupportAutoParking = extractBits(data[33], 6, 7);

    vehicle_info.ValetParkingR = extractBits(data[34], 0, 1);
    vehicle_info.MemoryParkingR = extractBits(data[34], 2, 3);
    vehicle_info.reserved_4 = extractBits(data[34], 4, 7);

    vehicle_info.ScreenAngleR = extractBits(data[35], 0, 1);
    vehicle_info.ScreenRotationActionR = extractBits(data[35], 2, 4);
    vehicle_info.reserved_5 = extractBits(data[35], 5, 7);

    vehicle_info.Veer = extractBits(data[36], 0, 1);
    vehicle_info.AcOpenState = extractBits(data[36], 2, 3);
    vehicle_info.reserved_6 = extractBits(data[36], 4, 7);

    vehicle_info.AcControllerDriverTempR = data[37];
    vehicle_info.AcControllerPassengerTempR = data[38];

    vehicle_info.AcFrontWindLevelR = extractBits(data[39], 0, 3);
    vehicle_info.AcTempCtrlSeparateState = extractBits(data[39], 4, 5);
    vehicle_info.AcDefrostR = extractBits(data[39], 6, 7);

    vehicle_info.AcCircleMode = extractBits(data[40], 0, 1);
    vehicle_info.AcVentilationStateR = extractBits(data[40], 2, 3);
    vehicle_info.TemperatureUnit = extractBits(data[40], 4, 4);
    vehicle_info.AcFaultNumShownState = extractBits(data[40], 5, 6);
    vehicle_info.AcDefrost = extractBits(data[40], 7, 7);

    vehicle_info.AutoType = data[41];

    vehicle_info.AcInteraction = data[42];
    vehicle_info.PanoramaBtInfo = data[43];
}
void PpsReader::ReadStrSt(SPI_REC& spi_rec) {

    if (fdStrSt_ < 0) {
        return;
    }

    pps_decoder_t decoder;
    pps_decoder_error_t ret;

    // data
    int str_status;

    // read buffer
    char buffer[1024] = {0};
    int read_len = read(fdStrSt_, buffer, sizeof(buffer));

    // parse buffer to data
    if (read_len > 0) {
        int ret = pps_decoder_initialize(&decoder, nullptr);
        if (ret == PPS_DECODER_OK) {
            pps_decoder_parse_pps_str(&decoder, buffer);
            pps_decoder_push(&decoder, nullptr);

            int t = pps_decoder_type(&decoder, nullptr);
            switch (t) {
                case PPS_TYPE_OBJECT: {
                    const char* name = pps_decoder_name(&decoder);
                    if (std::string(name) == "status") {
                        if (PPS_DECODER_OK == pps_decoder_push(&decoder, "status")) {
                            pps_decoder_get_int(&decoder, "n", &str_status);
                            pps_decoder_pop(&decoder);
                        }
                    }
                    break;
                }
                case PPS_TYPE_STRING:
                case PPS_TYPE_NULL:
                default:
                    break;
            }
            pps_decoder_pop(&decoder);
        }
    }
    pps_decoder_cleanup(&decoder);

    // parse data for dms
    spi_rec.str_status = str_status;
}
void PpsReader::ReadBootAnimationSt(SPI_REC& spi_rec) {
    if (fdBootAnimationSt_ < 0) {
        return;
    }

    pps_decoder_t decoder;
    pps_decoder_error_t ret;

    // data
    int len = 52;
    std::vector< int > data(len, 0);

    // read buffer
    char buffer[1024] = {0};
    int read_len = read(fdBootAnimationSt_, buffer, sizeof(buffer));

    // parse buffer to data
    if (read_len > 0) {
        int ret = pps_decoder_initialize(&decoder, nullptr);
        if (ret == PPS_DECODER_OK) {
            pps_decoder_parse_pps_str(&decoder, buffer);
            pps_decoder_push(&decoder, nullptr);

            int t = pps_decoder_type(&decoder, nullptr);
            switch (t) {
                case PPS_TYPE_OBJECT: {
                    const char* name = pps_decoder_name(&decoder);
                    if (std::string(name) == "data") {
                        if (PPS_DECODER_OK == pps_decoder_push(&decoder, "data")) {
                            pps_decoder_get_int(&decoder, "len", &len);
                            if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "data")) {
                                int length = pps_decoder_length(&decoder);
                                for (int i = 0; i < length; ++i) {
                                    int value = 0;
                                    int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                    if (PPS_DECODER_OK == ret) {
                                        data[i] = value;
                                    } else {
                                        std::cout << "pps_decoder_get_int error " << ret
                                                  << std::endl;
                                    }
                                }
                                pps_decoder_pop(&decoder);
                            }
                            pps_decoder_pop(&decoder);
                        }
                    }
                    break;
                }
                case PPS_TYPE_STRING:
                case PPS_TYPE_NULL:
                default:
                    break;
            }
            pps_decoder_pop(&decoder);
        }
    }
    pps_decoder_cleanup(&decoder);

    spi_rec.boot_animation_status = data[52];
}
void PpsReader::ReadAvmInterPara(SPI_REC& spi_rec) {

    auto read_avmlens = [&](int fd, AvmLens& avmlens) {
        if (fd < 0) {
            return;
        }
        pps_decoder_t decoder;
        pps_decoder_error_t ret;

        // read buffer
        char buffer[1024] = {0};
        int read_len = read(fd, buffer, sizeof(buffer));

        // parse buffer to data
        if (read_len > 0) {
            int ret = pps_decoder_initialize(&decoder, nullptr);
            if (ret == PPS_DECODER_OK) {
                pps_decoder_parse_pps_str(&decoder, buffer);
                pps_decoder_push(&decoder, nullptr);

                int t = pps_decoder_type(&decoder, nullptr);

                switch (t) {
                    case PPS_TYPE_OBJECT: {
                        const char* name = pps_decoder_name(&decoder);
                        if (std::string(name) == "data") {
                            if (PPS_DECODER_OK == pps_decoder_push(&decoder, "data")) {
                                if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "lens_fx")) {
                                    int length = pps_decoder_length(&decoder);
                                    for (int i = 0; i < length; ++i) {
                                        int value;
                                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                        avmlens.lens_fx[i] = value;
                                        if (PPS_DECODER_OK != ret) {
                                            std::cout << "pps_decoder_get_int error " << ret
                                                      << std::endl;
                                        }
                                    }
                                    pps_decoder_pop(&decoder);
                                }
                                if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "lens_fy")) {
                                    int length = pps_decoder_length(&decoder);
                                    for (int i = 0; i < length; ++i) {
                                        int value;
                                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                        avmlens.lens_fy[i] = value;
                                        if (PPS_DECODER_OK != ret) {
                                            std::cout << "pps_decoder_get_int error " << ret
                                                      << std::endl;
                                        }
                                    }
                                    pps_decoder_pop(&decoder);
                                }
                                if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "lens_cx")) {
                                    int length = pps_decoder_length(&decoder);
                                    for (int i = 0; i < length; ++i) {
                                        int value;
                                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                        avmlens.lens_cx[i] = value;
                                        if (PPS_DECODER_OK != ret) {
                                            std::cout << "pps_decoder_get_int error " << ret
                                                      << std::endl;
                                        }
                                    }
                                    pps_decoder_pop(&decoder);
                                }
                                if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "lens_k1")) {
                                    int length = pps_decoder_length(&decoder);
                                    for (int i = 0; i < length; ++i) {
                                        int value;
                                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                        avmlens.lens_k1[i] = value;
                                        if (PPS_DECODER_OK != ret) {
                                            std::cout << "pps_decoder_get_int error " << ret
                                                      << std::endl;
                                        }
                                    }
                                    pps_decoder_pop(&decoder);
                                }
                                if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "lens_k2")) {
                                    int length = pps_decoder_length(&decoder);
                                    for (int i = 0; i < length; ++i) {
                                        int value;
                                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                        avmlens.lens_k2[i] = value;
                                        if (PPS_DECODER_OK != ret) {
                                            std::cout << "pps_decoder_get_int error " << ret
                                                      << std::endl;
                                        }
                                    }
                                    pps_decoder_pop(&decoder);
                                }
                                if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "lens_k3")) {
                                    int length = pps_decoder_length(&decoder);
                                    for (int i = 0; i < length; ++i) {
                                        int value;
                                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                        avmlens.lens_k3[i] = value;
                                        if (PPS_DECODER_OK != ret) {
                                            std::cout << "pps_decoder_get_int error " << ret
                                                      << std::endl;
                                        }
                                    }
                                    pps_decoder_pop(&decoder);
                                }
                                if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "lens_k4")) {
                                    int length = pps_decoder_length(&decoder);
                                    for (int i = 0; i < length; ++i) {
                                        int value;
                                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                        avmlens.lens_k4[i] = value;
                                        if (PPS_DECODER_OK != ret) {
                                            std::cout << "pps_decoder_get_int error " << ret
                                                      << std::endl;
                                        }
                                    }
                                    pps_decoder_pop(&decoder);
                                }
                            }
                        }
                        break;
                    }
                    case PPS_TYPE_STRING:
                    case PPS_TYPE_NULL:
                    default:
                        break;
                }
                pps_decoder_pop(&decoder);
            }
        }
        pps_decoder_cleanup(&decoder);
    };

    read_avmlens(fdAvmLens0_, spi_rec.avm_inter_para.avmlens0);
    read_avmlens(fdAvmLens1_, spi_rec.avm_inter_para.avmlens1);
    read_avmlens(fdAvmLens2_, spi_rec.avm_inter_para.avmlens2);
    read_avmlens(fdAvmLens3_, spi_rec.avm_inter_para.avmlens3);
}
void PpsReader::ReadCameraCfg(SPI_REC& spi_rec) {

    if (fdCameraCfg_ < 0) {
        return;
    }
    pps_decoder_t decoder;
    pps_decoder_error_t ret;

    // data
    int AVMEnable;
    int DMSEnable;
    int RVCEnable;
    int Steering_wheel_position;

    // read buffer
    char buffer[1024] = {0};
    int read_len = read(fdCameraCfg_, buffer, sizeof(buffer));

    // parse buffer to data
    if (read_len > 0) {
        int ret = pps_decoder_initialize(&decoder, nullptr);
        if (ret == PPS_DECODER_OK) {
            pps_decoder_parse_pps_str(&decoder, buffer);
            pps_decoder_push(&decoder, nullptr);

            int t = pps_decoder_type(&decoder, nullptr);
            switch (t) {
                case PPS_TYPE_OBJECT: {
                    const char* name = pps_decoder_name(&decoder);
                    if (std::string(name) == "data") {
                        if (PPS_DECODER_OK == pps_decoder_push(&decoder, "data")) {
                            pps_decoder_get_int(&decoder, "AVMEnable", &AVMEnable);
                            pps_decoder_get_int(&decoder, "DMSEnable", &DMSEnable);
                            pps_decoder_get_int(&decoder, "RVCEnable", &RVCEnable);
                            pps_decoder_get_int(
                                &decoder, "Steering_wheel_position", &Steering_wheel_position);
                            pps_decoder_pop(&decoder);
                        }
                    }
                    break;
                }
                case PPS_TYPE_STRING:
                case PPS_TYPE_NULL:
                default:
                    break;
            }
            pps_decoder_pop(&decoder);
        }
    }
    pps_decoder_cleanup(&decoder);

    // parse data for dms
    spi_rec.camera_cfg.AVMEnable = AVMEnable;
    spi_rec.camera_cfg.DMSEnable = DMSEnable;
    spi_rec.camera_cfg.RVCEnable = RVCEnable;
    spi_rec.camera_cfg.Steering_wheel_position = Steering_wheel_position;
}

void PpsReader::ReceiveDataThread() {
    while (true) {
        SPI_REC spi_rec;

        ReadHalSensorInfo(spi_rec);
        ReadVehicleInfo(spi_rec);
        ReadStrSt(spi_rec);
        ReadBootAnimationSt(spi_rec);
        ReadAvmInterPara(spi_rec);
        ReadCameraCfg(spi_rec);

        // push data to queue
        std::lock_guard< std::mutex > lock(mutex_);
        if (dataQueue_.size() > 10) {
            dataQueue_.pop();
        }
        dataQueue_.push(spi_rec);

        if (!isRunning_) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
