#ifdef TEST_MODULE
#include <Arduino.h>
#include <hu_protocol.h>
#include <test_module_hardware.h>
#include <biterrors.h>
#include <logging2.h>
#include "SPI.h"
#include <Wire.h>
#include <cli_processing.h>
#include <avr/wdt.h>
#include <tm_display.h>
#include <MemoryFree.h>
#include <tm_rf_debugger.h>

// TFT
#include "TFT_22_ILI9225.h"
#include <../fonts/FreeSans12pt7b.h>
#include <../fonts/FreeSans24pt7b.h>
#include <../fonts/TomThumb.h>

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// RADIO
RH_ASK rh_ask(RA_BITRATE, RA_RX, RA_TX, RA_TR, false);


// Commands
#define MAIN_EXECS  3
int showmemory(cli_terminal_t* term, const char* full_command);
int main_help(cli_terminal_t* term, const char* full_command);
void command( const char* cmd );
static cli_executables_t mainexec;
static cli_function mainexecs[MAIN_EXECS]
{
  rf_debug_open,
  showmemory,
  main_help
};
static const char* mainnames[MAIN_EXECS]=
{
  "debug",
  "mem",
  "help"
};

// override the weak attribute of the logging2 library raw printing output
int testpos=0;
void log2_log_raw( const char* msg )
{
  Serial.print(msg);
  
  static char buff[64];
  strncpy(buff, msg, 63);
  Serial.print(strlen(msg));

  // Overwrite newline with a null for tft library.
  buff[strlen(msg)-1]='\0';
  tft.drawGFXText(10,testpos*10,String(buff),COLOR_BLUE);
  testpos++;
}

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
  char* msg = (char*)malloc(32);
  if(!msg) return 1;
  snprintf(msg, 32, "%db free\n", freeMemory()+32);
  cli_print(term, msg);
  cli_draw(term);
  free(msg); 
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
  
  memset(&cli, 0, sizeof(cli));

  cli.width=CLI_WIDTH;
  cli.height=CLI_HEIGHT;
  cli.buffersize=CLI_WIDTH*CLI_HEIGHT;
  cli.buffer=(char*)malloc(cli.buffersize);
  if(cli.buffer==NULL)
  {
    tft.drawText(0,0,"Out of memory", COLOR_RED);
    while(true);
  }
  cli.colors=(char*)malloc(cli.buffersize);
  if(cli.colors==NULL)
  {
    tft.drawText(0,0,"Out of memory", COLOR_RED);
    while(true);
  }

  memset(cli.buffer, 0, cli.buffersize);
  cli.current_color=0b11100000;
  cli_set( &cli, 10, 10, 'A');
  cli_set( &cli, 11, 10, 'B');
  cli.current_color=0b00011000;
  cli_set( &cli, 12, 10, 'C');
  //tft.drawGFXText(10,10,"HELLO",COLOR_BLUE);


  tm_test_cli(tft, cli);
  tm_draw_cli(tft, cli);

  cli.current_color=0b00011000;


  //Show memory
  cli_print(&cli, "memory: ");
  char* msg = (char*)malloc(32);
  snprintf(msg, 32, "%db free\n", freeMemory()+32);
  cli_print(&cli, msg);
  free(msg);

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