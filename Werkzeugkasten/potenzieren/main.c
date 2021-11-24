// Friedjof Noweck
// 01.09.2021 Mi

float potenzieren(int basis, int potenz);

int main()
{
    potenzieren(10, -5);

    return 0;
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
