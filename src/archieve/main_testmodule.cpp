// #ifdef TEST_MODULE
// #include <Arduino.h>
// #include <hu_protocol.h>
// #include <test_module_hardware.h>
// #include <biterrors.h>
// #include "SPI.h"
// #include <Wire.h>
// #include <cli_processing.h>
// #include <avr/wdt.h>
// #include <tm_display.h>
// #include <MemoryFree.h>
// #include <tm_rf_debugger.h>
// //#include <SdFat.h>
// #include <RH_ASK_mod.h>
// #include <ExtRAM.h>

// // TFT
// #include "TFT_22_ILI9225.h"



// // RADIO
// RH_ASK rh_ask(RA_BITRATE, RA_RX, RA_TX, RA_TR, false);

// int test(cli_terminal_t* term, const char* full_command)
// {
//   const uint8_t maxarglen=32;
//   int iter=0, arglen=0;
//   char buff[32];

//   cli_get_next_argument_iterative(&iter, full_command, buff, maxarglen, &arglen);
//   cli_get_next_argument_iterative(&iter, full_command, buff, maxarglen, &arglen);   

//   for(int i=0;i<atoi(buff);i++)
//   {
//     cli_print(term, "test\n");
//     cli_draw(term);
//   }
// }


// // Commands
// #define MAIN_EXECS  2
// int showmemory(cli_terminal_t* term, const char* full_command);
// int main_help(cli_terminal_t* term, const char* full_command);
// void command( const char* cmd );
// static cli_executables_t mainexec;
// static cli_function mainexecs[MAIN_EXECS]
// {
//   rf_debug_open,
//   test
//   //showmemory,
//   //main_help
// };
// static const char* mainnames[MAIN_EXECS]=
// {
//   "debug",
//   "test"
//   //"mem",
//   //"help"
// };




// int showmemory(cli_terminal_t* term, const char* full_command)
// {
//   cli_print(term, "memory: ");
//   //char* msg = (char*)malloc(32);
//   char msg[32];
//   if(!msg) return 1;
//   snprintf(msg, 32, "%db free\n", freeMemory()+32);
//   cli_print(term, msg);
//   cli_draw(term);
//   //free(msg); 
//   return 0;
// }

// int main_help(cli_terminal_t* term, const char* full_command)
// {
//   cli_print(term, "commands:\n");
//   for(int i=0;i<MAIN_EXECS;i++)
//   {
//     cli_print(term, mainnames[i]);
//     cli_put(term, '\n');
//   }

//   cli_draw(term);
//   return 0;

// }


// void command( const char* cmd )
// {

//   mainexec.number_of_executables=MAIN_EXECS;
//   mainexec.executable_names=mainnames;
//   mainexec.executable_functions=mainexecs;
//   cli_execute(&cli, cmd, &mainexec);
// }




// // Setup
// void setup() 
// {
//   SPI.begin();
//   pinMode(RAM_CS, OUTPUT);
//   extram.set_mode(EXTRAM_MODE_SEQ);
  
//   // Clean
//   for(uint16_t i=0;i<65535;i++)
//   {
//     extram.write_byte(i, 0);
//   }

//   // Enable watchdog
//   wdt_disable();
//   delay(3000);
//   wdt_enable(WDTO_2S);

//   //pinMode(6, OUTPUT);
//   Serial.begin(250000);

//   Wire.begin();

//   rh_ask.init();
//   tft.begin();
//   tft.clear();
  
//   tft.setFont(Terminal6x8);
//   tft.setOrientation(1);
  
//   // Initialization of terminal
//   memset(&cli, 0, sizeof(cli));
//   cli.width=CLI_WIDTH;
//   cli.height=CLI_HEIGHT;
//   cli.buffersize=CLI_WIDTH*CLI_HEIGHT;
//   //init_sd_cram(&cli);

//   cli_demo(&cli);



//   cli.current_color=0b00011000;


//   //Show memory
//   // cli_print(&cli, "memory: ");
//   // char* msg = (char*)malloc(32);
//   // snprintf(msg, 32, "%db free\n", freeMemory()+32);
//   // cli_print(&cli, msg);
//   // free(msg);

//   //Commands
//   cli_print(&cli, "enter help\nfor commands\n");
//   cli_draw(&cli);


//   wdt_reset();


// }

// // Loop
// void loop() 
// {

//   char entry[31];
//   static int entry_pos=0;
//   char t = tm_entry(tft, entry, &entry_pos, 30);
//   if(!t)
//   {
//     cli_print(&cli, entry);
//     cli_put(&cli, '\n');
//     tm_draw_cli(tft, cli);
//     command(entry);
//   }
//   wdt_reset();
//   delay(10);


  
// }



// #endif