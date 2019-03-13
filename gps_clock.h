#pragma once

#include <Arduino.h>

#include <Time.h>
#include <TimeLib.h>


class GpsClock
{
public:
  GpsClock();
  boolean is_time_set() const;
  time_t get_time() const;

  uint32_t get_micros() const;
  uint32_t get_millis() const;

  void set_gps_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
  void set_gps_time(time_t t);

  void on_pps_pulse();

private:
  void set_epoch(uint16_t year);

  uint64_t _epoch;
  volatile time_t _last_time_set;
  volatile unsigned long _last_micros;
  volatile time_t _last_gps_time_sec;
};
