// Friedjof Noweck
// 2021-10-29 Fri

void delay(int ms);


int main()
{
    int steps = 0;


    for (; steps < 10; steps++)
    {
        delay(1000);
    }

    return 0;
}

void delay(int ms)
{
    int x = 0;
    int y = 0;

    for (; x < ms*2; x++)
    {
        for (y = 0; y < 1000; y++)
        { }
    }
}
