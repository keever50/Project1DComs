/*
    Test module display helper functions.
    Written by Kevin Witteveen
*/
#ifdef TEST_MODULE

#include <tm_display.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Arduino.h>


void tm_draw_cli(TFT_22_ILI9225& tft, cli_terminal_t& cli)
{
    uint8_t buffer[CLI_MAX_WIDTH];
    uint8_t cbuffer[CLI_MAX_WIDTH];
    static char ecode[32];
    //tft.clear();
    Serial.print("\e[H");
    for(uint16_t y=0; y<cli.height; y++)
    {
        
        // Get line
        cli_mem_reads(&cli, buffer, y*cli.width, cli.width);
        cli_mem_reads(&cli, cbuffer, cli.buffersize+(y*cli.width), cli.width);

        for(uint16_t x=0; x<cli.width; x++)
        {
            int addr = x;
            char c = buffer[addr];
            if(c==0)
            {
                tft.fillRectangle(x*CLI_FONT_WIDTH, y*CLI_FONT_HEIGHT, x*CLI_FONT_WIDTH+CLI_FONT_WIDTH, y*CLI_FONT_HEIGHT+CLI_FONT_HEIGHT-1, COLOR_BLACK);
                Serial.print(" ");
                continue;
            }

            // Get RRRGGBBB
            uint8_t col = cbuffer[addr];
            
            // Check if this is the same color as previous.
            static uint8_t prev_col=0;
            static uint16_t RGB=0;
            if(col!=prev_col)
            {
                uint8_t B = col&0b00000111;
                col=col>>3;
                uint8_t G = col&0b00000011;
                col=col>>2;
                uint8_t R = col&0b00000111;

                // escape code for serial
                snprintf(ecode, sizeof(ecode), "\e[38;2;%d;%d;%dm",R*36,G*85,B*36);
                ecode[31]=0;
                Serial.print(ecode);
                
                // Convert to 16 bit
                RGB = tft.setColor(R*36,G*85,B*36);

                prev_col=col;
            }
            tft.setBackgroundColor(COLOR_DARKBLUE);
            tft.drawChar(x*CLI_FONT_WIDTH,y*CLI_FONT_HEIGHT,c,RGB);

            Serial.print(c);
        }  
        Serial.println("");
        wdt_reset();
    }    
}

void tm_test_cli(TFT_22_ILI9225& tft, cli_terminal_t& cli)
{
    // R
    for(uint16_t y=0; y<cli.height; y++)
    {
        for(uint16_t x=0; x<cli.width; x++)
        {
            
            cli.current_color=random(0, 7)<<5;
            cli_set(&cli, x, y, 'R');
        }  
    }   
    tm_draw_cli( tft, cli );
    wdt_reset();

    for(int i=0;i<cli.height;i++)
    {
        cli_shift_up(&cli);
        tm_draw_cli( tft, cli );
    }

    // G
    for(uint16_t y=0; y<cli.height; y++)
    {
        for(uint16_t x=0; x<cli.width; x++)
        {
            
            cli.current_color=random(0,3)<<3;
            cli_set(&cli, x, y, 'G');
        }  
    }   
    tm_draw_cli( tft, cli );
    wdt_reset();

    // B
    for(uint16_t y=0; y<cli.height; y++)
    {
        for(uint16_t x=0; x<cli.width; x++)
        {
            
            cli.current_color=random(0,7);
            cli_set(&cli, x, y, 'B');
        }  
    }   
    tm_draw_cli( tft, cli );  
    wdt_reset();

    // RGB
    for(uint16_t y=0; y<cli.height; y++)
    {
        for(uint16_t x=0; x<cli.width; x++)
        {
            
            cli.current_color=random(0,0xFF);
            cli_set(&cli, x, y, '#');
        }  
    }   
    tm_draw_cli( tft, cli );  
    wdt_reset();   

    // Clear
    for(uint16_t y=0; y<cli.height; y++)
    {
        for(uint16_t x=0; x<cli.width; x++)
        {
            
            cli.current_color=0;
            cli_set(&cli, x, y, '\0');
        }  
    }   
    tm_draw_cli( tft, cli );     

    wdt_reset();    

}

int tm_entry(TFT_22_ILI9225& tft, char* buff, int *buff_pos, int buff_len)
{
    static char drawn=0;
    if(!drawn)
    {
        drawn=1;
        tft.fillRectangle(0, SCR_HEIGHT-CLI_FONT_HEIGHT-1, SCR_WIDTH, SCR_HEIGHT, COLOR_DARKGRAY);
        tft.drawRectangle(0, SCR_HEIGHT-CLI_FONT_HEIGHT-2, SCR_WIDTH-1, SCR_HEIGHT-1, COLOR_DARKBLUE);
    }

    // Blinky
    static unsigned long blink_time;
    static char blink_toggle;
    if(millis()-blink_time>500)
    {
        blink_toggle=!blink_toggle;
        blink_time = millis();
        if(blink_toggle)
        {
            tft.fillRectangle((*buff_pos)*CLI_FONT_WIDTH, SCR_HEIGHT-CLI_FONT_HEIGHT-1, (*buff_pos)*CLI_FONT_WIDTH+4, SCR_HEIGHT-2, COLOR_GREEN);
        }else{
            tft.fillRectangle((*buff_pos)*CLI_FONT_WIDTH, SCR_HEIGHT-CLI_FONT_HEIGHT-1, (*buff_pos)*CLI_FONT_WIDTH+4, SCR_HEIGHT-2, COLOR_DARKGRAY);
        }
    }

    // Request character from keyboard
    Wire.requestFrom(0x5F, 2);
    char c = Wire.read();
    Wire.read();

    // Or serial
    if(Serial.available()) c = Serial.read();

    // When character is nothing, return
    if(c==0) return 1;

    // Check enter
    if(c=='\n' || c=='\r')
    {
        //tone(6, 1000, 10);
        buff[(*buff_pos)]=0; // Add null  
        (*buff_pos)=0;

        // Reset bar
        drawn=0;
        tft.fillRectangle(0, SCR_HEIGHT-CLI_FONT_HEIGHT-2, SCR_WIDTH, SCR_HEIGHT, COLOR_BLACK);
        return 0; 
    }

    // Check bounds and preserve null
    if((*buff_pos)>=buff_len-1)
    {
        //tone(6, 500, 100);  
        return 1;
    };

    // Other characters //
    //tone(6, 100, 10);

    // Add to screen
    tft.drawChar((*buff_pos)*CLI_FONT_WIDTH, SCR_HEIGHT-CLI_FONT_HEIGHT-1, c, COLOR_GREEN);

    // Add to buffer
    buff[(*buff_pos)++]=c;



    return 1;

}

#endif