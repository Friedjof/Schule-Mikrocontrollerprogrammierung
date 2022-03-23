/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:			Fabian Bonk
Klasse:			IH119
Datum:			02.03.2022
Datei:			
Einsatz:		
Beschreibung:	Zeit und Datum auf LCD Display Anzeigen
Funktionen:		
******************************************************************************
Aenderungen:

*****************************************************************************/
/******************* Text im Quelltext einbinden *********************/
#include "REG517A.h"
#include "I2C_api.h"
#include "lcd_api.h"
#include "RTC_api.h"
#include <string.h>
/*************************** Konstanten ******************************/
volatile unsigned char xdata i2c_reg _at_ 0xffa0;
volatile unsigned char xdata i2c_s1 _at_ 0xffa1;

unsigned char xdata xmem[0x0feff] _at_ 0;

volatile char xdata cmd _at_ 0xFF80;
volatile char xdata chr _at_ 0xFF82;

/*********************** globale Variablen ***************************/
unsigned char rtc_buf[16];
unsigned char RTC_ADDR = 0x0a0;
char Text[] = "Mo, 11.11.11,\n11:11:11:00 Uhr";

/************************** Prototypen *******************************/
void Einstellungen_LCD();
void Clear_LCD();
void Ausgabe(char value);	
void Display(char* value);
void Einstellungen_I2C();
void i2c_xmit(unsigned char slave_addr, unsigned char length,unsigned char * buffer);
unsigned char i2c_rcv(unsigned char slave_addr, unsigned char word_addr,unsigned char length, unsigned char * buffer);
void rtc_settime(unsigned char dt, unsigned char mt, unsigned char hh, unsigned char mm, unsigned char ss);
void rtc_readtime(unsigned char *dt, unsigned char *mt, unsigned char *hh, unsigned char *mm, unsigned char *ss, unsigned char *ms);
void Wochentag (unsigned char t);
/************************ Hauptprogramm ******************************/

void main()
{
  unsigned char ss = 0x00;
  unsigned char mm = 0x00;
  unsigned char hh = 0x00;
	unsigned char dt = 0x00;
	unsigned char mt = 0x00;
	unsigned char ms = 0x00;
	unsigned char yy = 0x00;
	
	int index;
	
	Clear_LCD();
	Einstellungen_LCD();
	Einstellungen_I2C();
	
	// 2022-12-31 23:59:55,00
	rtc_settime( 0x31, 0x12, 0x23, 0x59, 0x55);
	
	// 2023-12-31 23:59:55,00
	rtc_settime( 0xF1, 0x12, 0x23, 0x59, 0x55);

	while(1)
	{
		
    rtc_readtime(&dt, &mt, &hh, &mm, &ss, &ms);
		
		//Millisekunden
		Text[24] = (ms & 0x0F)+ 48;
		Text[23] = ((ms & 0xF0) >> 0x04) + 48;
		
		//Sekunden
		Text[21] = (ss & 0x0F) + 48;
		Text[20] = ((ss & 0xF0) >> 0x04) + 48;
		
		//Minuten
		Text[18] = (mm & 0x0F) + 48;
		Text[17] = ((mm & 0xF0) >> 0x04) + 48;
		
		//Stunden
		Text[15] = (hh & 0x0F) + 48;
		Text[14] = ((hh & 0xF0) >> 0x04) + 48;
		
		//Tag
		Text[5] = (dt & 0x0F) + 48;
		Text[4] = ((dt & 0x30) >> 0x04) + 48;
		
		//Jahr
		// Dies ist noch zu testen
		yy = ((dt & 0xD0) >> 0x06) + 20;
		for (index = 0x00; index < 2; index++)
		{
			Text[11 - index] = (yy % 10) + 0x30;
			yy /= 10;
		}
		
		//Text[10] = ((dt & 0xD0) >> 0x06) + 0x30;
		
		//Monat
		Text[8] = (mt & 0x0F) + 48;
		Text[7] = ((mt & 0x10) >> 0x04) + 48; 

    //Wochentag
		Wochentag((mt & 0xE0) >> 0x05);
		
		Display(Text);
	}
}
void Wochentag (unsigned char t)
{
	switch(t)
	{
		case 0x00: 
			      Text[0] = 'M';
		        Text[1] = 'o';
			break; 
		case 0x01: 
			      Text[0] = 'D';
		        Text[1] = 'i';
			break;
		case 0x02: 
			     Text[0] = 'M';
		       Text[1] = 'i';
			break;
		case 0x03: 
			     Text[0] = 'D';
		       Text[1] = 'o';
			break;
		case 0x04: 
			     Text[0] = 'F';
		       Text[1] = 'r';
			break;
		case 0x05: 
			     Text[0] = 'S';
		       Text[1] = 'a';
			break;
		case 0x06:
			    Text[0] = 'S';
		      Text[1] = 'o';
			break;
		default:
			break;
	}
}
void Einstellungen_LCD()
{
	int index1;

	//Funktion definieren: 2  zeilig, 5x7 Dots, 8 Bit
	cmd = 0x38; 
	for(index1=0;index1<7;index1++);//ca. 50ys warten (mind. 39)
	
	//Display An, Cursor Aus, Blinken aus
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
	  cmd = 0x02;
		cmd = 0x01;
	 for(index1=0;index1<470;index1++){}//ca. 2ms warten (mind. 1.57)
}
void Ausgabe(char value)
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
	for(index1=0;index1<10;index1++);//ca. 50ys warten
	}
void Display(char* value)
{
	int laenge = strlen(value); //Textlänge wird bestimmt
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
	Ausgabe(value[index]);

	index3++;
	}//Zweite Zeile
	else if(index3==16)
	{
	Ausgabe('\n');
	Ausgabe(value[index]);
	index3++;
	}
	else if(index3 >= 32)
	{
	Clear_LCD();
	index3 = -1;
	}
	}
	for(index=0;index<370;index++){}
}
void Einstellungen_I2C()
{
  i2c_s1 = 0x80;  // Select internal register S0
  i2c_reg = 0x55; // S0 = 0x55 (set effective own address to AAh)
  i2c_s1 = 0x0a0; // Select internal register S2
  i2c_reg = 0x1c; // System Clock is 12 MHz; SCL = 90 kHz
  i2c_s1 = 0x0c1; // Enable interface and select S0
}

void i2c_xmit(unsigned char slave_addr, unsigned char length,unsigned char * buffer)
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

unsigned char i2c_rcv(unsigned char slave_addr, unsigned char word_addr,unsigned char length, unsigned char * buffer)
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

void rtc_settime(unsigned char dt, unsigned char mt, unsigned char hh, unsigned char mm, unsigned char ss)
{
  unsigned char buffer[6];
	
  buffer[0] = 2;
  buffer[1] = ss;
  buffer[2] = mm;
  buffer[3] = hh;
  buffer[4] = dt;
  buffer[5] = mt;
	
  i2c_xmit(RTC_ADDR,6,buffer);
}

void rtc_readtime(unsigned char *dt, unsigned char *mt, unsigned char *hh, unsigned char *mm, unsigned char *ss, unsigned char *ms)
{
  unsigned char buffer[6] = {0,1,2,3,4,5};

  i2c_rcv(RTC_ADDR,1,6,buffer);

  *ms = buffer[1];
  *ss = buffer[2];
  *mm = buffer[3];
  *hh = buffer[4];
	*dt = buffer[5];
	*mt = buffer[6];
}