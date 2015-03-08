// ================================================================
// $Id$
//
// timer.h
//     CTimer class to measure the elapsed time
//
// Class: CTimer
//
// Initial coding by Vlastimil Havran, 1998.

#ifndef __TIMER_H__
#define __TIMER_H__

// standard headers
#ifdef __UNIX__
#include <sys/resource.h>
#endif
#include <ctime>

/// Class CTimer is designed to measure the user/(user+system)/real time
/// taken by the processor within a portion of source code.
class CTimer
{
private: // data
#ifdef __UNIX__
  // timing in OS UNIX
  mutable struct rusage begrusage;
  mutable struct rusage endrusage;
#endif // __UNIX__

  /// true if InitTiming() has already been called
  static bool initTimingCalled;

  /// accumulated real time (returned by RealTime() method)
  mutable double realTime;
  /// accumulated userTime
  mutable double userTime;
  /// accumulated user+system time
  mutable double systemTime;
  /// real time at the last start event
  mutable double lastRealTime;
  /// user time at the last start event
  mutable double lastUserTime;
  /// system time at the last start event
  mutable double lastSystemTime;

  /// the number of stopping this timer
  mutable int  countStop; 
  /// if the timer is running at this time
  mutable bool running;   

private: // methods

  /// interior implementation of Stop()
  void _start() const;
  // interior implementation of Start()
  void _stop() const;

  /// finds out whether performance timer is present (WIN32)
  void _initTiming();

public: // methods

  /// default constructor 
  CTimer() { Reset();  if( ! initTimingCalled ) _initTiming();  }
  /// default destructor
  ~CTimer() {}
  /// reset the time to zero all time values
  void Reset();
  /// restart the timer from zero
  void Restart() { Reset(); Start();}
  /// start/restart the timer to measure the following events
  void Start() { _start(); }
  /// stop the timer not to measure the following events
  void Stop()  { _stop(); }
  /// returns true if timer is running, otherwise false
  bool IsRunning() const { return running;}
  /// returns the real time measured by timer in seconds
  double RealTime() const;
  /// returns the user time measured by timer in seconds
  double UserTime() const;
  /// returns the user+system time measured by timer in seconds
  double SystemTime() const;
};

#endif // __TIMER_H__
