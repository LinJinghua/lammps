#ifndef _SLEEP_H
#define _SLEEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Windows.h>
// #include <WinBase.h>
// #include <winnt.h>
// #include <synchapi.h>
int usleep(__int64 usec) {
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);

    return 0;
}

unsigned int sleep(unsigned int seconds) {
    Sleep(seconds * 1000);
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* _SLEEP_H */
