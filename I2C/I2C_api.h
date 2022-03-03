xdata unsigned char at 0xffa0 i2c_reg;
xdata unsigned char at 0xffa1 i2c_s1;

xdata unsigned char at 0 xmem[0x0feff];


void i2c_init(void);
void i2c_send_byte(unsigned char slave_addr, unsigned char length, unsigned char * buffer);
unsigned char i2c_receive_byte(unsigned char slave_addr, 
                               unsigned char word_addr,
                               unsigned char length, 
                               unsigned char * buffer);