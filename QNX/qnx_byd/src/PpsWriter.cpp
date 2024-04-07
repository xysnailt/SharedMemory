#include "PpsWriter.h"

#include "PpsDataType.h"

#include <iostream>
#include <sys/pps.h>
#include <vector>
extern "C" {
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
}

int PpsWriter::init() {
    const char* test_str = "/tmp/pps/spi_service/tx?nopersist";

    fd_ = open(test_str, O_RDWR | O_CREAT);

    if (fd_ < 0) {
        std::cout << "open " << test_str << " failed." << std::endl;
        is_fd_opened_ = false;
        return -1;
    }

    is_fd_opened_ = true;

    return 0;
}

int PpsWriter::deInit() {}

int PpsWriter::sendData(std::vector< int >& data) {
    pps_encoder_t encoder;
    pps_encoder_initialize(&encoder, false);
    pps_encoder_start_object(&encoder, "xA106");
    pps_encoder_start_array(&encoder, "data");

    int len = data.size();
    for (int i = 0; i < len; ++i) {
        pps_encoder_add_int(&encoder, 0, data[i]);
        // std::cout << data[i] << ",";
    }
    // std::cout << std::endl;

    pps_encoder_end_array(&encoder);
    pps_encoder_end_object(&encoder);

    if (pps_encoder_buffer(&encoder) != nullptr) {
        int ret = write(fd_, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
        if (ret > 0) {
            // std::cout << "send msg len " << ret << std::endl;
        } else {
            std::cout << "write failed!" << std::endl;
        }
    }
    pps_encoder_cleanup(&encoder);
}