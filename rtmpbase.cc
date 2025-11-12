#include "rtmpbase.hh"
#include <stdio.h>

#define DEF_TIMEOUT 30 // seconds
#define DEF_BUFTIME (10 * 60 * 60 * 1000)

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

namespace LQF
{
bool RTMPBase::initRtmp() {
    bool ret_code = true;
    rtmp_ = RTMP_Alloc();
    RTMP_Init(rtmp_);
    return ret_code;
}

void RTMPBase::SetConnectUrl(std::string& url) {
    url_ = url;
}

RTMPBase::RTMPBase()
    : rtmp_obj_type_(RTMP_BASE_TYPE_UNKNOW),
    enable_audio_(true),
    enable_video_(true) {
    initRtmp();
}

RTMPBase::RTMPBase(RTMP_BASE_TYPE& rtmp_obj_type)
    : rtmp_obj_type_(rtmp_obj_type),
    enable_audio_(true),
    enable_video_(true) {
    initRtmp();
}

RTMPBase::RTMPBase(RTMP_BASE_TYPE& rtmp_obj_type, std::string& url)
    : rtmp_obj_type_(rtmp_obj_type),
    enable_audio_(true),
    enable_video_(true),
    url_(url) {
    initRtmp();
}

RTMPBase::RTMPBase(std::string& url, bool is_recv_audio, bool is_recv_video)
    : enable_audio_(is_recv_audio),
    enable_video_(is_recv_video),
    url_(url),
    rtmp_obj_type_(RTMP_BASE_TYPE_PLAY) {
    initRtmp();
}

RTMPBase::~RTMPBase() {
    if (IsConnect()) {
        Disconnect();
    }
    RTMP_Free(rtmp_);
    rtmp_ = nullptr;
#ifdef WIN32
    WSACleanup();
#endif
}

bool RTMPBase::SetReceiveAudio(bool is_recv_audio) {
    if (is_recv_audio == enable_audio_)
        return true;

    if (IsConnect()) {
        if (RTMP_SendReceiveAudio(rtmp_, is_recv_audio)) {
            enable_audio_ = is_recv_audio;
            return true;
        }
    }
    else {
        enable_audio_ = is_recv_audio;
    }
}
bool RTMPBase::SetReceiveVideo(bool is_recv_video) {
    if (is_recv_video == enable_video_) {
        return true;
    }

    if (IsConnect()) {
        if (RTMP_SendReceiveVideo(rtmp_, is_recv_video)) {
            enable_video_ = is_recv_video;
            return true;
        }
    }
    else {
        enable_video_ = is_recv_video;
    }
}

void RTMPBase::Disconnect() {
    RTMP_Close(rtmp_);
}

bool RTMPBase::Connect(std::string& url) {
    url_ = url;
    Connect();
}

bool RTMPBase::Connect() {
    RTMP_Free(rtmp_);
    rtmp_ = RTMP_Alloc();
    RTMP_Init(rtmp_);

    rtmp_->Link.timeout = 10;
    if (RTMP_SetupURL(rtmp_, (char*)url_.c_str())) {
        return false;
    }
    rtmp_->Link.lFlags |= RTMP_LF_LIVE;
    RTMP_SetBufferMS(rtmp_, 3600 * 1000);
    if (rtmp_obj_type_ == RTMP_BASE_TYPE_PUSH) {
        RTMP_EnableWrite(rtmp_);
    }
    if (!RTMP_Connect(rtmp_, NULL)) {
        return false;
    }

    if (!RTMP_ConnectStream(rtmp_, 0)) {
        return false;
    }

    // 判断是否打开音视频，默认打开
    if (rtmp_obj_type_ == RTMP_BASE_TYPE_PUSH) {
        if (!enable_video_) {
            RTMP_SendReceiveVideo(rtmp_, enable_video_);
        }
        if (!enable_audio_) {
            RTMP_SendReceiveVideo(rtmp_, enable_audio_);
        }
    }

    return true;
}

uint32_t RTMPBase::GetSampleRateByFreqIdx(uint8_t& freq_idx) {
    uint32_t freq_idx_table[] = { 96000, 88200, 64000, 48000, 44100, 32000,
                                  24000, 22050, 16000, 12000, 11025, 8000, 7350
    };

    if (freq_idx < 13 && freq_idx >= 0) {
        return freq_idx_table[freq_idx];
    }

    return 44100;
}
}