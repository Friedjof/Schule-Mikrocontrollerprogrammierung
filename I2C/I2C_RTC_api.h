#ifndef _RTC_I2C
#define _RTC_I2C

void i2c_init();
void i2c_xmit(unsigned char slave_addr, unsigned char length,
              unsigned char * buffer);
unsigned char i2c_rcv(unsigned char slave_addr, unsigned char word_addr,
                      unsigned char length, unsigned char * buffer);
void rtc_settime(unsigned char hh, unsigned char mm, unsigned char ss);
void rtc_readtime(unsigned char *hh, unsigned char *mm, unsigned char *ss);

#endif