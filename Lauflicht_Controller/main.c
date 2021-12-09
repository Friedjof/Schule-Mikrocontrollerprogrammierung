/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:			Friedjof Noweck
Klasse:			IH119
Datum:			2021-11-08
Datei:			main.c
Einsatz:		Auf dem REG517A Microcontroller
Beschreibung:	Lauflicht zweier zusammengeschalteter LED Leisten.
Funktionen:		l�sst dieses Lauflicht per Taster kontrollieren.
******************************************************************************
Aenderungen:
2021-11-08 - Anlegen dieses Projektes
2021-11-23 - Hinzuf�gen der Funktionalit�ten "umdrehen" und "stoppen"
2021-12-02 - Add timer0 and Interrupts
2021-12-08 - Interrupt modification
2021-12-09 - Conversion of the delay function

*****************************************************************************/
/******************* Text im Quelltext einbinden *********************/
#include "REG517A.h"

/*************************** Konstanten ******************************/


/*********************** globale Variablen ***************************/
int timer0_default = 10;
int timer0_counter = 10;

/************************** Prototypen *******************************/
void readButton(int* direction, char* stop, int* bit_index, int* max_bit_nr, char* buttonStatus);
void LEDRunner(int* max_bit_nr, int* direction, int* bit_index, int* port);
void flipDirection(int* direction, int* bit_index, int* max_bit_nr);
void delay(int ms, int* direction, char* stop, int* bit_index, int* max_bit_nr, char* buttonStatus);
void port_controller(int port, int *port_nr);
int potenzieren(int basis, int potenz);

// Interrupt functions
void IRQ_Timer0();

/************************ Hauptprogramm ******************************/

void main()
{
	// program vars
	const int max_bit_nr = 15;
	int direction = 1;
	int bit_index = 0;
	int port = 1;
	char stop = 0x00;
	
	char buttonStatus = 0x00;
	
	// Timer 0 Konfigurieren
	TR0 = 0; // aushalt
	TF0 = 0; // �berlauf zur�cksetzen
	IT0 = 0; // IR gel�scht

	TMOD = 0x01; // Timer 0 16Bit
	TL0  = 0xAF; // 
	TH0  = 0x3C; // = 0x3CAF
	
	// IR System Configuration
	ET0 = 1; // IR for Timer 0
	EAL = 0; // All IRs off
	
	// Set ports to zero
	P1 = 0x00;
	P4 = 0x00;
	
	EAL = 1;
	while (1)
	{
		delay(500, &direction, &stop, &bit_index, &max_bit_nr, &buttonStatus);
		
		if (stop == 0x00)
		{
			LEDRunner(&max_bit_nr, &direction, &bit_index, &port);
		}
		else
		{ }
	}
}
void readButton(int* direction, char* stop, int* bit_index, int* max_bit_nr, char* buttonStatus)
{
	char button2 = *buttonStatus & 0x02;
	// Port 5 wird abgefragt
	if ((P5 & 0x01) == 0x00 && (*buttonStatus & 0x01) == 0x00)
	{
		*buttonStatus = *buttonStatus | 0x01;
		
		flipDirection(direction, bit_index, max_bit_nr);
	}
	else if ((P5 & 0x01) == 0x01 && (*buttonStatus & 0x01) == 0x01)
	{
		*buttonStatus = *buttonStatus & 0x0FE;
	}
	// Port 6 wird abgefragt
	if ((P6 & 0x01) == 0x00 &&  button2 == 0x00)
	{
		*buttonStatus = *buttonStatus | 0x02;
		
		if (*stop == 1)
		{
			*stop = 0x00;
		}
		else
		{
			*stop = 0x01;
		}
	}
	else if ((P6 & 0x01) == 0x01 && button2 == 0x02)
	{
		*buttonStatus = *buttonStatus & 0x0FD;
	}
}
void flipDirection(int* direction, int* bit_index, int* max_bit_nr)
{
	if (*direction == 0x01)
	{
		*direction = 0x00;
	}
	else
	{
		*direction = 0x01;
	}
	
	*bit_index = (*max_bit_nr - *bit_index) + 1;
}

void LEDRunner(int* max_bit_nr, int* direction, int* bit_index, int* port)
{
	int index = 0;
	
	if (*bit_index <= *max_bit_nr)
	{
		if (*direction == 1)
		{
			if (*bit_index <= 7)
			{
				*port = 1;
				index = *bit_index;
			}
			else
			{
				*port = 4;
				index = 0x07 - (*max_bit_nr - *bit_index);
			}
		}
		else
		{
			if (*bit_index <= 7)
			{
				*port = 4;
				index = 0x07 - *bit_index;
			}
			else
			{
				*port = 1;
				index = *bit_index - *max_bit_nr;
			}
		}
		// switch LED's
		port_controller(index, port);
		
		// add one to the Bit Index
		*bit_index += 1;
	}
	else
	{
		// reset Bit Index
		*bit_index = 1;
		
		if (*direction == 1)
		{
			*direction = 0x00;
		}
		else
		{
			*direction = 0x01;
		}
	}
}
void port_controller(int port, int *port_nr)
{
  char portOn = 0x00;

	// turn all bits off
	P1 = P1 & 0x00;
	P4 = P4 & 0x00;

	if (*port_nr == 1)
	{
		// turn bit on
		portOn = potenzieren(2, port);
		P1 = P1 | portOn;
	}
	else if (*port_nr == 4)
	{
		// turn bit on
		portOn = potenzieren(2, port);
		P4 = P4 | portOn;
	}
	else
	{ }
}
int potenzieren(int basis, int potenz)
{
	// init index
	int positivePotenz = potenz;
	int index = 0;
	int zwischenergebnis = 1;

	if (potenz < 0) {
		positivePotenz = potenz * (-1);
	}
	
	// die Potenz in einer for-Schleife berechnen
	for (index = 0; index < positivePotenz; index = index + 1)
	{
		// das Zwischenergebnis wird in der Variablen "zwischenergebnis" gespeichert
		zwischenergebnis = zwischenergebnis * basis;
	}

	return zwischenergebnis;
}

void delay(int ms, int* direction, char* stop, int* bit_index, int* max_bit_nr, char* buttonStatus)
{
	TL0 = 0xAF; // 
	TH0 = 0x3C; // = 0x3CAF
	
	ET0 = 1;
	TR0 = 1;
	
	timer0_counter = ms / 50;
	
	while (timer0_counter > 0)
	{
		readButton(direction, stop, bit_index, max_bit_nr, buttonStatus);
	}
}
void IRQ_Timer0() interrupt 1
{
	TR0 = 0; // timer0 ausschalten
	TF0 = 0; // �berlauf zur�cksetzen
	
	TL0 = 0xAF; // 
	TH0 = 0x3C; // = 0x3CAF
	
	if (timer0_counter >= 1)
	{
		timer0_counter--;
	
		TR0 = 1; // timer0 einschalten
	}
	else
	{ }
}







