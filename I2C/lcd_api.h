volatile char xdata cmd _at_ 0xFF80;
volatile char xdata chr _at_ 0xFF82;

void Clear_LCD();
void Init_LCD();
void print_str_lcd(char* value);
void write_char(char value);