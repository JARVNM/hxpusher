#include "looper.hh"

namespace LQF {

class NaluLoop : public Looper {
public:
    NaluLoop(int queue_nalu_len);

private:
    virtual void addmsg(LooperMessage* msg, bool flush);

private:
    int max_nalu_;
};
}  // namespace LQF