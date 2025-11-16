#pragma once
#include "librtmp/rtmp.h"

namespace LQF {
enum RTMPChannel {
    RTMP_NETWORK_CHANNEL = 2,
    RTMP_SYSTEM_CHANNEL,
    RTMP_AUDIO_CHANNEL,
    RTMP_VIDEO_CHANNEL = 6,
    RTMP_SOURCE_CHANNEL = 8,
};
class RTMPPacketer {
public:
    virtual void pack(RTMPPacket* packet, char* buf, const char* data,
                      int length) const = 0;
    virtual void metadata(RTMPPacket* packet, char* buf, const char* data,
                          int length) const = 0;
};
}  // namespace LQF