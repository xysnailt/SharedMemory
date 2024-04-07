#include "CameraHandle.h"
#include "DmsResulPrinter.hpp"
#include "MessageService.hpp"
#include "PpsReader.h"
#include "PpsTools.hpp"
#include "PpsWriter.h"
#include "SocketServer.hpp"
#include "pmem.h"
#include "qcarcam.h"
#include "sensedriver_interface.h"

#include <array>
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mqueue.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

using namespace std;

struct DMSConfig {
    // config
    uint32_t fps;
    std::string video_path;
    std::string config_path;
};

static void GetTimeStamp(TimeStamp& timestamp) {
    auto now = std::chrono::steady_clock::now().time_since_epoch().count() / 1000L;
    timestamp.tv_sec = now / 1000L / 1000L;
    timestamp.tv_usec = now - timestamp.tv_sec * 1000L * 1000L;
}

struct ContextData {
    int dms_selfcheck_status;
    SDResult dms_reslut;
    int country;
    int dms_mode;
    int distraction_switch;
    int fatigue_switch;
};
int sendData(PpsWriter& pps_writer, const ContextData& ctxt_data) {
    // dms self check status
    std::vector< int > selfCheckResult;
    selfCheckResult.clear();
    selfCheckResult.push_back(0);
    selfCheckResult.push_back(161);
    selfCheckResult.push_back(6);
    selfCheckResult.push_back(6);
    selfCheckResult.push_back(0);
    selfCheckResult.push_back(0);
    selfCheckResult.push_back(0);
    selfCheckResult.push_back(0);
    selfCheckResult.push_back(0);
    selfCheckResult.push_back(ctxt_data.dms_selfcheck_status);
    pps_writer.sendData(selfCheckResult);

    // distraction monitoring attention monitoring
    if (ctxt_data.dms_reslut.person_number > 0) {
        std::vector< int > distractionResult;
        distractionResult.clear();
        distractionResult.push_back(0);
        distractionResult.push_back(161);
        distractionResult.push_back(6);
        distractionResult.push_back(6);
        distractionResult.push_back(0);
        distractionResult.push_back(1);
        distractionResult.push_back(0);
        distractionResult.push_back(0);
        distractionResult.push_back(0);
        auto dis_result =
            ctxt_data.dms_reslut.person_array_result[0].face_result.distraction_result;
        if (dis_result.result_valid) {
            switch (dis_result.level) {
                case M_DISTRACTION_LEVEL_NONE:
                    distractionResult.push_back(0x01);
                    break;
                case M_DISTRACTION_LEVEL_LIGHT:
                    distractionResult.push_back(0x02);
                    break;
                case M_DISTRACTION_LEVEL_MEDIUM:
                    distractionResult.push_back(0x03);
                    break;
                case M_DISTRACTION_LEVEL_HEAVY:
                    distractionResult.push_back(0x04);
                    break;
                case M_DISTRACTION_LEVEL_INVALID:
                    distractionResult.push_back(0x06);
                    break;
                default:
                    distractionResult.push_back(0x06);
                    break;
            }
        } else {
            distractionResult.push_back(0x00);
        }
        pps_writer.sendData(distractionResult);
    }

    // fatigue monitor
    std::vector< int > fatigueMonitorResult;
    fatigueMonitorResult.clear();
    fatigueMonitorResult.push_back(0);
    fatigueMonitorResult.push_back(161);
    fatigueMonitorResult.push_back(6);
    fatigueMonitorResult.push_back(6);
    fatigueMonitorResult.push_back(0);
    fatigueMonitorResult.push_back(2);
    fatigueMonitorResult.push_back(0);
    fatigueMonitorResult.push_back(0);
    fatigueMonitorResult.push_back(0);
    auto dro_result = ctxt_data.dms_reslut.person_array_result[0].face_result.drowsiness_result;
    if (dro_result.result_valid) {
        switch (dro_result.level) {
            case M_DROWSINESS_LEVEL_NONE:
                fatigueMonitorResult.push_back(4);
                break;
            case M_DROWSINESS_LEVEL_LIGHT:
                fatigueMonitorResult.push_back(1);
                break;
            case M_DROWSINESS_LEVEL_MEDIUM:
                fatigueMonitorResult.push_back(2);
                break;
            case M_DROWSINESS_LEVEL_HEAVY:
                fatigueMonitorResult.push_back(3);
                break;
            case M_DROWSINESS_LEVEL_INVALID:
                fatigueMonitorResult.push_back(0);
                break;
            default:
                fatigueMonitorResult.push_back(0);
                break;
        }
    } else {
        fatigueMonitorResult.push_back(5);
    }
    pps_writer.sendData(fatigueMonitorResult);

    // country
    std::vector< int > country;
    country.clear();
    country.push_back(0);
    country.push_back(161);
    country.push_back(6);
    country.push_back(6);
    country.push_back(0);
    country.push_back(3);
    country.push_back(0);
    country.push_back(0);
    country.push_back(0);
    country.push_back(ctxt_data.country);
    pps_writer.sendData(country);

    // dms mode
    std::vector< int > dmsMode;
    dmsMode.clear();
    dmsMode.push_back(0);
    dmsMode.push_back(161);
    dmsMode.push_back(6);
    dmsMode.push_back(6);
    dmsMode.push_back(0);
    dmsMode.push_back(4);
    dmsMode.push_back(0);
    dmsMode.push_back(0);
    dmsMode.push_back(0);
    dmsMode.push_back(ctxt_data.dms_mode);
    pps_writer.sendData(dmsMode);

    // distraction switch
    std::vector< int > distractionSwitch;
    distractionSwitch.clear();
    distractionSwitch.push_back(0);
    distractionSwitch.push_back(161);
    distractionSwitch.push_back(6);
    distractionSwitch.push_back(6);
    distractionSwitch.push_back(0);
    distractionSwitch.push_back(5);
    distractionSwitch.push_back(0);
    distractionSwitch.push_back(0);
    distractionSwitch.push_back(0);
    distractionSwitch.push_back(ctxt_data.distraction_switch);
    pps_writer.sendData(distractionSwitch);

    // fatigue switch
    std::vector< int > fatigueSwitch;
    fatigueSwitch.clear();
    fatigueSwitch.push_back(0);
    fatigueSwitch.push_back(161);
    fatigueSwitch.push_back(6);
    fatigueSwitch.push_back(6);
    fatigueSwitch.push_back(0);
    fatigueSwitch.push_back(6);
    fatigueSwitch.push_back(0);
    fatigueSwitch.push_back(0);
    fatigueSwitch.push_back(0);
    fatigueSwitch.push_back(ctxt_data.fatigue_switch);
    pps_writer.sendData(fatigueSwitch);
    return 0;
}

/**/
int dms_thread(unsigned char* buf_addr) {
    // handle
    std::shared_ptr< SDHandle > dms_handle =
        std::shared_ptr< SDHandle >(new SDHandle(nullptr), [](SDHandle* p) {
            SenseDriverDestroy(p);
            delete p;
        });

    // dms init
    std::string config_path = "./data/config/";
    auto ret = SenseDriverInit(
        config_path.c_str(), static_cast< int >(config_path.length()), dms_handle.get());
    if (MDMS_SUCCESS != ret) {
        std::cerr << "SenseDriverInit error, error code = " << ret << std::endl;
        return -1;
    }

    ContextData ctxt_data;
    ctxt_data.dms_selfcheck_status = 0x03;
    ctxt_data.country = 0x01;
    ctxt_data.dms_mode = 0x01;
    ctxt_data.distraction_switch = 0x01;
    ctxt_data.fatigue_switch = 0x01;

    DMSResultPrinter dms_result_printer;
    PpsWriter pps_writer;
    int pps_ret = pps_writer.init();
    if (0 != pps_ret) {
        std::cout << "pps init failed!" << std::endl;
        return -1;
    }

    // run
    while (true) {
        // GetTimeStamp
        TimeStamp time_stamp;
        GetTimeStamp(time_stamp);

        // get image
        // create image handle
        auto image_handle = CreateSingleImageHandle(buf_addr,
                                                    nullptr,
                                                    time_stamp,
                                                    MPixelFormat::M_PIX_FMT_GRAY8,
                                                    IMAGE_WIDTH,
                                                    IMAGE_HEIGHT,
                                                    MMemoryType::MEM_ON_CPU);

        // image handle deleter
        std::shared_ptr< char > image_handle_deletor(
            nullptr, [&](char* p) { DestroySingleImageHandle(&image_handle); });
        // dms result
        std::shared_ptr< SDResult > result(new SDResult, [](SDResult* p) {
            SenseDriverDestroyResult(p);
            delete p;
        });

        // run
        auto ret = SenseDriverRun(*dms_handle, image_handle, result.get());
        if (MDMS_SUCCESS != ret) {
            std::cerr << "SenseDriverRun failed with error: " << static_cast< int >(ret)
                      << std::endl;
            return -1;
        }

        // print result to screen
        dms_result_printer.PrintDMSResult(*result);
        ctxt_data.dms_reslut = *result;
        sendData(pps_writer, ctxt_data);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        std::cout << std::endl;
    }
    pps_writer.deInit();
};

int socket_thread() {
    BYDD9Msg::MessageService msg_service(2048, 2048);
    msg_service.FuncBatchRegister();

    Socket::ServerTCP tcp_server(25535);
    tcp_server.ServerInit(false);
    tcp_server.SetSelectTimeout(1000);

    std::function< int(int fd) > msg_proc = [&msg_service](int fd) {
        return msg_service.MessageRecv(fd);
    };
    tcp_server.SetRecvProcFunc(msg_proc);
    tcp_server.ServerEventLoop();
}

int main(int argc, char* argv[]) {
    // create instance
    CameraHandle& instance = CameraHandle::getInstance();

    // init
    instance.initialize();

    // start
    instance.start();

    unsigned char* buf_addr = instance.get_buf_addr();

    // wait
    while (true) {
        std::cout << "dms_thread start" << std::endl;
        std::thread thread_dms(dms_thread, buf_addr);
        std::thread thread_socket(socket_thread);
        thread_dms.join();
        thread_socket.join();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // stop
    instance.stop();

    // uninit
    instance.uninitialize();

    return 0;
}
