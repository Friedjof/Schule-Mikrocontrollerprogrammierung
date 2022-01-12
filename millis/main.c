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

*****************************************************************************/
/******************* Text im Quelltext einbinden *********************/
#include "REG517A.h"

/*************************** Konstanten ******************************/


/*********************** globale Variablen ***************************/
unsigned long int millis = 0;

/************************** Prototypen *******************************/


/************************ Hauptprogramm ******************************/

/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:			Meyer
Klasse:			IH119
Datum:			1.12.2021		
Datei:			main.c
Einsatz:		Testprogr für Timer0	
Beschreibung:	
Funktionen:		
******************************************************************************
Aenderungen:

*****************************************************************************/
/******************* Text im Quelltext einbinden *********************/
#include "REG517A.h"

/*************************** Konstanten ******************************/


/*********************** globale Variablen ***************************/
// ms since startup
unsigned long int milliCounter = 0;

/************************** Prototypen *******************************/
void IRQ_Timer0();
unsigned long int milli();

/************************ Hauptprogramm ******************************/

void main()
{
	//Timer0 konfigurieren
	TR0 = 0; //ausgeschaltet
	TF0 = 0; //Überlauf zurückgesetzt
	IT0 = 0; //IR gelöscht
	TMOD = 0x01;//Timer1: Timer, 8bit prescale, Timer0: Timer, 16bit
	
	//Startwert 15535 -> 0x3CAF
	//Startwert 55535 -> 0xD8EF
	TL0 = 0xD8;
	TH0 = 0xEF;
	
	//IR System konfigurieren
	ET0 = 1; //IR für Timer0 aktiv
	EAL = 0; //Alles aus
	
	while(1)
	{
		EAL = 1;//Interrupts aktivieren
		TR0 = 1; // Timer0 aktiv!!
		
		milliCounter = milli();
	}
}

void IRQ_Timer0() interrupt 1
{
	TR0 = 0;
	EAL = 0;
	
	milliCounter++;
	
	TL0 = 0xD8;
	TH0 = 0xEF;
	
	EAL = 1;
	TR0 = 1;
}


unsigned long int milli()
{
	return milliCounter;
}