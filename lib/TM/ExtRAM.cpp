/*
    External RAM 23A512/23LC512 driver
    Written by Kevin Witteveen
*/

#ifdef TEST_MODULE

#include <ExtRAM.h>

void ExtRAM::set_bytes(uint16_t start, uint8_t value, uint16_t count )
{
  set_cs(true);
  write_address(start);
  for(uint16_t i=0;i<count;i++)
  {
    write(value);
  }
  set_cs(false);  
}

void ExtRAM::write_address(uint16_t address)
{
  SPI.beginTransaction(SPISettings(RAM_SPEED, true, SPI_MODE0));
  
  SPI.transfer(EXTRAM_REG_WRITE);
  SPI.transfer16(address);

  SPI.endTransaction();  
}

void ExtRAM::read_address(uint16_t address)
{
  SPI.beginTransaction(SPISettings(RAM_SPEED, true, SPI_MODE0));
  
  SPI.transfer(EXTRAM_REG_READ);
  SPI.transfer16(address);

  SPI.endTransaction();  
}

void ExtRAM::write_reg(uint8_t reg, uint8_t byte)
{
  SPI.beginTransaction(SPISettings(RAM_SPEED, true, SPI_MODE0));

  SPI.transfer(reg);
  SPI.transfer(byte);

  SPI.endTransaction();
}

uint8_t ExtRAM::read_reg(uint8_t reg)
{
  SPI.beginTransaction(SPISettings(RAM_SPEED, true, SPI_MODE0));

  SPI.transfer(reg);
  uint8_t b = SPI.transfer(0x00);

  SPI.endTransaction();

  return b;
}

int ExtRAM::set_mode(uint8_t mode)
{
  // Set
  set_cs(true);
  delay(2);
  write_reg(EXTRAM_REG_WRMR, mode);
  set_cs(false);

  delay(2);

  // Read and verifiy
  set_cs(true);
  delay(2);
  uint8_t read = read_reg(EXTRAM_REG_RDMR);
  set_cs(false);

  Serial.print(read);

  delay(2);
  if(read!=mode)
  {
    return 1;
  }
  return 0;
}



void ExtRAM::write(uint8_t byte)
{
  SPI.beginTransaction(SPISettings(RAM_SPEED, true, SPI_MODE0));

  SPI.transfer(byte);

  SPI.endTransaction();
}

uint8_t ExtRAM::read()
{
  SPI.beginTransaction(SPISettings(RAM_SPEED, true, SPI_MODE0));

  uint8_t b = SPI.transfer(0x00);

  SPI.endTransaction();

  return b;
}

void ExtRAM::write_byte(uint16_t address, uint8_t byte)
{
  set_cs(true);
  write_address(address);
  write(byte);
  set_cs(false);
  //delay(1);
}

uint8_t ExtRAM::read_byte(uint16_t address)
{
  set_cs(true);
  read_address(address);
  uint8_t byte = read();
  set_cs(false);
  //delay(1);

  return byte;
}

void ExtRAM::write_bytes(uint16_t address, const uint8_t* source, uint16_t count)
{
  set_cs(true);
  write_address(address);
  for(uint16_t i=0;i<count;i++)
  {
    write(source[i]);
  }
  set_cs(false);
}
void ExtRAM::read_bytes(uint16_t address, uint8_t* destination, uint16_t count)
{
  set_cs(true);
  read_address(address);
  for(uint16_t i=0;i<count;i++)
  {
    destination[i]=read();
  }
  set_cs(false);
}

void ExtRAM::reset()
{
  set_cs(true);
  write(EXTRAM_REG_RSTIO);
  set_cs(false);
}

ExtRAM extram;


#endif