#ifndef _SENSEDRIVER_SAMPLES_CAPI_BLOCKQUEUE_HPP_
#define _SENSEDRIVER_SAMPLES_CAPI_BLOCKQUEUE_HPP_

#include <mutex>
#include <queue>
#include <iostream>
#include <condition_variable>

template < typename T_ > class MMessageQueue {
public:
    MMessageQueue() : queue_(), mutex_(), condition_() {}
    virtual ~MMessageQueue() {}
    virtual void push(T_&& t) {
        std::unique_lock< std::mutex > lock(this->mutex_);
        this->queue_.push(t);
        this->condition_.notify_all();
    }
    virtual void push(T_& t) {
        std::unique_lock< std::mutex > lock(this->mutex_);
        this->queue_.push(t);
        this->condition_.notify_all();
    }

    inline void clear() {
        std::unique_lock< std::mutex > lock(this->mutex_);
        std::queue< T_ > q;
        this->queue_.swap(q);
    }

    T_ pop() {
        std::unique_lock< std::mutex > lock(this->mutex_);
        while (this->queue_.size() <= 0U) {
            this->condition_.wait(lock);
        }

        auto ret = std::move(this->queue_.front());
        this->queue_.pop();
        return ret;
    }

protected:
    std::queue< T_ > queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

template < typename T_ > class MBoundedUnblockedQueue final : public MMessageQueue< T_ > {
public:
    MBoundedUnblockedQueue(uint32_t size) : size_(size) {}
    void push(T_&& t) override {
        std::unique_lock< std::mutex > lock(this->mutex_);
        if (this->queue_.size() >= this->size_) {
            this->queue_.pop();
        }
        this->queue_.push(t);
        this->condition_.notify_all();
    }

    void push(T_& t) override {
        std::unique_lock< std::mutex > lock(this->mutex_);
        if (this->queue_.size() >= this->size_) {
            this->queue_.pop();
        }
        this->queue_.push(t);
        this->condition_.notify_all();
    }

private:
    uint32_t size_;
};

#endif