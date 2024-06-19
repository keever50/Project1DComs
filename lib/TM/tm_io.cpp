/*
    I/O interface
    Written by Kevin Witteveen
*/

#ifdef TEST_MODULE

#include <tm_io.h>

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// Override weak function of CLI
void cli_draw(cli_terminal_t *term)
{
  tm_draw_cli(tft, *term);
}

cli_terminal_t cli;

void cli_mem_write(cli_terminal_t *term, int address, uint8_t byte)
{

  // CRAM[address]=byte;
  extram.write_byte(address, byte);
}
uint8_t cli_mem_read(cli_terminal_t *term, int address)
{

  // return CRAM[address];
  return extram.read_byte(address);
}
void cli_mem_writes(cli_terminal_t *term, uint8_t *source, int address, int len)
{

  extram.write_bytes(address, source, len);

  // for(int i=0;i<len;i++)
  // {
  //   CRAM[address+i]=source[i];
  // }
}
void cli_mem_reads(cli_terminal_t *term, uint8_t *destination, int address, int len)
{

  extram.read_bytes(address, destination, len);

  // for(int i=0;i<len;i++)
  // {
  //   destination[i]=CRAM[address+i];
  // }
}

void Tm_io::init()
{
  SPI.begin();
  Wire.begin();
  pinMode(RAM_CS, OUTPUT);
  delay(50);
  // Set mode
  extram.reset();
  extram.set_mode(EXTRAM_MODE_SEQ);

  // Clean out ext ram graphic section
  extram.set_bytes(RAM_SECT_GRAPHICS_START, 0, RAM_SECT_GRAPHICS_END - RAM_SECT_GRAPHICS_START);
  extram.set_bytes(RAM_SECT_GRAPHICS_START, 0, RAM_SECT_GRAPHICS_END - RAM_SECT_GRAPHICS_START);
  // Clean out ext ram persistance graphic section
  extram.set_bytes(RAM_SECT_PERSISTANCE_START, 0, RAM_SECT_PERSISTANCE_END - RAM_SECT_PERSISTANCE_START);
  extram.set_bytes(RAM_SECT_PERSISTANCE_START, 0, RAM_SECT_PERSISTANCE_END - RAM_SECT_PERSISTANCE_START);

  delay(10);
  // TFT init
  tft.begin();
  tft.clear();

  tft.setFont(Terminal6x8);
  tft.setOrientation(1);

  // Initialization of terminal
  memset(&cli, 0, sizeof(cli));
  cli.width = CLI_WIDTH;
  cli.height = CLI_HEIGHT;
  cli.buffersize = CLI_WIDTH * CLI_HEIGHT;
  // init_sd_cram(&cli);

  cli.current_color = 0b00011000;
}

void Tm_io::print(String msg)
{
  tm_sys.yield();
  cli_print(&cli, msg.c_str());
  if (auto_flush)
    flush();
  Serial.print(msg);
}

void Tm_io::put(char c)
{
  cli_put(&cli, c);
  if (auto_flush)
    flush();
  Serial.print(c);
}

void Tm_io::set_color(uint8_t raw)
{
  cli.current_color = raw;
}

char Tm_io::get_char(bool blocking)
{
  char c = 0;

  do
  {
    tm_sys.yield();

    // Get key from keyboard
    Wire.requestFrom(0x5F, 2);
    c = Wire.read();
    Wire.read();

    // Serial overrides keyboard when char is available
    if (Serial.available())
      c = Serial.read();

    // Return char when not null
    if (c != '\0')
      return c;
  } while (blocking);

  // When non blocking, return char as is.
  return c;
}

String Tm_io::input(bool echo)
{
  print(">");

  String in;
  // Allocate a few bytes to reduce fragmentation.
  in.reserve(20);

  for (;;)
  {
    char c;
    // Receive char and echo
    c = tm_io.get_char(true);
    if (echo)
      tm_io.put(c);

    // When enter, return string + extra null terminator
    if (c == '\n' || c == '\r')
    {
      in = in + '\0';
      return in;
    }

    // Add char to string
    in = in + c;
  }
}

void Tm_io::flush()
{
  cli_draw(&cli);
}

void Tm_io::redraw()
{
  // Clean TFT
  tft.clear();
  // Clean out the persistance graphical section
  extram.set_bytes(RAM_SECT_PERSISTANCE_START, 0, RAM_SECT_PERSISTANCE_END - RAM_SECT_PERSISTANCE_START);
  flush();
}

void Tm_io::brightness(uint8_t bright)
{
  tft.setBacklightBrightness(bright);
}

Tm_io tm_io;

#endif