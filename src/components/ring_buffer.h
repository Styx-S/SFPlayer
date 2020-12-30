//
//  ring_buffer.h
//  SFPlayer
//
//  Created by StyxS on 2020/12/24.
//

#ifndef ring_buffer_h
#define ring_buffer_h

namespace sfplayer {
    template<typename T>
    class RingBuffer {
    public:
        explicit RingBuffer(size_t size);
        ~RingBuffer();
        
        /* PlayerElement中不能使用WaitAndRead 会导致无法响应播放控制 */
        /* 但是可以使用WaitAndWrite，因为播放控制是从前往后的，后面的组件仍然会进行消费*/
        
        bool Write(std::shared_ptr<T> t);
        std::shared_ptr<T> Read();
        bool WaitAndWrite(std::shared_ptr<T> t);
        std::shared_ptr<T> WaitAndRead();

        void Clear();
        
        /// 注意先取个数，再Read或Write的用法不是线程安全的
        size_t GetLength();
        
        /* 需要和lock/unlock配合 */
        
        void UnsafeWrite(std::shared_ptr<T> t);
        std::shared_ptr<T> UnsafeRead();
        
        std::shared_ptr<T> UnsafeFront();
        void UnsafePop();
        
        size_t UnsafeGetLength();
        
    private:
        std::shared_ptr<T> *buffer;
        size_t size_;
        // 可读区域头
        size_t head_;
        // 可读区域尾
        size_t tail_;
        
    public:
        std::mutex mutex_;
        std::condition_variable cond_;
        
    };


    template<typename T>
    RingBuffer<T>::RingBuffer(size_t size)
    : size_(size)
    , head_(0)
    , tail_(0) {
        buffer = new std::shared_ptr<T>[size];
    }

    template<typename T>
    RingBuffer<T>::~RingBuffer() {
        delete [] buffer;
    }

    // 由调用方保证可写和临界区
    template<typename T>
    void RingBuffer<T>::UnsafeWrite(std::shared_ptr<T> t) {
        buffer[tail_++] = t;
        tail_ %= size_;
}

    // 由调用方保证可读和临界区
    template<typename T>
    std::shared_ptr<T> RingBuffer<T>::UnsafeRead() {
        std::shared_ptr<T> t = buffer[head_];
        // 这里释放一下计数，否则需要到下一次使用这个位置时才释放
        buffer[head_] = nullptr;
        head_++;
        head_ %= size_;
        return t;
    }

    template<typename T>
    std::shared_ptr<T> RingBuffer<T>::UnsafeFront() {
        if (head_ != tail_) {
            return buffer[head_];
        } else {
            return nullptr;
        }
    }

    template<typename T>
    void RingBuffer<T>::UnsafePop() {
        if (head_ != tail_) {
            buffer[head_] = nullptr;
            head_++;
            head_ %= size_;
        }
    }

    template<typename T>
    size_t RingBuffer<T>::UnsafeGetLength() {
        if (head_ <= tail_) {
            return tail_ - head_;
        } else {
            return size_ - head_ + tail_;
        }
    }

    template<typename T>
    bool RingBuffer<T>::Write(std::shared_ptr<T> t) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (head_ != (tail_ + 1) % size_) {
            UnsafeWrite(t);
            cond_.notify_all();
            return true;
        } else {
            return false;
        }
    }

    template<typename T>
    bool RingBuffer<T>::WaitAndWrite(std::shared_ptr<T> t) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, ([this]() {
            return head_ != (tail_ + 1 ) % size_;
        }));
        
        UnsafeWrite(t);
        cond_.notify_all();
        return true;
    }

    template<typename T>
    std::shared_ptr<T> RingBuffer<T>::Read() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (head_ != tail_) {
            std::shared_ptr<T> t = UnsafeRead();
            cond_.notify_all();
            return t;
        } else {
            return nullptr;
        }
    }

    template<typename T>
    std::shared_ptr<T> RingBuffer<T>::WaitAndRead() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, ([this]() {
            return head_ != tail_;
        }));
        
        std::shared_ptr<T> t = UnsafeRead();
        cond_.notify_all();
        return t;
    }

    template<typename T>
    size_t RingBuffer<T>::GetLength() {
        std::lock_guard<std::mutex> lock(mutex_);
        return UnsafeGetLength();
    }

    template<typename T>
    void RingBuffer<T>::Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (head_ != tail_) {
            buffer[head_++] = nullptr;
            head_ %= size_;
        }
        cond_.notify_all();
    }
}


#endif /* ring_buffer_h */
