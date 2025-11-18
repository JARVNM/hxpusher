#include "commonlooper.hh"
#include <cstddef>
#include "log/easylogging++.h"
#include "mediabase.hh"

namespace LQF {
void* CommonLooper::trampoline(void* p) {
    ((CommonLooper*)p)->loop();
    return NULL;
}

CommonLooper::CommonLooper() {
    request_exit_ = false;
}

RET_CODE CommonLooper::start() {
    worker_ = new std::thread(trampoline, this);
    if (worker_ == NULL) {
        return RET_FAIL;
    }

    running_ = true;
    return RET_OK;
}

CommonLooper::~CommonLooper() {
    if (running_) {
        stop();
    }
}

void CommonLooper::stop() {
    request_exit_ = true;
    if (worker_) {
        worker_->join();
        delete worker_;
        worker_ = NULL;
    }

    running_ = false;
}
}