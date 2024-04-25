#ifdef TEST_MODULE
#include <Arduino.h>
#include <hu_protocol.h>
#include <test_module_hardware.h>
#include <biterrors.h>
#include <logging2.h>
#include "SPI.h"
#include <Wire.h>
#include <CI_processing.h>

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

#define CLI_HEIGHT  20
#define CLI_WIDTH   20
cli_terminal_t cli;

uint16_t colors[]=
{
  tft.setColor(255,0,0),
  tft.setColor(0,255,0),
  tft.setColor(0,0,255)
};

// Setup
void setup() 
{
  Serial.begin(9600);

  rh_ask.init();
  tft.begin();
  tft.clear();
  
  tft.setFont(Terminal6x8, true);

  
  cli.width=CLI_WIDTH;
  cli.height=CLI_HEIGHT;
  cli.buffersize=CLI_WIDTH*CLI_HEIGHT;
  cli.buffer=(char*)malloc(cli.buffersize);
  cli.colors=(char*)malloc(cli.buffersize);

  memset(cli.buffer, 0, cli.buffersize);
  cli.current_color=0;
  cli_put( &cli, 10, 10, 'A');
  cli_put( &cli, 11, 10, 'B');
  cli.current_color=1;
  cli_put( &cli, 12, 10, 'C');
  //tft.drawGFXText(10,10,"HELLO",COLOR_BLUE);
}

// Loop
void loop() 
{


  int x=0;
  int written=0;
  while(Serial.available())
  {
    char c = Serial.read();
    if(c=='\n') 
    {
      cli_shift_up( &cli );
    }else{
      cli_put( &cli, x, cli.height-1, c);
    }
    
    written=1;
    x++;
  }

  if(written)
  {
    tft.clear();
    for(uint16_t y=0; y<cli.height; y++)
    {
      for(uint16_t x=0; x<cli.width; x++)
      {
        int addr = y*cli.width+x;
        char c = cli.buffer[addr];
        uint16_t col = colors[(int)cli.colors[addr]];
        
        if(c==0) continue;
        tft.drawChar(x*8,y*8,c,col);
      }  
    }
  }
  
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