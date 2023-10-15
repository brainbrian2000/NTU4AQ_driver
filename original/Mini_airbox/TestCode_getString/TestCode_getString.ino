/*
   這是一個讀取序列埠指令的程式
   Date: 2020/08/22
   Author:Kyle
   License: MIT
   reference：https://www.arduino.cc/reference/en/language/functions/communication/serial/readstring/
*/

String getCommand(void)  // 把UART的資料讀出成String回傳
{
  String _buffer;
  while (Serial.available())
  {
    // Serial.readString() 可以逐一把Serial得到的指令字元逐一回傳成String
    _buffer += Serial.readString();
  }
  return _buffer;
}


void setup() {
  Serial.begin(115200);
  while(!Serial) ;
  Serial.println("[SYS] Setup Done, Enter Loop");
}


void loop() {
  String _CMD = getCommand();
  if (_CMD.length() > 0)
  {
    Serial.println("[CMD] " + _CMD);
  }
  delay(1000);
}
