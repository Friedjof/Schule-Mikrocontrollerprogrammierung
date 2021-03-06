// Friedjof Noweck
// 2021-10-30 Sat

float potenzieren(int basis, int potenz);
void float2string(float zahl, char* text);
int stringLength(char* given_string);
float absFloat(float value);

int main()
{
    float num = 1234.0;
    char text[6] = "000000";

    // convertieren einer ganzen Zahl zu einem Text
    float2string(-10.5893534, text);

    return 0;
}

void float2string(float zahl, char *text)
{
    char modulo = 0;
    int zwischenergebnis = absFloat(zahl);
    float nachkomma = absFloat(zahl) - zwischenergebnis;
    int nachkommastellen = 0;
    int index = stringLength(text);
    int negativ = 0;
    int lengthInt = 0;
    int roundBy = 0;
    int i = 0;

    // Eigangszahl negativ?
	if (zahl < 0)
    {
        text[0] = '-';
        negativ = 1;
	}

    // Länge der Zahl vor dem Komma bestimmen
    while (zwischenergebnis > 0)
    {
        lengthInt++;
        modulo = zwischenergebnis % 10;
        
        zwischenergebnis = zwischenergebnis / 10;
    }

    zwischenergebnis = absFloat(zahl);
    
    // Gibt es stellen nach dem Komma?
    if (nachkomma > 0.0)
    {
        text[lengthInt + negativ] = '.';

        roundBy = stringLength(text) - (lengthInt + 1 + negativ);

        if (roundBy > 9)
        {
            roundBy = 9;
        }
        else if (roundBy < 0)
        {
            roundBy = 0;
        }

        for (i = 0; i <= roundBy; ++i)
        {
            nachkommastellen = nachkomma * potenzieren(10, i);
        }

        // Aufrunden?
        if ((int)(nachkomma * potenzieren(10, i)) % 10 >= 5)
        {
            nachkommastellen += 1;
        }
        else
        { }

        if (nachkommastellen >= potenzieren(10, roundBy) || roundBy == 0)
        {
            zwischenergebnis += 1;
            text[lengthInt + negativ] = '\0';
        }
        else
        {
            index = lengthInt + roundBy + 1;

            i = !negativ;
            while (nachkommastellen > 0)
            {
                modulo = nachkommastellen % 10;
                text[index - i] = modulo + 48;
                i++;
                
                nachkommastellen = nachkommastellen / 10;
            }
            text[index - i + roundBy + 1] = '\0';
        }
    }


    for (; lengthInt > 0; lengthInt--)
    {
        modulo = zwischenergebnis % 10;
        if (!negativ)
        {
            text[lengthInt - 1] = modulo + 48;
        }
        else
        {
            text[lengthInt] = modulo + 48;
        }
        zwischenergebnis = zwischenergebnis / 10;
    }

}

float absFloat(float value)
{
    if (value < 0)
    {
        return value * (-1);
    }
    else
    {
        return value;
    }
}

float potenzieren(int basis, int potenz)
{
	// init index
	int positivePotenz = potenz;
	int index = 0;
	float zwischenergebnis = 1;

	if (potenz < 0) {
		positivePotenz = potenz * (-1);
	}

	// die Potenz in einer for-Schleife berechnen
	for (index = 0; index < positivePotenz; index = index + 1)
	{
		// das Zwischenergebnis wird in der Variablen "zwischenergebnis" gespeichert
		zwischenergebnis = zwischenergebnis * basis;
	}

	if (potenz < 0)
	{
		zwischenergebnis = 1 / zwischenergebnis;
	}
	
	return zwischenergebnis;
}

// Source => https://www.geeksforgeeks.org/length-string-using-pointers/
int stringLength(char* given_string)
{
    // variable to store the
    // length of the string
    int length = 0;

    while (*given_string != '\0') {
        length++;
        given_string++;
    }
  
    return length;
}
