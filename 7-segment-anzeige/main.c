/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:          Friedjof Noweck
Klasse:         IH119
Datum:          2021-12-10
Datei:          main.c
Einsatz:        Schulung
Beschreibung:   Ein Zähler für die 7 Segment Anzeigen + Eingabe auf
                der 4x4 Taster Matrix, Inkrementalgeber, 2 Taster
                (P5.0 & P6.0)
Funktionen:     main (mit Setup & main loop), display (zeigt eine
                Zahl an der spezifischen Stelle an),
                readButtonMatrix (ließt die 4x4 Taster Matrix aus),
                getNumber (gibt die Ziffer der, gegebenen Zahl an
                4x4 Taster Matrix zu Zahlen auf), fixPotenz
                (errechnet die Zahl für die gegebene Potenz und die
                Basis zehn), IRQ_Timer0 (die Interrupt Funktion
                lässt die Variable "timer" hochzählen)

+-------------------------------------------------------------------+
| Anleitung:                                                        |
|  Der Controller starte im Editier-Modus. Erkennbar auch an dem    |
|  blickenden Cursor. Hier können Zahlen über die Matrixtastatur    |
|  gesetzt werden. Der Cursor wandert nach rechts weiter.           |
|  Alternativ kann auch über die Button-Matrix eine exakte Position |
|  festgelegt werden (A 1000er, B 100er, C 10er, D 1er). Auch kann  |
|  die Zahl in dem Editor Modus per Inkrementalgeber hoch bzw.      |
|  runter geregelt werden. Der Cursor wird hierbei nicht beachtet.  |
|                                                                   |
|  Durch das Drücken vom vorderen Taster [P5.0] wird der Zähler     |
|  gestartet. Die Richtung kann auch umgekehrt werden mit dem       |
|  hinteren Taster [P6.0]. Durch das erneute Drücken auf den        |
|  vorderen Taster wird der Zähler wieder angehalten und der        |
|  Editiermodus gestartet. Beim laufenden Zähler ist die            |
|  Button-Matrix deaktiviert.                                       |
|                                                                   |
|  Die Zählgeschwindigkeit kann geändert werden, indem der          |
|  Inkrementalgeber beim laufenden Zähler hoch bzw. runter geregelt |
|  wird.                                                            |
|                                                                   |
|  Durch das Drücken auf den Inkrementalgeber wird der Timer Modus  |
|  aktiviert. Auch erkennbar an dem leuchtenden Licht [P1.2]. Dies  |
|  bedeutet, dass bei dem Wert 0 angehalten wird und das Display    |
|  aufblickt. Dieser Modus wird deaktiviert, wenn  während des      |
|  Blickens eine der beiden anderen Taster oder die 4x4 Taster      |
|  Matrix gedrückt wird. Im Anschluss daran wird der Editor         |
|  aufgerufen. Durch erneutes Drücken des Inkrementalgeber wird     |
|  dieser Modus ebenfalls wieder deaktiviert und der Zähler läuft   |
|  weiter.                                                          |
+-------------------------------------------------------------------+
| Quellen:                                                          |
| ● Timer beim Arduino                                              |
|   ● https://www.exp-tech.de/blog/arduino-tutorial-timer           |
| ● Arduino Mega Ports                                              |
|   ● https://aws1.discourse-cdn.com/arduino/original/4X/6/f/b/6fb6102c3ff917a32b3fabaa7b01c72fb208919e.png
| ● Port Adressierung beim Arduino:                                 |
|   ● https://hartmut-waller.info/arduinoblog/leds-schalten-port-ddr/
+-------------------------------------------------------------------+
| PINBELEGUNG                                                       |
+------------------------------+--------------------+---------------+
| Elemente                     |    Arduino Mega    |    REG517A    |
|                              |   Ports      Pins  |               |
+------------------------------+-----------+--------+---------------+
| ● 4x4 Taster Matrix          |           |        |               |
|   ● ROWS                     | PORTA.0-3 | D22-25 | P6.4-7        |
|   ● COLUMNS                  | PORTA.4-7 | D26-29 | P8.0-3        |
| ● 4x 7 Segment Anzeigen      |           |        |               |
|   ● Segmente                 | PORTL.0-7 | D42-49 | P4.0-7        |
|   ● Anzeigen                 | PORTB.0-3 | D50-53 | P6.4-7        |
| ● 2x Taster                  |           |        |               |
|   ● Start/Stopp              | PORTG.0   | D41    | P5.0          |
|   ● Richtungswechsel         | PORTG.1   | D40    | P6.0          |
| ● Inkrementalgeber           |           |        |               |
|   ● Taster - Timer Mode      | PORTF.2   | A2     | P3.4          |
|   ● Inkrementalgeber Takt    | PORTF.0-1 | A0-1   | P3.3&5        |
+------------------------------+-+---------+--------+---------------+
| Belegung der 4x4 Taster Matrix |                                  |
+--------------------------------+                                  |
|                                                                   |
|             | 1  2  3  4 | Cursor Steuerung per Taste |           |
|           --+------------+----------------------------+           |
|           A | V  1  2  3 | V = 1000er Stelle          |           |
|           B | X  4  5  6 | X = 100er Stelle           |           |
|           C | Y  7  8  9 | Y = 10er Stelle            |           |
|           D | Z  <  0  > | Z = 1er Stelle             |           |
|           --+------------+----------------------------+           |
|                                                                   |
+------------------+------------------------------------------------+
| Inkrementalgeber |                                                |
+------------------+                                                |
|                                                                   |
|                            Timer Mode                             |
|                                ⮟                                 |
|      Editor - Zahl    +1  ⮜──  +  ──⮞  -1                        |
|      Zähler - Speed   +5  ⮜──  +  ──⮞  -5                        |
|                               ___                                 |
|                             +     +                               |
|                            |   ●   |                              |
|                             +     +                               |
|                               \_/                                 |
|                                                                   |
+-------------------------------------------------------------------+

***********************************************************************
Änderungen:
2021-12-10 init project + add simple functions
2021-12-14 work on the functions
2022-01-19 migation to the school controller
2022-02-06 Add Timer0 and memory optimization
2022-02-09 Fix edit mode bug
2022-02-10 Save cache and add timer mode

***********************************************************************/
/******************* Text im Quelltext einbinden **********************/
#include "REG517A.h"

/*************************** Konstanten *******************************/
// Maximal darstellbare Zahl
#define maxNumber 9999


/*********************** globale Variablen ****************************/
// Zähler zum Speichern von Zeit
unsigned int timer = 0;

/************************** Prototypen ********************************/
char readButtonMatrix(char maxRows, char maxColumns);
void display(char number, char segment);
char index2number(char index);
char getNumber(int numbers, char index);
int fixPotenz(char potenz);
void IRQ_Timer0();


// Main Funktion
void main()
{
    // Konstanten
    // Die Anzahl an vorliegenden 7-Segment-Anzeigen
    const char nrOfSegments = 0x04;

    // Speichert die Zahl, welche auf dem Display angezeigt werden soll
    int number2display = 0x00;

    // Zählgeschwindigkeit
    unsigned char speed = 0x32;

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
    // Speichert den Zeitpunkt der letzten Aktualisierung des Zählerwertes
    unsigned int counterTimer = timer;
    // Speichert die Ziffer des aktuellen Segments
    char number = 0x00;

    /* +-------------------------------------------------------------------+
     * | Speichert die Konfiguration der einzelnen Ziffern auf der         |
     * | 7-Segment-Anzeige. So könne einstellige Zahlen einfach der        |
     * | Ziffern Konfiguration der 7-Segment-Anzeigen zugeordnet werden.   |
     * +------------------------+-----------------+------------------------+
     * | REG517A                |     Display     | Arduino Mega           |
     * +------------------------+                 +-----------------+------+
     * | c P f e d g b a | HEX  |                 | P g f e d c b a | HEX  |
     * +-----------------+------+-----------------+-----------------+------+
     * | 1 0 1 1 1 0 1 1 | 0xBB |        0        | 0 0 1 1 1 1 1 1 | 0x3F |
     * | 1 0 0 0 0 0 1 0 | 0x82 |        1        | 0 0 0 0 0 1 1 0 | 0x06 |
     * | 0 0 0 1 1 1 1 1 | 0x1F |        2        | 0 1 0 1 0 1 1 1 | 0x5B |
     * | 1 0 0 0 1 1 1 1 | 0x8F |        3        | 0 1 0 0 1 1 1 1 | 0x4F |
     * | 1 0 1 0 0 1 1 0 | 0xA6 |        4        | 0 1 1 0 0 1 1 0 | 0x66 |
     * | 1 0 1 0 1 1 0 1 | 0xAD |        5        | 0 1 1 0 1 1 0 1 | 0x6D |
     * | 1 0 1 1 1 1 0 1 | 0xBD |        6        | 0 1 1 1 1 1 0 1 | 0x7D |
     * | 1 0 1 0 0 0 1 1 | 0xA3 |        7        | 0 0 0 0 0 1 1 1 | 0x07 |
     * | 1 0 1 1 1 1 1 1 | 0xBF |        8        | 0 1 1 1 1 1 1 1 | 0x7F |
     * | 1 0 1 0 1 1 1 1 | 0xAF |        9        | 0 1 1 0 1 1 1 1 | 0x6F |
     * +-----------------+------+-----------------+------------------------+
     * |                                 a                                 |
     * |                             ---------                             |
     * |                            |         |                            |
     * |                          f |         | b                          |
     * |                            |    g    |                            |
     * |                             ---------                             |
     * |                            |         |                            |
     * |                          e |         | c                          |
     * |                            |         |                            |
     * |                             ---------   ●                         |
     * |                                 d        P                        |
     * +-------------------------------------------------------------------+
     */
    const unsigned char segments[10] = {0xBB, 0x82, 0x1F, 0x8F, 0xA6, 0xAD, 0xBD, 0xA3, 0xBF, 0xAF};

    // Speichert die aktuellen Curor Position ab
    // Ab Start liegt der Wert auf der ganz linken 7-Segment-Anzeige.
    unsigned char cursor = nrOfSegments - 0x01;

    /* +----------------------------------------------------+--------------+
     * | Speichert folgende wichtige Werte [specialButtons] |              |
     * +----------------------------------------------------+              |
     * |                                                                   |
     * | Bit Index | Beschreibung                                          |
     * +-----------+-------------------------------------------------------+
     * |     7     | Timer Modus ( 1 = an & 0 = aus)                       |
     * |     6     | Inkrementalgeber Taster Zwischenspeicher              |
     * |     5     | Inkrementalgeber 02 Zwischenspeicher                  |
     * |     4     | Inkrementalgeber 01 Zwischenspeicher                  |
     * |     3     | Zähl-Richtung (1 = runter & 0 = rauf)                 |
     * |     2     | Zähler Modus  (1 = Zähler & 0 = Editor)               |
     * |     1     | Richtungs Taster Zwischenspeicher                     |
     * |     0     | Zähler Modus Taster Zwischenspeicher                  |
     * +-----------+-------------------------------------------------------+
     */
    unsigned char specialButtons = 0x43;

    // Dient als temporärer Zwischenspeicher von ermitteten Werte der verschiedenen Taster
    unsigned char specialButtonsResult = 0x00;

    // Schaltet die Timer-Mode-LED aus
    P1 = P1 & 0x0FD;
    
    // Timer 0 ausgeschaltet
    TR0 = 0;
    // Überlauf zurückgesetzt von Timer 0
    TF0 = 0;
    // IR gelöscht von Timer 0
    IT0 = 0;
    // Timer 1: Timer, 8bit prescale, Timer0: Timer, 16bit
    TMOD = 0x01;
    // Setze den Startwert von Timer 0 auf 64535
    // Somit benötigt der Timer eine ms für einen Durchlauf
    TL0 = 0x17;
    TH0 = 0x0FC;
    
    // IR für Timer 0 aktivieren
    ET0 = 1;
    
    // Interrupt System aktivieren
    EAL = 1;
    // Timer 0 aktiv
    TR0 = 1;

    // Haupt-Schleife
    while (1)
    {
        // Wenn die vergangene Zeit seit der letzten Abfrage der seperaten Taster, sowie der Button Matrix größer 50ms:
        if (timer - buttonTimeout > 0x32)
        {
            // Wenn Zähler nicht aktiv:
            if (!((specialButtons & 0x04) >> 0x02) || ((specialButtons & 0x80) >> 0x07 && !number2display && (specialButtons & 0x04)))
            {
                // Matrix abfragen
                matrixResult = readButtonMatrix(0x04, 0x04);
            }
            else
            { }
            
            // Zum Abfragen der Taster an Port 5.0 und 6.0 werden diese zunächst auf 1 gesetzt.
            P6 = P6 | 0x03;
            
            // Abfrage der beiden seperaten Taster
            specialButtonsResult = (P5 & 0x01) | ((P6 & 0x01) << 0x01);

            // Wenn sich der rechte Taster an Port P5.0 verändert hat:
            if ((specialButtons & 0x01) ^ (specialButtonsResult & 0x01))
            {
                    // Aktualisiere das gespeichert Bit des Tasters an Port P5.0 [specialButtons: Bit Index 0]
                    specialButtons = (specialButtons & (0x0FE | (specialButtonsResult & 0x01))) | (0x01 & (specialButtonsResult & 0x01));

                    // Wenn eine abfallende Flanke vorliegt:
                    if (!(specialButtons & 0x01))
                    {
                        // Setze der Cursor ab die ganz linke Position
                        cursor = nrOfSegments - 0x01;

                        // Wenn der Timer Mode an ist und der wert auf dem Display 0 ist (kurz: wenn das Display Blickt)
                        if (((specialButtons & 0x80) >> 0x07) && !number2display && (specialButtons & 0x04))
                        {
                            // Timer Modus Bit löschen
                            specialButtons = specialButtons & 0x7F;
                            P1 = P1 & 0x0FD;
                        }
                        else
                        { }
                                                    
                        // Drehe das stop/start Bit um
                        specialButtons = 0x04 ^ specialButtons;
                    }
                    else
                    { }
            }
            else
            { }

            // Wenn sich etwas am linken Taster an Port P6.0 verändert hat:
            if ((specialButtons & 0x02) ^ (specialButtonsResult & 0x02))
            {
                    // Aktualisiere das gespeichert Bit des Tasters an Port P6.0 [specialButtons: Bit Index 1]
                    specialButtons = (specialButtons & (0x0FD | (specialButtonsResult & 0x02))) | (0x02 & (specialButtonsResult & 0x02));

                    // Wenn eine abfallende Flanke vorliegt:
                    if (!(specialButtons & 0x02))
                    {
                        // Wenn Timer Mode an ist und der wert auf dem Display 0 ist (kurz: wenn das Display Blickt)
                        if ((specialButtons & 0x80) >> 0x07 && !number2display && (specialButtons & 0x04))
                        {
                            // Timer Modus Bit löschen
                            specialButtons = specialButtons & 0x7F;
                            P1 = P1 & 0x0FD;
                        }
                        else
                        { }
                        // Drehe das richtungs Bit um
                        specialButtons = 0x08 ^ specialButtons;
                    }
                    else
                    { }
            }
            else
            { }

            // Wenn ein Taster der Matrix Tastatur gedrückt wurde und sich der Zustand geändert hat:
            if (matrixResult && (currentButtonState ^ matrixResult))
            {
                // Neuen Matrix-Tastatur-Status setzen
                currentButtonState = matrixResult;

                // Wenn Timer Mode an ist und der wert auf dem Display 0 ist (kurz: wenn das Display Blickt)
                if ((specialButtons & 0x80) >> 0x07 && !number2display && (specialButtons & 0x04))
                {
                    // Timer Modus Bit löschen
                    specialButtons = specialButtons & 0x7F;
                    P1 = P1 & 0x0FD;
                
                    specialButtons = specialButtons & 0x0FB;
                }
                else
                {
                    // Versetzen des Cursors
                    // Wenn A gedrückt:
                    if (currentButtonState == 0x0D)
                    {
                        // Setze Cursor auf 1000er Stelle
                        cursor = nrOfSegments - 0x04;
                    }
                    // Sonst wenn B gedrückt:
                    else if (currentButtonState == 0x0E)
                    {
                        // Setze Cursor auf 100er Stelle
                        cursor = nrOfSegments - 0x03;
                    }
                    // Sonst wenn C gedrückt:
                    else if (currentButtonState == 0x0F)
                    {
                        // Setze Cursor auf 10er Stelle
                        cursor = nrOfSegments - 0x02;
                    }
                    // Sonst wenn D gedrückt:
                    else if (currentButtonState == 0x10)
                    {
                        // Setze Cursor auf 1er Stelle
                        cursor = nrOfSegments - 0x01;
                    }
                    // Sonst wenn * gedrückt:
                    else if (currentButtonState == 0x0C)
                    {
                        if (cursor < (nrOfSegments - 0x01))
                        {
                            cursor++;
                        }
                        else
                        {
                            cursor = 0x00;
                        }
                    }
                    // Sonst wenn # gedrückt:
                    else if (currentButtonState == 0x04)
                    {
                        if (cursor > 0x00)
                        {
                            cursor--;
                        }
                        else
                        {
                            cursor = nrOfSegments - 0x01;
                        }
                    }
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
            }
            // Sonst, wenn keine Taste gedückt, allerdings noch ein anderer Wert außer 0 gespeichert ist (kurz: Bei einer fallende Flanke
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
        specialButtonsResult = ((P3 & 0x18) >> 0x02) | ((P3 & 0x20) >> 0x05);

        // Inkrementalgeber Button
        // Wenn sich der gespeicherte und der gerade abgefragte Zustand unterscheiden:
        if ((specialButtonsResult & 0x04) ^ ((specialButtons & 0x40) >> 0x04))
        {
            // Bei einer fallenden Flanke //Wenn das Bit für den Inkrementalgeber Button gesetzt ist:
            if (specialButtonsResult & 0x04)
            {
                P1 = 0x02 ^ P1;

                // Drehe das Timer-Modus-Bit um
                specialButtons = 0x80 ^ specialButtons;
            }
            else
            { }

            // Speichere den aktuellen Wert des Inkrementalgeber Tasters ab
            specialButtons = (specialButtons & (0xBF | ((specialButtonsResult & 0x04) << 0x04))) | (0x40 & ((specialButtonsResult & 0x04) << 0x04));
        }
        else
        { }

        // Wenn die gespeichten Werte des Inkrementalgebers von den aktuell gemessenen unterscheiden:
        if (((specialButtonsResult & 0x03) ^ ((specialButtons & 0x30) >> 0x04)))
        {
            // Wenn beide gespeicherten Werte auf 1 stehen:
            if ((specialButtons & 0x30) / 0x30)
            {
                // Wenn das erste aktuelle Bit 1 und das zweite aktuelle Bit 0 ist  (bzw. nach rechts gedreht wird)
                if ((specialButtonsResult & 0x01) && !((specialButtonsResult & 0x02) >> 0x01))
                {
                    // Wenn der Zähler aktuell läuft ist:
                    if ((specialButtons & 0x04) >> 0x02)
                    {
                        // Wenn die Zähl-Gewindigkeit aktuell kleiner oder gleich 64ms ist:
                        if (speed < 0x0F0)
                        {
                            // Setze die Zähl-Gewindigkeit auf das doppelte der aktuellen Geschwindigkeit
                            speed += 0x0A;
                        }
                        else
                        { }
                    }
                    // Sonst, wenn der Zähler gestoppt ist (bzw. der Editor-Modus aktiv ist)
                    else
                    {
                        // Wenn der aktuelle Zählwert plus 10 nicht den maximalen Zählerwert überschreitet:
                        if (number2display < maxNumber)
                        {
                            // Addiere 10 auf den aktuelle Zählerwert drauf
                            number2display++;
                        }
                        // Sonst, wenn aktuelle Zählwert plus 10 den maximalen Zählerwert überschreitet:
                        else
                        {
                            // Setze den Zählerwert wieder auf 0 zurück
                            number2display = 0x00;
                        }
                    }
                }
                // Sonst, wenn das erste aktuelle Bit 0 und das zweite aktuelle Bit 1 ist (bzw. nach links gedreht wird)
                else if (!(specialButtonsResult & 0x01) && ((specialButtonsResult & 0x02) >> 0x01))
                {
                    // Wenn der Zähler aktiv ist:
                    if ((specialButtons & 0x04) >> 0x02)
                    {
                        // Wenn die aktuelle Zählgeschwindigkeit größer ist als 1ms:
                        if (speed > 0x0A)
                        {
                            // Halbiere die aktuelle Zählgeschwindigkeit
                            speed -= 0x0A;
                        }
                        else
                        { }
                    }
                    // Sonst, wenn der Zähler aktuell gestoppt ist (bzw. der Editor Modus aktiv ist)
                    else
                    {
                        // Wenn die aktuell angezeigte Zahl minus 10 die Grenze von 0 nicht unterschreitet:
                        if (number2display > 0x00)
                        {
                            // Ziehe von der aktuell angezeigten Zahl 10 ab
                            number2display--;
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
        if ((specialButtons & 0x80) && !number2display && (specialButtons & 0x04))
        {
            // Wenn der Timer % 50 kleiner 10 ist:
            if ((timer % 1000) < 500)
            {
                // Zeige die Nummer des aktuellen Segments an
                display(segments[(int)number], segmentCounter);
            }
            // Sonst, wenn der Timer kleiner als 500ms ist:
            else
            {
                // Zeige nichts auf dem aktuellen Segment an
                display(0x00, segmentCounter);
            }
        }
        // Sonst, wenn der Timer Modus nicht aktiv ist und der Zählerwert nicht 0 ist:
        else
        {
            // Wenn das aktuelle Segment nicht dem aktuellen Werte des Cursors entspricht oder der Editor Modus nicht aktiv ist:
            if (((specialButtons & 0x04) >> 0x02) || segmentCounter != cursor)
            {
                // Zeige die aktuelle Ziffer auf dem aktuellen Segment an
                display(segments[(int)number], segmentCounter);
            }
            // Wenn der Timer % 50 kleiner 10 ist:
            else if ((timer % 1000) < 500)
            {
                // Zeige die aktuelle Ziffer auf dem aktuellen Segment an
                display(segments[(int)number], segmentCounter);
            }
            // Sonst, wenn der Timer kleiner als 500ms ist und wenn das aktuelle Segment nicht dem aktuellen Werte des Cursors entspricht oder der Editor Modus nicht aktiv ist:
            else
            {
                // Zeige nichts auf dem aktuellen Segment an
                display(0x00, segmentCounter);
            }
        }
        
        // Wenn die Differenz zwischen der aktuelle und dem letzten Zählvorgang größer als die Zählgeschwindigkeit und der Timer aktiv ist:
        if ((timer - counterTimer) > speed && (specialButtons & 0x04))
        {
            // Speicher den Zeitpunkt der aktuellen Aktualisierung des Zählerwertes ab
            counterTimer = timer;

            if (((specialButtons & 0x80) >> 0x07) && number2display == 0)
            { }
            else
            {
                // Wenn der aktuelle Zählerstand der für die Richtung spezifischen Endzahl entspricht:
                if (number2display == maxNumber * ((0x08 ^ (specialButtons & 0x08)) >> 0x03)) //((((specialButtons & (0x0F7 | (0x08 ^ (specialButtons & 0x08)))) | (0x08 & (0x08 ^ (specialButtons & 0x08)))) & 0x08) >> 0x03))
                {
                    // Setze den Zählerwert auf die für die Richtung spezifischen Startwert zurück
                    number2display = maxNumber * ((specialButtons & 0x08) >> 0x03);
                }
                else
                {
                    number2display += (1 + ((-2) * ((specialButtons & 0x08) >> 0x03)));
                }
            }
        }
        else
        { }

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
    }
}

void IRQ_Timer0() interrupt 1
{
    TR0 = 0;
    EAL = 0;

    // Der Counter für die messbare Zeit wird um eins erhöht
    timer++;

    TL0 = 0x17;
    TH0 = 0x0FC;

    EAL = 1;
    TR0 = 1;
}

// Gibt die Zahl an dem gewünschten Index zurück
// Bsp.: numbers = 1234, index = 3, return => 1
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

    char row;
    char column;
    
    // Display ausschalten
    P4 = 0x00;

    for (row = 0; row < maxRows; row++)
    {
        // Setzen der Ausgänge (aktiver Eingang auf LOW)
        // 0b1110XXXX > 0b1101XXXX > 0b1011 > 0b0111XXXX
        P6 = (P6 & 0x0F) | (0x0F0 ^ (0x10 << row));

        for (column = 0; column < maxColumns; column++)
        {
            // Wenn an einem Eingang 0 anliegt
            if (!((P8 & (0x01 << column)) >> column))
            {
                // Gebe den Index des gedrückten Buttons zurück (1 - 16)
                return (maxColumns * row) + column + 0x01;
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
        case 0x08: return 0x00;
        case 0x03: return 0x09;
        case 0x07: return 0x08;
        case 0x0B: return 0x07;
        case 0x02: return 0x06;
        case 0x06: return 0x05;
        case 0x0A: return 0x04;
        case 0x01: return 0x03;
        case 0x05: return 0x02;
        case 0x09: return 0x01;
        default: return 0x00;
    }
}

// Setzt die Segment Konfiguration auf das angegebene Segment
void display(char segmentConfiguration, char segment)
{
    P6 = P6 | 0x0F0;
    
    // Setzen der einzelnen Segmente
    P4 = segmentConfiguration;
    
    // Setzen der Eingänge
    P6 = (P6 & 0x0F) | (0x0F0 ^ (0x10 << segment));
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