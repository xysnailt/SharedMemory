#ifndef _SENSEDRIVER_SAMPLES_CAPI_IMAGEREADER_HPP_
#define _SENSEDRIVER_SAMPLES_CAPI_IMAGEREADER_HPP_
#include "BlockQueue.hpp"
#include "MImageHelper.h"
#include "SenseDriverCommon.h"
#include "sensedriver_interface.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <thread>

#ifdef CONFIG_AD_NN_ENABLE_CVFLOW
#include "hardware.hpp"
#endif

struct DMSConfig {
    // config
    uint32_t fps;
    std::string video_path;
    std::string config_path;
    // handle
    std::shared_ptr< SDHandle > handle =
        std::shared_ptr< SDHandle >(new SDHandle(nullptr), [](SDHandle* p) {
            SenseDriverDestroy(p);
            delete p;
        });
};

static inline void YU12_to_YUV420SP(unsigned char* p, int origin_size, bool nv12_or_nv21) {
    long unsigned int nLenY = origin_size;
    long unsigned int nLenUV = nLenY / 4;

    std::vector< unsigned char > u(nLenUV);
    std::vector< unsigned char > v(nLenUV);

    memcpy(u.data(), p + nLenY, nLenUV);
    memcpy(v.data(), p + nLenY + nLenUV, nLenUV);
    for (long unsigned int i = 0; i < nLenUV; i++) {
        p[nLenY + 2 * i] = (nv12_or_nv21) ? u[i] : v[i];
        p[nLenY + 2 * i + 1] = (nv12_or_nv21) ? v[i] : u[i];
    }
}

static void GetTimeStamp(MTimeStamp& timestamp) {
    auto now = std::chrono::steady_clock::now().time_since_epoch().count() / 1000L;
    timestamp.tv_sec = now / 1000L / 1000L;
    timestamp.tv_usec = now - timestamp.tv_sec * 1000L * 1000L;
}

static void UpdateTimeStampByFPS(uint32_t fps, MTimeStamp& cur_time_stamp) {
    cur_time_stamp.tv_usec += (1000L * 1000L) / fps;
    cur_time_stamp.tv_sec += cur_time_stamp.tv_usec / (1000L * 1000L);
    cur_time_stamp.tv_usec = cur_time_stamp.tv_usec % (1000L * 1000L);
}

struct ImageWithTimestamp {
    cv::Mat image;
    MTimeStamp time_stamp;
};

// todo put (image frame rate + image queue size + image list or video flag) to json
class ImageReader {
public:
    ImageReader(const uint32_t frame_rate, const uint32_t queue_size = 1)
        : frame_rate_(frame_rate),
          image_queue_(new MBoundedUnblockedQueue< ImageWithTimestamp >(queue_size)),
          is_run_(true) {
        interval_micro_seconds_ = 1000000 / frame_rate_;
    }

    int ReadImage(const std::string& path, bool is_video = false);

private:
    int ReadImageFroADImageList(const std::string& path);
    int ReadImageFromVideo(const std::string& path);
    void PushDataWithFrameControl(const std::chrono::high_resolution_clock::time_point& start,
                                  const cv::Mat& image);

public:
    // run status
    std::atomic< bool > is_run_;
    // image queue
    std::shared_ptr< MBoundedUnblockedQueue< ImageWithTimestamp > > image_queue_;

private:
    uint32_t frame_rate_ = 30;
    int interval_micro_seconds_ = 0;
};

int ImageReader::ReadImage(const std::string& path, bool is_video) {
    if (is_video) {
        return ReadImageFromVideo(path);
    } else {
        return ReadImageFroADImageList(path);
    }
}

// read image list
int ImageReader::ReadImageFroADImageList(const std::string& path) {

    const std::string list_file_path = path + "/list";
    std::ifstream fin(list_file_path);
    if (!fin) {
        std::cerr << "open image list failed: " << list_file_path << std::endl;
    }

    std::string image_name;
    while (is_run_) {
        if (!std::getline(fin, image_name)) { // file end
            // push empty image to queue
            image_queue_->push({cv::Mat(), MTimeStamp()});
            break;
        }
        auto start = std::chrono::high_resolution_clock::now();
        // read image
        std::string image_full_path = path + "/" + image_name;
        const auto image = cv::imread(image_full_path);
        if (image.empty()) {
            std::cerr << "read image error. " << std::endl;
            // push empty image to queue
            image_queue_->push({cv::Mat(), MTimeStamp()});
            return -1;
        }
        PushDataWithFrameControl(start, image);
    }
    return 0;
}

// read video
int ImageReader::ReadImageFromVideo(const std::string& path) {
    cv::VideoCapture video_capture;
    if (!video_capture.open(path)) {
        std::cerr << "open video failed: " << path << std::endl;
        // push empty image to queue
        image_queue_->push({cv::Mat(), MTimeStamp()});
        return -1;
    }

    cv::Mat image;
    while (is_run_) {
        auto start = std::chrono::high_resolution_clock::now();
        video_capture >> image;
        if (image.empty()) {
            // push empty image to queue
            image_queue_->push({cv::Mat(), MTimeStamp()});
            break;
        }
        PushDataWithFrameControl(start, image);
    }
    return 0;
}

void ImageReader::PushDataWithFrameControl(
    const std::chrono::high_resolution_clock::time_point& start,
    const cv::Mat& image) {
    MTimeStamp time_stamp;
    GetTimeStamp(time_stamp);
    // push image to queue
    image_queue_->push({image, time_stamp});

    auto end = std::chrono::high_resolution_clock::now();

    // use duration time to control frame rate
    long duration = std::chrono::duration_cast< std::chrono::microseconds >(end - start).count();

    if (duration < interval_micro_seconds_) {
        std::this_thread::sleep_for(std::chrono::microseconds(interval_micro_seconds_ - duration));
    }
}

#endif