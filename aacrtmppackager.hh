#pragma once

#include <librtmp/rtmp.h>

#include <cstdint>

#include "rtmppackager.hh"

namespace LQF {

typedef struct AudioSpecificConfig {
    uint8_t type : 5;
    uint8_t samplerate : 4;
    uint8_t channel_num : 4;
    uint8_t tail : 3;
} AudioSpecificConfig, *AudioSpecificConfigPtr;

class AACRTMPPackager : public RTMPPacketer {
public:
    static const int AAC_MAIN = 1;
    static const int AAC_LC = 2;
    static const int AAC_SSR = 3;
    virtual void pack(RTMPPacket* packet, char* buf, const char* data,
                      int length) const override;
    virtual void metadata(RTMPPacket* packet, char* buf, const char* data,
                          int length) const override;

    static int get_audio_specific_config(uint8_t* data, const uint32_t profile,
                                         const uint32_t samplerate,
                                         const uint32_t channel_num);
    static void packet_adts_header(uint8_t* packet, int packet_len, int profile,
                                   int freq_idx, int channel_config);
};
}  // namespace LQF