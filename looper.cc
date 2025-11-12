#include "looper.hh"
#include "mediabase.hh"

namespace LQF {

void Looper::Post(int what, void *data, bool flush) {
  LooperMessage *msg = new LooperMessage();
  msg->what = what;
  msg->obj = (MsgBaseObj *)data;
  msg->quit = false;
  addmsg(msg, flush);
}

void Looper::addmsg(LooperMessage *msg, bool flush) { int64_t t }
} // namespace LQF
