#ifndef PPSREADER_H
#define PPSREADER_H

#include "PpsDataType.h"

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class PpsReader {
public:
    PpsReader() : isRunning_(true){};
    PpsReader(const PpsReader&) = delete;
    PpsReader& operator=(const PpsReader&) = delete;
    virtual ~PpsReader();

    int init();
    int deInit();
    void start();
    void stop();
    int getData(SPI_REC& data_rec);
    static PpsReader& getInstance() {
        static PpsReader instance;
        return instance;
    }

private:
    uint8_t extractBits(uint8_t byte, int n, int m);
    void ReadHalSensorInfo(SPI_REC& spi_rec);
    void ReadVehicleInfo(SPI_REC& spi_rec);
    void ReadStrSt(SPI_REC& spi_rec);
    void ReadBootAnimationSt(SPI_REC& spi_rec);
    void ReadAvmInterPara(SPI_REC& spi_rec);
    void ReadCameraCfg(SPI_REC& spi_rec);
    void ReceiveDataThread();

private:
    const char* ppsNodeVehicInfo_ = "/tmp/pps/spi_service/a012?wait,delta,nopersist";
    const char* ppsNodeHalSensorInfo_ = "/tmp/pps/spi_service/a020?wait,delta,nopersist";
    const char* ppsNodeStrSt_ = "/tmp/pps/str/status?wait,delta,nopersist";
    const char* ppsNodeBootAnimationSt_ = "/tmp/pps/spi_service/a00f?wait,delta,nopersist";
    const char* ppsNodeAvmLens0_ = "/tmp/pps/camera/avmlens0?wait,delta,nopersist";
    const char* ppsNodeAvmLens1_ = "/tmp/pps/camera/avmlens1?wait,delta,nopersist";
    const char* ppsNodeAvmLens2_ = "/tmp/pps/camera/avmlens2?wait,delta,nopersist";
    const char* ppsNodeAvmLens3_ = "/tmp/pps/camera/avmlens3?wait,delta,nopersist";
    const char* ppsNodeCameraCfg_ = "/tmp/pps/eolcfg/CameraCfg?wait,delta,nopersist";
    int fdVehicInfo_;
    int fdHalSensorInfo_;
    int fdStrSt_;
    int fdBootAnimationSt_;
    int fdAvmLens0_;
    int fdAvmLens1_;
    int fdAvmLens2_;
    int fdAvmLens3_;
    int fdCameraCfg_;
    std::thread dataThread_;
    mutable std::mutex mutex_;
    std::queue< SPI_REC > dataQueue_;
    bool isRunning_;
};

#endif