#include "SocketServer.hpp"

#include <arpa/inet.h>
#include <cmath>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>

namespace Socket {

ServerTCP::ServerTCP(const int16_t port, const char* ip) {
    if (-1 != port) {
        SetPort(port);
    }
    if (nullptr != ip) {
        SetIpAddr(ip);
    }
}

bool ServerTCP::SetNoBlock(const int fd) {
    int opts = fcntl(fd, F_GETFL);
    if (opts < 0) {
        std::cout << "logui fcntl get failed, errno=" << errno << '\n';
        return false;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) < 0) {
        std::cout << "logui fcntl set failed, errno=" << errno << '\n';
        return false;
    }
    return true;
}

void ServerTCP::SetSelectTimeout(const int timeout_ms) {
    if (timeout_ms >= 1000) {
        m_tv.tv_sec = std::ceil(timeout_ms / 1000);
        int time_ms = timeout_ms;
        m_tv.tv_usec = (time_ms % 1000) * 1000;
        std::cout << "timeout: " << m_tv.tv_sec << "." << m_tv.tv_usec << '\n';
    } else {
        m_tv.tv_sec = 0;
        m_tv.tv_usec = timeout_ms * 1000;
    }
}

int ServerTCP::ServerInit(const bool block) {
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (!block) {
        SetNoBlock(m_listen_fd);
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (m_ip_addr.empty()) {
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        inet_pton(AF_INET, m_ip_addr.c_str(), &server_addr.sin_addr);
    }
    server_addr.sin_port = htons(m_port);

    std::cout << "port: " << m_port << "  m_ip_addr: " << m_ip_addr << '\n';

    int opt = 1;
    if (-1 ==
        setsockopt(
            m_listen_fd, SOL_SOCKET, SO_REUSEADDR, static_cast< const void* >(&opt), sizeof(opt))) {
        std::cout << "setsockopt error, errno=" << errno << '\n';
        return -1;
    }
    if (-1 ==
        setsockopt(
            m_listen_fd, SOL_SOCKET, SO_REUSEPORT, static_cast< const void* >(&opt), sizeof(opt))) {
        std::cout << "setsockopt error, errno=" << errno << '\n';
        return -1;
    }
    if (-1 == bind(m_listen_fd,
                   reinterpret_cast< sockaddr* >(&server_addr),
                   static_cast< socklen_t >(sizeof(server_addr)))) {
        std::cout << "bind error, errno=" << errno << '\n';
        return -1;
    }

    if (0 != listen(m_listen_fd, 2)) {
        std::cout << "listen error, errno=" << errno << '\n';
        return false;
    }

    signal(SIGPIPE, SIG_IGN);
    m_running = true;

    std::cout << "listen fd: " << m_listen_fd << " waiting connect ..." << '\n';

    return 0;
}

int ServerTCP::ServerEventLoop() {
    fd_set client_fdset;
    FD_ZERO(&client_fdset);
    int max_socket = m_listen_fd;
    while (m_running) {
        FD_SET(m_listen_fd, &client_fdset);
        max_socket = (m_listen_fd > m_socket_fd) ? m_listen_fd : m_socket_fd;
        struct timeval tv = m_tv;
        int err = select(max_socket + 1, &client_fdset, nullptr, nullptr, &tv);
        if (0 > err) {
            std::cout << "select err, errno=" << errno << '\n';
            continue;
        }

        if (FD_ISSET(m_listen_fd, &client_fdset)) {
            struct sockaddr_in client_addr;
            size_t size = sizeof(struct sockaddr_in);
            m_socket_fd = accept(m_listen_fd,
                                 reinterpret_cast< struct sockaddr* >(&client_addr),
                                 reinterpret_cast< socklen_t* >(&size));
            if (m_socket_fd < 0) {
                continue;
            }
            // int nNetTimeout = 10;
            // setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, ( char* )&nNetTimeout, sizeof(int));
            // SetSocketSize(fd, 5);
            std::cout << "one client connect svr, m_socket_fd: " << m_socket_fd << '\n';
        }

        if (m_socket_fd > 0) {
            if (FD_ISSET(m_socket_fd, &client_fdset) && m_recv_proc) {
                int ret = m_recv_proc(m_socket_fd);
                if (ret < 0) {
                    if (errno != EAGAIN) {
                        shutdown(m_socket_fd, 0);
                        close(m_socket_fd);
                        m_socket_fd = -1;
                    }
                }
            }
            FD_SET(m_socket_fd, &client_fdset);
        }
    }
}

int ServerTCP::ReadDataN(const int fd, uint8_t* buff, const int read_len) {
    int recv_len = 0;
    while (recv_len < read_len) {
        ssize_t ret = read(fd, &buff[recv_len], read_len - recv_len);
        if (ret > 0) {
            recv_len += static_cast< int >(ret);
        } else {
            break;
        }
    }

    return recv_len;
}

} // namespace Socket
