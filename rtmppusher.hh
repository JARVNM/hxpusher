#pragma once
#include <cstdint>

#include "librtmp/rtmp.h"
#include "mediabase.hh"
#include "naluloop.hh"
#include "rtmpbase.hh"

namespace LQF {
enum RTMPPusherMES {
    RTMPPUSHER_MES_H264_DATA = 1,
    RTMPPUSHER_MES_AAC_DATA = 2
};

typedef struct _RTMPMetadata {
    unsigned int width;
    unsigned int height;
    unsigned int frame_rate;
    unsigned int video_data_rate;
    unsigned int sps_len;
    unsigned char sps[1024];
    unsigned int pps_len;
    unsigned char pps[1024];

    bool bHasAudio;
    unsigned int audio_sample_rate;
    unsigned int audio_sample_size;
    unsigned int audio_channels;
    char audio_spec_cfg;
    unsigned int audio_spec_cfg_len;
} RTMPMetadata, *LPRTMPMetadata;

class RTMPPusher : public NaluLoop, public RTMPBase {
    using Super = RTMPBase;

public:
    RTMPPusher();

    bool send_meta_data(FLVMetadataMsg* metadata);

    bool send_audio_specificConfig(char* data, int length);

private:
    virtual void handle(int what, void* data);
    bool send_h264_sequence_header(VideoSequenceHeaderMsg* seq_header);
    bool send_h264_packet(char* data, int size, bool is_keyframe,
                          unsigned int timestamp);
    int send_packet(unsigned int packet_type, unsigned char* data,
                    unsigned int size, unsigned int timestamp);

    int64_t time_ = 0;

    enum {
        FLV_CODECID_H264 = 7,
        FLV_CODECID_AAC = 10,
    };

    bool is_first_metadata_ = false;
    bool is_first_video_sequence_ = false;
    bool is_first_video_frame_ = false;
    bool is_first_audio_sequence_ = false;
    bool is_first_audio_frame_ = false;
    uint32_t video_pre_timestamp = 0;
    uint32_t audio_pre_timestamp = 0;
};

}  // namespace LQF