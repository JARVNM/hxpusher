#pragma once
#include "log/easylogging++.h"
#include <bits/types/struct_timeval.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <mutex>
#include <stdint.h>
#include <sys/time.h>

class AVPublishTime {
public:
  typedef enum PTS_STRATEGY { PTS_RECTIFY = 0, PTS_REAL_TIME } PTS_STRATEGY;

public:
  static AVPublishTime *get_instance() {
    if (s_publish_time == NULL) {
      s_publish_time = new AVPublishTime();
    }

    return s_publish_time;
  }

  AVPublishTime() { start_time_ = get_current_time_msec(); }

  void reset() { start_time_ = get_current_time_msec(); }

  void set_pause(bool pause) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pause_ == pause) {
      return;
    }
    pause_ = pause;
    if (!pause_) {
      offset_time_ = (int64_t)audio_pre_pts_;
      start_time_ = get_current_time_msec();
    }
  }

  int64_t get_time() { return (int64_t)audio_pre_pts_; }

  void set_offset_time(int64_t offset_time) {
    std::lock_guard<std::mutex> lock(mutex_);
    offset_time_ = offset_time;
  }

  void set_audio_frame_duration(const double frame_duration) {
    audio_frame_duration_ = frame_duration;
    audio_frame_threshold_ = (int64_t)(frame_duration * 2);
  }

  int64_t get_audio_pts() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pause_) {
      return -1;
    }
    int64_t pts = get_current_time_msec() - start_time_ + offset_time_;
    if (PTS_RECTIFY == audio_pts_strategy_) {
      int64_t diff =
          (int64_t)abs(pts - (int64_t)(audio_pre_pts_ + audio_frame_duration_));
      if (diff < audio_frame_threshold_) {
        audio_pre_pts_ += audio_frame_duration_;
        return (int64_t)audio_pre_pts_;
      }

      audio_pre_pts_ = (double)pts;
      return pts;
    } else {
      audio_pre_pts_ = (double)pts;
      LOG(INFO) << "get_audio_pts REAL_TIME" << audio_pre_pts_;
      return pts;
    }
  }

private:
  int64_t get_current_time_msec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((unsigned long long)tv.tv_sec * 1000 + (long)tv.tv_usec / 1000);
  }

  std::mutex mutex_;
  bool pause_ = false;
  int64_t start_time_ = 0;
  int64_t offset_time_ = 0;

  PTS_STRATEGY audio_pts_strategy_ = PTS_RECTIFY;
  double audio_frame_duration_ = 21.3333;
  int64_t audio_frame_threshold_ = (int64_t)(audio_frame_duration_ * 2);
  double audio_pre_pts_ = 0;

  PTS_STRATEGY video_pts_strategy_ = PTS_REAL_TIME;
  double video_frame_duration_ = 40;
  int64_t video_frame_threshold_ = (int64_t)(video_frame_duration_ * 2);
  double video_pre_pts_ = 0;

  static AVPublishTime *s_publish_time;
}
