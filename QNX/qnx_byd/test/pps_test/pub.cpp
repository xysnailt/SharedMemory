#include <iostream>
#include <vector>
#include <mutex>
#include <sys/pps.h>

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
}

using namespace std;

 class PPSTools {
    public:
        PPSTools(PPSTools const&) = delete;
        PPSTools& operator=(PPSTools const&) = delete;

        static PPSTools* GetInstance() {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {
                instance = new PPSTools();
            }
            return instance;
        }
        // 功能区
        void PPSSend(vector<int> data);
        void PPSGet();

    private:
        static PPSTools* instance;
        static std::mutex mutex;

    protected:
        const char *SHARED_MEMORY_ADDR =  "/tmp/pps/spi_service/tx?nopersist";
        const int PPS_BUFFER_CACHE_SIZE = 1024;
        PPSTools() {};
};

void PPSTools::PPSSend(vector<int> data)
{

    int test_fd= open(SHARED_MEMORY_ADDR, O_RDWR | O_CREAT);
    if (test_fd < 0){
        cout << "ERROR, open " << SHARED_MEMORY_ADDR << " failed." << endl;
        return;
    }
    
    pps_encoder_t encoder;
    // TO DO: while 1和sleep仅用作测试用途，后面需要删除
    while (1) {
        pps_encoder_initialize(&encoder, false);
        pps_encoder_start_object(&encoder, "data");
        pps_encoder_add_int(&encoder, "len", data.size());
        pps_encoder_start_array(&encoder, "data");

        int len = data.size();
        for (int i = 0; i < len; ++i) {
            pps_encoder_add_int(&encoder, 0, data[i]);
        }

        pps_encoder_end_array(&encoder);
        pps_encoder_end_object(&encoder);

    // TO DO: 需要返回发送结果，是否发送成功
        if ( pps_encoder_buffer(&encoder) != nullptr ) {
            int ret = write(test_fd, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
            if (ret > 0) {
                cout << "send msg len " << ret << endl;
            }
        }

        sleep(2);
    }
    pps_encoder_cleanup(&encoder);
}

void PPSTools::PPSGet()
{
    // TO DO：需要检查下是否清空了之前的接受结果 
    // TO DO: 这里需要返回失败状态
    int test_fd = open(SHARED_MEMORY_ADDR, O_RDONLY);
    if (test_fd < 0){
        cout << "ERROR, [PPS] open " << SHARED_MEMORY_ADDR << " failed." << endl;
        return;
    }

    pps_decoder_t decoder;
    pps_decoder_error_t ret;

    while (1) {
        char buffer[PPS_BUFFER_CACHE_SIZE] = {0};
        int read_len = read(test_fd, buffer, sizeof(buffer));
        if (read_len <= 0) {
            sleep(1);
            // TO DO： 这里需要给打印分级，这里不需要一直打印
            cout<<"[PPS] No Data" <<endl;
        } 
        int ret = pps_decoder_initialize(&decoder, nullptr);
        if (ret != PPS_DECODER_OK) {
            cout<<"ERROR, [PPS] Fail to initialize PPS Decoder"<<endl;
            return;
        }
        pps_decoder_parse_pps_str(&decoder, buffer);
        pps_decoder_push(&decoder, nullptr);

        int ppsType = pps_decoder_type(&decoder, nullptr);
        switch (ppsType) {
            case PPS_TYPE_OBJECT:
            {
                const char *name = pps_decoder_name(&decoder);
                if (std::string(name) == "data") {
                    if (PPS_DECODER_OK != pps_decoder_push(&decoder, "data")) {
                        cout<<"ERROR,[PPS] Fail to decode PPS object \"data\""<<endl;
                        return;
                    }

                    if (PPS_DECODER_OK != pps_decoder_push_array(&decoder, "data")) {
                        cout<<"ERROR, [PPS] Fail to decode array   \"data\" in the PPS object "<<endl;
                        return;
                    }

                    int length = pps_decoder_length(&decoder);
                    vector<int> data_vec(length, 0);
                    for (int i = 0; i < length; ++i) {
                        int value = 0;
                        int ret = pps_decoder_get_int(&decoder, nullptr, &value);
                        if (PPS_DECODER_OK == ret) {
                            data_vec[i] = value;
                        } else {
                            cout << "ERROR, [PPS] pps_decoder_get_int error " << ret << std::endl;
                        }
                    }
                    for (int i = 0; i < length; ++i) {
                        cout << "idx: " << i << " --- " << data_vec[i] << endl;
                    }
                    // 退出PPS Object中的array
                    pps_decoder_pop(&decoder);
                    // 退出PPS Object
                    pps_decoder_pop(&decoder);
                }
            }
            case PPS_TYPE_STRING:
            case PPS_TYPE_NULL:
            default:
                break;
        }
        pps_decoder_pop(&decoder);
    }
    pps_decoder_cleanup(&decoder);
}

    // 初始化静态成员变量
    PPSTools* PPSTools::instance{nullptr};
    std::mutex PPSTools::mutex;


int main(int argc, char **argv)
{
    vector<int> data_vec;
    data_vec.push_back(9);
    data_vec.push_back(8);
    data_vec.push_back(7);
    data_vec.push_back(6);
    data_vec.push_back(5);

    PPSTools* pps = PPSTools::GetInstance();
    pps->PPSSend(data_vec);

    return 0;
}