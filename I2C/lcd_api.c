void Init_LCD()
{
	int index1;

	//Funktion definieren: 2  zeilig, 5x7 Dots
	cmd = 0x38;
	for(index1=0;index1<7;index1++);//ca. 50ys warten (mind. 39)
	//Display ON,Cursor OFF, Blink OFF
	cmd = 0x0C;
	for(index1=0;index1<7;index1++);//ca. 50ys warten
	
	Clear_LCD();
	
	//Entry Mode: autom Increment ohne Shift
	cmd = 0x06;
	for(index1=0;index1<7;index1++);//ca. 50ys warten
}
void Clear_LCD()
{
	int index1;
	cmd = 0x01;
	for(index1=0;index1<370;index1++);//ca. 2ms warten (mind. 1.57)
}
void write_char(char value)
{
	int index1;
  if(value == 0)
	{
		return;
	}
	if(value != '\n')
	{
		chr = value;
	}
	else
	{
		cmd = 0xC0; //->  Adresse auf die zweite Zeile setzen: 1 ( 1 0 0   0 0 0 0) 
			    //-> DRAM Adresse 0x40!
	}
	for(index1=0;index1<7;index1++);//ca. 50ys warten
}
void print_str_lcd(char* value)
{
	int index;

	Clear_LCD();

	for(index = 0;index < 4;index++)
	{
				write_char(value[index]);
	}
	for(index=0;index<1000;index++);//warten
}