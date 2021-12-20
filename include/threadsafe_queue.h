#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>


template <typename T>
class threadsafe_queue
{
    mutable std::mutex _mut;
    std::queue<T> _queue;
    std::condition_variable _cv;

public:
    threadsafe_queue(){}

    threadsafe_queue& operator=(const threadsafe_queue& other) = delete;

    threadsafe_queue(const threadsafe_queue& other)
    {
        std::lock_guard<std::mutex> lg(_mut);
        _queue = other._queue;
    }

    void push(const T& new_value)
    {
        std::lock_guard<std::mutex> lg(_mut);
        _queue.push(new_value);
        _cv.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> ul(_mut);
        _cv.wait(ul, [this](){return !_queue.empty();});
        value = _queue.front();
        _queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> ul(_mut);
        _cv.wait(ul, [this](){return !_queue.empty();});
        std::shared_ptr<T> res = std::make_shared<T>(_queue.front());
        _queue.pop();
        return res;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lg(_mut);
        if(_queue.empty())
            return false;
        value = _queue.front();
        _queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lg(_mut);
        if(_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(_queue.front()));
        _queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lg(_mut);
        return _queue.empty();
    }

    std::size_t size() const
    {
        std::lock_guard<std::mutex> lg(_mut);
        return _queue.size();
    }

};