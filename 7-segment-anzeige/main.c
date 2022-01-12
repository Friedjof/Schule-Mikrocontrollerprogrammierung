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
// ms since startup
unsigned long int milliCounter = 0;

/************************** Prototypen *******************************/
void set_number(char nrs[10], int numDigits, int nr, int segment);
void reset();
void checkIncrementalEncoder(char *IncrementalEncoderBools, char *mainBools, unsigned int *counter, unsigned long int *timer0, char IncrementalEncoderSpeedLimit);

char potenzieren(int basis, int potenz);
int fixPotenz(int potenz);

char bitCopy(char masterChar, char fromBit, char toBit);
char bitFlip(char masterChar, char bitIndex);
char setBit(char masterChar, char bitIndex, char state);
char getBit(char masterChar, char bitIndex);

void IRQ_Timer0();
unsigned long int milli();
/************************ Hauptprogramm ******************************/

void main()
{
  // Die Pins zu den Ziffern werden festgelegt
  //byte digitPins[4] = {2, 3, 4, 5};
  // Die Pins zu den //Segmenten werden festgelegt
  //byte segmentPins[8] = {6, 7, 8, 9, 10, 11, 12, 13};
  // Konfigurierung der einzelnen Ziffern
  char nrs[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

  unsigned long int timer0 = milli();
  unsigned long int timer1 = milli();

  // | effectStepTrigger | effect | numDigitsBit2 | numDigitsBit1 | segmentBit2 | segmentBit1 | flash | countdown |
  char mainBools = 0x00;

  // Hier wird die Anzahl der Ziffernbloecke angegeben
  int numDigits = 4;

  // | 0 | 0 | 0 | Button | CLK[1] | CLK[0] | DT[1] | DT[0] |
  char IncrementalEncoderBools = 0x00;

  const char IncrementalEncoderSpeedLimit = 20;

  unsigned int counter = 0;
  int alertCounter = 0;
  
  char i = 2;
	
	//Timer0 konfigurieren
	TR0 = 0; //ausgeschaltet
	TF0 = 0; //�berlauf zur�ckgesetzt
	IT0 = 0; //IR gel�scht
	TMOD = 0x01;//Timer1: Timer, 8bit prescale, Timer0: Timer, 16bit
	
	//Startwert 15535 -> 0x3CAF
	//Startwert 55535 -> 0xD8EF
	TL0 = 0xD8;
	TH0 = 0xEF;
	
	//IR System konfigurieren
	ET0 = 1; //IR f�r Timer0 aktiv
	EAL = 0; //Alles aus
	
	P6 = setBit(setBit(setBit(setBit(P6, 0x07, 0x01), 0x06, 0x01), 0x05, 0x01), 0x04, 0x01);
	
	// Main Loop
  while(1)
  {
		EAL = 1;//Interrupts aktivieren
		TR0 = 1; // Timer0 aktiv!!
		
    // liesst den Inkrementalgeber aus [Pin A0 und A1]
    IncrementalEncoderBools = setBit(setBit(IncrementalEncoderBools, 0x01, getBit(P3, 0x03)), 0x04, getBit(P3, 0x5));

    checkIncrementalEncoder(&IncrementalEncoderBools, &mainBools, &counter, &timer0, IncrementalEncoderSpeedLimit);

    if (!getBit(mainBools, 0x0C))
    {
      // addiert eins auf den Segment Zahler drauf
      mainBools = setBit(setBit(mainBools, 0x08, getBit(((mainBools & 0x0C) / 0x04) + 0x01, 0x02)), 0x04, getBit(((mainBools & 0x0C) / 0x04) + 0x01, 0x01));
    }
    else
    {
      // Wenn der Segment Zahler bei drei angelangt ist, wird hier der Zahler wieder auf 0 zur�ck gesetzt.
      mainBools = setBit(setBit(mainBools, 0x04, 0x00), 0x08, 0x00);
    }
    
    // Wenn flash Bit nicht gesetzt
    if (!getBit(mainBools, 0x02))
    {
        // Anzeigen des aktuellen Display Segmentes
      set_number(nrs, numDigits, (counter / fixPotenz((mainBools & 0x0C) / 0x04)) % 10, (numDigits - 1) - ((mainBools & 0x0C) / 0x04));
    }
    // flash effect:
    else if (getBit(mainBools, 0x02))
    {
      if (counter > 0 && getBit(mainBools, 0x02))
      {
        // flash Bit loeschen
        mainBools = setBit(mainBools, 0x02, 0x00);
      }

      if (milli() - timer1 < 1000)
      {
        // Setzen der Display Segmente
        if (alertCounter % 2 == 0)
        {
					
          //set(numDigits, 0x5C, potenzieren(2, alertCounter / 2));
        }
        else
        {
          //set(numDigits, 0x63, potenzieren(2, alertCounter / 2));
        }

        if (!((milli() - timer1) % 62) && alertCounter != 7 * !getBit(mainBools, 0x40) && !getBit(mainBools, 0x80))
        {
          alertCounter += 1 + ((-2) * getBit(mainBools, 0x40));
          mainBools = setBit(mainBools, 0x80, 1);
        }
        else if (!((milli() - timer1) % 62) && alertCounter == 7 * !getBit(mainBools, 0x40) && !getBit(mainBools, 0x80))
        {
          alertCounter = 7 * !getBit(mainBools, 0x40);
          mainBools = bitFlip(mainBools, 0x40);
          mainBools = setBit(mainBools, 0x80, 1);
        }
        else if (((milli() - timer1) % 62) > 0x00 && getBit(mainBools, 0x80))
        {
          mainBools = setBit(mainBools, 0x80, 0x00);
        }
        else
        { }
      }
      else if (milli() - timer1 < 2000 && 0x00)
      {
        // Anzeigen des aktuellen Display Segmentes
        set_number(nrs, numDigits, (counter / fixPotenz((mainBools & 0x0C) / 0x04)) % 10, (numDigits - 1) - ((mainBools & 0x0C) / 0x04));
      }
      else
      {
        // Den Timer auf die aktuelle Zeit zur�ck setzen
        timer1 = milli();

        mainBools = setBit(mainBools, 0x80, 0x00);
        alertCounter = 7 * !getBit(mainBools, 0x40);
      }
    }
    else
    { }

    // Sekunden runter z�hlen
    if (milli() - timer0 >= 100 && getBit(mainBools, 0x01))
    {
      timer0 = milli();
      if (counter > 0)
      {
        counter--;
      }
      else if (counter == 0)
      {
        // flash Bit setzen und countdown Bit loeschen
        mainBools = setBit(setBit(mainBools, 0x02, 0x01), 0x01, 0x00);
      }
    }
  }
}

void set_number(char nrs[10], int numDigits, int nr, int segment)
{
	
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
void checkIncrementalEncoder(char *IncrementalEncoderBools, char *mainBools, unsigned int *counter, unsigned long int *timer0, char IncrementalEncoderSpeedLimit)
{
  char buttonStatus = getBit(P3, 0x04);

  // Wenn der Button Status nicht dem gespeicherten Wert entspricht 
  if (buttonStatus ^ getBit(*IncrementalEncoderBools, 0x10))
  {
    // Wenn der Button Status 0x00 ist
    if (!buttonStatus)
    {
      // Gespcherten Button Status loeschen
      *IncrementalEncoderBools = setBit(*IncrementalEncoderBools, 0x10, 0x00);

      if (getBit(*mainBools, 0x01))
      {
        // loescht das Countdown Bit
        *mainBools = setBit(*mainBools, 0x01, 0x00);
      }
      // Sonst wenn flash Bit gesetzt
      else if (getBit(*mainBools, 0x02))
      {
        // loesche das flash Bit
        *mainBools = setBit(*mainBools, 0x02, 0x00);
      }
      else if (*counter > 0)
      {
        // setzt das Countdown Bit
        *mainBools = setBit(*mainBools, 0x01, 0x01);
      }
      else
      { }
    }
    else
    {
      *IncrementalEncoderBools = setBit(*IncrementalEncoderBools, 0x10, buttonStatus);
    }
  }
  
  // Wenn sich der Status von neu 0x01 und alt 0x02 unterschieden
  if (getBit(*IncrementalEncoderBools, 0x01) ^ getBit(*IncrementalEncoderBools, 0x02))
  {
    // Stelle den Counter aus, wenn er aktiv ist und der Inkrementalgeber getreht wird
    if (getBit(*mainBools, 0x01))
    {
      *mainBools = setBit(*mainBools, 0x01, 0x00);
    }
    // Stelle den falsh aus, wenn er aktiv ist und der Inkrementalgeber getreht wird
    if (getBit(*mainBools, 0x02))
    {
      *mainBools = setBit(*mainBools, 0x02, 0x00);
    }

    if ((*IncrementalEncoderBools & 0x03) == 0x01)
    {
      if (getBit(*IncrementalEncoderBools, 0x04))
      {
        if (millis() - *timer0 > IncrementalEncoderSpeedLimit || *counter < 10)
        {
          if (*counter > 0)
          {
            *counter -= 1;
          }
        }
        else
        {
          if (*counter > 9)
          {
            *counter -= 10;
          }
        }
      }
      else
      { }
    }
    else if ((*IncrementalEncoderBools & 0x01) == 0x00 && (*IncrementalEncoderBools & 0x02) == 0x02)
    {
      *timer0 = millis();
    }

    *IncrementalEncoderBools = bitCopy(*IncrementalEncoderBools, 0x01, 0x02);
  }

  // Wenn sich der Status von neu 0x04 und alt 0x08 unterschieden
  if (getBit(*IncrementalEncoderBools, 0x04) ^ getBit(*IncrementalEncoderBools, 0x08))
  {
    // Stelle den Counter aus, wenn er aktiv ist und der Inkrementalgeber getreht wird
    if (getBit(*mainBools, 0x01))
    {
      *mainBools = setBit(*mainBools, 0x01, 0x00);
    }
    // Stelle den falsh aus, wenn er aktiv ist und der Inkrementalgeber getreht wird
    if (getBit(*mainBools, 0x02))
    {
      *mainBools = setBit(*mainBools, 0x02, 0x00);
    }

    if (getBit(*IncrementalEncoderBools, 0x04) && !getBit(*IncrementalEncoderBools, 0x08))
    {
      if ((*IncrementalEncoderBools & 0x01) == 0x01)
      {
        if (millis() - *timer0 > IncrementalEncoderSpeedLimit || *counter > 9990)
        {
          if (*counter < 9999)
          {
            *counter += 1;
          }
        }
        else
        {
          if (*counter < 9990)
          {
            *counter += 10;
          }
        }
      }
    }
    else if (!getBit(*IncrementalEncoderBools, 0x04) && getBit(*IncrementalEncoderBools, 0x08))
    {
      *timer0 = millis();
    }

    *IncrementalEncoderBools = bitCopy(*IncrementalEncoderBools, 0x04, 0x08);
  }
}

char bitCopy(char masterChar, char fromBit, char toBit)
{
  // Wenn 'fromBit' gesetzt
  if ((masterChar & fromBit) == fromBit)
  {
    // setze 'toBit'
    masterChar = setBit(masterChar, toBit, 0x01);
  }
  else
  {
    // Sonst loesche 'toBit'
    masterChar = setBit(masterChar, toBit, 0x00);
  }

  return masterChar;
}

char setBit(char masterChar, char bitIndex, char state)
{
  // Wenn 'bit' nicht 'state' entspricht
  if ((masterChar & bitIndex) != state * bitIndex)
  {
    // Wenn bit gesetzt
    if ((masterChar & bitIndex) == bitIndex)
    {
      // Bit loeschen
      masterChar = masterChar & (0x0FF ^ bitIndex);
    }
    else
    {
      // Bit setzen
      masterChar = masterChar | bitIndex;
    }
  }
  else
  { }

  return masterChar;
}

char getBit(char masterChar, char bitIndex)
{
  return (masterChar & bitIndex) / bitIndex;
}


char bitFlip(char masterChar, char bitIndex)
{
  if (!(masterChar & bitIndex))
  {
    masterChar = masterChar | bitIndex;
  }
  else
  {
    masterChar = masterChar & (0x0FF ^ bitIndex);
  }
  return masterChar;
}

char potenzieren(int basis, int potenz)
{
	// init index
	int index = 0;
	char zwischenergebnis = 0x01;

	// die Potenz in einer for-Schleife berechnen
	for (index = 0; index < potenz; index = index + 1)
	{
		// das Zwischenergebnis wird in der Variablen "zwischenergebnis" gespeichert
		zwischenergebnis = zwischenergebnis * basis;
	}
  return zwischenergebnis;
}

int fixPotenz(int potenz)
{
	// init index
	int index = 0;
	int zwischenergebnis = 1;

	// die Potenz in einer for-Schleife berechnen
	for (index = 0; index < potenz; index = index + 1)
	{
		// das Zwischenergebnis wird in der Variablen "zwischenergebnis" gespeichert
		zwischenergebnis = zwischenergebnis * 10;
	}
  return zwischenergebnis;
}