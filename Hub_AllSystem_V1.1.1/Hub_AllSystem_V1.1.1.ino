/*
   NTU4AQ Sensor Hub 系統
   Ver 1.0
   功能：
    1.  取得Airbox資料
    2.  取得聲音模組資料
    3.  讀取RTC 時間
    4.  寫入SD卡：一天一筆資料
    5.  進入睡眠
   TODO:
    1.  SD 寫入
    2.  RTC 校時

*/

#include <Wire.h>

#include <RTC.h>
static DS3231 RTC;


#define SensorHubI2C    32
#define SensorHubAirBox 33
#define SensorHubSound  34

#define _soundReset 23
uint8_t _errCount = 0;

String _Date, _Time;

struct I2C_String {
  String WindSpeed;
  String WindDirection;
  String Rain;
  String PM010;
  String PM025;
  String PM100;
  String TEMP;
  String HUM;
  String kPa;
  String LUX;
  String dBA;
  String rtcDate;
  String rtcTime;
} i2cStr;


void info(String _msg, uint8_t _TITLE = true, uint8_t _OEL = true) {

  if (_TITLE)  Serial.print("[INFO] ");
  if (_OEL)    Serial.println(_msg);
  else        Serial.print(_msg);
}


// 重新啟動聲音模組
void resetSound(uint8_t _pin)
{
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(200);
  pinMode(_pin, INPUT_PULLUP);
}

// 確認聲音模組是否接上
uint8_t checkWire(uint8_t _pin)
{
  pinMode(_pin, INPUT);
  return digitalRead(_pin);
}

String getI2C(int _addr, int _length)
{
  String _strBuffer;

  Wire.requestFrom(_addr, _length);    // request 6 bytes from slave device #8
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    //    Serial.print(c);         // print the character
    _strBuffer += String(c);
  }

  //  Serial.print("\n");

  if (_strBuffer.length() > 0)
  {
    if (_strBuffer.indexOf("-inf") > 0 || _strBuffer.indexOf("0.00") > 0 )
    {
      _strBuffer = "Error:" + _strBuffer.substring(11);
    }
    else if (_strBuffer.indexOf(":") > 0)
    {
      _strBuffer = _strBuffer.substring(11);
    }
    //    Serial.print("[I2C] ");
    //    Serial.print(_strBuffer);
    //    Serial.println(" dB(A)");
  }
  return _strBuffer;
}

float getSound()
{
  String _strBuffer, _valueBuffer;
  float _value = 0.00;

  if (checkWire(_soundReset))
  {
    _strBuffer = getI2C(0x71, 16);

    if (_strBuffer.indexOf("Error") > -1 || _strBuffer == _valueBuffer)
    {
      _errCount++;
      if (_errCount > 5)
      {
        resetSound(_soundReset);
        _errCount = 0;
        Serial.println("[Sound] Reset Sensor");
      }
    }
    else
    {
      _errCount = 0;
      _valueBuffer = _strBuffer;
      _value = _strBuffer.toFloat();
    }
  }
  else
  {
    Serial.println("[Sound] No Sensor");
  }

  return _value;
}


String getStrSound()
{
  String _value;
  float _f_buffer;

  for (uint8_t _i; _i < 10; _i++)
  {
    _f_buffer = getSound();
    if (_f_buffer > 0) {
      _value = String(_f_buffer);
      break;
    }
    delay(1000);
    info(String(_i));
  }

  return _value;
}


String getCMD()
{
  String str_buffer = "";

  while (Serial1.available())
  {
    char _c = (char) Serial1.read();
    str_buffer += String(_c);
    delay(10);
  }

  str_buffer.replace("\r", "");
  str_buffer.replace("\n", "");

  if (str_buffer.length() > 1)
  {
    //    Serial.println("get CMD:\t" + str_buffer);
  }

  return str_buffer;
}

void sendCMD(String _cmd)
{
  //  Serial.println("send CMD:\t" + _cmd);
  Serial1.println(_cmd);
  Serial1.flush();
}

String getData(String _cmd, int _delay)
{
  sendCMD(_cmd);
  delay(_delay);
  return getCMD();
}

void sendI2C(String _str)
{
  info(_str);
  Wire.beginTransmission(0x70); // transmit to device #8
  Wire.write(_str.c_str());          // sends five bytes
  Wire.endTransmission();       // stop transmitting
  delay(500);
}

String modifydecimal(uint8_t _int)
{
  String _result = "0";
  if (_int < 10)
  {
    _result += String(_int);
  }
  else
  {
    _result = String(_int);
  }
  return _result;
}

void setup() {

  Serial.begin(9600);             // 訊號出出用
  Serial1.begin(9600);            // Air Box 用

  info("OEPN I2C Single");
  pinMode(SensorHubI2C, OUTPUT);
  digitalWrite(SensorHubI2C, HIGH);
  pinMode(SensorHubSound, OUTPUT);
  digitalWrite(SensorHubSound, HIGH);

  delay(5000); // 停5 sec 確保PMU的I2C 狀態切換完成

  info("Setting Sensor Control");
  pinMode(SensorHubAirBox, OUTPUT);
  digitalWrite(SensorHubAirBox, HIGH);

  Wire.begin();

  String _strBUFFER = "12.3";


  sendI2C("[Hub] begin transmission Data");

  sendI2C("[Hub] INIT RTC");
  RTC.begin();
  
  if (!RTC.isRunning()) {
    RTC.setHourMode(CLOCK_H24);
    RTC.setDateTime(__DATE__, __TIME__);
  }

  i2cStr.rtcDate ="rtcDATE,";
  i2cStr.rtcDate += RTC.getYear();
  i2cStr.rtcDate += "-";
  i2cStr.rtcDate += modifydecimal(RTC.getMonth());
  i2cStr.rtcDate += "-";
  i2cStr.rtcDate += modifydecimal(RTC.getDay());

  i2cStr.rtcTime ="rtcTIME,";
  i2cStr.rtcTime += modifydecimal(RTC.getHours());
  i2cStr.rtcTime += ":";
  i2cStr.rtcTime += modifydecimal(RTC.getMinutes());

  sendI2C(i2cStr.rtcDate);
  sendI2C(i2cStr.rtcTime);


  digitalWrite(SensorHubAirBox, HIGH);
  delay(1000);
  // 取得Airbox資料
  _strBUFFER = getData("windspeed", 2500);
  i2cStr.WindSpeed = "WS," + _strBUFFER;
  sendI2C(i2cStr.WindSpeed);
  delay(500);

  _strBUFFER = getData("temperature", 500);
  i2cStr.TEMP = "TEMP," + _strBUFFER;
  sendI2C(i2cStr.TEMP);
  delay(500);

  _strBUFFER = getData("humidity", 500);
  i2cStr.HUM = "HUM," + _strBUFFER;
  sendI2C(i2cStr.HUM);

  _strBUFFER = getData("pm100", 10750);
  i2cStr.PM100 = "PM100," + _strBUFFER;
  sendI2C(i2cStr.PM100);

  _strBUFFER = getData("pm025", 10750);
  i2cStr.PM025 = "PM025," + _strBUFFER;
  sendI2C(i2cStr.PM025);

  _strBUFFER = getData("pm010", 10750);
  i2cStr.PM010 = "PM010," + _strBUFFER;
  sendI2C(i2cStr.PM010);

  _strBUFFER = getData("light", 1000);
  i2cStr.LUX = "LUX," + _strBUFFER;
  sendI2C(i2cStr.LUX);


  digitalWrite(SensorHubAirBox, LOW);
  delay(1000);
  // 取得Sound資料
  digitalWrite(SensorHubSound, HIGH);
  resetSound(_soundReset);
  _strBUFFER = getStrSound();
  //  _strBUFFER = "-00.00";
  i2cStr.dBA = "Sound," + _strBUFFER;
  sendI2C(i2cStr.dBA);

  sendI2C("SLEEP");
}

void loop() {
  Serial.println("Sleep failed, Resend command");
  sendI2C("SLEEP");
  delay(500);
}
