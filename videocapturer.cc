#include "videocapturer.hh"
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <QtGlobal>

namespace LQF {

static void show_dshow_device() {
    AVFormatContext* format_ctx = avformat_alloc_context();
    AVDictionary* options = nullptr;
    av_dict_set(&options, "list_devices", "true", 0);
    AVInputFormat *iformat = (AVInputFormat*)av_find_input_format("dshow");
    // 使用名叫dummy的虚拟视频设备
    avformat_open_input(&format_ctx, "video=dummy", iformat, &options);
}

static void show_dshow_device_option() {
    AVFormatContext* format_ctx = avformat_alloc_context();
    AVDictionary* options = nullptr;
    av_dict_set(&options, "list_options", "true", 0);
    AVInputFormat* iformat = (AVInputFormat*)av_find_input_format("dshow");
    


}