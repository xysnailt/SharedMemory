#include "DmsManager.h"
#include "MessageService.hpp"
#include "SocketServer.hpp"

#include <iostream>

int main(int argc, char* argv[]) {

    // init
    DmsManager::getInstance().init();

    Socket::ServerTCP tcp_server(25535);
    tcp_server.ServerInit(false);
    tcp_server.SetSelectTimeout(1000);

    BYDD9Msg::MessageService msg_service(2048, 2048);
    msg_service.FuncBatchRegister();
    std::function< int(int fd) > msg_proc = [&msg_service](int fd) {
        return msg_service.MessageRecv(fd);
    };
    tcp_server.SetRecvProcFunc(msg_proc);

    std::function< int(const SDResult&) > result_proc = [&tcp_server,
                                                         &msg_service](const SDResult& result) {
        return msg_service.DmsResultProc(tcp_server.GetSocketFd(), result);
    };

    // register result proc
    DmsManager::getInstance().registerResultProc(result_proc);

    // start
    std::thread thread_socket(&Socket::ServerTCP::ServerEventLoop, &tcp_server);
    DmsManager::getInstance().start();

    // wait
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // stop
    DmsManager::getInstance().stop();

    // deinit
    DmsManager::getInstance().deInit();

    thread_socket.join();

    return 0;
}
