#include <IRremote.h>

IRsend irsend;
const int IR_CODE = 0xFA2;
unsigned long last_fired = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);

}

void loop() {
  if(millis() >= (last_fired + 500) && digitalRead(7) == LOW)
  {
    irsend.sendSony(IR_CODE, 12);
    digitalWrite(13, HIGH);
    delay(20);
    digitalWrite(13, LOW);
    delay(20);

    last_fired = millis();
  }
}
