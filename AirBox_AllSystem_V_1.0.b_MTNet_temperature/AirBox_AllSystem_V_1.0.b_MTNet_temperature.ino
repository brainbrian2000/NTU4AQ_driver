/*
  Name:    NTU_1.ino
  Created: 2020/8/3 下午 06:06:23
  Author:  Liu
*/
#define DEBUG false
// #define DEBUG true

#include <Wire.h>

/***************************************************************************
  SHT31 模組
***************************************************************************/
#include "SHT31.h"

#define SHT31_ADDRESS   0x44
SHT31 sht31;


/***************************************************************************
  BME280 模組
***************************************************************************/
#include <BME280I2C.h>
#define BME280_ADDRESS   0x60
BME280I2C bme;

BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);

float temp(NAN), hum(NAN), pres(NAN);

enum source {
  source_BME280,
  source_SHT31
};

source Source;
/***************************************************************************
  Example sketch for the SI1145_WE library

  This sketch shows how to do PS, ALS, VIS an UV measurements in continuous mode.

  Further information can be found on:
  https://wolles-elektronikkiste.de/en/si1145-als-ir-uv-und-ps-sensor

***************************************************************************/
#include <SI1145_WE.h>
SI1145_WE mySI1145 = SI1145_WE();

unsigned long amb_als = 0;
unsigned long amb_ir = 0;
float lx = 0.0;
byte failureCode = 0;

/***************************************************************************
  Example sketch for the MAX44009 library
  光照度計(委外-邁特)
***************************************************************************/
#include <MAX44009.h>
MAX44009 light;

/***************************************************************************
  氣象站設定

***************************************************************************/

#define WD A3
#define WS 3

volatile unsigned int WS_count = 0;
volatile unsigned long WS_time = 0;
unsigned long SYS_time = 0;
double WeedSpead;

void WS_ISR()
{
  if (millis() - WS_time > 50) {
    WS_count++;
    WS_time = millis();
    if(DEBUG) Serial.print("+");
  }
}

#include <SoftwareSerial.h>
#define PM_ctrlPin A2
SoftwareSerial PMS(9, 8); // RX, TX

float pmat010 = 0;
float pmat025 = 0;
float pmat100 = 0;
float Temp = 0;
float Humid = 0;
char buf[50];

String code_CMD;

String getCMD()
{
  String str_buffer = "";

  while (Serial.available())
  {
    char _c = (char)Serial.read();
    str_buffer += String(_c);
    delay(5);
  }

  str_buffer.replace("\r", "");
  str_buffer.replace("\n", "");

  if (str_buffer.length() > 1)
  {
    if (DEBUG) Serial.println("get CMD:\t" + str_buffer);
  }

  return str_buffer;
}

void sendCMD(String _cmd)
{
  Serial.println(_cmd);
}


/* PMS 5003 T 用的資料 */
void getPM()
{
  int count = 0;
  unsigned char c;
  unsigned char high;

  digitalWrite(PM_ctrlPin, HIGH);
  delay(10000);

  while (PMS.available())
  {
    c = PMS.read();
    if ((count == 0 && c != 0x42) || (count == 1 && c != 0x4d)) {
      break;
    }
    if (count > 27) {
      break;
    }
    else if (count == 10 || count == 12 || count == 14 || count == 24 || count == 26) {
      high = c;
    }
    else if (count == 11) {
      pmat010 = 256 * high + c;
    }
    else if (count == 13) {
      pmat025 = 256 * high + c;
    }
    else if (count == 15) {
      pmat100 = 256 * high + c;
    }
    else if (count == 25) {
      Temp = (256 * high + c) / 10.0;
    }
    else if (count == 27) {
      Humid = (256 * high + c) / 10.0;
    }
    count++;
  }

  while (PMS.available()) PMS.read();
  digitalWrite(PM_ctrlPin, LOW);
}

void getWindSpeed()
{
  double _totalWS = 0;
  int _count = 0;

  attachInterrupt(digitalPinToInterrupt(WS), WS_ISR, CHANGE);

  for (int _t = 0; _t < 20; _t++)
  {
    if (millis() - SYS_time > 200) {
      WeedSpead = WS_count * 0.33 / 0.2;

      if (_t > 0) {
        _totalWS += WeedSpead; // 累計風速
        _count++;  // 計算累加次數
      }
      WS_count = 0;
      SYS_time = millis();
    }
    delay(100);
  }
  // 算平均值
  WeedSpead = _totalWS / _count;

  detachInterrupt(digitalPinToInterrupt(WS));
}


void MeasuringLightComposition()
{
  for (int i = 0; i < 50; i++) {
    mySI1145.clearAllInterrupts();
    mySI1145.startSingleMeasurement();
    amb_als += mySI1145.getAlsVisData();
    amb_ir += mySI1145.getAlsIrData();
  }
  amb_als /= 50;
  amb_ir /= 50;
  lx = calcLux(amb_als, amb_ir);

  if (DEBUG) {
    Serial.print("Ambient Light: ");    Serial.println(amb_als);
    Serial.print("Infrared Light: ");   Serial.println(amb_ir);
    Serial.print("Lux: ");              Serial.println(lx);
    Serial.println("---------");
  }

  failureCode = mySI1145.getFailureMode();  // reads the response register
  if ((failureCode & 128)) { // if bit 7 is set in response register, there is a failure
    handleFailure(failureCode);
  }
}

void handleFailure(byte code) {
  String msg = "";
  switch (code) {
    case SI1145_RESP_INVALID_SETTING:
      msg = "Invalid Setting";
      break;
    case SI1145_RESP_PS1_ADC_OVERFLOW:
      msg = "PS ADC Overflow";
      break;
    case SI1145_RESP_ALS_VIS_ADC_OVERFLOW:
      msg = "ALS VIS ADC Overflow";
      break;
    case SI1145_RESP_ALS_IR_ADC_OVERFLOW:
      msg = "ALS IR Overflow";
      break;
    case SI1145_RESP_AUX_ADC_OVERFLOW:
      msg = "AUX ADC Overflow";
      break;
    default:
      msg = "Unknown Failure";
      break;
  }
  if (DEBUG) Serial.println(msg);
  mySI1145.clearFailure();
}

float calcLux(uint16_t vis, uint16_t ir) {
  const unsigned int vis_dark = 256; // empirical value
  const unsigned int ir_dark = 250; // empirical value
  const float gainFactor = 1.0;
  const float visCoeff = 5.41; // application notes AN523
  const float irCoeff = 0.08; // application notes AN523
  const float visCountPerLux = 0.319; // for incandescent bulb (datasheet)
  const float irCountPerLux = 8.46; // for incandescent bulb (datasheet)
  const float corrFactor = 0.18; // my empirical correction factor

  // According to application notes AN523:
  float lux = ((vis - vis_dark) * visCoeff - (ir - ir_dark) * irCoeff) * gainFactor;

  // the equation above does not consider the counts/Lux depending on light source type
  // I suggest the following equation
  // float lux = (((vis - vis_dark) / visCountPerLux) * visCoeff - ((ir - ir_dark) / irCountPerLux) * irCoeff) * gainFactor * corrFactor;

  return lux;
}

void setup()
{
  Serial.begin(9600);

  PMS.begin(9600);
  pinMode(PM_ctrlPin, OUTPUT);
  if (DEBUG)  Serial.println("PMS5003T Ready");

  pinMode(WS, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WS), WS_ISR, CHANGE);
  WS_time = millis();
  SYS_time = millis();
  if (DEBUG)  Serial.println("Wind Ready");
  detachInterrupt(digitalPinToInterrupt(WS));

  Wire.begin();
  Wire.beginTransmission(BME280_ADDRESS);
  if (!Wire.endTransmission()) Source = source_BME280;
  Wire.beginTransmission(SHT31_ADDRESS);
  if (!Wire.endTransmission()) Source = source_SHT31;

  switch (Source) {
    case source_BME280:
      bme.begin();
      if (DEBUG)  Serial.println("BME280 Ready");
      break;
    case source_SHT31:
      sht31.begin(SHT31_ADDRESS);
      if (DEBUG)  Serial.println("SHT31 Ready");
      break;
  }

  // 使用 MAX44009 光照度Sensor
  light.begin();

  // 使用 SI1145 光照度Sensor
  mySI1145.init();
  mySI1145.setAlsVisAdcGain(0);
  mySI1145.setPsAdcGain(0);
  /* choices: PS_TYPE, ALS_TYPE, PSALS_TYPE, ALSUV_TYPE, PSALSUV_TYPE || FORCE, AUTO, PAUSE */
  mySI1145.enableMeasurements(PSALSUV_TYPE, AUTO);
  mySI1145.enableMeasurements(ALS_TYPE, FORCE);
  if (DEBUG)  Serial.println("Light Ready");

}


void loop()
{
  code_CMD = getCMD();
  code_CMD.toLowerCase();

  if (code_CMD.equalsIgnoreCase("light"))
  {
    // MeasuringLightComposition(); // 量測光的組成
    lx = light.get_lux();
    if (DEBUG)    Serial.println("Light: " + String(lx, 2) + " lx");
    Serial.println(String(lx, 2));
  }
  else if (code_CMD.equalsIgnoreCase("pm025"))
  {
    getPM();
    if (DEBUG)    Serial.println("PM2.5 : " + String(pmat025) + " ug/m3");
    Serial.println(pmat025,2);
  }
  else if (code_CMD.equalsIgnoreCase("pm100"))
  {
    getPM();
    if (DEBUG)    Serial.println("PM10.0 : " + String(pmat100) + " ug/m3");
    Serial.println(pmat100,2);
  }
  else if (code_CMD.equalsIgnoreCase("pm010"))
  {
    getPM();
    if (DEBUG)    Serial.println("PM1.0 : " + String(pmat010) + " ug/m3");
    Serial.println(pmat010,2);
  }
  else if (code_CMD.equalsIgnoreCase("temperature"))
  {
    if (DEBUG)    Serial.print(F("Temperature(C): "));

    switch (Source)
    {
      case source_BME280:
        bme.read(pres, temp, hum, tempUnit, presUnit);
        Serial.println(temp, 2);
        break;
      case source_SHT31:
        sht31.read(true);
        Serial.println(sht31.getTemperature(), 2);
        break;
    }
  }
  else if (code_CMD.equalsIgnoreCase("humidity"))
  {
    if (DEBUG)    Serial.print(F("Humidity(%RH): "));
    switch (Source)
    {
      case source_BME280:
        bme.read(pres, temp, hum, tempUnit, presUnit);
        Serial.println(hum, 2);
        break;
      case source_SHT31:
        sht31.read(true);
        Serial.println(sht31.getHumidity(), 2);
        break;
    }
  }
  else if (code_CMD.equalsIgnoreCase("pressure"))
  {
    if (DEBUG)    Serial.print(F("Pressure(kPa): "));
    switch (Source)
    {
      case source_BME280:
        bme.read(pres, temp, hum, tempUnit, presUnit);
        Serial.println(pres / 1000.0, 3);
        break;
      case source_SHT31:
        Serial.println(0.0, 3);
        break;
    }
  }
  else if (code_CMD.equalsIgnoreCase("windspeed"))
  {
    getWindSpeed();
    if (DEBUG) {
      Serial.print(F("WS: "));
      Serial.print(WeedSpead, 3);
      Serial.println(F(" m/s"));
    }
    Serial.println(WeedSpead, 3);
  }
  else if (code_CMD.length() > 0)
  {
    if (DEBUG)  Serial.println("ERROR: " + code_CMD);
  }

}
