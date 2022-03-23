//Init I2C 
void i2c_init(void)
{
  i2c_s1 = 0x80;  // Select internal register S0
  i2c_reg = 0x55; // S0 = 0x55 (set effective own address to AA)
  i2c_s1 = 0x0a0; // Select internal register S2
  i2c_reg = 0x1c; // System Clock is 12 MHz; SCL = 90 kHz
  i2c_s1 = 0x0c1; // Enable interface and select S0
}

//send Byte to slave
void i2c_send_byte(unsigned char slave_addr, unsigned char length, unsigned char * buffer)
{
  unsigned char i;
  while ((i2c_s1 & 1) ==0) {};
  i2c_s1 = 0x0c5;         // START condition
  i2c_reg = slave_addr;   // Set slave address
  for (i=0; i<length; i++)
  {
    while ((i2c_s1 & 0x80) !=0);        // poll for ready
    if ((i2c_s1 & 0x08) !=0) break;     // if ack, leave loop
    i2c_reg = buffer[i];                // send data
  }
  while ((i2c_s1 & 0x80) !=0);          // poll for ready
 i2c_s1 = 0x0c3; // sTOP
}

//reveive Byte from Slave
unsigned char i2c_receive_byte(unsigned char slave_addr, 
                               unsigned char word_addr,
                               unsigned char length, 
                               unsigned char * buffer)
{
  unsigned char i;
  unsigned char error = 0;
  while ((i2c_s1 & 1) ==0) {}; // wait for free bus

  i2c_s1 = 0x0c5;
  i2c_reg = slave_addr;
    while ((i2c_s1 & 0x80) !=0);
    if ((i2c_s1 & 0x08) !=0) { // no slave ACK
      i2c_s1 = 0x0c3; // sTOP
      return 1;
    }
  i2c_reg = word_addr;
    while ((i2c_s1 & 0x80) !=0);
    if ((i2c_s1 & 0x08) !=0) { // no slave ACK
      i2c_s1 = 0x0c3; // sTOP
      return 1;
    }

  i2c_s1 = 0x0c5;         // START condition
 
  i2c_reg = slave_addr+1; // Set slave address
  for (i=0; i<length; i++)
  {
    while ((i2c_s1 & 0x80) !=0);  // poll for ready
    if ((i2c_s1 & 0x08) !=0) { // no slave ACK
      error = 1;
      break;
    }
  buffer[i] = i2c_reg;
  }
  if (error == 0)
  {
    i2c_s1 = 0x040; // prepare NACK
    buffer[i] = i2c_reg; // read final byte
    while ((i2c_s1 & 0x80) !=0);
  }
  i2c_s1 = 0x0c3; // sTOP
  return error;
}