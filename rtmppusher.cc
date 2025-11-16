#include "rtmppusher.hh"

#include <librtmp/amf.h>
#include <librtmp/rtmp.h>

#include <cstdint>
#include <cstring>

#include "aacrtmppackager.hh"
#include "avpublishtime.hh"
#include "log/easylogging++.h"
#include "mediabase.hh"
#include "timeutil.hh"

namespace LQF {
char* put_byte(char* output, uint8_t nval) {
    output[0] = nval;
    return output + 1;
}

char* put_be16(char* output, uint16_t nval) {
    output[1] = nval & 0xFF;
    output[0] = nval >> 8;
    return output + 2;
}

char* put_be24(char* output, uint32_t nval) {
    output[2] = nval & 0xFF;
    output[1] = nval >> 8;
    output[0] = nval >> 16;
    return output + 3;
}

char* put_be32(char* output, uint32_t nval) {
    output[3] = nval & 0xFF;
    output[2] = nval >> 8;
    output[1] = nval >> 16;
    output[0] = nval >> 24;
    return output + 4;
}

char* put_be64(char* output, uint64_t nval) {
    output = put_be32(output, nval >> 32);
    output = put_be32(output, nval);
    return output;
}

char* put_amf_string(char* c, const char* str) {
    uint16_t len = strlen(str);
    c = put_be16(c, len);
    memcpy(c, str, len);
    return c + len;
}

char* put_amf_double(char* c, double d) {
    *c++ = AMF_NUMBER;
    unsigned char *ci, *co;
    ci = (unsigned char*)&d;
    co = (unsigned char*)c;
    co[0] = ci[7];
    co[1] = ci[6];
    co[2] = ci[5];
    co[3] = ci[4];
    co[4] = ci[3];
    co[5] = ci[2];
    co[6] = ci[1];
    co[7] = ci[0];

    return c + 8;
}

void RTMPPusher::handle(int what, void* data) {
    if (!IsConnect()) {
        LOG(WARNING) << "begin reconnect...";
        if (!Connect()) {
            LOG(ERROR) << "reconnect failed!";
            delete data;
            return;
        }
    }

    switch (what) {
        case RTMP_BODY_METADATA:
            if (!is_first_metadata_) {
                is_first_metadata_ = true;
                LOG(INFO) << AVPublishTime::get_instance()->get_meta_data_tag()
                          << ":t"
                          << AVPublishTime::get_instance()->get_current_time();
            }
            FLVMetadataMsg* metadata = (FLVMetadataMsg*)data;
            if (!send_meta_data(metadata)) {
                LOG(ERROR) << "send_meta_data failed";
            }
            delete metadata;
            break;
        case RTMP_BODY_VID_CONFIG:
            if (!is_first_audio_sequence_) {
                is_first_video_sequence_ = true;
                LOG(INFO) << AVPublishTime::get_instance()->get_avc_header_tag()
                          << ":t"
                          << AVPublishTime::get_instance()->get_current_time();
            }
            VideoSequenceHeaderMsg* vid_cfg_msg = (VideoSequenceHeaderMsg*)data;
            if (!send_h264_sequence_header(vid_cfg_msg)) {
                LOG(ERROR) << "send_h264_sequence_header failed";
            }
            delete vid_cfg_msg;
            break;
        case RTMP_BODY_VID_RAW:
            if (!is_first_video_frame_) {
                is_first_video_frame_ = true;
                LOG(INFO) << AVPublishTime::get_instance()->get_avc_frame_tag()
                          << ":t"
                          << AVPublishTime::get_instance()->get_current_time();
            }
            NaluStruct* nalu = (NaluStruct*)data;
            if (send_h264_packet((char*)nalu->data, nalu->size,
                                 (nalu->type == 0x05) ? true : false,
                                 nalu->pts)) {
            } else {
                LOG(WARNING) << "at heandle send h264 pack failed";
            }
            delete nalu;
            break;
        case RTMP_BODY_AUD_SPEC:
            if (!is_first_audio_sequence_) {
                is_first_audio_sequence_ = true;
            }
            AudioSpecMsg* audio_spec = (AudioSpecMsg*)data;
            uint8_t aac_spec_[4];
            aac_spec_[0] = 0xAF;
            aac_spec_[1] = 0x0;
            AACRTMPPackager::get_audio_specific_config(
                &aac_spec_[2], audio_spec->profile_, audio_spec->sample_rate_,
                audio_spec->channels_);
            send_audio_specificConfig((char*)aac_spec_, 4);
            break;
        case RTMP_BODY_AUD_RAW:
            if (!is_first_audio_frame_) {
                is_first_audio_frame_ = true;
            }
            AudioRawMsg* audio_raw = (AudioRawMsg*)data;
            if (send_packet(RTMP_PACKET_TYPE_AUDIO,
                            (unsigned char*)audio_raw->data, audio_raw->size,
                            audio_raw->pts)) {
            } else {
                LOG(INFO) << "at handle send audio pack failed";
            }
            delete audio_raw;
            break;
        default:
            break;
    }
}

bool RTMPPusher::send_meta_data(FLVMetadataMsg* metadata) {
    if (metadata == NULL) {
        return false;
    }

    char body[1024] = {0};
    char* p = (char*)body;
    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, "@setDataFrame");
    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, "onMetaData");
    p = put_byte(p, AMF_OBJECT);
    p = put_amf_string(p, "copyright");
    p = put_byte(p, AMF_STRING);
    p = put_amf_string(p, "firehood");

    if (metadata->has_video) {
        p = put_amf_string(p, "width");
        p = put_amf_double(p, metadata->width);
        p = put_amf_string(p, "height");
        p = put_amf_double(p, metadata->height);
        p = put_amf_string(p, "framerate");
        p = put_amf_double(p, metadata->framerate);
        p = put_amf_string(p, "videodatarate");
        p = put_amf_double(p, metadata->videodatarate);
        p = put_amf_string(p, "videocodecid");
        p = put_amf_double(p, FLV_CODECID_H264);
    }
    if (metadata->has_audio) {
        p = put_amf_string(p, "audiodatarate");
        p = put_amf_double(p, (double)metadata->audiodatarate);
        p = put_amf_string(p, "audiosamplerate");
        p = put_amf_double(p, (double)metadata->audiosamplerate);
        p = put_amf_string(p, "audiosamplesize");
        p = put_amf_double(p, (double)metadata->audiosamplesize);
        p = put_amf_string(p, "stereo");
        p = put_amf_double(p, (double)metadata->channles);
        p = put_amf_string(p, "audiocodecid");
        p = put_amf_double(p, (double)FLV_CODECID_AAC);
    }
    p = put_amf_string(p, "");
    p = put_byte(p, AMF_OBJECT_END);
    return send_packet(RTMP_PACKET_TYPE_INFO, (unsigned char*)body, p - body,
                       0);
}

bool RTMPPusher::send_h264_sequence_header(VideoSequenceHeaderMsg* seq_header) {
    if (seq_header == NULL) {
        return false;
    }

    uint8_t body[1024] = {0};
    int i = 0;
    body[i++] = 0x17;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x01;
    body[i++] = seq_header->sps_[1];
    body[i++] = seq_header->sps_[2];
    body[i++] = seq_header->sps_[3];
    body[i++] = 0xFF;

    body[i++] = 0xE1;
    body[i++] = (seq_header->sps_size_ >> 8) & 0xFF;
    body[i++] = seq_header->sps_size_ & 0xFF;
    memcpy(&body[i], seq_header->sps_, seq_header->sps_size_);
    i = i + seq_header->sps_size_;
    body[i++] = 0x01;
    body[i++] = (seq_header->pps_size_ >> 8) & 0xFF;
    body[i++] = seq_header->pps_size_ & 0xFF;

    memcpy(&body[i], seq_header->pps_, seq_header->pps_size_);
    i = i + seq_header->pps_size_;
}
}  // namespace LQF