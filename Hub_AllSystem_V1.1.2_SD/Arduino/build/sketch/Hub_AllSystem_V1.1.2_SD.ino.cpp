#include <Arduino.h>
#line 1 "E:\\.NTU\\IPCS\\NTU4AQ\\library\\Hub_AllSystem_V1.1.2_SD\\Hub_AllSystem_V1.1.2_SD.ino"
/*
   NTU4AQ Sensor Hub 系統
   Ver 1.0
   功能：
    1.  取得Airbox資料
    2.  取得聲音模組資料
    3.  讀取RTC 時間
    4.  寫入SD卡：一天一筆資料->每次一筆
    5.  進入睡眠
   TODO:
    1.  SD 寫入
    2.  RTC 校時

*/

#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <I2C_RTC.h>
#define RESET_RTC_TIME 0
// #define RESET_RTC_TIME 1
static DS3231 RTC;

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
#if RESET_RTC_TIME == 1
void setup(){
  #define SensorHubI2C    32
  #define SensorHubSound  34
  Serial.begin(115200);             // 訊號輸出用
  Serial.println("Running Test");
  pinMode(SensorHubI2C, OUTPUT);
  pinMode(SensorHubSound, OUTPUT);
  digitalWrite(SensorHubI2C, HIGH);
  digitalWrite(SensorHubSound, HIGH);

  // delay(500); // 停5 sec 確保PMU的I2C 狀態切換完成
  Wire.begin();
  Serial.println("I2C setting");
  // delay(5000);
  Serial.println("I2C Trying");
  if(RTC.begin()){

  // delay(1000);
  Serial.println("RTC is reseting");
  RTC.setHourMode(CLOCK_H24);
  RTC.setDateTime(__DATE__, __TIME__);
  }
  else{
    Serial.println("RTC Turn on failure");
  }

}
void loop(){
  delay(5000);
  String rtcDate ="";
  String rtcTime ="";
  rtcDate += RTC.getYear();
  rtcDate += "-";
  rtcDate += modifydecimal(RTC.getMonth());
  rtcDate += "-";
  rtcDate += modifydecimal(RTC.getDay());
  rtcTime += modifydecimal(RTC.getHours());
  rtcTime += ":";
  rtcTime += modifydecimal(RTC.getMinutes());
  rtcTime += ":";
  rtcTime += modifydecimal(RTC.getSeconds());
  Serial.println("RTC: " +rtcDate+" "+ rtcTime);
}

#else
const String Name = "00025";                         // NTU4AQ_Number.
const String File_path = "data/NTU4AQ/"+Name+".csv"; // Setting the SD file save path.
const int chipSelect = 53;     //arduino mega ->@ NTU4AQ, other boards need to check the pin of SD.
bool Reset_time = false;       // Time upload or not.
// bool Reset_time = true;
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
  String output(){
    String output = "NTU4AQ";
    output = output+Name;
    output = output+",";
    output = output+TEMP;
    output = output+",";
    output = output+HUM;
    output = output+",";
    output = output+PM100;
    output = output+",";
    output = output+PM025;
    output = output+",";
    output = output+PM010;
    output = output+",";
    output = output+WindSpeed;
    output = output+",";
    output = output+LUX;
    output = output+",";
    output = output+dBA;
    output = output+",";
    output = output+rtcDate;
    output = output+",";
    output = output+rtcTime+"\n";
    return output;
  };
  void write_sd(String Writing,String Path = File_path){
    // file in "data/NTU4AQ", check the file exists or not, if not, mkdir:
  // Serial.println("Now in "+Path+"\n"+Writing);
    if(!SD.exists("data")){
      SD.mkdir("data");
    }
    if(!SD.exists("data/NTU4AQ")){
      SD.mkdir("data/NTU4AQ");
    }
    File file;
    file = SD.open(Path,FILE_WRITE);
    int len = Writing.length();
    for(int i=0; i<len; i++){
      file.write(Writing[i]);
    } 
    file.close();
    return;
  }
  String read_sd(String _path = File_path){
    File file;
    file=SD.open(_path,FILE_READ);
    String file_read_output = "";
    if (file) {
      while (file.available()) {
        file_read_output = file_read_output+char(file.read());
      }
    }
    file.close();
    return file_read_output;
  }
  void SD_init(){  
    if(!SD.begin(chipSelect)){
      Serial.println('Fail, No SD card!');
    }
    return;
  }
} i2cStr;
struct SD_String {
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
  String output(){
    String output = "NTU4AQ_";
    output = output+Name;
    output = output+",";
    output = output+TEMP;
    output = output+",";
    output = output+HUM;
    output = output+",";
    output = output+PM100;
    output = output+",";
    output = output+PM025;
    output = output+",";
    output = output+PM010;
    output = output+",";
    output = output+WindSpeed;
    output = output+",";
    output = output+LUX;
    output = output+",";
    output = output+dBA;
    output = output+",";
    output = output+rtcDate;
    output = output+",";
    output = output+rtcTime+"\n";
    // info(output);
    return output;
  };
  void info(String _msg, uint8_t _TITLE = true, uint8_t _OEL = true) {
    if (_TITLE)  Serial.print("[INFO] ");
    if (_OEL)    Serial.println(_msg);
    else        Serial.print(_msg);
  }
  void write_sd(String Writing,String Path = File_path){
    // file in "data/NTU4AQ", check the file exists or not, if not, mkdir:
  Serial.println("Now in "+Path+"\n"+Writing);
    if(!SD.exists("data")){
      SD.mkdir("data");
    }
    if(!SD.exists("data/NTU4AQ")){
      SD.mkdir("data/NTU4AQ");
    }
    File file;
    file = SD.open(Path,FILE_WRITE);
    int len = Writing.length();
    for(int i=0; i<len; i++){
      file.write(Writing[i]);
    } 
    file.close();
    return;
  }
  String read_sd(String _path = File_path){
    File file;
    file=SD.open(_path,FILE_READ);
    String file_read_output = "";
    if (file) {
      while (file.available()) {
        file_read_output = file_read_output+char(file.read());
      }
    }
    file.close();
    return file_read_output;
  }
  bool SD_init(){  
    if(!SD.begin(chipSelect)){
      Serial.println('Fail, No SD card!');
      return 0;
    }else{
      Serial.println("SD card initialized!");
    }
    return 1;
  }
  void writing_without_reading(){
    String output_writing,output_reading;
    Serial.println(output());
    if(SD_init()){
    output_writing = output();
    write_sd(output_writing);
    info("Writing SD done.");
    }else{
      info("Open failed.");
    }
  }
  void writing_with_reading(){

    String output_writing,output_reading;
    if (SD_init()){

      output_writing = output();
      write_sd(output_writing);
      info("Writing SD_done");
      output_reading = read_sd();
      Serial.print(output_reading);
    }
    else{
      Serial.println("SD error.");
    }
  }

} SD_Str;


#define SensorHubI2C    32
#define SensorHubAirBox 33
#define SensorHubSound  34

#define _soundReset 23
uint8_t _errCount = 0;


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



void setup() {

  Serial.begin(9600);             // 訊號出出用
  Serial1.begin(9600);            // Air Box 用
  info("SystemVersion 1.1.2_SD");
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
  // /*
  if (Reset_time) {
    info("RTC is reseting");
    RTC.setHourMode(CLOCK_H24);
    RTC.setDateTime(__DATE__, __TIME__);
  }
  // */
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

  SD_Str.rtcDate += RTC.getYear();
  SD_Str.rtcDate += "-";
  SD_Str.rtcDate += modifydecimal(RTC.getMonth());
  SD_Str.rtcDate += "-";
  SD_Str.rtcDate += modifydecimal(RTC.getDay());

  SD_Str.rtcTime += modifydecimal(RTC.getHours());
  SD_Str.rtcTime += ":";
  SD_Str.rtcTime += modifydecimal(RTC.getMinutes());
  sendI2C(i2cStr.rtcDate);
  sendI2C(i2cStr.rtcTime);


  digitalWrite(SensorHubAirBox, HIGH);
  delay(1000);
  // 取得Airbox資料
  _strBUFFER = getData("windspeed", 2500);
  i2cStr.WindSpeed = "WS," + _strBUFFER;
  SD_Str.WindSpeed = _strBUFFER;
  sendI2C(i2cStr.WindSpeed);
  delay(500);

  _strBUFFER = getData("temperature", 500);
  i2cStr.TEMP = "TEMP," + _strBUFFER;
  SD_Str.TEMP = _strBUFFER;
  sendI2C(i2cStr.TEMP);
  delay(500);

  _strBUFFER = getData("humidity", 500);
  i2cStr.HUM = "HUM," + _strBUFFER;
  SD_Str.HUM = _strBUFFER;
  sendI2C(i2cStr.HUM);
  #if 1
  _strBUFFER = getData("pm100", 10750);
  i2cStr.PM100 = "PM100," + _strBUFFER;
  SD_Str.PM100 = _strBUFFER;
  sendI2C(i2cStr.PM100);

  _strBUFFER = getData("pm025", 10750);
  i2cStr.PM025 = "PM025," + _strBUFFER;
  SD_Str.PM025 = _strBUFFER;
  sendI2C(i2cStr.PM025);

  _strBUFFER = getData("pm010", 10750);
  i2cStr.PM010 = "PM010," + _strBUFFER;
  SD_Str.PM010 = _strBUFFER;
  sendI2C(i2cStr.PM010);

  _strBUFFER = getData("light", 1000);
  i2cStr.LUX = "LUX," + _strBUFFER;
  SD_Str.LUX = _strBUFFER;
  sendI2C(i2cStr.LUX);


  digitalWrite(SensorHubAirBox, LOW);
  delay(1000);
  // 取得Sound資料
  digitalWrite(SensorHubSound, HIGH);
  resetSound(_soundReset);
  _strBUFFER = getStrSound();
  //  _strBUFFER = "-00.00";
  i2cStr.dBA = "Sound," + _strBUFFER;
  SD_Str.dBA = _strBUFFER;
  #else
  _strBUFFER = "TESTing";
  i2cStr.WindSpeed = "WS," + _strBUFFER;
  SD_Str.WindSpeed = _strBUFFER;
  i2cStr.TEMP = "TEMP," + _strBUFFER;
  SD_Str.TEMP = _strBUFFER;
  i2cStr.HUM = "HUM," + _strBUFFER;
  SD_Str.HUM = _strBUFFER;
  i2cStr.PM100 = "PM100," + _strBUFFER;
  SD_Str.PM100 = _strBUFFER;
  i2cStr.PM025 = "PM025," + _strBUFFER;
  SD_Str.PM025 = _strBUFFER;
  i2cStr.PM010 = "PM010," + _strBUFFER;
  SD_Str.PM010 = _strBUFFER;
  i2cStr.LUX = "LUX," + _strBUFFER;
  SD_Str.LUX = _strBUFFER;
  i2cStr.dBA = "Sound," + _strBUFFER;
  SD_Str.dBA = _strBUFFER;
  #endif
  Serial.println("========SD_writing=========");
// /*SD writing
  SD_Str.writing_without_reading();
  // SD_Str.writing_with_reading();
  // String output_writing,output_reading;
  // i2cStr.SD_init();
  // output_writing = i2cStr.output();
  // i2cStr.write_sd(output_writing);
  // output_reading = i2cStr.read_sd();
  // Serial.print(output_reading);
// */SD writing end.
  sendI2C(i2cStr.dBA);
  sendI2C("SLEEP");
}

void loop() {
  Serial.println("Sleep failed, Resend command");
  sendI2C("SLEEP");
  delay(500);
}
#endif

// #if RESET_RTC_TIME == 1

