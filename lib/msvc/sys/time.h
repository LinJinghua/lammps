#ifndef _TIME_H
#define _TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <winsock.h>
inline int gettimeofday(struct timeval * tv, struct timezone * tz) {
    if (NULL == tv)
        return -1;
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tv->tv_sec = (long)((time - EPOCH) / 10000000L);
    tv->tv_usec = (long)(system_time.wMilliseconds * 1000);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H */
