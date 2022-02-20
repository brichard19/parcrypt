#include "util.h"


namespace util {

// Simple timer class
Timer::Timer()
{
}


void Timer::start()
{
  _running = true;
  _start = util::get_time();
}

void Timer::stop()
{
  _running = false;
  _end = util::get_time();
}

double Timer::elapsed()
{
  if(_running) {
    return util::get_time() - _start;
  } else {
    return _end - _start;
  }
}

};