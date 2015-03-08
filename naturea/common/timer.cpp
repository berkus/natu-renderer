// ===================================================================
// $Id$
//
// timer.cpp
//     Timing for running process
//
// REPLACEMENT_STRING
//
// Initial coding by Vlastimil Havran, 1998.
// Windows time measuring added by Jaroslav Krivanek, October 2003.

// GOLEM headers
#include "timer.h"

// ------------------------------------------------------
// Use ftime instead of time, see > man ftime
#define _USE_FTIME_CALL

// standard headers
#include <ctime>
#ifdef __UNIX__

#include <sys/time.h>
#include <sys/resource.h>

#ifdef _USE_FTIME_CALL
#include  <sys/timeb.h>
#endif // _USE_FTIME_CALL

#endif // __UNIX__
#ifdef _MSC_VER
#include <windows.h>
#include <sys/types.h> 
#include <sys/timeb.h>
#endif // _MSC_VER


// Staic variable
bool
CTimer::initTimingCalled = false;

// The following are not static's in the CTimer class to prevent
// #including <windows.h> into the "timer.h" header.
#ifdef  _MSC_VER
/// true if the performance timer works under windows
static BOOL hasHRTimer;
/// frequency of the performance timer under windows
static LARGE_INTEGER hrFreq;
#endif // MSC_VER

void 
CTimer::_initTiming()
{
#ifdef  _MSC_VER
  hasHRTimer = QueryPerformanceFrequency(&hrFreq);
#endif
  initTimingCalled = true;
}

void
CTimer::Reset()
{
  // Real times in seconds
#ifdef _USE_FTIME_CALL
  timeb beg;
  ftime(&beg);
  lastRealTime = (double)(beg.time + 0.001 * beg.millitm);  
#else //  _USE_FTIME_CALL
  time_t beg;
  time(&beg);
  lastRealTime = (double)beg;
#endif // _USE_FTIME_CALL
  
  lastUserTime   = 0.0;
  lastSystemTime = 0.0;

  realTime   = 0.0;
  userTime   = 0.0;
  systemTime = 0.0;

  countStop = 0;
  running = false;

#ifdef __UNIX__
  // Timing in OS UNIX
  begrusage.ru_utime.tv_usec = begrusage.ru_utime.tv_sec =
  begrusage.ru_stime.tv_usec = begrusage.ru_stime.tv_sec = 0L;
  endrusage = begrusage;
#endif // __UNIX__
}

void
CTimer::_start() const
{
  if (running)
    return; // timer is already running

  // Measure the real time
#ifdef _USE_FTIME_CALL
  timeb beg;
  ftime(&beg);
  lastRealTime = (double)(beg.time + 0.001 * beg.millitm);  
#else //  _USE_FTIME_CALL
  time_t beg;
  time(&beg);
  lastRealTime = (double)beg;
#endif //  _USE_FTIME_CALL

  // Measure the real and system time
#ifdef __UNIX__
  // Measure under UNIX
  struct rusage begrusage;
  getrusage(RUSAGE_SELF, &begrusage);

  lastUserTime = (double)begrusage.ru_utime.tv_sec +
	             1e-6 * begrusage.ru_utime.tv_usec;

  lastSystemTime = (double)begrusage.ru_stime.tv_sec +
		           1e-6 * begrusage.ru_stime.tv_usec;
#endif // __UNIX__

#ifdef  _MSC_VER
  // Mesure under Windows
  if (hasHRTimer) {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    lastUserTime = (double)counter.QuadPart / (double)hrFreq.QuadPart;
    lastSystemTime = 0;	  
  } 
  else {
    static struct _timeb mtime;
    _ftime(&mtime);
    lastUserTime   = (double)mtime.time + 1e-3 * mtime.millitm;
    lastSystemTime = 0;
  }
#endif
  // Begin trace.
  running = true;
}

void
CTimer::_stop() const
{
  if (!running)
    return; // timer is not running
  
  // Begin trace.
#ifdef _USE_FTIME_CALL
  timeb end;
  ftime(&end);
  realTime += (double)(end.time + 0.001 * end.millitm) - lastRealTime;  
#else //  _USE_FTIME_CALL
  time_t end;
  time(&end);
  realTime += (double)end - lastRealTime;
#endif //  _USE_FTIME_CALL

#ifdef __UNIX__
  // timing in unix OS
  getrusage(RUSAGE_SELF, &endrusage);

  userTime += (double)endrusage.ru_utime.tv_sec +
	  1e-6 * endrusage.ru_utime.tv_usec -
	  lastUserTime;

  systemTime += (double)endrusage.ru_stime.tv_sec +
	  1e-6 * endrusage.ru_stime.tv_usec -
	  lastSystemTime;
#endif // __UNIX__

#ifdef  _MSC_VER
  // Mesure under Windows
  if (hasHRTimer) {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    userTime += (double)counter.QuadPart / (double)hrFreq.QuadPart - 
      lastUserTime;
    systemTime = 0;
   } 
  else {
    static struct _timeb mtime;
    _ftime(&mtime);
    userTime   += (double)mtime.time + 1e-3 * mtime.millitm -
      lastUserTime;
    systemTime = 0;
  }
#endif

  running = false;
  countStop++;
}

// returns the real time measured by timer in seconds
double
CTimer::RealTime() const
{
  if (running) {
    _stop();
    _start();
  }
  return realTime;
}

// returns the user time measured by timer in seconds
double
CTimer::UserTime() const
{
  if (running) {
    _stop();
    _start();
  }
  return userTime;
}

// returns the user+system time measured by timer in seconds
double
CTimer::SystemTime() const
{
  if (running) {
    _stop();
    _start();
  }
  return systemTime;
}

