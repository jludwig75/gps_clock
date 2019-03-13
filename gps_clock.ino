#include <SoftwareSerial.h>

#include "my_Adafruit_GPS.h"
#include "gps_clock.h"

#define RX_PIN  8
#define TX_PIN  9
const byte interruptPin = 2;

SoftwareSerial gps_serial(RX_PIN, TX_PIN);

Adafruit_GPS gps(&gps_serial);

GpsClock gps_clock;

const uint32_t tz_seconds = 6 * 60 * 60;

void show_current_time()
{
    if (gps_clock.is_time_set())
    {
      time_t t = gps_clock.get_time() - tz_seconds;
//      Serial.print("Got clock time "); Serial.println((unsigned long)t);
      Serial.print("Clock Time: ");
      Serial.print(hour(t), DEC); Serial.print(':');
      Serial.print(minute(t), DEC); Serial.print(':');
      Serial.print(second(t), DEC); //Serial.print('.');
      //Serial.println(gps.milliseconds);
      Serial.print(" - ");
      Serial.print(day(t), DEC); Serial.print('/');
      Serial.print(month(t), DEC); Serial.print("/");
      Serial.print(year(t), DEC);
      Serial.print(" ");
      Serial.println(gps_clock.get_millis());
    }
    else
    {
      Serial.println("Time not yet set\n");
    }
}

void pps_interrupt() {
  //Serial.println("PPS pulse");
  gps_clock.on_pps_pulse();
  show_current_time();
}

void setup()  
{
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(9600);
  Serial.println("GPS Clock");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  gps.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  gps.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  gps_serial.println(PMTK_Q_RELEASE);

  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), pps_interrupt, RISING);
}


uint32_t timer = millis();
void loop()                     // run over and over again
{
  gps.read();
  
  if (gps.newNMEAreceived()) {
  
    boolean date_and_time_updated = false;
    if (!gps.parse(gps.lastNMEA(), &date_and_time_updated))   // this also sets the newNMEAreceived() flag to false
    {
      return;  // we can fail to parse a sentence in which case we should just wait for another
    }
    //Serial.print("\nFix: "); Serial.print((int)gps.fix);
    //Serial.print(" quality: "); Serial.println((int)gps.fixquality); 
    if (gps.fix > 0 && date_and_time_updated)
    {
//      Serial.println("Setting GPS time...");
      gps_clock.set_gps_time(2000 + gps.year, gps.month, gps.day, gps.hour, gps.minute, gps.seconds);
//      Serial.print("Setting GPS time to "); Serial.println((unsigned long)gps_time);
    }
  }
}
