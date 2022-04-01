// Friedjof Noweck
// 2021-10-29 Fri

float potenzieren(int basis, int potenz);
int string_length(char* given_string);
float str2int(char *string);

int main()
{
    char str[8] = "12.50656";
    float result = 0.0;

    result = str2int(str);

    return 0;
}

float str2int(char *string)
{
    int size = string_length(string);
    int index = 0;
    float entireNumber = 0;
    int singleNumber = 0;
    int ifSign = 0;
    int behindCommer = 0;
    char dot = 0x00;

    if (string[0] == '+' || string[0] == '-')
    {
        ifSign = 1;
    }

    for (;index + ifSign < size; index++)
    {
        if (string[size - (index + 1)] == '.')
        {
            dot = 0x01;
            behindCommer = index;
        }
        else
        {
            singleNumber = string[size - (index + 1)] - 48;
            if (dot == 0x01)
            {
                entireNumber += singleNumber * potenzieren(10, index - 1);
            }
            else
            {
                entireNumber += singleNumber * potenzieren(10, index);
            }
        }
    }

    if (string[0] == '-')
    {
        entireNumber *= -1;
    }

    entireNumber /= potenzieren(10, behindCommer);

    return entireNumber;
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

    // wenn die Potenz negativ ist, dann teile 1 durch das "zwischenergebnis"
	if (potenz < 0)
	{
		zwischenergebnis = 1 / zwischenergebnis;
	}

	return zwischenergebnis;
}

// Source => https://www.geeksforgeeks.org/length-string-using-pointers/
int string_length(char* given_string)
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
