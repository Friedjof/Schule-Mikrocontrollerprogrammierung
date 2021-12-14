/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:			Friedjof Noweck
Klasse:			IH119
Datum:			2021-12-10
Datei:			main.c
Einsatz:		
Beschreibung:	can conroll the 7 segment display
Funktionen:		display numbers on 7 segment displays
******************************************************************************
Aenderungen:
2021-12-10 init project + add simple functions
2021-12-14 work on the functions

*****************************************************************************/
/******************* Text im Quelltext einbinden *********************/
#include "REG517A.h"

/*************************** Konstanten ******************************/


/*********************** globale Variablen ***************************/


/************************** Prototypen *******************************/
void set_number(char nr, char segment);
void set_numbers(int nrs);
char potenzieren(int basis, int potenz);

/************************ Hauptprogramm ******************************/

void main()
{
	TR0 = 0; // Timer0
	TF0 = 0; // Überlauf zurück setzen
	IT0 = 0;
	
	TMOD = 0x01;
	
	// Startwert
	TL0 = 0x00;
	TH0 = 0x00;
	
	ET0 = 1;
	EAL = 0;
	
	P4 = 0x00;
	P6 = 0x00;
	
	set_numbers(1234);
}

void set_numbers(int nrs)
{
	int segment = 0;
	
	for (segment = 0; segment < 4; segment++)
	{
		set_number(nrs % 10, segment);
		nrs /= 10;
	}
}

void set_number(char nr, char segment)
{
	nr = nr;
	
	P6 = P6 | potenzieren(2, segment + 4);
	
	if (nr == 0)
	{
		P4 = P4 | 0xFB;
	}
	else if(nr == 1)
	{
		P4 = P4 | 0xC2;
	}
	else if(nr == 2)
	{
		P4 = P4 | 0x5F;
	}
	else if(nr == 3)
	{
		P4 = P4 | 0xCF;
	}
	else if(nr == 4)
	{
		P4 = P4 | 0xE6;
	}
	else if(nr == 5)
	{
		P4 = P4 | 0xED;
	}
	else if(nr == 6)
	{
		P4 = P4 | 0xFD;
	}
	else if(nr == 7)
	{
		P4 = P4 | 0xEB;
	}
	else if(nr == 8)
	{
		P4 = P4 | 0xFF;
	}
	else if(nr == 9)
	{
		P4 = P4 | 0xEF;
	}
	else
	{ }
}

char potenzieren(int basis, int potenz)
{
	// init index
	int positivePotenz = potenz;
	int index = 0;
	char zwischenergebnis = 1;

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