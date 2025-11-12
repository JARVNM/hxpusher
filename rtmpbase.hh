#pragma once
#include <iostream>
#include "librtmp/rtmp.h"

namespace LQF
{

enum RTMP_BASE_TYPE {
    RTMP_BASE_TYPE_UNKNOW,
    RTMP_BASE_TYPE_PLAY,
    RTMP_BASE_TYPE_PUSH
};

class RTMPBase {
public:
    virtual bool Connect(std::string& url);
    bool Connect();

    void SetConnectUrl(std::string& url);
    void Disconnect();
    bool IsConnect();

    bool SetReceiveAudio(bool is_recv_audio);
    bool SetReceiveVideo(bool is_recv_video);
    // 从采样率表中获取采样率
    static uint32_t GetSampleRateByFreqIdx(uint8_t& freq_idx);

    RTMPBase();
    RTMPBase(RTMP_BASE_TYPE& rtmp_obj_type);
    RTMPBase(RTMP_BASE_TYPE& rtmp_obj_type, std::string& url);
    RTMPBase(std::string& url, bool is_recv_audio, bool is_recv_video);
    virtual ~RTMPBase();

private:
    bool initRtmp();
    RTMP_BASE_TYPE rtmp_obj_type_;

protected:
    RTMP* rtmp_;
    std::string url_;
    bool enable_video_;
    bool enable_audio_;
};
}