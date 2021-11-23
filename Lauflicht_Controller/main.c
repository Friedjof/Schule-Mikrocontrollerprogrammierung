/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:			Friedjof Noweck
Klasse:			IH119
Datum:			
Datei:			
Einsatz:		
Beschreibung:	
Funktionen:		
******************************************************************************
Aenderungen:
2021-11-08 - Anlegen dieses Projektes

*****************************************************************************/
/******************* Text im Quelltext einbinden *********************/
#include "REG517A.h"

/*************************** Konstanten ******************************/


/*********************** globale Variablen ***************************/


/************************** Prototypen *******************************/
void readButton(int* direction, char* stop, int* bit_index, int* max_bit_nr, char* buttonStatus);
void LEDRunner(int* max_bit_nr, int* direction, int* bit_index, int* port);
void flipDirection(int* direction, int* bit_index, int* max_bit_nr);
void delay(int ms);
void port_controller(int port, int *port_nr);
int potenzieren(int basis, int potenz);

/************************ Hauptprogramm ******************************/

void main()
{
	const int max_bit_nr = 15;
	int direction = 1;
	int bit_index = 0;
	int port = 1;
	char stop = 0x00;
	int counter = 0;
	
	char buttonStatus = 0x00;
	
	// Set ports to zero
	P1 = 0x00;
	P4 = 0x00;
	
	while (1)
	{
		readButton(&direction, &stop, &bit_index, &max_bit_nr, &buttonStatus);
		
		if (stop == 0x00)
		{
			LEDRunner(&max_bit_nr, &direction, &bit_index, &port);
		}
		else
		{ }
		
		// Counter
		if (counter >= 10000)
		{
			counter = 0;
		}
		else
		{
			counter++;
		}
	}
}

void readButton(int* direction, char* stop, int* bit_index, int* max_bit_nr, char* buttonStatus)
{
	// Port 5 wird abgefragt
	if ((P5 & 0x01) == 0x00 && (*buttonStatus & 0x01) == 0x00)
	{
		*buttonStatus = *buttonStatus | 0x01;
		
		flipDirection(direction, bit_index, max_bit_nr);
	}
	else if ((P5 & 0x01) == 0x01 && (*buttonStatus & 0x01) == 0x01)
	{
		*buttonStatus = *buttonStatus & 0x0FF;
	}
	
	// Port 6 wird abgefragt
	if ((P6 & 0x01) == 0x00 && (*buttonStatus & 0x02) == 0x00)
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
	else if ((P6 & 0x01) == 0x01 && (*buttonStatus & 0x02) == 0x01)
	{
		*buttonStatus = *buttonStatus & 0x0FE;
	}
	delay(5);
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
	
	*bit_index = *max_bit_nr - *bit_index;
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
				index = (*bit_index - *max_bit_nr) - 1;
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


void delay(int ms)
{
 int index1 = 0;
 int index2 = 0;

	for (; index1 <= (ms * 2); index1++)
	{
		for (index2 = 0; index2 <= 1000; index2++)
		{ }
	}
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