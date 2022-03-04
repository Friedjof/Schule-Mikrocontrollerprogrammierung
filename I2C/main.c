/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:		   	Friedjof Noweck
Klasse:		  	IH119
Datum:	   		2022-02-19 Sa
Datei:			  main.c
Einsatz:		
Beschreibung:	
Funktionen:		
******************************************************************************
Aenderungen:
2022-02-19 Init project
2022-03-03 Add libraries
*****************************************************************************/
/******************* Text im Quelltext einbinden *********************/
#include "REG517A.h"
#include "lcd_api.h"
// #include "I2C_api.h"
#include <string.h>

/*************************** Konstanten ******************************/
volatile unsigned char xdata xmem[0x0feff] _at_ 0;
unsigned char rtc_buf[16];

#define RTC_ADDR 0x0a0

/*********************** globale Variablen ***************************/

char i2c_s1;  // Select internal register S0
char i2c_reg; // S0 = 0x55 (set effective own address to AAh)
char i2c_s1;  // Select internal register S2
char i2c_reg; // System Clock is 12 MHz; SCL = 90 kHz
char i2c_s1;  // Enable interface and select S0

int counter = 0;
int richtung = 0;
int zaehler = 0;

char incl = 0x00;

/************************** Prototypen *******************************/
#ifndef _RTC_I2C
	#define _RTC_I2C
	
	void i2c_init();
	void i2c_xmit(unsigned char slave_addr, unsigned char length, unsigned char * buffer);
	unsigned char i2c_rcv(unsigned char slave_addr, unsigned char word_addr, unsigned char length, unsigned char * buffer);
	void rtc_settime(unsigned char hh, unsigned char mm, unsigned char ss);
	void rtc_readtime(unsigned char *hh, unsigned char *mm, unsigned char *ss);
#endif

/************************ Hauptprogramm ******************************/

void main()
{
	char *string01 = "Hello World";
	
	// init i2c bus
	i2c_init();
	
	// init RTC
	// rtc_settime(0x00, 0x00, 0x00);
	
	// Init LCD
	Init_LCD();
	
	print_str_lcd(string01);
	
	cmd = 0x50;
	
	while (1)
	{
		
	}
}

//initialisiert den I2C Bus
void i2c_init()
{
  i2c_s1 = 0x80;  // Select internal register S0
  i2c_reg = 0x55; // S0 = 0x55 (set effective own address to AAh)
  i2c_s1 = 0x0a0; // Select internal register S2
  i2c_reg = 0x1c; // System Clock is 12 MHz; SCL = 90 kHz
  i2c_s1 = 0x0c1; // Enable interface and select S0
}


void i2c_xmit(unsigned char slave_addr, unsigned char length, unsigned char * buffer)
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


unsigned char i2c_rcv(unsigned char slave_addr, unsigned char word_addr, unsigned char length, unsigned char * buffer)
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

void Timer0_ISR() interrupt 1
{
	counter--;
	if(counter<=0)
	{
		if(richtung == 0)
		{
			if(zaehler < 10000)
			{
				zaehler++;
			}
			else
			{
				zaehler = 9999;
				richtung = 1;
			};
			counter = 1;
	  }
		else if(richtung == 1)
		{
			if(zaehler > 0)
			{
				zaehler--;
			}
			else
			{
				zaehler = 0;
				richtung = 0;
			};
			counter = 1;			
		}
		else
		{
		}
	}
	else
	{
		return;
	}
}


void Init_LCD()
{
	int index1;

	//Funktion definieren: 2  zeilig, 5x7 Dots
	cmd = 0x38;
	for(index1=0;index1<7;index1++);//ca. 50ys warten (mind. 39)
	//Display ON,Cursor OFF, Blink OFF
	cmd = 0x0C;
	for(index1=0;index1<7;index1++);//ca. 50ys warten
	
	Clear_LCD();
	
	//Entry Mode: autom Increment ohne Shift
	cmd = 0x06;
	for(index1=0;index1<7;index1++);//ca. 50ys warten
}

void Clear_LCD()
{
	int index1;
	cmd = 0x01;
	for(index1=0;index1<370;index1++);//ca. 2ms warten (mind. 1.57)
}


void write_char(char value)
{
	int index1;
  if(value == 0)
	{
		return;
	}
	if(value != '\n')
	{
		chr = value;
	}
	else
	{
		cmd = 0xC0; //->  Adresse auf die zweite Zeile setzen: 1 ( 1 0 0   0 0 0 0) 
			    //-> DRAM Adresse 0x40!
	}
	for(index1=0;index1<7;index1++);//ca. 50ys warten
}

void print_str_lcd(char* value)
{
	int laenge = strlen(value);
	int index = 0;
	int index2 = 0;
	int index3 = 0;
	Clear_LCD();

	for(index = 0;index < laenge;index++)
	{
		//Erste Zeile
		if(index3 < 16 || (index3 > 16 && index3 < 32))
		{
				if(value[index] == '\n' && index3 < 16)
				{
					index3 = 16;
				}
				else if(value[index] == '\n' && index3 > 16)
				{
					index3 = 32;
				}
				write_char(value[index]);



				index3++;
		}//Zweite Zeile
		else if(index3==16)
		{
				write_char('\n');
				write_char(value[index]);			
				index3++;
		}
		else if(index3 >= 32)
		{
		 Clear_LCD();
		 index3 = -1;
		}
	}
}