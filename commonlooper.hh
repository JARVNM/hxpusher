#pragma once

#include <thread>
#include "mediabase.hh"

namespace LQF {
class CommonLooper {
public:
    CommonLooper();
    virtual ~CommonLooper();
    virtual RET_CODE start();
    virtual void stop();
    virtual void loop() = 0;

private:
    static void* trampoline(void* p);

protected:
    std::thread *worker_ = NULL;
    bool request_exit_ = false;
    bool running_ = false;
};
}