#pragma once

#include <deque>
#include <mutex>
#include <thread>
#include "mediabase.hh"
#include "semaphore.hh"

namespace LQF {

class Looper {

public:
    Looper(const int deque_max_size = 30);
    virtual ~Looper();

    void Post(int what, void *data, bool flush = false);
    void Stop();

    virtual void handle(int waht, void *data);

private:
    virtual void addmsg(LooperMessage *msg, bool flush);
    static void* trampoline(void *p);
    void loop();

protected:
    std::deque<LooperMessage*> msg_queue_;
    std::thread *worker_;
    Semaphore *head_data_available_;
    std::mutex queue_mutex_;
    bool running_;
    int deque_max_size_ = 30;
};
}