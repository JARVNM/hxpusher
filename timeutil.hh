#pragma once

#include <bits/types/struct_timeval.h>
#include <chrono>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
using namespace std::chrono;

namespace LQF {
class TimesUtil {
public:
    static inline int64_t get_time_millisecond() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return ((int64_t)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
    }
}
}
