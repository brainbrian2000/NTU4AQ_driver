# 1 "e:\\.NTU\\IPCS\\NTU4AQ\\proj_SD\\ds3231\\ds3231.ino"
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
# 3 "e:\\.NTU\\IPCS\\NTU4AQ\\proj_SD\\ds3231\\ds3231.ino" 2

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Serial.begin(9600);
  Serial.println("Testing...");
// #ifndef ESP8266
  while (!Serial){
   ;
  } // wait for serial port to connect. Needed for native USB
// #endif
  Serial.println("Testing1");
  rtc.begin();
  Serial.println("Testing2");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
 Serial.println("Test2");
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime((reinterpret_cast<const __FlashStringHelper *>(
# 29 "e:\\.NTU\\IPCS\\NTU4AQ\\proj_SD\\ds3231\\ds3231.ino" 3
                       (__extension__({static const char __c[] __attribute__((__progmem__)) = ("Aug  3 2022"); &__c[0];}))
# 29 "e:\\.NTU\\IPCS\\NTU4AQ\\proj_SD\\ds3231\\ds3231.ino"
                       )), (reinterpret_cast<const __FlashStringHelper *>(
# 29 "e:\\.NTU\\IPCS\\NTU4AQ\\proj_SD\\ds3231\\ds3231.ino" 3
                                    (__extension__({static const char __c[] __attribute__((__progmem__)) = ("10:46:10"); &__c[0];}))
# 29 "e:\\.NTU\\IPCS\\NTU4AQ\\proj_SD\\ds3231\\ds3231.ino"
                                    ))));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
 Serial.println("Test3");

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop () {
    DateTime now = rtc.now();

    Serial.print(now.year(), 10);
    Serial.print('/');
    Serial.print(now.month(), 10);
    Serial.print('/');
    Serial.print(now.day(), 10);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), 10);
    Serial.print(':');
    Serial.print(now.minute(), 10);
    Serial.print(':');
    Serial.print(now.second(), 10);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    // calculate a date which is 7 days, 12 hours, 30 minutes, 6 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    Serial.print(" now + 7d + 12h + 30m + 6s: ");
    Serial.print(future.year(), 10);
    Serial.print('/');
    Serial.print(future.month(), 10);
    Serial.print('/');
    Serial.print(future.day(), 10);
    Serial.print(' ');
    Serial.print(future.hour(), 10);
    Serial.print(':');
    Serial.print(future.minute(), 10);
    Serial.print(':');
    Serial.print(future.second(), 10);
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    delay(3000);
}
