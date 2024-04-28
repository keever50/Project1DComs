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

// TFT
#include "TFT_22_ILI9225.h"
#include <../fonts/FreeSans12pt7b.h>
#include <../fonts/FreeSans24pt7b.h>
#include <../fonts/TomThumb.h>

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// RADIO
RH_ASK rh_ask(RA_BITRATE, RA_RX, RA_TX, RA_TR, false);

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



cli_terminal_t cli;

uint16_t colors[]=
{
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE
};

// Setup
void setup() 
{
  // Enable watchdog
  wdt_disable();
  delay(3000);
  wdt_enable(WDTO_2S);

  //pinMode(6, OUTPUT);

  Serial.begin(9600);

  Wire.begin();

  rh_ask.init();
  tft.begin();
  tft.clear();
  
  tft.setFont(Terminal6x8);
  tft.setOrientation(1);
  
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
  cli_put( &cli, 10, 10, 'A');
  cli_put( &cli, 11, 10, 'B');
  cli.current_color=0b00011000;
  cli_put( &cli, 12, 10, 'C');
  //tft.drawGFXText(10,10,"HELLO",COLOR_BLUE);


  tm_test_cli(tft, cli);
  tm_draw_cli(tft, cli);

  cli.current_color=0b00011000;

  wdt_reset();
}

// Loop
void loop() 
{
  char entry[30];
  static int entry_pos=0;
  tm_entry(tft, entry, &entry_pos, 30);
  wdt_reset();



  delay(100);
}


//Test stuff
  // hu_packet_t packet;

  // static unsigned long next=0;
  // if(millis()-next>7000)
  // {
  //   packet.function=0x02;
  //   packet.length=HU_PROTOCOL_MIN_PACKET_LEN+HU_PROTOCOL_MAX_DATA_SIZE;
  //   for(int i=0;i<HU_PROTOCOL_MAX_DATA_SIZE;i++)
  //   {
  //     packet.data[i]=random(0x00,0xFF);
  //   }
  //   packet.start=HU_PROTOCOL_START_BYTE;
  //   packet.end=HU_PROTOCOL_END_BYTE;


  //   Serial.println("Transmit");
  //   hu_protocol_transmit( &rh_ask, &packet );
  //   next=millis();
  // }

  // // Receive
  // hu_prot_receive_err_t err = hu_protocol_receive( &rh_ask, &packet );
  // Serial.print("Receive: ");
  // Serial.println(err);
  // if(err != HU_PROT_RECEIVE_IGNORE && err != HU_PROT_RECEIVE_LISTENING)
  // {
  //   hu_protocol_print_packet(&packet);
  // }

  // log2_log(LOG2_INFO, "A", "SDADSdfsf",5);


#endif