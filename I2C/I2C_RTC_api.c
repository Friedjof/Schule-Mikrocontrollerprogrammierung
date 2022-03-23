volatile unsigned char xdata i2c_reg _at_ 0xffa0;
volatile unsigned char xdata i2c_s1 _at_ 0xffa1 ;
#define RTC_ADDR 0x0a0

volatile unsigned char xdata xmem[0x0feff] _at_ 0;
unsigned char rtc_buf[16];

//initialisiert den I2C Bus
void i2c_init()
{
  i2c_s1 = 0x80;  // Select internal register S0
  i2c_reg = 0x55; // S0 = 0x55 (set effective own address to AAh)
  i2c_s1 = 0x0a0; // Select internal register S2
  i2c_reg = 0x1c; // System Clock is 12 MHz; SCL = 90 kHz
  i2c_s1 = 0x0c1; // Enable interface and select S0
}

void i2c_xmit(unsigned char slave_addr, unsigned char length,
              unsigned char * buffer)
{
  unsigned char i;
  //Waiting for Bus free
  while ((i2c_s1 & 1) ==0) {};
  i2c_s1 = 0x0c5;         // START condition

  i2c_reg = slave_addr;   // Set slave address

  for (i=0; i<length; i++)
  {
    //wait for Client
    while ((i2c_s1 & 0x80) !=0);        // poll for ready
    if ((i2c_s1 & 0x08) !=0) 
		break;     						// if ack, leave loop
		//send Byte
    i2c_reg = buffer[i];                // send data
  }
  while ((i2c_s1 & 0x80) !=0);          // poll for ready
 i2c_s1 = 0x0c3; // sTOP
}

unsigned char i2c_rcv(unsigned char slave_addr, unsigned char word_addr,
                      unsigned char length, unsigned char * buffer)
{
  unsigned char i;
  unsigned char error = 0;
  while ((i2c_s1 & 1) ==0) {}; // wait for free bus

  i2c_s1 = 0x0c5;  //START Condition
  i2c_reg = slave_addr;
  //wait for Slave
    while ((i2c_s1 & 0x80) !=0);

    if ((i2c_s1 & 0x08) !=0) 
	{ // no slave ACK
      i2c_s1 = 0x0c3; // sTOP
      return 1;
    }
	else
	{}
    //Registeradresse
    i2c_reg = word_addr;

	//Wait for Slave
    while ((i2c_s1 & 0x80) !=0);

    if ((i2c_s1 & 0x08) !=0) 
	{ // no slave ACK
      i2c_s1 = 0x0c3; // sTOP
      return 1;
    }
	else
	{}

  i2c_s1 = 0x0c5;  // START condition
 
  i2c_reg = slave_addr+1; // Set slave address
  for (i=0; i<length; i++)
  {
    //wait for Slave
    while ((i2c_s1 & 0x80) !=0);  // poll for ready

    if ((i2c_s1 & 0x08) !=0) { // no slave ACK
      error = 1;
      break;
    }
	else
	{}

	//Byte lesen
  	buffer[i] = i2c_reg;
  }
  if (error == 0)
  {
    i2c_s1 = 0x040; // prepare NACK
    buffer[i] = i2c_reg; // read final byte
    while ((i2c_s1 & 0x80) !=0);
  }

  i2c_s1 = 0x0c3; // sTOP

  return error;
}



void rtc_settime(unsigned char hh, unsigned char mm, unsigned char ss)
{
  unsigned char buffer[6];
  buffer[0] = 2;
  buffer[1] = ss;
  buffer[2] = mm;
  buffer[3] = hh;
  buffer[4] = 1;
  buffer[5] = 1;
  i2c_xmit(RTC_ADDR,6,buffer);
}

void rtc_readtime(unsigned char *hh, unsigned char *mm, unsigned char *ss)
{
  unsigned char buffer[6] = {0,1,2,3,4,5};

  i2c_rcv(RTC_ADDR,2,6,buffer);

  *ss = buffer[1];// & 0x3F;
  *mm = buffer[2];
  *hh = buffer[3];
}
