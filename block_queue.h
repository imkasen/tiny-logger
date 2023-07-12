#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

template<typename T>
class BlockQueue
{
private:
    std::queue<T> que;
    typename std::queue<T>::size_type cap;

    std::mutex mtx;
    std::atomic_bool isClose;

    std::condition_variable consumer;
    std::condition_variable producer;

public:
    explicit BlockQueue(const size_t maxCap);
    ~BlockQueue();

    BlockQueue() = delete;
    BlockQueue(const BlockQueue &rhs) = delete;
    BlockQueue(BlockQueue &&rhs) = delete;
    BlockQueue &operator=(const BlockQueue &rhs) = delete;
    BlockQueue &operator=(BlockQueue &&rhs) = delete;

    void close();
    void clear();
    void flush();
    bool empty();
    bool full();

    typename std::queue<T>::size_type size();
    typename std::queue<T>::size_type capacity();

    T front();
    T back();

    void push(const T &item);
    bool pop();
};

template<typename T>
BlockQueue<T>::BlockQueue(const size_t maxCap) : cap(maxCap)
{
    this->isClose = ATOMIC_VAR_INIT(false);
}

template<typename T>
BlockQueue<T>::~BlockQueue()
{
    this->close();
}

template<typename T>
void BlockQueue<T>::close()
{
    {
        std::lock_guard<std::mutex> lck(mtx);
        while (!this->que.empty())
        {
            this->que.pop();
        }
        this->isClose = true;
    }
    this->producer.notify_all();
    this->consumer.notify_all();
}

template<typename T>
void BlockQueue<T>::clear()
{
    std::lock_guard<std::mutex> lck(mtx);
    while (!this->que.empty())
    {
        this->que.pop();
    }
}

template<typename T>
void BlockQueue<T>::flush()
{
    std::lock_guard<std::mutex> lck(mtx);
    this->consumer.notify_one();
}

template<typename T>
bool BlockQueue<T>::empty()
{
    std::lock_guard<std::mutex> lck(mtx);
    return this->que.empty();
}

template<typename T>
bool BlockQueue<T>::full()
{
    std::lock_guard<std::mutex> lck(mtx);
    return this->que.size() >= this->cap;
}

template<typename T>
typename std::queue<T>::size_type BlockQueue<T>::size()
{
    std::lock_guard<std::mutex> lck(mtx);
    return this->que.size();
}

template<typename T>
typename std::queue<T>::size_type BlockQueue<T>::capacity()
{
    std::lock_guard<std::mutex> lck(mtx);
    return this->cap;
}

template<typename T>
T BlockQueue<T>::front()
{
    std::lock_guard<std::mutex> lck(mtx);
    return this->que.front();
}

template<typename T>
T BlockQueue<T>::back()
{
    std::lock_guard<std::mutex> lck(mtx);
    return this->que.back();
}

template<typename T>
void BlockQueue<T>::push(const T &item)
{
    std::unique_lock<std::mutex> lck(mtx);
    while (this->que.size() >= this->cap)
    {
        this->producer.wait(lck);
    }
    this->que.push(item);
    this->consumer.notify_one();
}

template<typename T>
bool BlockQueue<T>::pop()
{
    std::unique_lock<std::mutex> lck(mtx);
    while (this->que.empty())
    {
        this->consumer.wait(lck);
        if (this->isClose)
        {
            return false;
        }
    }
    this->que.pop();
    this->producer.notify_one();
    return true;
}

#endif
