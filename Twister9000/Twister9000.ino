#include <DS1302.h> //Clock
#include <IRremote.h> //IR sensor
#include <Servo.h>

#define SERVO 9   //digital PWM servo pin
#define SERVO_ENABLE 11
#define CLK_RST 2 //three real time clock pins
#define CLK_IO 3
#define CLK_SCLK 4
#define IR 5      //IR sensor pin
#define WS A0     //water sensor analog in
#define PUMP 10   //water pump pin

typedef struct
{
  //Time::Day day;
  uint8_t hour;
  uint8_t minute;
  bool wasFed =false;
}  feedingtime;

//defining all important thingies
//RTC
DS1302 rtc(CLK_RST, CLK_IO, CLK_SCLK);
//Servo
Servo servo;
//IR receiver
IRrecv irrecv(IR);
decode_results results;
int i;
int j;
int water_level;

//defining model parameters
const int water_full = 650;
const int water_empty = 500;
#define feedingsPerDay 2
int food_refill_time_ms = 3000; //miliseconds
bool isFed[feedingsPerDay]; //array with information if there were feedings 

feedingtime feedingcalendar[feedingsPerDay]; //this is supposed to be an array with feeding dates
                                 //and i really don't know if this will work



String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}
void setup()
{
  // creating the feeding calendar
  feedingcalendar[0].hour = 12;
  feedingcalendar[0].minute = 36;
  feedingcalendar[1].hour = 12;
  feedingcalendar[1].minute = 37;
  
  servo.attach(SERVO);
  pinMode(PUMP, OUTPUT);
  pinMode(SERVO_ENABLE, OUTPUT);
  irrecv.enableIRIn();
  rtc.halt(false);
  irrecv.enableIRIn(); // Start the IR receiver
  //following things are here only for development testing purposes
  //feedingtest.day = Time::kWednesday;

  Serial.begin(9600);
  i=0;
  j=0;
}

void loop()
{
   Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
 // Serial.println(buf);

  for(j=0;j<2;j++)
  {
//    Serial.print(feedingcalendar[j].hour);
//    Serial.print(":");
//    Serial.print(feedingcalendar[j].minute);
//    Serial.print(" was already fed?  ");
//    Serial.print(feedingcalendar[j].wasFed);
//    Serial.print("  ");
  }
  if(CanWeFeed(feedingcalendar, feedingsPerDay)==true)
  {
    //Serial.println("we should feed now!!!!");
    refill_food(); // feeding time :D
  }
  
  //else Serial.println("No feeding, sorry :(");

 // refill_water();
 IRreceive();

}

void refill_water()
{

  water_level = analogRead(WS);
  if (water_level > water_empty) return;

  digitalWrite(PUMP, HIGH);
  do
  {
    water_level = analogRead(WS);
    delay(100);
  } while (water_level < water_full);
  digitalWrite(PUMP, LOW);
}

void refill_food()
{
  digitalWrite(SERVO_ENABLE, HIGH);
  servo.writeMicroseconds(0);
  delay(food_refill_time_ms);
  servo.writeMicroseconds(1500);
  //delay(5000);  //wating for servo to stabilize
  digitalWrite(SERVO_ENABLE, LOW);
}


bool CanWeFeed(feedingtime ftime[], int arraySize)
{
  Time currenttime = rtc.time();

  if(ftime[i].wasFed !=0) 
  {
    return false;
  }
  else if(currenttime.hr>ftime[i].hour || (currenttime.hr==ftime[i].hour&&currenttime.min>=ftime[i].minute))
    {
      ftime[i].wasFed = true;
      i++;

      if(i==arraySize) // all of the feedings have been done today
      {
        i=0;
      }
      if(currenttime.hr == 0 && currenttime.min == 0) // end of the day
      {
        for(j=0;j<=arraySize;j++)
        {
          ftime[j].wasFed = false;
        }
      }
      return true;
    }
    else 
    {
      return false;
    }
}

void IRreceive()
{
  if (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    if((results.value == 0x9716BE3F) || (results.value == 0xFF30CF))
    {
      refill_food();
    }
    irrecv.resume(); // Receive the next value
  }
}

