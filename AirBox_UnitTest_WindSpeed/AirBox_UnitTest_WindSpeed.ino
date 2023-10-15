#define WD A3
#define WS 3

volatile unsigned long WS_count = 0;
volatile unsigned long WS_time = 0;
unsigned long SYS_time = 0;
double WeedSpead;

void WS_ISR()
{
  if (millis() - WS_time > 30) {
    WS_count++;
    WS_time = millis();
    Serial.print("+");
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(WS, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WS), WS_ISR, CHANGE);
  WS_time = millis();
  SYS_time = millis();
}

void loop() {
  noInterrupts();
  if (millis() - SYS_time > 5000) {
    WeedSpead = WS_count*0.33 /5.0;
    Serial.print("Wind Speed: ");
    Serial.print(WeedSpead,3);
    Serial.println(" m/s");
    WS_count = 0;
    SYS_time = millis();
  }
  delay(500);
  interrupts();
  delay(500);
}
