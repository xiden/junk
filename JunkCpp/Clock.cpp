#include "Clock.h"
#if defined __GNUC__

#include <time.h>

#else

#include <Windows.h>

static _FINLINE int64_t GetQpf() {
	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);
	return li.QuadPart;
}

static int64_t Qpf = GetQpf();

#endif

_JUNK_BEGIN



//! システム時間の取得(nsec)
int64_t Clock::SysNS() {
#if defined __GNUC__
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
#else
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	return INT64_C(1000000000) * li.QuadPart / Qpf;
#endif
}

//! システム時間の取得(sec)
double Clock::SysS() {
#if defined __GNUC__
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)((int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec) / 1000000000.0;
#else
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	return (double)li.QuadPart / Qpf;
#endif
}

_JUNK_END
