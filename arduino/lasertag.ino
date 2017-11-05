#include <IRremote.h>
#include "max7456.h"

// settings
const int HIT_MSG_TIME = 2000;
const int HIT_DAMAGE = 10;
const int HIT_COOLDOWN = 500;
const int UI_UPDATE_RATE = 1000;
const int IR_CODE = 0xFAA;    // unique code of this quad

// stats
int health = 100;
int hit_state = 0;  // 0: not hit, 1: hit, 2: displayed message
unsigned long hit_time = 0;
int fire_state = 0; // 0: did not fire, 1: fire, 2: displayed message

// ui
bool update_ui = true;
unsigned long last_ui_update = 0;

// irrecv
IRrecv irrecv(A3);

// irsend
unsigned long last_fire = 0;

// trigger
bool triggered = false;

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(A3, INPUT);
  pinMode(A1, OUTPUT);
  irrecv.enableIRIn();
  //Serial.begin(9600);
  
  max7456_init(6, 2, MODE_NTSC);
}

void loop()
{
  //trigger_task();
  ui_task();
  irrecv_task();
  irsend_task();
}

// updates the ui
void ui_task()
{
  if(update_ui && millis() >= last_ui_update + UI_UPDATE_RATE)
  {
    max7456_clear();
    
    // draw crosshair
    max7456_draw_string(14, 6, String((char)0x01));
    max7456_draw_string(15, 6, String((char)0x02));

    // draw health
    max7456_draw_string(18, 12, "Health:    ");
    max7456_draw_string(26, 12, String(health));

    if(hit_state == 1)
    {
      max7456_draw_string(13, 1, "HIT!");
      hit_state = 2;
      update_ui = true;
    }
    else if(hit_state == 2)
    {
      max7456_draw_string(13, 1, "HIT!");
      if(millis() >= (hit_time + HIT_MSG_TIME))
      {
        hit_state = 0;
      }
      
      update_ui = true;
    }
    else if(fire_state == 1)
    {
      max7456_draw_string(1, 12, "FIRE");
      fire_state = 2;
      update_ui = true;
    }
    else if(fire_state == 2)
    {
      max7456_draw_string(1, 12, "FIRE");
      if(millis() >= (fire_state + 2000))
      {
        fire_state = 0;
      }
      
      update_ui = true;
    }
    else
    {
      update_ui = false;
    }

    last_ui_update = millis();
  }
}

// detect a hit
void irrecv_task()
{
    decode_results results;
    if(irrecv.decode(&results) && millis() >= hit_time + HIT_COOLDOWN)
    { 
      digitalWrite(13, HIGH);
        delay(20);
        digitalWrite(13, LOW);
        
      if(results.value == 0xFAA)
      {
        
        // we've been hit!
        health -= HIT_DAMAGE;
        if(health < 0) health = 0;
        hit_state = 1;
        hit_time = millis();
        update_ui = true;
      }

      irrecv.resume();
    }
    
}

// fire
void irsend_task()
{
  if(triggered == true && millis() >= last_fire + 500)
  {
    //send the leading pulse
    ir_carrier(2400);
    delayMicroseconds(400);
  
    //send the 12 bit code
    for (int i = 11; i >= 0; i--)
    {
      if(get_bit(IR_CODE, i) == 1)
      {
        ir_carrier(1200);
      }
      else
      {
        ir_carrier(600);
      }
      delayMicroseconds(400);
    }

    last_fire = millis();

    fire_state = 1;
    triggered = false;
  }
}

void ir_carrier(unsigned int us)
{
  for(int i=0; i < (us / 26); i++)
    {
    digitalWrite(A1, HIGH);   //turn on the IR LED
    //NOTE: digitalWrite takes about 3.5us to execute, so we need to factor that into the timing.
    delayMicroseconds(9);          //delay for 13us (9us + digitalWrite), half the carrier frequnecy
    digitalWrite(A1, LOW);    //turn off the IR LED
    delayMicroseconds(9);          //delay for 13us (9us + digitalWrite), half the carrier frequnecy
    }
}

void trigger_task()
{
    uint8_t  data=0;
    trigger_sendMSP(105,&data, 0);
    trigger_readData();
}

void trigger_sendMSP(uint8_t cmd,uint8_t *data, uint8_t n_bytes) {

    uint8_t checksum = 0;

    Serial.write((byte *) "$M<", 3);
    Serial.write(n_bytes);
    checksum ^= n_bytes;
    
    Serial.write(cmd);
    checksum ^= cmd;

    Serial.write(checksum);
}

void trigger_readData() {
    delay (20);
    
    byte count = 0;
    
    int16_t trig;

    while (Serial.available()) {
        count += 1;
        byte c = Serial.read();
        switch (count) {
            case 15:
                trig <<= 8;
                trig += c;
                trig = (trig & 0xFF00) >> 8 | (trig & 0x00FF) << 8; // Reverse the order of bytes
                break;
        }
    }

    if(String(trig).toInt()>=1500 && String(trig).toInt()<=2000)
    {
      triggered = true;
    }
}
