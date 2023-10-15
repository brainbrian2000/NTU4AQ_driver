#include <Wire.h>
#include <RTC.h>

static DS3231 RTC;

void setup()
{
  Serial.begin(9600);
  RTC.begin();



  if (RTC.getHourMode() == CLOCK_H12)
  {
    // RTC.setMeridiem(HOUR_AM);
  }
/*
  RTC.setDay(13);
  RTC.setMonth(05);
  RTC.setYear(2020);

  RTC.setHours(9);
  RTC.setMinutes(47);
  RTC.setSeconds(56);
*/
  RTC.setWeek(1);

    RTC.setHourMode(CLOCK_H12);
  //RTC.setHourMode(CLOCK_H24);

  RTC.setDate(22,07,29);
  //RTC.setTime(22,10,20);

  RTC.setHours(22);
  


}

void loop()
{
  switch (RTC.getWeek())
  {
    case 1:
      Serial.print("SUN");
      break;
    case 2:
      Serial.print("MON");
      break;
    case 3:
      Serial.print("TUE");
      break;
    case 4:
      Serial.print("WED");
      break;
    case 5:
      Serial.print("THU");
      break;
    case 6:
      Serial.print("FRI");
      break;
    case 7:
      Serial.print("SAT");
      break;
  }
  Serial.print(" ");
  Serial.print(RTC.getDay());
  Serial.print("-");
  Serial.print(RTC.getMonth());
  Serial.print("-");
  Serial.print(RTC.getYear());

  Serial.print(" ");

  Serial.print(RTC.getHours());
  Serial.print(":");
  Serial.print(RTC.getMinutes());
  Serial.print(":");
  Serial.print(RTC.getSeconds());
  Serial.print(" ");

  if (RTC.getHourMode() == CLOCK_H12)
  {
    switch (RTC.getMeridiem()) {
      case HOUR_AM :
        Serial.print(" AM");
        break;
      case HOUR_PM :
        Serial.print(" PM");
        break;
    }
  }
  Serial.println("");
  delay(1000);
}
