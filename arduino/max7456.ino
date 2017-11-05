#include <SPI.h>

#include "Arduino.h"
#include "max7456_registers.h"

int pin_ss;
int pin_vsync;
int video_mode;
int num_lines;

void max7456_init(int ss, int vsync, int vmode)
{
  pin_ss = ss;
  pin_vsync = vsync;
  video_mode = vmode;
  
  if(video_mode == MODE_NTSC)
  {
    num_lines = 13;
  }
  else if(video_mode == MODE_PAL)
  {
    num_lines = 16;
  }

  // initialize SPI hardware
  pinMode(pin_ss, OUTPUT);
  digitalWrite(pin_ss, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

  delay(50);
  
  // soft reset max7456
  max7456_write(REG_VM0_W, 0b00000010);
  delayMicroseconds(100);
  while(get_bit(max7456_read(REG_VM0_R), 1) != 0);
  
  // set video settings
  byte video_settings = 0b00001000;
  if(video_mode == MODE_PAL) set_bit(video_settings, 6);
  max7456_write(REG_VM0_W, video_settings);

  // enable automatic black level control
  byte black_level = max7456_read(REG_OSDBL_R);
  black_level = unset_bit(black_level, 4);
  max7456_write(REG_OSDBL_W, black_level);
}

void max7456_clear()
{
  max7456_write(REG_DMM_W, 0b100);
  
  for(int i = 0; i < num_lines * 30; i++)
  {
    max7456_write(REG_DMAH_W, i >> 8);
    max7456_write(REG_DMAL_W, i & 0xFF);
    max7456_write(REG_DMDI_W, 0x20);
  }
}

void max7456_draw_string(int x, int y, String str)
{
  for(int i = 0; i < str.length() && x < 30; i++)
  {
    int index = x + y * 30;
  
    max7456_write(REG_DMAH_W, index >> 8);
    max7456_write(REG_DMAL_W, index & 0xFF);
    max7456_write(REG_DMDI_W, str.charAt(i));
    
    x++;
  }
}

void max7456_write(byte reg, byte data)
{
  digitalWrite(pin_ss, LOW);

  SPI.transfer(reg);
  SPI.transfer(data);

  digitalWrite(pin_ss, HIGH);
}

byte max7456_read(byte reg)
{
  digitalWrite(pin_ss, LOW);

  SPI.transfer(reg);
  byte data = SPI.transfer(0xFF);

  digitalWrite(pin_ss, HIGH);

  return data;
}

