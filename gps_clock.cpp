#include "gps_clock.h"

GpsClock::GpsClock() :
  _epoch(0),
  _last_time_set(0),
  _last_micros(0),
  _last_gps_time_sec(0)
{
}

boolean GpsClock::is_time_set() const
{
  return _last_time_set != 0;
}

void GpsClock::set_epoch(uint16_t year)
{
  tmElements_t tm;
  tm.Second = 0;
  tm.Minute = 0;
  tm.Hour = 0;
  tm.Day = 1;
  tm.Month = 1;
  tm.Year = CalendarYrToTm(year);
  _epoch = (uint64_t)makeTime(tm) * 1000000ULL;
}

uint32_t GpsClock::get_micros() const
{
  noInterrupts();
  uint32_t m = (uint32_t)(_epoch + _last_time_set * 1000000ULL + (micros() - _last_micros));
  interrupts();
  return m;
}

uint32_t GpsClock::get_millis() const
{
  return get_micros() / 1000UL;
}

void GpsClock::set_gps_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
  tmElements_t tm;
  tm.Second = second;
  tm.Minute = minute;
  tm.Hour = hour;
  tm.Day = day;
  tm.Month = month;
  tm.Year = CalendarYrToTm(year);
  time_t t = makeTime(tm);
  set_gps_time(t);
  set_epoch(year);
}

void GpsClock::set_gps_time(time_t t)
{
//    Serial.print("Set GPS time "); Serial.println((unsigned long)t);
  noInterrupts();
  _last_gps_time_sec = t;
  interrupts();
}

void GpsClock::on_pps_pulse()
{
  _last_micros = micros();
  _last_time_set = _last_gps_time_sec + 1;
//    Serial.print("Set clock time to "); Serial.println((unsigned long)_last_time_set);
}

time_t GpsClock::get_time() const
{
  // NOTE: This breaks if the time hasn't been set in a little over an hour,
  // because the microseconds wrap. May be an issue if the wrap happens at
  // any time. I think it needs to be synchronized to ensure this. Well
  // the wrap will result in a larger value for the previous time and the
  // subtraction will wrap too. This should stil work, but synchronization would be
  // good to be sure.
  noInterrupts();
  time_t t = _last_time_set + (micros() - _last_micros) / 1000000UL;
  interrupts();
  
  return t;
}
