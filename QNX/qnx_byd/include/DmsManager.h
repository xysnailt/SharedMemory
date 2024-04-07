#ifndef DMSMANAGER_H
#define DMSMANAGER_H

#include "DmsResulPrinter.hpp"
#include "PpsReader.h"
#include "PpsWriter.h"
#include "sensedriver_interface.h"
#include "sv_log.h"
#include "sv_oem_pm_client_lib.h"

#include <iostream>
#include <memory>

// Mode
enum class DmsMode { Standard, Factory };

class DmsManager {
public:
    DmsManager();
    DmsManager(const DmsManager&) = delete;
    DmsManager& operator=(const DmsManager&) = delete;
    virtual ~DmsManager();

    static DmsManager& getInstance() {
        static DmsManager instance;
        return instance;
    }

    // str
    int resume(void* ctx);
    int suspend(void* ctx);

    int init();
    int start();
    int stop();
    int deInit();
    int registerResultProc(std::function< int(const SDResult&) > result_proc);
    void abortDms();
    bool isRunning() const;
    int setDmsMode(DmsMode dms_mode);
    DmsMode getCurrentMode() const { return currentMode_; }

private:
    static void getTimeStamp(TimeStamp& timestamp);
    int processImage(int width, int height, MPixelFormat format, unsigned char* data_addr);
    void processLoop();
    int sendPpsData(const SDResult& result);

private:
    bool abortFlag_;
    bool isRunning_;
    DmsMode currentMode_ = DmsMode::Standard;

    std::thread thread_;
    std::string config_path_ = "../resource/config/";
    std::shared_ptr< SDHandle > dms_handle_;
    DMSResultPrinter dms_result_printer_;
    PpsWriter pps_writer_;
    std::function< int(const SDResult&) > result_proc_;

    sv_oem_pm_client_t hdl_;
    struct sv_oem_pm_ops_s ops_;
};
#endif