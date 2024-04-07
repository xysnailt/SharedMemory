#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace Socket {
class SocketTCP {
public:
    virtual ~SocketTCP() {
        if (m_socket_fd > 0) {
            close(m_socket_fd);
            m_socket_fd = -1;
        }
    }

    // ----------------------------------
    //! @brief Set port for TCP service
    void SetPort(const int16_t port) { m_port = port; }
    // ----------------------------------
    //! @brief Set ip addr for TCP service
    void SetIpAddr(const std::string& ip_addr) { m_ip_addr = ip_addr; }
    // ----------------------------------
    //! @brief Get ip addr of TCP service
    int16_t GetPort() const { return m_port; }
    // ----------------------------------
    //! @brief Get ip addr of TCP service
    std::string GetIpAddr() { return m_ip_addr; }

    // ----------------------------------
    //! @brief Send data by socket
    //! @param[in] data : Data to be sent.
    //! @param[in] data_len : The length of data to be sent.
    ssize_t SendData(const uint8_t* data, const ssize_t data_len);

    // ----------------------------------
    //! @brief Get connected socket fd
    int GetSocketFd() const { return m_socket_fd; }

protected:
    int16_t m_port{-1};      //! service port
    int m_socket_fd{-1};     //! connected fd
    std::string m_ip_addr{}; //! service ip
};

class ServerTCP final : public SocketTCP {
public:
    // ----------------------------------
    //! @brief Construct tcp server with ip and port
    //! @param[in] port : service port.
    //! @param[in] ip : service ip.
    //! @note
    //! if ip is nullptr，service ip will use INADDR_ANY
    ServerTCP(const int16_t port, const char* ip = nullptr);
    ~ServerTCP() {
        m_running = false;
        if (m_listen_fd > 0) {
            close(m_listen_fd);
            m_listen_fd = -1;
        }
    }
    // ----------------------------------
    //! @brief Select timeout time
    //! @param[in] timeout_ms : ms.
    void SetSelectTimeout(const int timeout_ms);
    // ----------------------------------
    //! @brief Create tcp server
    //! @param[in] block : using block mode or noblock mode
    //! @note
    //! this func will get valid listen fd
    //! need invoke before ServerEventLoop
    int ServerInit(const bool block);
    // ----------------------------------
    //! @brief tcp server event loop
    //! @note
    //! this func will block the thread after being invoked
    int ServerEventLoop();
    // ----------------------------------
    //! @brief Set event proc func
    //! @param[in] recv_proc : customized proc func
    void SetRecvProcFunc(std::function< int(int fd) > recv_proc) { m_recv_proc = recv_proc; }

    // ----------------------------------
    //! @brief read specified length data
    //! @param[in] fd : socket fd
    //! @param[in] buff : save data buff
    //! @param[in] read_len : need read length
    //! @return Actual read length
    //! @note
    //! this func can't return before read finished, unless fd being closed
    static int ReadDataN(const int fd, uint8_t* buff, const int read_len);

private:
    bool SetNoBlock(const int fd);

private:
    bool m_running{false};
    int m_listen_fd{-1};
    struct timeval m_tv {
        0, 0
    };
    std::function< int(int fd) > m_recv_proc;
};

} // namespace Socket
