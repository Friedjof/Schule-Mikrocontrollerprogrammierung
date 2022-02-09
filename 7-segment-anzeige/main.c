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
2022-01-19 migation to the school controller
2022-02-06 Add Timer0 and memory optimization
2022-02-09 Fix edit mode bug

*****************************************************************************/
/******************* Text im Quelltext einbinden **********************/
#include "REG517A.h"

/*************************** Konstanten *******************************/


/*********************** globale Variablen ****************************/
// Zähler zum Speichern von Zeit; Einheit in ms [max 1000ms]
unsigned char timerCounter = 0;
unsigned int timer = 0;

int number2display = 0x00;

/************************** Definitionen ******************************/
// REG517A specific
#define maxNumber 9999

/************************** Prototypen ********************************/
char readButtonMatrix(char maxRows, char maxColumns);
void display(char number, char segment);
char index2number(char index);
char getNumber(int numbers, char index);
int fixPotenz(char potenz);
void IRQ_Timer0();
// Arduino Mega specific
//ISR(TIMER0_COMPA_vect);

/*
 * Friedjof Noweck
 * 2022-01-25 Di
 * 
 * +-------------------------------------------------------------------+
 * | Anleitung:                                                        |
 * |  Der Controller starte im Editier-Modus. Erkennbar auch an dem    |
 * |  blickenden Cursor. Hier können Zahlen über die Matrixtastatur    |
 * |  gesetzt werden. Der Cursor wandert nach rechts weiter.           |
 * |  Alternativ kann auch über die Button-Matrix eine exakte Position |
 * |  festgelegt werden (A 1000er, B 100er, C 10er, D 1er). Auch kann  |
 * |  die Zahl in dem Editor Modus per Inkrementalgeber hoch bzw.      |
 * |  runtergeregelt werden. Der Cursor wird hierbei nicht beachtet.   |
 * |                                                                   |
 * |  Durch das Drücken von dem rechten Taster wird der Zähler         |
 * |  gestartet. Die Richtung kann auch umgekehrt werden mit dem       |
 * |  linken Taster. Durch das erneute Drücken auf den rechten Taster  |
 * |  wird der Zähler wieder angehalten und der Editiermodus           |
 * |  gestartet. Beim laufenden Zähler ist die Button-Matrix           |
 * |  deaktiviert.                                                     |
 * |                                                                   |
 * |  Die Zählgeschwindigkeit kann geändert werden, indem der          |
 * |  Inkrementalgeber beim laufenden Zähler hoch bzw. runtergeregelt  |
 * |  wird.                                                            |
 * |                                                                   |
 * |  >> Aufgrund von Speichermangel deaktiviert <<                    |
 * |  Durch das Drücken auf den Inkrementalgeber wird der Timer Modus  |
 * |  aktiviert. Dies bedeutet, dass bei dem Wert 0 angehalten wird    |
 * |  und das Display aufblickt. Dieser Modus wird deaktiviert, wenn   |
 * |  während des Blickens eine der beiden anderen Taster gedrückt     |
 * |  wird. Durch erneutes Drücken wird dieser Modus ebenfalls wieder  |
 * |  deaktiviert.                                                     |
 * +-------------------------------------------------------------------+
 * | PINBELEGUNG                                                       |
 * +------------------------------+--------------------+---------------+
 * | Elemente                     |    Arduino Mega    |    REG517A    |
 * |                              |   Ports      Pins  |               |
 * +------------------------------+-----------+--------+---------------+
 * | ● 4x4 Taster Matrix          |           |        |               |
 * |   ⸰ ROWS                     | PORTA.0-3 | D22-25 | P6.4-7        |
 * |   ⸰ COLUMNS                  | PORTA.4-7 | D26-29 | P8.0-3        |
 * | ● 4x 7 Segment Anzeigen      |           |        |               |
 * |   ⸰ Segmente                 | PORTL.0-7 | D42-49 | P4.0-7        |
 * |   ⸰ Anzeigen                 | PORTB.0-3 | D50-53 | P6.4-7        |
 * | ● 2x Taster                  |           |        |               |
 * |   ⸰ Start/Stopp              | PORTG.0   | D41    | P5.0          |
 * |   ⸰ Richtungswechsel         | PORTG.1   | D40    | P6.0          |
 * | ● Inkrementalgeber           |           |        |               |
 * |   ⸰ Taster                   | PORTF.2   | A2     | P3.4          |
 * |   ⸰ Inkrementalgeber Takt    | PORTF.0-1 | A0-1   | P3.3&5        |
 * +------------------------------+-----------+--------+---------------+
 * | Quellen:                                                          |
 * | ● Timer beim Arduino                                              |
 * |   ⸰ https://www.exp-tech.de/blog/arduino-tutorial-timer           |
 * | ● Arduino Mega Ports                                              |
 * |   ⸰ https://aws1.discourse-cdn.com/arduino/original/4X/6/f/b/6fb6102c3ff917a32b3fabaa7b01c72fb208919e.png
 * | ● Port Adressierung beim Arduino:                                 |
 * |   ⸰ https://hartmut-waller.info/arduinoblog/leds-schalten-port-ddr/
 * +-------------------------------------------------------------------+
 * 
 */


// Arduino Mega specific
//void setup()
void main()
{
    // Konstanten
    // Die Anzahl an vorliegenden 7-Segment-Anzeigen
    const char nrOfSegments = 0x04;
    // Die maximal anzuzeigende Zahl
		// Arduino Mega specific
    //const short maxNumber = 9999;

    // Speed - Die Zählgeschwindigkeit in ms
    char speed = 0x05;

    // Speicherung des letzten ermittelten Button Matrix Indexes [0 - 16, 0 = nichts gedrückt]
    char currentButtonState = 0x00;

    // Speicherung des gerade ermittelten Button Matrix Indexes [0 - 16, 0 = nichts gedrückt]
    char matrixResult = 0x00;

    // Speichert den Zeitpunkt der letzten Ermittlung des Button Matrix Indexes in ms
    unsigned int buttonTimeout = timer;

    // Display Variables
    // Speichert den Index der aktuell anzuzeigenden 7-Segment-Anzeige
    char segmentCounter = 0x00;
    // Speichert den Zeitpunkt der letzten Aktualisierung des Indexes der aktuell anzuzeigenden 7-Segment-Anzeige in ms
    unsigned int segmentCounterTimer = timer;
    // Speichert den aktuellen Zählerwert
    //int number2display = 0x00;
    // Speichert den Zeitpunkt der letzten Aktualisierung des Zählerwertes
    unsigned int counterTimer = timer;
    // Speichert die Ziffer des aktuellen Segments
    char number = 0x00;
    // Speichert die Konfiguration der einzelnen Ziffern auf der 7-Segment-Anzeige (beginnend bei 0x3F = 0)
    // > So könne einstellige Zahlen einfach der Ziffern Konfiguration der 7-Segment-Anzeigen zugeordnet werden.
		// Arduino Mega specific
    //const unsigned char segments[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
		
		/* REG517A specific
		 * c P f e d g b a | HEX  | Display
		 * ----------------+------+--------
		 * 1 0 1 1 1 0 1 1 | 0xBB |   0
		 * 1 0 0 0 0 0 1 0 | 0x82 |   1
		 * 0 0 0 1 1 1 1 1 | 0x1F |   2
		 * 1 0 0 0 1 1 1 1 | 0x8F |   3
		 * 1 0 1 0 0 1 1 0 | 0xA6 |   4
		 * 1 0 1 0 1 1 0 1 | 0xAD |   5
		 * 1 0 1 1 1 1 0 1 | 0xBD |   6
		 * 1 0 1 0 0 0 1 1 | 0xA3 |   7
		 * 1 0 1 1 1 1 1 1 | 0xBF |   8
		 * 1 0 1 0 1 1 1 1 | 0xAF |   9
		 */
    const unsigned char segments[10] = {0xBB, 0x82, 0x1F, 0x8F, 0xA6, 0xAD, 0xBD, 0xA3, 0xBF, 0xAF};

    // Speichert die aktuellen Curor Position ab
    // > Ab Start liegt der Wert auf der ganz linken 7-Segment-Anzeige.
    unsigned char cursor = nrOfSegments - 0x01;

    // Speichert folgende wichtige Werte:
    // { Timer Mode , IG Button , CIG02 , CIG01 , Richtung , Start/Stop , Richtung Button , Start/Stop Button }
    unsigned char specialButtons = 0x44; // 0b00000100;
    // Dient als temporärer Zwischenspeicher der ermitteten Werte
    unsigned char specialButtonsResult = 0x00;

		// Set pin modes
		// 4x4 Button Matrix
		// > Ersten 4 als 'OUTPUT', letzten 4 als 'INPUT'
		//DDRA = 0x0F0;

		// 4x 7 Segment Anzeigen
		// > Ersten 4 als 'INPUT', letzten 4 als 'OUTPUT'
		//DDRB = 0x0F;
		// Segments - Definiert alle Pins des Ports als 'OUTPUT'
		//DDRL = 0x0FF;

		// Special Buttons
		// Setzt die ersten zwei Pins dieses Ports als 'INPUTS', die anderen Pins werden als 'OUTPUT' definiert
		//DDRG = 0x0FC;

		// Inkrementalgeber
		// Setzt die ersten drei Pins dieses Ports als 'INPUTS', die anderen Pins werden als 'OUTPUT' definiert
		//DDRF = 0x0F8;

		// Timer Setup
		// Setzen des CTC Moduses
		// Arduino Mega specific
		//TCCR0A = (1 << WGM01);
		// Setzt den Startwert des Timers
		// > Durch den Startwert von 0x0F9 beträgt ein Druchlauf genau eine ms
		// Arduino Mega specific
		//OCR0A = 0x0F9;
		
		// Setzen der Interrupt Anfrage
		// Arduino Mega specific
		//TIMSK0 |= (1 << OCIE0A);
		// Aktiviert den Interrupt
		// Arduino Mega specific
		//sei();
		
		// Setzen der Skalierung von 1 zu 64
		// Somit wird ist der Timer 64 so langsam wie standartmäßig vorgegeben
		// Arduino Mega specific
		//TCCR0B |= (1 << CS01);
		// Arduino Mega specific
		//TCCR0B |= (1 << CS00);
		
		// Timer0 konfigurieren
		// ausgeschaltet
		TR0 = 0;
		// Überlauf zurückgesetzt
		TF0 = 0;
		// IR gelöscht
		IT0 = 0;
		// Timer1: Timer, 8bit prescale, Timer0: Timer, 16bit
		TMOD = 0x01;
		// Startwert 55535 -> 0x0FC17
		// 1000 = 1ms
		//TL0 = 0x017;
		//TH0 = 0x0FC;
		TL0 = 0x0EF;
		TH0 = 0x0D8;
		
		// IR System konfigurieren
		// IR für Timer0 aktiv
		ET0 = 1;
		// Alles aus
		EAL = 0;
		
		// Interrupts aktivieren
		EAL = 1;
		// Timer0 aktiv
		TR0 = 1;

    while (1)
    {
        // Wenn die vergangene Zeit seit der letzten Abfrage der seperaten Taster, sowie der Button Matrix größer 50ms:
        if (timer - buttonTimeout > 5 && 0x01)
        {
            // Abfrage der beiden seperaten Taster
            // Arduino Mega specific
            //specialButtonsResult = PING & 0x03;
					  specialButtonsResult = (P5 & 0x01) | ((P3 & 0x04) >> 0x01); //((P6 & 0x01) << 0x01);

            // Wenn Zähler nicht aktiv:
            if ((0x01 ^ ((specialButtons & 0x04) >> 0x02)))
            {
                // Matrix abfragen
                matrixResult = readButtonMatrix(4, 4);
            }
            else
            { }

            // Wenn eine Veränderung an den beiden seperaten Tastern vorliegt:
            if (0x01 ^ ((((specialButtonsResult & 0x02) >> 0x01) & (specialButtonsResult & 0x01)) & (((specialButtons & 0x02) >> 0x01) & (specialButtons & 0x01))))
            {
                // Wenn sich der rechte Taster verändert hat:
                if ((specialButtons & 0x01) ^ (specialButtonsResult & 0x01))
                {
                    // Aktualisiere das gespeichert Bit
                    specialButtons = (specialButtons & (0x0FE | (specialButtonsResult & 0x01))) | (0x01 & (specialButtonsResult & 0x01));

                    // Wenn ansteigende Flanke:
                    if (0x01 ^ (specialButtons & 0x01))
                    {
                        // Drehe das stop/start Bit um
                        specialButtons = (specialButtons & (0x0FB | (0x04 ^ (specialButtons & 0x04)))) | (0x04 & (0x04 ^ (specialButtons & 0x04)));

                        // Setze der Cursor ab die ganz linke Position
                        cursor = nrOfSegments - 0x01;

                        // Wenn Timer Mode an ist und der wert auf dem Display 0 ist (kurz: wenn das Display Blickt):
                        //if ((specialButtons & 0x80) >> 0x07 && number2display == 0)
                        //{
                        //    // Timer Modus Bit löschen
                        //    specialButtons = specialButtons & 0x7F;
                        //}
                        //else
                        //{ }
                    }
                    else
                    { }
                }
                else
                { }

                // Wenn sich etwas am linken Taster verändert hat:
                if (((specialButtons & 0x02) >> 0x01) ^ ((specialButtonsResult & 0x02) >> 0x01))
                {
                    // Aktualisiere das gespeichter Bit
                    specialButtons = (specialButtons & (0x0FD | (specialButtonsResult & 0x02))) | (0x02 & (specialButtonsResult & 0x02));

                    // Wenn ansteigende Flanke:
                    if (0x02 ^ (specialButtons & 0x02))
                    {
                        // Drehe das richtungs Bit um
                        specialButtons = (specialButtons & (0x0F7 | (0x08 ^ (specialButtons & 0x08)))) | (0x08 & (0x08 ^ (specialButtons & 0x08)));

                        // Wenn Timer Mode an ist und der wert auf dem Display 0 ist (kurz: wenn das Display Blickt):
                        //if ((specialButtons & 0x80) >> 0x07 && number2display == 0)
                        //{
                        //    // Timer Modus Bit löschen
                        //    specialButtons = specialButtons & 0x7F;
                        //}
                        //else
                        //{ }
                    }
                    else
                    { }
                }
                else
                { }
            }

            // Wenn ein Taster der Matrix Tastatur gedrückt wurde und sich der Zustand geändert hat:
            if (matrixResult && (currentButtonState ^ matrixResult))
            {
                // Neuen Matrix-Tastatur-Status setzen 
                currentButtonState = matrixResult;

                // Versetzen des Cursors
                // Wenn A gedrückt:
                if (currentButtonState == 0x01)
                {
                    // Setze Cursor auf 1000er Stelle
                    cursor = nrOfSegments - 0x04;
                }
                // Sonst wenn B gedrückt:
                else if (currentButtonState == 0x05)
                {
                    // Setze Cursor auf 100er Stelle
                    cursor = nrOfSegments - 0x03;
                }
                // Sonst wenn C gedrückt:
                else if (currentButtonState == 0x09)
                {
                    // Setze Cursor auf 10er Stelle
                    cursor = nrOfSegments - 0x02;
                }
                // Sonst wenn D gedrückt:
                else if (currentButtonState == 0x0D)
                {
                    // Setze Cursor auf 1er Stelle
                    cursor = nrOfSegments - 0x01;
                }
                // Sonst wenn * gedrückt:
                else if (currentButtonState == 0x04)
                { }
                // Sonst wenn # gedrückt:
                else if (currentButtonState == 0x02)
                { }
                // Andernfalls bei einer anderen Taste:
                else
                {
                    // Ziehe die Zahl des Cursor von der angezeigten Zahl ab
                    number2display -= getNumber(number2display, cursor + 0x01) * fixPotenz(cursor);
                    // Füge die gedrückte Zahl auf der Matrix Tastatur an die Stelle des Cursors ein 
                    number2display += index2number(currentButtonState) * fixPotenz(cursor);

                    // Wenn der Cursor größer ist als 0:
                    if (cursor > 0x00)
                    {
                        // Versetze den Cursor einen weiter nch rechts
                        cursor--;
                    }
                    else
                    {
                        // Sonst setze den Cursor wieder nach ganz links
                        cursor = nrOfSegments - 0x01;
                    }
                }
            }
            // Sonst, wenn keine Taste gedückt, allerdings noch ein anderer Wert außer 0 gespeichert ist (kurz: Bei einer fallende Flanke):
            else if (!matrixResult && currentButtonState)
            {
                // Setze den gespeicherten Wert zurück. 
                currentButtonState = 0x00;
            }
            else
            { }

            // Die Zeit seit dem letzten mal Drücken wird auf "jetzt" gesetzt (aktualisiert)
            buttonTimeout = timer;
        }
        else
        { }

				// Inkrementalgeber auslesen
				// Der Port des Inkrementalgebers wird abgefragt
				// Arduino Mega specific
				//specialButtonsResult = PINF & 0x07;
				specialButtonsResult = ((P4 & 0x18) >> 0x02) | ((P4 & 0x20) >> 0x05);

        // Inkrementalgeber Button
        // Wenn sich der gespeicherte und der gerade abgefragte Zustand unterschieden:
        if ((specialButtonsResult & 0x04) ^ ((specialButtons & 0x40) >> 0x04))
        {
            // Wenn das Bit für den Inkrementalgeber Button gesetzt ist:
            if (specialButtonsResult & 0x04)
            {
                // Wenn der Timer Modus aktiviert ist und der aktuelle Zäjlerwert bei 0 liegt (kurz: Wenn das Display blickt):
                //if (((specialButtons & 0x80) >> 0x07) && number2display == 0)
                //{
                //    // Setze das Bit für den Richtungs Button auf 1
                //    specialButtons = specialButtons | 0x04;
                //}
                //else
                //{ }

                // Drehe das Richtungs-Bit um
                specialButtons = (specialButtons & (0x7F | (0x80 ^ (specialButtons & 0x80)))) | (0x80 & (0x80 ^ (specialButtons & 0x80)));
            }
            else
            { }

            // Speichere den aktuellen Wert des Inkrementalgeber Tasters ab
            specialButtons = (specialButtons & (0xBF | ((specialButtonsResult & 0x04) << 0x04))) | (0x40 & ((specialButtonsResult & 0x04) << 0x04));
        }
        else
        { }

        // Wenn die gespeichten Werte des Inkrementalgebers von den aktuell gemessenen unterscheiden:
        if (((specialButtonsResult & 0x03) ^ ((specialButtons & 0x30) >> 0x04)) && 0x00)
        {
            // Wenn beide gespeicherten Werte auf 1 stehen:
            if ((specialButtons & 0x30) / 0x30)
            {
                // Wenn das erste aktuelle Bit 1 und das zweite aktuelle Bit 0 ist  (bzw. nach rechts gedreht wird):
                if ((specialButtonsResult & 0x01) && !((specialButtonsResult & 0x02) >> 0x01))
                {
                    // Wenn der Zähler aktuell läuft ist:
                    if ((specialButtons & 0x04) >> 0x02)
                    {
                        // Wenn die Zähl-Gewindigkeit aktuell kleiner oder gleich 64ms ist:
                        if ((speed * 2) <= 0x64)
                        {
                            // Setze die Zähl-Gewindigkeit auf das doppelte der aktuellen Geschwindigkeit
                            speed *= 2;
                        }
                        else
                        { }
                    }
                    // Sonst, wenn der Zähler gestoppt ist (bzw. der Editor-Modus aktiv ist):
                    else
                    {
                        // Wenn der aktuelle Zählwert plus 10 nicht den maximalen Zählerwert überschreitet:
                        if ((number2display + 0x0A) <= maxNumber)
                        {
                            // Addiere 10 auf den aktuelle Zählerwert drauf
                            number2display += 0x0A;
                        }
                        // Sonst, wenn aktuelle Zählwert plus 10 den maximalen Zählerwert überschreitet:
                        else
                        {
                            // Setze den Zählerwert wieder auf 0 zurück
                            number2display = 0x00;
                        }   
                    }
                }
                // Sonst, wenn das erste aktuelle Bit 0 und das zweite aktuelle Bit 1 ist (bzw. nach links gedreht wird):
                else if (!(specialButtonsResult & 0x01) && ((specialButtonsResult & 0x02) >> 0x01))
                {
                    // Wenn der Zähler aktiv ist:
                    if ((specialButtons & 0x04) >> 0x02)
                    {
                        // Wenn die aktuelle Zählgeschwindigkeit größer ist als 1ms:
                        if (speed > 0x01)
                        {
                            // Halbiere die aktuelle Zählgeschwindigkeit
                            speed /= 2;
                        }
                        else
                        { }
                    }
                    // Sonst, wenn der Zähler aktuell gestoppt ist (bzw. der Editor Modus aktiv ist):
                    else
                    {
                        // Wenn die aktuell angezeigte Zahl minus 10 die Grenze von 0 nicht unterschreitet:
                        if ((number2display - 0x0A) >= 0x00)
                        {
                            // Ziehe von der aktuell angezeigten Zahl 10 ab
                            number2display -= 0x0A;
                        }
                        // Andernfalls, wenn die aktuell angezeigte Zahl minus 10 die Grenze von 0 unterschreitet:
                        else
                        {
                            // Setze den Zähler auf die maximale Zahl hoch
                            number2display = maxNumber;
                        }
                    }
                }
                else
                { }
            }
            else
            { }

            // Setze den gespeicherten Wert des Inkrementalgebers auf den gerade gemessenen Wert
            specialButtons = (specialButtons & (0xCF | ((specialButtonsResult & 0x03) << 0x04))) | (0x30 & ((specialButtonsResult & 0x03) << 0x04));
        }
        else
        { }
        
        // Display
        // Extrahiert die Zahl des aktuellen Segments aus der gesamten Zahl und speichert diesen in der Variablen 'number'
        // Bsp.: number2display = 9876, segmentCounter + 0x01 = 0x02, return => 7
        number = getNumber(number2display, segmentCounter + 0x01);

        // Wenn der Timer Modus aktiv ist und der Zählerwert 0 ist:
        //if (((specialButtons & 0x80) >> 0x07) && number2display == 0)
        //{
        //    // Wenn der Timer größer oder gleich 500ms ist:
        //    if (timer >= 0x1F4)
        //    {
        //        // Zeige die Nummer des aktuellen Segments an
        //        display(segments[(int)number], (nrOfSegments - 0x01) - segmentCounter);
        //    }
        //    // Sonst, wenn der Timer kleiner als 500ms ist:
        //    else
        //    {
        //        // Zeige nichts auf dem aktuellen Segment an
        //        display(0x00, (nrOfSegments - 0x01) - segmentCounter);
        //    }
        //}
        // Sonst, wenn der Timer Modus nicht aktiv ist und der Zählerwert nicht 0 ist:
        //else
        //{ }
				
				// Wenn das aktuelle Segment nicht dem aktuellen Werte des Cursors entspricht oder der Editor Modus nicht aktiv ist:
				if (((specialButtons & 0x04) >> 0x02) || segmentCounter != cursor)
				{
						// Zeige die aktuelle Ziffer auf dem aktuellen Segment an
						//display(segments[(int)number], (nrOfSegments - 0x01) - segmentCounter);
					  display(segments[(int)number], segmentCounter);
				}
				// Sonst, wenn der Timer größer oder gleich 500ms ist:
				else if ((timer % 20) < 10)
				{
						// Zeige die aktuelle Ziffer auf dem aktuellen Segment an
		        // Arduino Mega specific
						//display(segments[(int)number], (nrOfSegments - 0x01) - segmentCounter);
					  display(segments[(int)number], segmentCounter);
				}
				// Sonst, wenn der Timer kleiner als 500ms ist und wenn das aktuelle Segment nicht dem aktuellen Werte des Cursors entspricht oder der Editor Modus nicht aktiv ist:
				else
				{
						// Zeige nichts auf dem aktuellen Segment an
		        // Arduino Mega specific
						//display(0x00, (nrOfSegments - 0x01) - segmentCounter);
					  display(0x00, segmentCounter);
				}

				// Notiz: NUR bei speed = 5 gibt es einen Bug beim Counter?!
				// TODO: Start/Stop Taster funktionier noch nicht korrekt
				//       Speziell funktioniert der Editor Modus noch nicht.
				
				// Counter defs
				// Wenn die Differenz zwischen der aktuelle und dem letzten Zählvorgang größer oder gleich der Zählgeschwindigkeit und der Timer aktiv ist:
				if (timer - counterTimer > 4 && ((specialButtons & 0x04) >> 0x02))
				{
						// Speicher den Zeitpunkt der aktuellen Aktualisierung des Zählerwertes ab
						counterTimer = timer;
						
						// Wenn der aktuelle Zählerstand der für die Richtung spezifischen Endzahl entspricht:
						if (number2display == maxNumber * ((((specialButtons & (0x0F7 | (0x08 ^ (specialButtons & 0x08)))) | (0x08 & (0x08 ^ (specialButtons & 0x08)))) & 0x08) >> 0x03))
						//if (number2display)
					  {
								// Setze den Zählerwert auf die für die Richtung spezifischen Startwert zurück
								number2display = maxNumber * ((specialButtons & 0x08) >> 0x03);
						}
						// Sonst, wenn der aktuelle Zählerstand der für die Richtung spezifischen Endzahl nicht entspricht:
						else
						{
								// Addiere auf die aktuell angezeigte Zahl den für die Richtung spezifischen Wert (1 oder -1) 
								number2display += 1 + ((-2) * ((specialButtons & 0x08) >> 0x03));
						}
				}
				else
				{ }
				
        //}

        // Wenn die Differenz zwischen der letzten Segment-Aktualisierung und der aktuellen Zeit größer oder gleich 5ms ist:
        //if (timer - segmentCounterTimer >= 0x01)
        //{
				// Wenn das aktuelle Segment nicht das letzte in der Richtung ist:
				if (segmentCounter < (nrOfSegments - 0x01))
				{
						// Gehe ein Segment weiter
						segmentCounter++;
				}
				// Sonst, wenn das aktuelle Segment das letzte in der Richtung ist:
				else
				{
						// Springe auf das erste zurück
						segmentCounter = 0x00;
				}
				//		// Speicher die Zeit der letzten Segment-Aktualisierung ab
				//		segmentCounterTimer = timer;
        //}
        //else
        //{ }
				
				//if (timer < buttonTimeout)
				//{
				//	 buttonTimeout = 0x00;
				//}
				//else if (timer < segmentCounterTimer)
				//{
				//	 segmentCounterTimer = 0x00;
				//}
				//else if (timer < counterTimer)
				//{
				//	counterTimer = 0x00;
				//}

        // Wenn der Timer die Zeit von einer Sekunde überschreitet:
        //if (timer > 0x3e8)
        //{
        //    // Setze den Timer, die Zeit seit der letzten Abfrage der seperaten Taster, sowie der Button Matrix und
        //    // die Zeit seit der letzten Segment-Aktualisierung auf 0 zurück.
        //    timer = buttonTimeout = segmentCounterTimer = counterTimer = 0;
        //}
        //else
        //{ }
    }
}

//void loop()
//{ }


// This is the interrupt request
// Addiere nach 1ms 1 auf den Timer
//ISR(TIMER0_COMPA_vect)
//{
//  timer++;
//}

void IRQ_Timer0() interrupt 1
{
	TR0 = 0;
	EAL = 0;
	
	if (timerCounter < 1)
	{
			timerCounter++;
	}
	else
	{
		timerCounter = 0x00;
		timer++;
  //		number2display++;
	}
	
	//TL0 = 0x017;
	//TH0 = 0x0FC;
	
	TL0 = 0x0EF;
	TH0 = 0x0D8;
	
	EAL = 1;
	TR0 = 1;
}

// Gibt die Zahl an dem gewünschten Index zurück
// Bsp.: numbers = 1234, index = 4, return => 1
char getNumber(int numbers, char index)
{
    // Index var
    char i = 0x00;
    // Zwischenspeicher
    char cache = 0;

    for (i = 0x00; i < index; i++)
    {
        // Gibt den Rest zurück
        cache = numbers % 10;

        // Teilen durch 10
        numbers /= 10;
    }

    // Rückgabe der gewünschten Zahl
    return cache;
}

// Ließt die Buttton Matrix aus und gibt den INDEX des gedrückten Tasters zurück
char readButtonMatrix(char maxRows, char maxColumns)
{
    // Zwischenspeicher [LOOPS]
		
    char row = 0;
    char column = 0;
		
		// Display ausschalten
		P4 = 0x00;

    for (row = 0; row < maxRows; row++)
    {
				// Setzen der Ausgänge (aktiver Eingang auf LOW)
				// 0b1110XXXX > 0b1101XXXX > 0b1011 > 0b0111XXXX
				//PORTA = 0x0F0 - (0x01 << (row + 0x04));
			  P6 = (P6 & 0x0F) | (0x0F0 - (0x01 << (row + 0x04)));
			  //P8 = (P8 & 0x0F0) | (0x0F - (0x01 << row));

        for (column = 0; column < maxColumns; column++)
        {
						// Wenn an einem Eingang 0 anliegt
						// Arduino Mega specific
						//if (!((PINA & (0x01 << column)) >> column))
					  if (!((P8 & (0x01 << column)) >> column))
					  //if (!((P6 & (0x10 << column)) >> (column + 0x04)))
						{
								// Gebe den Index des gedrückten Buttons zurück (1 - 16)
								return (maxColumns * row) + column + 0x01;
							  //column = column;
						}
						else
						{ }
        }
    }

    // Gibt 0x00 zurück, wenn kein Button gedrückt ist.
    return 0x00;
}

// Löst den Index der Button Matrix zu Ziffern auf
char index2number(char index)
{
    switch(index)
    {
        case 0x03: return 0x00;
        case 0x06: return 0x09;
        case 0x07: return 0x08;
        case 0x08: return 0x07;
        case 0x0A: return 0x06;
        case 0x0B: return 0x05;
        case 0x0C: return 0x04;
        case 0x0E: return 0x03;
        case 0x0F: return 0x02;
        case 0x10: return 0x01;
        default: return 0x00;
    }
}

// Setzt die Segment Konfiguration auf das angegebene Segment
void display(char segmentConfiguration, char segment)
{
		P6 = P6 | 0x0F0;
		
		// Setzen der einzelnen Segmente
		//PORTL = segmentConfiguration;
		P4 = segmentConfiguration;
		
		// Setzen der Eingänge
		//PORTB = 0x0F - (0x01 << segment);
		//P6 = P6 | (0x0F - (0x01 << segment));
	  P6 = (P6 & 0x0F) | (0x0F0 - (0x01 << (segment + 0x04)));
}

// Gibt die angegebenen Potenz zur Basis 10 zurück
int fixPotenz(char potenz)
{
	// init index
	char index = 0x00;
	int zwischenergebnis = 1;

	// die Potenz in einer for-Schleife berechnen
	for (index = 0; index < potenz; index++)
	{
		// das Zwischenergebnis wird in der Variablen "zwischenergebnis" gespeichert
		zwischenergebnis = zwischenergebnis * 10;
	}
    return zwischenergebnis;
}