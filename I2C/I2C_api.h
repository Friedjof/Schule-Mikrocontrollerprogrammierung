void i2c_init(void);
void i2c_send_byte(unsigned char slave_addr, unsigned char length, unsigned char * buffer);
unsigned char i2c_receive_byte(unsigned char slave_addr,unsigned char word_addr,unsigned char length,unsigned char * buffer);