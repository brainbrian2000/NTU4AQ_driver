#define INT_1 2
#define INT_2 3

enum pState 
{
  OFF,
  ON
};

void pinControl(int _pin, pState _state)
{
  
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, _state);
  
  Serial.print("PIN: " + (String)_pin + " -> ");
  if (_state) Serial.println("HIGH");
  else        Serial.println("LOW");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Setup Done");
}

void loop()
{
  pinControl(INT_1, ON);
  delay(500);
  
  pinControl(INT_1, OFF);
  delay(500);
  
  pinControl(INT_2, ON);
  delay(500);
  
  pinControl(INT_2, OFF);
  delay(500);

}
