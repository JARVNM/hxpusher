#include "h264encoder.hh"
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavcodec/codec_id.h>
#include <libavcodec/packet.h>
#include <libavutil/avutil.h>
#include <libavutil/dict.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include "log/easylogging++.h"
#include "mediabase.hh"

namespace LQF {

H264Encoder::H264Encoder() {}

int H264Encoder::init(const Properties& properties, AVRational tb) {
    width_ = properties.GetProperty("width", 0);
    if (width_ == 0 || width_ % 2 != 0) {
        LOG(ERROR) << "width:" << width_;
        return RET_ERR_NOT_SUPPORT;
    }

    height_ =  properties.GetProperty("height", 0);
    if (height_ == 0 || height_ % 2 != 0) {
        LOG(ERROR) << "height:" << height_;
        return RET_ERR_NOT_SUPPORT;
    }

    fps_ = properties.GetProperty("fps", 25);
    b_frames_ = properties.GetProperty("b_frames", 0);
    bitrate_ = properties.GetProperty("bitrate", 500 * 1024);
    gop_ = properties.GetProperty("gop", fps_);

    codec_ = (AVCodec*)avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec_) {
        LOG(ERROR) << "Can not find encoder ";
        return RET_FAIL;
    }

    count = 0;
    framecnt = 0;
    ctx_ = avcodec_alloc_context3(codec_);
    ctx_->qmin = 10;
    ctx_->qmax = 31;

    ctx_->width = width_;
    ctx_->height = height_;
    ctx_->bit_rate = bitrate_;
    ctx_->gop_size = gop_;
    ctx_->time_base = tb;
    ctx_->framerate.num = fps_;
    ctx_->framerate.den = 1;

    ctx_->pix_fmt = AV_PIX_FMT_YUV420P;
    ctx_->codec_type = AVMEDIA_TYPE_VIDEO;
    ctx_->max_b_frames = b_frames_;
    if (ctx_->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
    }
    if (ctx_->codec_id == AV_CODEC_ID_H265) {
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }

    ctx_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (avcodec_open2(ctx_, codec_, &param) < 0) {
        return RET_FAIL;
    }

    if (ctx_->extradata) {
        LOG(WARNING) << "extradata_size: " << ctx_->extradata_size;
        uint8_t* sps = ctx_->extradata + 4;
        int sps_len = 0;
        uint8_t* pps = NULL;
        int pps_len = 0;
        uint8_t* data = ctx_->extradata + 4;
        for (int i = 0; i < ctx_->extradata_size - 4; ++i) {
            if (0 == data[i] && 0 == data[i + 1] && 0 == data[i + 2] && 1 == data[i + 3]) {
                pps = &data[i + 4];
                break;
            }
        }

        sps_len = int(pps - sps) - 4;
        pps_len = ctx_->extradata_size - 4 * 2 - sps_len;
        sps_.append(sps, sps + sps_len);
        pps_.append(pps, pps + pps_len);
    }

    return 0;
}

H264Encoder::~H264Encoder() {
    if (ctx_) {
        avcodec_close(ctx_);
    }
}

RET_CODE H264Encoder::input(const AVFrame* frame) {
    int ret = avcodec_send_frame(ctx_, frame);
    if (ret != 0) {
        if (AVERROR(EAGAIN) == ret) {
            return RET_ERR_EAGAIN;
        }
        else if (AVERROR_EOF == ret) {
            return RET_OK;
        }

        return RET_FAIL;
    }

    return RET_OK;
}

RET_CODE H264Encoder::output(AVPacket *pkt) {
    int ret = avcodec_receive_packet(ctx_, pkt);
    if (ret != 0) {
        if (AVERROR(EAGAIN) == ret) {
            return RET_ERR_EAGAIN;
        }
        else if (AVERROR_EOF == ret) {
            return RET_ERR_EOF;
        }

        return RET_FAIL;
    }

    return RET_OK;
}

int H264Encoder::get_sps(uint8_t* sps, int& sps_len) {
    if (!sps || sps_len < sps_.size()) {
        return -1;
    }

    sps_len = sps_.size();
    memcpy(sps, sps_.c_str(), sps_len);
    return 0;
}

int H264Encoder::get_pps(uint8_t* pps, int& pps_len) {
    if (!pps ||  pps_len < pps_.size()) {
        return -1;
    }

    pps_len = pps_.size();
    memcpy(pps, pps_.c_str(), pps_len);
    return 0;
}
}