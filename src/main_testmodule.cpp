#ifdef TEST_MODULE
#include <Arduino.h>
#include <hu_protocol.h>
#include <test_module_hardware.h>
#include <biterrors.h>
#include "SPI.h"
#include <Wire.h>
#include <cli_processing.h>
#include <avr/wdt.h>
#include <tm_display.h>
#include <MemoryFree.h>
#include <tm_rf_debugger.h>
//#include <SdFat.h>
#include <RH_ASK_mod.h>

// TFT
#include "TFT_22_ILI9225.h"

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// RADIO
RH_ASK rh_ask(RA_BITRATE, RA_RX, RA_TX, RA_TR, false);

int test(cli_terminal_t* term, const char* full_command)
{
  const uint8_t maxarglen=32;
  int iter=0, arglen=0;
  char buff[32];

  cli_get_next_argument_iterative(&iter, full_command, buff, maxarglen, &arglen);
  cli_get_next_argument_iterative(&iter, full_command, buff, maxarglen, &arglen);   

  for(int i=0;i<atoi(buff);i++)
  {
    cli_print(term, "test\n");
    cli_draw(term);
  }
}


// Commands
#define MAIN_EXECS  2
int showmemory(cli_terminal_t* term, const char* full_command);
int main_help(cli_terminal_t* term, const char* full_command);
void command( const char* cmd );
static cli_executables_t mainexec;
static cli_function mainexecs[MAIN_EXECS]
{
  rf_debug_open,
  test
  //showmemory,
  //main_help
};
static const char* mainnames[MAIN_EXECS]=
{
  "debug",
  "test"
  //"mem",
  //"help"
};


// Override weak function of CLI
void cli_draw( cli_terminal_t* term )
{
  tm_draw_cli( tft, *term );
}

cli_terminal_t cli;

uint16_t colors[]=
{
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE
};

int showmemory(cli_terminal_t* term, const char* full_command)
{
  cli_print(term, "memory: ");
  //char* msg = (char*)malloc(32);
  char msg[32];
  if(!msg) return 1;
  snprintf(msg, 32, "%db free\n", freeMemory()+32);
  cli_print(term, msg);
  cli_draw(term);
  //free(msg); 
  return 0;
}

int main_help(cli_terminal_t* term, const char* full_command)
{
  cli_print(term, "commands:\n");
  for(int i=0;i<MAIN_EXECS;i++)
  {
    cli_print(term, mainnames[i]);
    cli_put(term, '\n');
  }

  cli_draw(term);
  return 0;

}


void command( const char* cmd )
{

  mainexec.number_of_executables=MAIN_EXECS;
  mainexec.executable_names=mainnames;
  mainexec.executable_functions=mainexecs;
  cli_execute(&cli, cmd, &mainexec);
}

// Console graphical RAM
uint8_t CRAM[CLI_HEIGHT*CLI_WIDTH*2+CLI_WIDTH];
// SdFat SD;
// File CRAM_file;
// void init_sd_cram(cli_terminal_t *term)
// {

//   Serial.print(F("Initializing SD card..."));
//   if (!SD.begin(SD_CS,16000000)) {
//     Serial.println(F("Card failed, or not present"));
//     while (1);
//   }
//   Serial.println(F("card initialized."));

//   CRAM_file = SD.open("/CRAM", O_CREAT | O_RDWR);  
//   uint8_t n=0;
//   for(uint16_t i=0;i<term->buffersize*2;i++)
//   {
//     if(!CRAM_file.write(n))
//     {
//       while(true)
//       {
//         Serial.println(F("SD error"));
//         delay(500);
//       }
//     }
//   }
//   CRAM_file.flush();
//   wdt_reset();
// }

void cli_mem_write(cli_terminal_t* term, int address, uint8_t byte)
{
  // wdt_reset();
  // static int prev_addr;
  // if(address!=prev_addr+1)
  // {
  //   CRAM_file.seek(address);
  // }

  // prev_addr=address;
  // CRAM_file.write(byte);
  CRAM[address]=byte;
}
uint8_t cli_mem_read(cli_terminal_t* term, int address)
{
  // wdt_reset();
  // static int prev_addr;
  // if(address!=prev_addr+1)
  // {
  //   CRAM_file.seek(address);  
  // }  

  // prev_addr=address;
  // return CRAM_file.read();
  return CRAM[address];
}
void cli_mem_writes(cli_terminal_t* term, uint8_t* source, int address, int len)
{
  // wdt_reset();
  // CRAM_file.seek(address);  
  // CRAM_file.write(source, len);
  for(int i=0;i<len;i++)
  {
    CRAM[address+i]=source[i];
  }
}
void cli_mem_reads(cli_terminal_t* term, uint8_t* destination, int address, int len)
{
  // wdt_reset();
  // CRAM_file.seek(address);  
  // CRAM_file.read(destination, len);
  for(int i=0;i<len;i++)
  {
    destination[i]=CRAM[address+i];
  }  
}


// Setup
void setup() 
{
  // Enable watchdog
  wdt_disable();
  delay(3000);
  wdt_enable(WDTO_2S);

  //pinMode(6, OUTPUT);
  Serial.begin(250000);

  Wire.begin();

  rh_ask.init();
  tft.begin();
  tft.clear();
  
  tft.setFont(Terminal6x8);
  tft.setOrientation(1);
  
  // Initialization of terminal
  memset(&cli, 0, sizeof(cli));
  cli.width=CLI_WIDTH;
  cli.height=CLI_HEIGHT;
  cli.buffersize=CLI_WIDTH*CLI_HEIGHT;
  //init_sd_cram(&cli);

  cli_demo(&cli);



  cli.current_color=0b00011000;


  //Show memory
  // cli_print(&cli, "memory: ");
  // char* msg = (char*)malloc(32);
  // snprintf(msg, 32, "%db free\n", freeMemory()+32);
  // cli_print(&cli, msg);
  // free(msg);

  //Commands
  cli_print(&cli, "enter help\nfor commands\n");
  cli_draw(&cli);


  wdt_reset();


}

// Loop
void loop() 
{

  char entry[31];
  static int entry_pos=0;
  char t = tm_entry(tft, entry, &entry_pos, 30);
  if(!t)
  {
    cli_print(&cli, entry);
    cli_put(&cli, '\n');
    tm_draw_cli(tft, cli);
    command(entry);
  }
  wdt_reset();
  delay(10);


  
}



#endif