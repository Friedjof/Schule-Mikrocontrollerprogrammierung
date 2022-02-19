// Friedjof Noweck
// 2021-12-22 Mi

#include <Arduino.h>

void set_number(char nrs[10], byte numDigits, int nr, int segment);
void set(byte numDigits, char nr, char segment);
void reset();
void checkIncrementalEncoder(char *IncrementalEncoderBools, char *mainBools, unsigned int *counter, unsigned long int *timer0, char IncrementalEncoderSpeedLimit);

char potenzieren(int basis, int potenz);
int fixPotenz(int potenz);

char bitCopy(char masterChar, char fromBit, char toBit);
char bitFlip(char masterChar, char bit);
char setBit(char masterChar, char bit, char state);
char getBit(char masterChar, char bit);

void setup()
{
  // Die Pins zu den Ziffern werden festgelegt
  //byte digitPins[4] = {2, 3, 4, 5};
  // Die Pins zu den //Segmenten werden festgelegt
  //byte segmentPins[8] = {6, 7, 8, 9, 10, 11, 12, 13};
  // Konfigurierung der einzelnen Ziffern
  char nrs[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

  unsigned long int timer0 = millis();
  unsigned long int timer1 = millis();

  // | effectStepTrigger | effect | numDigitsBit2 | numDigitsBit1 | segmentBit2 | segmentBit1 | flash | countdown |
  char mainBools = 0x00;

  // Hier wird die Anzahl der Ziffernbloecke angegeben
  byte numDigits = 4;

  // | 0 | 0 | 0 | Button | CLK[1] | CLK[0] | DT[1] | DT[0] |
  char IncrementalEncoderBools = 0x00;

  const char IncrementalEncoderSpeedLimit = 20;

  unsigned int counter = 0;
  byte alertCounter = 0;
  

  char i = 2;

  // Init serial interface
  Serial.begin(115200);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  // init display ports
  for (; i <= 13; i++){
    pinMode(i, OUTPUT);
    if (i <= 5)
    {
      digitalWrite(i, HIGH);
    }
  }

  // Main Loop
  while(true)
  {
    // liesst den Inkrementalgeber aus [Pin A0 und A1]
    IncrementalEncoderBools = setBit(setBit(IncrementalEncoderBools, 0x01, (char)digitalRead(A1)), 0x04, (char)digitalRead(A0));

    checkIncrementalEncoder(&IncrementalEncoderBools, &mainBools, &counter, &timer0, IncrementalEncoderSpeedLimit);

    if (!getBit(mainBools, 0x0C))
    {
      // addiert eins auf den Segment Zahler drauf
      mainBools = setBit(setBit(mainBools, 0x08, getBit(((mainBools & 0x0C) / 0x04) + 0x01, 0x02)), 0x04, getBit(((mainBools & 0x0C) / 0x04) + 0x01, 0x01));
    }
    else
    {
      // Wenn der Segment Zahler bei drei angelangt ist, wird hier der Zahler wieder auf 0 zurück gesetzt.
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

      if (millis() - timer1 < 1000)
      {
        // Setzen der Display Segmente
        if (alertCounter % 2 == 0)
        {
          set(numDigits, 0x5C, potenzieren(2, alertCounter / 2));
        }
        else
        {
          set(numDigits, 0x63, potenzieren(2, alertCounter / 2));
        }

        if (!((millis() - timer1) % 62) && alertCounter != 7 * !getBit(mainBools, 0x40) && !getBit(mainBools, 0x80))
        {
          alertCounter += 1 + ((-2) * getBit(mainBools, 0x40));
          mainBools = setBit(mainBools, 0x80, 1);
        }
        else if (!((millis() - timer1) % 62) && alertCounter == 7 * !getBit(mainBools, 0x40) && !getBit(mainBools, 0x80))
        {
          alertCounter = 7 * !getBit(mainBools, 0x40);
          mainBools = bitFlip(mainBools, 0x40);
          mainBools = setBit(mainBools, 0x80, 1);
        }
        else if (((millis() - timer1) % 62) > 0x00 && getBit(mainBools, 0x80))
        {
          mainBools = setBit(mainBools, 0x80, 0x00);
        }
        else
        { }
      }
      else if (millis() - timer1 < 2000 && 0x00)
      {
        // Anzeigen des aktuellen Display Segmentes
        set_number(nrs, numDigits, (counter / fixPotenz((mainBools & 0x0C) / 0x04)) % 10, (numDigits - 1) - ((mainBools & 0x0C) / 0x04));
      }
      else
      {
        // Den Timer auf die aktuelle Zeit zurück setzen
        timer1 = millis();

        mainBools = setBit(mainBools, 0x80, 0x00);
        alertCounter = 7 * !getBit(mainBools, 0x40);
      }
    }
    else
    { }

    // Sekunden runter zählen
    if (millis() - timer0 >= 100 && getBit(mainBools, 0x01))
    {
      timer0 = millis();
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

void loop()
{ }

void set_number(char nrs[10], byte numDigits, int nr, int segment)
{
  if (segment == 2)
  {
    set(numDigits, setBit(nrs[nr], 0x80, 0x01), potenzieren(2, segment));
  }
  else
  {
    set(numDigits, nrs[nr], potenzieren(2, segment));
  }
}

void set(byte numDigits, char nr, char segment)
{
  int i = 0;

  for (int i = 0; i < 8; i++)
  {
    digitalWrite(i + 6, LOW);
  }

  for (i = 0; i < numDigits; i++)
  {
    if ((segment & potenzieren(2, i)) == potenzieren(2, i))
    {
      digitalWrite(2 + i, LOW);
    }
    else
    {
      digitalWrite(2 + i, HIGH);
    }
  }
  
  for (int i = 0; i < 8; i++)
  {
    if ((nr & potenzieren(2, i)) == potenzieren(2, i))
    {
      digitalWrite(i + 6, HIGH);
    }
    else
    {
      digitalWrite(i + 6, LOW);
    }
  }
}

void reset()
{
  int i = 0;

  for (i = 0; i < 8; i++)
  {
    digitalWrite(i + 6, LOW);
  }
}

void checkIncrementalEncoder(char *IncrementalEncoderBools, char *mainBools, unsigned int *counter, unsigned long int *timer0, char IncrementalEncoderSpeedLimit)
{
  char buttonStatus = digitalRead(A2);

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

char setBit(char masterChar, char bit, char state)
{
  // Wenn 'bit' nicht 'state' entspricht
  if ((masterChar & bit) != state * bit)
  {
    // Wenn bit gesetzt
    if ((masterChar & bit) == bit)
    {
      // Bit loeschen
      masterChar = masterChar & (0x0FF ^ bit);
    }
    else
    {
      // Bit setzen
      masterChar = masterChar | bit;
    }
  }
  else
  { }

  return masterChar;
}

char getBit(char masterChar, char bit)
{
  return (masterChar & bit) / bit;
}

char bitFlip(char masterChar, char bit){
  if (!(masterChar & bit))
  {
    masterChar = masterChar | bit;
  }
  else
  {
    masterChar = masterChar & (0x0FF ^ bit);
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