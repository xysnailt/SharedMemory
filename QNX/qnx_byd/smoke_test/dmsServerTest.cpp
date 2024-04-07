#include <iostream>
#include <vector>
#include <sys/pps.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int main(int argc, char **argv)
{
    const char *test_str = "/tmp/pps/spi_service/tx?wait,delta,nopersist";
    int test_fd = open(test_str, O_RDONLY);
    if (test_fd < 0){
        cout << "open " << test_str << " failed." << endl;
        return -1;
    }

    pps_decoder_t decoder;
    pps_decoder_error_t ret;

    while (1) {
        char buffer[1024] = {0};
        int read_len = read(test_fd, buffer, sizeof(buffer));
        if (read_len > 0) {
            int ret = pps_decoder_initialize(&decoder, nullptr);
            if (ret == PPS_DECODER_OK) {
                pps_decoder_parse_pps_str(&decoder, buffer);
                pps_decoder_push(&decoder, nullptr);

                int t = pps_decoder_type(&decoder, nullptr);
                switch (t) {
                    case PPS_TYPE_OBJECT:
                    {
                        const char *name = pps_decoder_name(&decoder);
                        std::cout << "name:" << std::string(name) << std::endl;
                        if (std::string(name) == "xA106") {
                            if (PPS_DECODER_OK == pps_decoder_push(&decoder, "xA106")) {
                                if (PPS_DECODER_OK == pps_decoder_push_array(&decoder, "data")) {
                                    int length = pps_decoder_length(&decoder);
                                    vector<int> data_vec(length, 0);
                                    for (int i = 0; i < length; ++i) {
                                        int value = 0;
                                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                                        if (PPS_DECODER_OK == ret) {
                                            data_vec[i] = value;
                                        } else {
                                            cout << "pps_decoder_get_int error " << ret << std::endl;
                                        }
                                    }
                                    for (int i = 0; i < length; ++i) {
                                        cout << "idx: " << i << " --- " << data_vec[i] << endl;
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
        } else {
            sleep(1);
        }
    }
    pps_decoder_cleanup(&decoder);

    return 0;
}