#include "aacrtmppackager.hh"

#include <librtmp/rtmp.h>
#include <string.h>

#include <cstdint>

#include "rtmppackager.hh"
namespace LQF {
void AACRTMPPackager::pack(RTMPPacket* packet, char* buf, const char* data,
                           int length) const {
    char* body = buf + RTMP_MAX_HEADER_SIZE;

    RTMPPacket_Reset(packet);
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nChannel = RTMP_AUDIO_CHANNEL;
    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = length - 5;
    packet->m_body = body;

    *(body++) = 0xAF;
    *(body++) = 0x01;
    memcpy(body, data + 7, length - 7);
}

void AACRTMPPackager::metadata(RTMPPacket* packet, char* buf, const char* data,
                               int length) const {
    char* body = buf + RTMP_MAX_HEADER_SIZE;
    RTMPPacket_Reset(packet);
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nChannel = RTMP_AUDIO_CHANNEL;
    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = length + 2;
    packet->m_body = body;

    *(body++) = 0xAF;
    *(body++) = 0x00;
    memcpy(body, data, length);
}

int AACRTMPPackager::get_audio_specific_config(uint8_t* data,
                                               const uint32_t profile,
                                               const uint32_t samplerate,
                                               const uint32_t channel_num) {
    uint16_t _profile = (uint16_t)profile + 1;
    _profile <<= 11;
    uint32_t _samplerate = 0;
    switch (samplerate) {
        case 96000:
            _samplerate = 0;
            break;
        case 88200:
            _samplerate = 1;
            break;
        case 64000:
            _samplerate = 2;
            break;
        case 48000:
            _samplerate = 3;
            break;
        case 44100:
            _samplerate = 4;
            break;
        case 32000:
            _samplerate = 5;
            break;
        case 24000:
            _samplerate = 6;
            break;
        case 22050:
            _samplerate = 7;
            break;
        case 16000:
            _samplerate = 8;
            break;
        case 12000:
            _samplerate = 9;
            break;
        case 11025:
            _samplerate = 10;
            break;
        case 8000:
            _samplerate = 11;
            break;
        case 7350:
            _samplerate = 12;
            break;
        default:
            _samplerate = 4;
            return -1;
            break;
    }

    _samplerate <<= 7;
    uint16_t _channel_num = (uint16_t)channel_num;
    _channel_num <<= 3;
    uint16_t audio_spec = _profile | _samplerate | _channel_num;
    data[0] = (uint8_t)(audio_spec >> 8);
    data[1] = 0xFF & audio_spec;

    return 0;
}
void AACRTMPPackager::packet_adts_header(uint8_t* packet, int packet_len,
                                         int profile, int freq_idx,
                                         int channel_config) {
    packet[0] = (uint8_t)0xFF;
    packet[1] = (uint8_t)0xF9;
    packet[2] = (uint8_t)(((profile - 1) << 6) + (freq_idx << 2) +
                          (channel_config >> 2));
    packet[3] = (uint8_t)(((channel_config & 3) << 6) + (packet_len >> 11));
    packet[4] = (uint8_t)((packet_len & 0x7FF) >> 3);
    packet[5] = (uint8_t)(((packet_len & 7) << 5) + 0x1F);
    packet[6] = (uint8_t)0xFC;
}
}  // namespace LQF