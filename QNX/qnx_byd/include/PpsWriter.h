#ifndef PPSWRITER_H
#define PPSWRITER_H
#include <string>
#include <sys/pps.h>

class PpsWriter {
public:
    PpsWriter(){};
    ~PpsWriter(){};
    PpsWriter(const PpsWriter&) = delete;
    PpsWriter(PpsWriter&&) = delete;
    PpsWriter& operator=(const PpsWriter&) = delete;
    PpsWriter& operator=(PpsWriter&&) = delete;

    int init();
    int deInit();
    int sendData(std::vector< int >& data);

private:
    int fd_;
    bool is_fd_opened_;
};
#endif