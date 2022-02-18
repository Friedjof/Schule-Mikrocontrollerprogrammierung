/*****************************************************************************
h e i n r i c h -h e r t z -b e r u f s k o l l e g  d e r  s t a d t  b o n n
Autor:          Friedjof Noweck
Klasse:         IH119
Datum:          2021-02-18
Datei:          main.c
Einsatz:        Schulung
Beschreibung:   Test für den Inkrementalgeber
Funktionen:     Bei jeder Veränderung des Inkrementalgebers ändert die
                LED/Brine den Zustand.

Erwartungen:
Wie im Datenblatt beschrieben: (vgl. labmultiv2.pdf S. 10 dritter Absatz)
"Das Programm erkennt, dass der Geber bewegt wurde daran, dass eine Leitung
ihren Zustand gegenüber dem letzten Abtastevorgang verändert hat".
Somit dürfte sich die LED/Birne nur beim Drehen des Inkrementalgeber verändern.
Dies ist unabhängig von der Drehrichtung.

Folgende Ergebnisse werden beobachtet:
Controller 3:
LED/Birne blinkt unabhängig von einer Aktion am Inkrementalgeber in einem festen
Rhythmus auf. Bei einer Bewegung des Inkrementalgebers schaltet die LED/Birne in
einer der Drehgeschwindigkeit angemessener Gewaltigkeit zwischen den Zuständen.

Controller X:
LED/Birne verändert den Status vorerst nicht. Bei einer Aktion am Inkrementalgeber
wechselt der Status der Drehgeschwindigkeit angemessener Gewaltigkeit zwischen den
Zuständen.

+-------------------------------------------------------------------+
| PINBELEGUNG                                                       |
+------------------------------+--------------------+---------------+
| Elemente                     |    Arduino Mega    |    REG517A    |
|                              |   Ports      Pins  |               |
+------------------------------+-----------+--------+---------------+
| ● Inkrementalgeber           |           |        |               |
|   ● Inkrementalgeber Takt    | PORTF.0-1 | A0-1   | P3.3&5        |
+------------------------------+-----------+--------+---------------+
***********************************************************************
Änderungen:
2022-02-18 Inkrementalgeber Tests

***********************************************************************/
/******************* Text im Quelltext einbinden **********************/
#include "REG517A.h"

/*************************** Konstanten *******************************/


/*********************** globale Variablen ****************************/

/************************** Prototypen ********************************/

void main()
{
        // Speichert das aktuelle Messergebnis ab
        char i01_Current_Status = 0x00;
        char i02_Current_Status = 0x00;

        // Speichert den vorherigen gemessenen Wert ab
        char i01_Last_Status = 0x00;
        char i02_Last_Status = 0x00;

    // Haupt-Schleife
    while (0x01)
    {
        // Inkrementalgeber auslesen
        // Die Ports des Inkrementalgebers werden abgefragt:
        // zB.:  10101010b
        //     & 00001000b
        //     = 00001000b >> 3 (Bitshift: Bit wird um drei nach rechts verschoben) => 00000001b
        i01_Current_Status = (P3 & 0x08) >> 0x03;
        // zB.:   10101010b
        //      & 00100000b
        //      = 00100000b >> 5 (Bitshift: Bit wird um fünf nach rechts verschoben) => 00000001b
        i02_Current_Status = (P3 & 0x20) >> 0x05;

        // Wenn sich mindestens einer der gespeicherten Werte des Inkrementalgebers von den aktuell gemessenen Werten unterscheiden:
        if (i01_Current_Status != i01_Last_Status || i02_Current_Status != i02_Last_Status)
        {
            // Drehe den Wert der LED bzw. der Glühbirne um
            // BSP.: (P1 = ) 10101010b ^ (Maske = ) 00000010b = 0x00 und (P1 = ) 01010101b ^ (Maske = ) 00000010b = 0x01
            P1 = 0x02 ^ P1;

            // Speichert den aktuellen Wert ab
            i01_Last_Status = i01_Current_Status;
            i02_Last_Status = i02_Current_Status;
        }
        else
        { }
    }
}