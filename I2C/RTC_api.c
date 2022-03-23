//Setzt die RTC Uhr auf dem Modellboard
void rtc_settime(unsigned char hh, unsigned char mm, unsigned char ss)
{
  unsigned char buffer[6] = {2,ss,mm,hh,1,1};
  i2c_xmit(RTC_ADDR,6,buffer);
}

//Liest die RTC Uhr auf dem Modellboard aus
void rtc_readtime(unsigned char *hh, unsigned char *mm, unsigned char *ss)
{
  unsigned char buffer[6] = {0,1,2,3,4,5};
  i2c_rcv(RTC_ADDR,2,5,buffer);
  *ss = buffer[1];
  *mm = buffer[2];
  *hh = buffer[3];
}

//Beispiel
/*
#include <stdio.h>
#include <string.h>
#include "I2C_api.h"
#include "RTC_api.h"
 
void main(void) 
{
  unsigned char str1[16];
  unsigned char hh,mm,ss;
  ti = 1;
  printf("I2C Demo\n");
  i2c_init();
  printf("Init OK\n");
  delay(60000);
  rtc_settime(0x11,0x22,0x33);
  printf("Set Time OK\n");
  while (1) {
  delay(60000);
  rtc_readtime(&hh,&mm,&ss);
  printf("Time: %02x:%02x:%02x\n",hh,mm,ss);
  }
}
*/