#include <SoftwareSerial.h>

#define MSP_RC 105

SoftwareSerial mspSerial(10, 11); // RX TX

boolean triggered = false;
boolean cooldown = false;

unsigned long Timer;

void setup() {
    mspSerial.begin(9600);
    Serial.begin(9600);                      
}

void loop() {
    uint8_t datad = 0;
    uint8_t *data = &datad;

    Trigger_sendMSP(MSP_RC, data, 0);
    Trigger_readData();
    if (triggered==true){
      Serial.println(" Works ");
      triggered = false;
    }
}

void Trigger_sendMSP(uint8_t cmd, uint8_t *data, uint8_t n_bytes) {

    uint8_t checksum = 0;

    mspSerial.write((byte *) "$M<", 3);
    mspSerial.write(n_bytes);
    checksum ^= n_bytes;
    
    mspSerial.write(cmd);
    checksum ^= cmd;

    mspSerial.write(checksum);
}

void Trigger_readData() {
    delay (20);
    
    byte count = 0;
    
    int16_t trig;

    while (mspSerial.available()) {
        count += 1;
        byte c = mspSerial.read();
        switch (count) {
            case 15:
                trig <<= 8;
                trig += c;
                trig = (trig & 0xFF00) >> 8 | (trig & 0x00FF) << 8; // Reverse the order of bytes
                break;
        }
    }

    if(String(trig).toInt()>=1500 && String(trig).toInt()<=2000 && cooldown == false)
    {
      Timer = millis();
      cooldown = true;
      Serial.println(" YUP");
      triggered = true;
    }

    if(millis()-Timer >= 500UL)
    {
      cooldown = false;
    }
    Serial.println(" Trig: " + String(trig));
}
