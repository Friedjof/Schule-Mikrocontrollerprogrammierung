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


/*********************** globale Variablen ****************************/

/************************** Prototypen ********************************/


// Main Funktion
void main()
{
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
    
    // Haupt-Schleife
    while (1)
    {
        // Inkrementalgeber auslesen
        // Der Port des Inkrementalgebers wird abgefragt
        specialButtonsResult = ((P3 & 0x18) >> 0x02) | ((P3 & 0x20) >> 0x05);

        // Wenn die gespeichten Werte des Inkrementalgebers von den aktuell gemessenen unterscheiden und beide gespeicherten Werte auf 1 stehen:
        if (((specialButtonsResult & 0x03) ^ ((specialButtons & 0x30) >> 0x04)))
        {
						P1 = 0x02 ^ P1;

            // Setze den gespeicherten Wert des Inkrementalgebers auf den gerade gemessenen Wert
            specialButtons = (specialButtons & (0xCF | ((specialButtonsResult & 0x03) << 0x04))) | (0x30 & ((specialButtonsResult & 0x03) << 0x04));
        }
        else
        { }
    }
}