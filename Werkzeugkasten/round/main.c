// Friedjof Noweck
// 2021-10-30 Sat

float roundFloat(float number, int digits);
float absFloat(float value);
float potenzieren(int basis, int potenz);


int main()
{
    // Kann maximal auf 9 Nachkommastellen runden
    float rounded = roundFloat(1466.38766, 1);

    return 0;
}

float roundFloat(float number, int digits)
{
    int nx = number;
    int nnx = 0;
    float xn = absFloat(number) - absFloat(nx);
    
    int i = 9;

    if (digits < 0)
    {
        digits = 0;
    }
    else if (digits > 9)
    {
        digits = 9;
    }
    else
    { }

    nnx = absFloat(xn) * potenzieren(10, 9);

    while (i > digits)
    {
        if (nnx % 10 >= 5)
        {
            nnx /= 10;
            nnx += 1;
        }
        else
        {
            nnx /= 10;
        }
        i--;
    }


    if (nx >= 0)
    {
        return nx + (nnx * potenzieren(10, -digits));
    }
    else
    {
        return nx - (nnx * potenzieren(10, -digits));
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
