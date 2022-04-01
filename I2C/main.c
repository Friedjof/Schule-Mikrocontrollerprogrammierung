/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:			Friedjof Noweck
Klasse:			IH119
Datum:			2022-04-01 Fr
Datei:			main.c
Einsatz:		Schulungskontroller REG517A
Beschreibung:		Zeigt Zeit an im Format WT, TT.MM.JJJJ - hh:mm:ss.ms
Funktionen:		Kann über den I2C Bus die Uhr setzen und abfragen
******************************************************************************
Aenderungen:
2022-03-04	APIs einbinden
2022-03-09	mm:hh:ss hinzugefüht
2022-03-18	Hinzufügen der TT.MM
2022-03-23	Hinzufügen des Jahres
2022-03-25	Lösung des Wochentagsproblem
2022-03-26	Vierstellige Jahreszahlen
2022-03-27	Hinzufügen von Test Daten und das Durchführen von Tests
*****************************************************************************/
/******************* Text im Quelltext einbinden *********************/
#include "REG517A.h"
#include "I2C_api.h"
#include "lcd_api.h"
#include "RTC_api.h"
/*************************** Konstanten ******************************/
volatile unsigned char xdata i2c_reg _at_ 0xffa0;
volatile unsigned char xdata i2c_s1 _at_ 0xffa1;

unsigned char xdata xmem[0x0feff] _at_ 0;

volatile char xdata cmd _at_ 0xFF80;
volatile char xdata chr _at_ 0xFF82;

/*********************** globale Variablen ***************************/
unsigned char rtc_buf[16];
unsigned char RTC_ADDR = 0x0a0;
char Text[] = "XX, XX.XX.XXXX\nXX:XX:XX.XX";

/************************** Prototypen *******************************/
void init_I2C();
void init_LCD();
void Clear_LCD();
void showChar(char value);	
void showText(char* value);
void i2c_xmit(unsigned char slave_addr, unsigned char length,unsigned char * buffer);
unsigned char i2c_rcv(unsigned char slave_addr, unsigned char word_addr,unsigned char length, unsigned char * buffer);
void rtc_settime(unsigned char dt, unsigned char mt, unsigned char hh, unsigned char mm, unsigned char ss);
void rtc_readtime(unsigned char *dt, unsigned char *mt, unsigned char *hh, unsigned char *mm, unsigned char *ss, unsigned char *ms);
void Wochentag (unsigned char t);
int stringLength(char* given_string);
/************************ Hauptprogramm ******************************/

/* 
 * 000X XXXX => 0x00 = Mo
 * 001X XXXX => 0x20 = Di
 * 010X XXXX => 0x40 = Mi
 * 011X XXXX => 0x60 = Do
 * 100X XXXX => 0x80 = Fr
 * 101X XXXX => 0xA0 = Sa
 * 110X XXXX => 0xC0 = So
 * 111X XXXX => 0x00 = XX
 */

void main()
{
	unsigned char ss = 0x00;
	unsigned char mm = 0x00;
	unsigned char hh = 0x00;
	unsigned char dt = 0x00;
	unsigned char mt = 0x00;
	unsigned char ms = 0x00;
	unsigned char lastYY = 0x00;
	unsigned char currentYY = 0x00;
	unsigned char jahreSeitStart = 0x00;
	
	int yy;
	int aktuellesYYYY;
	
	unsigned char index;
	
	Clear_LCD();
	init_LCD();
	init_I2C();
	
	// [Schaltjahr - 28. bis 29. Februar]
	// Fr 2020-02-28 23:59:55,00
	aktuellesYYYY = 2020;
	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x28, 0x82, 0x23, 0x59, 0x55);
	
//	// [Schaltjahr - 29. Februar bis 01. März]
//	// Sa 2020-02-29 23:59:55,00
//	aktuellesYYYY = 2020;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x29, 0xA2, 0x23, 0x59, 0x55);

//  // [Kein Schaltjahr - 28. Februar bis 01. März]
//	// Mo 2022-02-28 23:59:55,00
//	aktuellesYYYY = 2022;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x28, 0x02, 0x23, 0x59, 0x55);

//  // [Heute]
//	// Fr 2022-04-01 11:59:55,00
//	aktuellesYYYY = 2022;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x01, 0x84, 0x11, 0x59, 0x55);

//	// [Jahreswechsel 2022 auf 2023]
//	// Sa 2022-12-31 23:59:55,00
//	aktuellesYYYY = 2022;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x31, 0xB2, 0x23, 0x59, 0x55);

//	// [Jahreswechsel 2023 auf 2024]
//	// So 2023-12-31 23:59:55,00
//	aktuellesYYYY = 2023;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x31, 0xD2, 0x23, 0x59, 0x55);
	
//	// [Jahreswechsel 2099 auf 2100]
//	// Do 2023-12-31 23:59:55,00
//	aktuellesYYYY = 2099;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x31, 0x72, 0x23, 0x59, 0x55);
	
//	// [Jahreswechsel 9999 auf 0000]
//	// Mi 2023-12-31 23:59:55,00
//	aktuellesYYYY = 9999;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x31, 0x52, 0x23, 0x59, 0x55);
	
//	// [Jahreswechsel 999 auf 1000]
//	// Sa 999-12-31 23:59:55,00
//	aktuellesYYYY = 999;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x31, 0xB2, 0x23, 0x59, 0x55);
	
//	// [Jahreswechsel 1 auf 2]
//	// Sa 1-12-31 23:59:55,00
//	aktuellesYYYY = 1;
//	rtc_settime(((aktuellesYYYY % 0x04) << 0x06) | 0x31, 0xB2, 0x23, 0x59, 0x55);
	
	
	// Aktuelles Jahr der Uhr
	lastYY = (aktuellesYYYY % 0x04);

	while(1)
	{
    rtc_readtime(&dt, &mt, &hh, &mm, &ss, &ms);
		
		// Millisekunden
		Text[25] = (ms & 0x0F)+ 48;
		Text[24] = (ms >> 0x04) + 48;
		
		// Sekunden
		Text[22] = (ss & 0x0F) + 48;
		Text[21] = (ss >> 0x04) + 48;
		
		// Minuten
		Text[19] = (mm & 0x0F) + 48;
		Text[18] = (mm >> 0x04) + 48;
		
		// Stunden
		Text[16] = (hh & 0x0F) + 48;
		Text[15] = (hh >> 0x04) + 48;
		
		// Tage
		Text[5] = (dt & 0x0F) + 48;
		Text[4] = ((dt & 0x30) >> 0x04) + 48;

		// Wochentage
		Wochentag(mt >> 0x05);
		
		// Monate
		Text[8] = (mt & 0x0F) + 48;
		Text[7] = ((mt & 0x10) >> 0x04) + 48;
		
		// Jahre
		currentYY = (dt >> 0x06);
		
		if (currentYY ^ lastYY)
		{
			lastYY = currentYY;
			jahreSeitStart++;
		}
		
		yy = jahreSeitStart + aktuellesYYYY;
		for (index = 0x00; index < 4; index++)
		{
			Text[13 - index] = (yy % 10) + 0x30;
			yy /= 10;
		}
		
		showText(Text);
	}
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
	unsigned char buffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};

	i2c_rcv(RTC_ADDR,1,8,buffer);

	*ms = buffer[1];
	*ss = buffer[2];
	*mm = buffer[3];
	*hh = buffer[4];
	*dt = buffer[5];
	*mt = buffer[6];
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
void init_LCD()
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
void showChar(char value)
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
void showText(char* value)
{
	int laenge = stringLength(value); //Textlänge wird bestimmt
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
	showChar(value[index]);

	index3++;
	}//Zweite Zeile
	else if(index3==16)
	{
	showChar('\n');
	showChar(value[index]);
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
void init_I2C()
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
// Source => https://www.geeksforgeeks.org/length-string-using-pointers/
int stringLength(char* given_string)
{
    // variable to store the
    // length of the string
    int length = 0;

    while (*given_string != '\0') {
        length++;
        given_string++;
    }
  
    return length;
}