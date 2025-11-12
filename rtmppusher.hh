#pragma once
#include "librtmp/rtmp.h"
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


}