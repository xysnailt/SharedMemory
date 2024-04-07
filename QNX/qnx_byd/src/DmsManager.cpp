#include "DmsManager.h"

#include "CameraHandle.h"
#include "PpsDataType.h"
#include "VehicleConfigManage.hpp"

DmsManager::DmsManager() {
    dms_handle_ = std::shared_ptr< SDHandle >(new SDHandle(nullptr));
}

DmsManager::~DmsManager() {}

int DmsManager::suspend(void* ctx) {
    stop();
    DmsManager::getInstance().abortDms();
    SenseDriverDestroy(dms_handle_.get());
}
int DmsManager::resume(void* ctx) {
    start();
    // init dms algo
    auto ret = SenseDriverInit(
        config_path_.c_str(), static_cast< int >(config_path_.length()), dms_handle_.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error, error code = " << ret << std::endl;
        return -1;
    }
}

int DmsManager::init() {
    // init dms algo
    auto ret = SenseDriverInit(
        config_path_.c_str(), static_cast< int >(config_path_.length()), dms_handle_.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error, error code = " << ret << std::endl;
        return -1;
    }

    // register dms handle
    FunctionSwitchManage::Instance().DmdHandleRegister(dms_handle_);

    // init camera handle
    CameraHandle::getInstance().initialize();

    // init pps reader
    PpsReader::getInstance().init();

    // init pps writer
    int pps_ret = pps_writer_.init();
    if (0 != pps_ret) {
        std::cout << "pps init failed!" << std::endl;
        return -1;
    }

    return 0;
}

int DmsManager::start() {
    std::cout << "DmsManager start" << std::endl;
    // start camera handle
    CameraHandle::getInstance().start();

    // start pps reader
    PpsReader::getInstance().start();

    // start thread
    if (isRunning_) {
        std::cerr << "Error: Thread is already running." << std::endl;
        return 0;
    }
    thread_ = std::thread(&DmsManager::processLoop, this);
    isRunning_ = true;
    abortFlag_ = false;

    return 0;
}

int DmsManager::stop() {
    abortFlag_ = false;

    std::cout << "DmsManager stop" << std::endl;
    // start camera handle
    CameraHandle::getInstance().stop();

    // stop pps reader
    PpsReader::getInstance().stop();

    return 0;
}

int DmsManager::deInit() {
    std::cout << "DmsManager deInit" << std::endl;
    // dms handle
    SenseDriverDestroy(dms_handle_.get());

    // deinit camera handle
    CameraHandle::getInstance().uninitialize();

    // deinit pps reader
    PpsReader::getInstance().deInit();

    // deinit pps writer
    pps_writer_.deInit();
    std::cout << "DmsManager deInit out" << std::endl;

    return 0;
}

bool DmsManager::isRunning() const {
    return isRunning_;
}

int DmsManager::setDmsMode(DmsMode dms_mode) {
    currentMode_ = dms_mode;
    return 0;
}

int DmsManager::registerResultProc(std::function< int(const SDResult&) > result_proc) {
    result_proc_ = result_proc;
}

void DmsManager::abortDms() {
    std::cout << "set abortFlag_ true" << std::endl;
    abortFlag_ = true;
}

void DmsManager::getTimeStamp(TimeStamp& timestamp) {
    auto now = std::chrono::steady_clock::now().time_since_epoch().count() / 1000L;
    timestamp.tv_sec = now / 1000L / 1000L;
    timestamp.tv_usec = now - timestamp.tv_sec * 1000L * 1000L;
    return;
}

int DmsManager::processImage(int width, int height, MPixelFormat format, unsigned char* data_addr) {
    // getTimeStamp
    TimeStamp time_stamp;
    getTimeStamp(time_stamp);

    // create image handle
    auto image_handle = CreateSingleImageHandle(
        data_addr, nullptr, time_stamp, format, width, height, MMemoryType::MEM_ON_CPU);

    // image handle deleter
    std::shared_ptr< char > image_handle_deletor(
        nullptr, [&](char* p) { DestroySingleImageHandle(&image_handle); });
    // dms result
    std::shared_ptr< SDResult > result(new SDResult, [](SDResult* p) {
        SenseDriverDestroyResult(p);
        delete p;
    });

    // run
    auto ret = SenseDriverRun(*dms_handle_, image_handle, result.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverRun failed with error: " << static_cast< int >(ret) << std::endl;
        return -1;
    }

    // print result to screen
    dms_result_printer_.PrintDMSResult(*result);
    std::cout << std::endl;

    // pps read data
    SPI_REC data_rec;
    if (0 == PpsReader::getInstance().getData(data_rec)) {
        // std::cout << "speed: " << static_cast< int
        // >(data_rec.hal_sensor_info.SpeedSignal144S)
        //           << std::endl;
        FunctionSwitchManage::Instance().SetVehicleSingal(data_rec);
    }

    // QNX -> Android message by socket
    result_proc_(*result);

    // logic for vehicle singal
    // TODO

    // send data
    sendPpsData(*result);

    return 0;
};

void DmsManager::processLoop() {
    while (true) {
        if (abortFlag_) {
            std::cout << "abort dms" << std::endl;
            isRunning_ = false;
            break;
        }
        if (isRunning_) {
            unsigned char* buf_addr = ( unsigned char* )malloc(IMAGE_STRIDE * IMAGE_HEIGHT);
            CameraHandle::getInstance().getImage(buf_addr);

            DmsManager::getInstance().processImage(
                IMAGE_WIDTH, IMAGE_HEIGHT, MPixelFormat::M_PIX_FMT_GRAY8, buf_addr);
            free(buf_addr);
        }
    }
    return;
}

int DmsManager::sendPpsData(const SDResult& result) {
    // dms self check status
    std::vector< int > selfCheckResult = {1, 161, 6, 6, 0, 0, 0, 0, 0, 3};
    if (result.camera_occlusion_result.result_valid) {
        if (result.camera_occlusion_result.result == M_CAMERA_OCCLUSION) {
            selfCheckResult[9] = 0x05;
        }
    }
    pps_writer_.sendData(selfCheckResult);

    // distraction monitoring attention monitoring
    std::vector< int > distractionResult = {1, 161, 6, 6, 0, 1, 0, 0, 0, 0};
    if (result.person_number > 0 && result.person_array_result != nullptr) {
        auto dis_result = result.person_array_result[0].face_result.distraction_result;
        if (dis_result.result_valid) {
            switch (dis_result.level) {
                case M_DISTRACTION_LEVEL_NONE:
                    distractionResult[9] = 0x01;
                    break;
                case M_DISTRACTION_LEVEL_LIGHT:
                    distractionResult[9] = 0x02;
                    break;
                case M_DISTRACTION_LEVEL_MEDIUM:
                    distractionResult[9] = 0x03;
                    break;
                case M_DISTRACTION_LEVEL_HEAVY:
                    distractionResult[9] = 0x04;
                    break;
                case M_DISTRACTION_LEVEL_INVALID:
                    distractionResult[9] = 0x06;
                    break;
                default:
                    distractionResult[9] = 0x06;
                    break;
            }
        } else {
            distractionResult[9] = 0x00;
        }
    }
    pps_writer_.sendData(distractionResult);

    // fatigue monitor
    std::vector< int > fatigueMonitorResult = {1, 161, 6, 6, 0, 2, 0, 0, 0, 0};
    if (result.person_number > 0 && result.person_array_result != nullptr) {
        auto dro_result = result.person_array_result[0].face_result.drowsiness_result;
        if (dro_result.result_valid) {
            switch (dro_result.level) {
                case M_DROWSINESS_LEVEL_NONE:
                    fatigueMonitorResult[9] = 0x04;
                    break;
                case M_DROWSINESS_LEVEL_LIGHT:
                    fatigueMonitorResult[9] = 0x01;
                    break;
                case M_DROWSINESS_LEVEL_MEDIUM:
                    fatigueMonitorResult[9] = 0x02;
                    break;
                case M_DROWSINESS_LEVEL_HEAVY:
                    fatigueMonitorResult[9] = 0x03;
                    break;
                case M_DROWSINESS_LEVEL_INVALID:
                    fatigueMonitorResult[9] = 0x00;
                    break;
                default:
                    fatigueMonitorResult[9] = 0x04;
                    break;
            }
        } else {
            fatigueMonitorResult[9] = 0x00;
        }
    }
    pps_writer_.sendData(fatigueMonitorResult);

    // country
    std::vector< int > country = {1, 161, 6, 6, 0, 3, 0, 0, 0, 1};
    pps_writer_.sendData(country);

    // dms mode
    std::vector< int > dmsMode = {1, 161, 6, 6, 0, 4, 0, 0, 0, 1};
    switch (currentMode_) {
        case DmsMode::Factory:
        case DmsMode::Standard:
            fatigueMonitorResult[9] = 0x01;
            break;
        default:
            fatigueMonitorResult[9] = 0x01;
            break;
    }
    dmsMode[9] = pps_writer_.sendData(dmsMode);

    // distraction switch
    std::vector< int > distractionSwitch = {1, 161, 6, 6, 0, 5, 0, 0, 0, 1};
    pps_writer_.sendData(distractionSwitch);

    // fatigue switch
    std::vector< int > fatigueSwitch = {0, 161, 6, 6, 0, 6, 0, 0, 0, 1};
    pps_writer_.sendData(fatigueSwitch);

    return 0;
}
