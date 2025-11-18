#pragma once

#include <libavcodec/packet.h>
#include <libavutil/dict.h>
#include <libavutil/rational.h>
#include <cstdint>
#include <string>
#include "mediabase.hh"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#ifdef __cplusplus
};
#endif

namespace LQF {
using std::string;
class H264Encoder {
public:
    H264Encoder();

    virtual int init(const Properties & properties, AVRational tb);
    virtual ~H264Encoder();
    virtual RET_CODE input(const AVFrame* frame);
    virtual RET_CODE output(AVPacket *pkt);
    int get_sps(uint8_t *sps, int &sps_len);
    int get_pps(uint8_t *pps, int &pps_len);
    inline int get_width() {
        return ctx_->width;
    }

    inline int get_height() {
        return ctx_->height;
    }

    virtual AVRational get_time_base() {
        return ctx_->time_base;
    }

    double get_framerate() {
        return ctx_->framerate.num / ctx_->framerate.den;
    }

    inline int64_t get_bit_rate() {
        return ctx_->bit_rate;
    }

    inline uint8_t *get_sps_data() {
        return (uint8_t*)sps_.c_str();
    }

    inline int get_sps_size() {
        return sps_.size();
    }

    inline uint8_t* get_pps_data() {
        return (uint8_t*)pps_.c_str();
    }

    inline int get_pps_size() {
        return pps_.size();
    }

    inline int get_pix_fmt() {
        return ctx_->pix_fmt;
    }

    AVCodecContext* get_codec_context() {
        return ctx_;
    }

private:
    int count;
    int framecnt;

    std::string codec_name_;
    int width_;
    int height_;
    int fps_;
    int b_frames_;
    int bitrate_;
    int gop_;
    bool annexb_;
    int threads_;
    std::string profile_;
    std::string level_id_;

    std::string sps_;
    std::string pps_;

    AVCodec* codec_ = NULL;
    AVDictionary *param = NULL;
    AVCodecContext* ctx_;

    int64_t pts_ = 0;
};
}