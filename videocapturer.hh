#pragma once

#include <libavformat/avio.h>
#include <libavutil/frame.h>
#include <functional>
#include "mediabase.hh"
#include "commonlooper.hh"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#ifdef __cplusplus
#endif

namespace LQF {

class VideoCapturer : public CommonLooper {
public:
    VideoCapturer();
    virtual ~VideoCapturer();

    RET_CODE init(const Properties& properties);
    virtual void loop();
    void add_callback(std::function<void(AVFrame*)> callback) {
        
    }

private:

    int video_test_ = 0;
    int x_ = 0;
    int y_ = 0;
    int video_enc_width_ = 0;
    int video_enc_height_ = 0;
    int video_enc_pix_fmt = 0;
    int capture_fps_;

    std::function<void(AVFrame*)> callback_get_frame_ = NULL;
    AVFormatContext* ifmt_ctx_ = nullptr;
    int video_stream_ = -1;
    AVCodecContext *codec_ctx_ = nullptr;

    AVFrame* video_frame_ = nullptr;
    AVFrame* yuv_frame_ = nullptr;

    std::string device_name_;
    bool is_first_frame_ = false;
};
}