#include "looper.hh"
#include "log/easylogging++.h"
#include "mediabase.hh"
#include "timeutil.hh"
namespace LQF {

void Looper::Post(int what, void *data, bool flush) {
  LooperMessage *msg = new LooperMessage();
  msg->what = what;
  msg->obj = (MsgBaseObj *)data;
  msg->quit = false;
  addmsg(msg, flush);
}

void Looper::addmsg(LooperMessage *msg, bool flush) { 
    int64_t t1 = TimesUtil::get_time_millisecond();
    queue_mutex_.lock();
    if (flush || msg_queue_.size() > deque_max_size_) {
        while (msg_queue_.size() > 0) {
            LooperMessage *tmp_msg = msg_queue_.front();
            msg_queue_.pop_front();
            delete tmp_msg->obj;
            delete tmp_msg;
        }
    }
    msg_queue_.push_back(msg);
    queue_mutex_.unlock();
    if (msg->what == 1) {
        LOG(INFO) << "post msg what: " << msg->what << ", size: " << msg_queue_.size()
            << ", t:"
    }
    head_data_available_->post();
    int64_t t2 = TimesUtil::get_time_millisecond();
    if (t2 - t1 > 10) {
    }
}
} // namespace LQF
