#include <DS1302.h>

#define CLK_RST 2 //three real time clock pins
#define CLK_IO 3
#define CLK_SCLK 4

DS1302 rtc(CLK_RST, CLK_IO, CLK_SCLK);

void setup()
{
  Serial.begin(9600);
  rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  Time t(2017, 11, 22, 22, 53, 00, Time::kTuesday);

  // Set the time and date on the chip.
  rtc.time(t);

}

void loop()
{
    printTime();
  delay(1000);

}

void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           t.day,
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  Serial.println(buf);
}
