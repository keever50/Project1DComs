/*
    Test module display helper functions.
    Written by Kevin Witteveen
*/
#ifdef TEST_MODULE

#include <tm_display.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <Arduino.h>
#include <test_module_hardware.h>
#include <ExtRAM.h>

uint16_t colors[] =
    {
        COLOR_RED,
        COLOR_GREEN,
        COLOR_BLUE};

void tm_draw_cli(TFT_22_ILI9225 &tft, cli_terminal_t &cli)
{
    uint8_t buffer[CLI_WIDTH];
    uint8_t color_buffer[CLI_WIDTH];
    uint8_t persistance_buffer[CLI_WIDTH];
    uint8_t persistance_buffer_color[CLI_WIDTH];
    for (uint16_t y = 0; y < CLI_HEIGHT; y++)
    {
        // Get bytes
        extram.read_bytes(RAM_SECT_GRAPHICS_START + y * CLI_WIDTH, buffer, sizeof(buffer));
        extram.read_bytes(RAM_SECT_GRAPHICS_START + CLI_SIZE + y * CLI_WIDTH, color_buffer, sizeof(color_buffer));

        extram.read_bytes(RAM_SECT_PERSISTANCE_START + y * CLI_WIDTH, persistance_buffer, sizeof(persistance_buffer));
        extram.read_bytes(RAM_SECT_PERSISTANCE_START + CLI_SIZE + y * CLI_WIDTH, persistance_buffer_color, sizeof(persistance_buffer_color));

        for (uint16_t x = 0; x < CLI_WIDTH; x++)
        {
            // Optimize font width calculation
            char c = buffer[x];
            char c_old = persistance_buffer[x];
            uint8_t color_old = persistance_buffer_color[x];
            // Replace unknown characters
            if (c >= '' || c <= 0)
                c = '?';

            // Calculate colors from 8bit to RRRGGBBB to 16-bit
            uint8_t color = color_buffer[x];
            uint8_t R = (color >> 5) & 0b111;
            uint8_t G = (color >> 3) & 0b11;
            uint8_t B = color & 0b111;
            uint16_t calculated_color = tft.setColor(R * 36, G * 85, B * 36);

            // Do not redraw unchanged characters
            if (c != c_old || color != color_old)
            {
                // tft.setBackgroundColor(COLOR_BLUE);
                if (c == ' ')
                {
                    tft.fillRectangle(x * CLI_FONT_WIDTH, y * CLI_FONT_HEIGHT, x * CLI_FONT_WIDTH + CLI_FONT_WIDTH + 2, y * CLI_FONT_HEIGHT + CLI_FONT_HEIGHT - 1, COLOR_BLACK);
                }
                else
                {
                    tft.drawChar(x * CLI_FONT_WIDTH, y * CLI_FONT_HEIGHT, c, calculated_color);
                }

                persistance_buffer[x] = c;
                persistance_buffer_color[x] = color;
            }
        }

        // Update buffer
        extram.write_bytes(RAM_SECT_PERSISTANCE_START + y * CLI_WIDTH, persistance_buffer, sizeof(persistance_buffer));
        extram.write_bytes(RAM_SECT_PERSISTANCE_START + CLI_SIZE + y * CLI_WIDTH, persistance_buffer_color, sizeof(persistance_buffer_color));
    }
}

void tm_test_cli(TFT_22_ILI9225 &tft, cli_terminal_t &cli)
{
    // R
    for (uint16_t y = 0; y < cli.height; y++)
    {
        for (uint16_t x = 0; x < cli.width; x++)
        {

            cli.current_color = random(0, 7) << 5;
            cli_set(&cli, x, y, 'R');
        }
    }
    tm_draw_cli(tft, cli);
    wdt_reset();

    for (int i = 0; i < cli.height; i++)
    {
        cli_shift_up(&cli);
        tm_draw_cli(tft, cli);
    }

    // G
    for (uint16_t y = 0; y < cli.height; y++)
    {
        for (uint16_t x = 0; x < cli.width; x++)
        {

            cli.current_color = random(0, 3) << 3;
            cli_set(&cli, x, y, 'G');
        }
    }
    tm_draw_cli(tft, cli);
    wdt_reset();

    // B
    for (uint16_t y = 0; y < cli.height; y++)
    {
        for (uint16_t x = 0; x < cli.width; x++)
        {

            cli.current_color = random(0, 7);
            cli_set(&cli, x, y, 'B');
        }
    }
    tm_draw_cli(tft, cli);
    wdt_reset();

    // RGB
    for (uint16_t y = 0; y < cli.height; y++)
    {
        for (uint16_t x = 0; x < cli.width; x++)
        {

            cli.current_color = random(0, 0xFF);
            cli_set(&cli, x, y, '#');
        }
    }
    tm_draw_cli(tft, cli);
    wdt_reset();

    // Clear
    for (uint16_t y = 0; y < cli.height; y++)
    {
        for (uint16_t x = 0; x < cli.width; x++)
        {

            cli.current_color = 0;
            cli_set(&cli, x, y, '\0');
        }
    }
    tm_draw_cli(tft, cli);

    wdt_reset();
}

int tm_entry(TFT_22_ILI9225 &tft, char *buff, int *buff_pos, int buff_len)
{
    static char drawn = 0;
    if (!drawn)
    {
        drawn = 1;
        tft.fillRectangle(0, SCR_HEIGHT - CLI_FONT_HEIGHT - 1, SCR_WIDTH, SCR_HEIGHT, COLOR_DARKGRAY);
        tft.drawRectangle(0, SCR_HEIGHT - CLI_FONT_HEIGHT - 2, SCR_WIDTH - 1, SCR_HEIGHT - 1, COLOR_DARKBLUE);
    }

    // Blinky
    static unsigned long blink_time;
    static char blink_toggle;
    if (millis() - blink_time > 500)
    {
        blink_toggle = !blink_toggle;
        blink_time = millis();
        if (blink_toggle)
        {
            tft.fillRectangle((*buff_pos) * CLI_FONT_WIDTH, SCR_HEIGHT - CLI_FONT_HEIGHT - 1, (*buff_pos) * CLI_FONT_WIDTH + 4, SCR_HEIGHT - 2, COLOR_GREEN);
        }
        else
        {
            tft.fillRectangle((*buff_pos) * CLI_FONT_WIDTH, SCR_HEIGHT - CLI_FONT_HEIGHT - 1, (*buff_pos) * CLI_FONT_WIDTH + 4, SCR_HEIGHT - 2, COLOR_DARKGRAY);
        }
    }

    // Request character from keyboard
    Wire.requestFrom(0x5F, 2);
    char c = Wire.read();
    Wire.read();

    // Or serial
    if (Serial.available())
        c = Serial.read();

    // When character is nothing, return
    if (c == 0)
        return 1;

    // Check enter
    if (c == '\n' || c == '\r')
    {
        // tone(6, 1000, 10);
        buff[(*buff_pos)] = 0; // Add null
        (*buff_pos) = 0;

        // Reset bar
        drawn = 0;
        tft.fillRectangle(0, SCR_HEIGHT - CLI_FONT_HEIGHT - 2, SCR_WIDTH, SCR_HEIGHT, COLOR_BLACK);
        return 0;
    }

    // Check bounds and preserve null
    if ((*buff_pos) >= buff_len - 1)
    {
        // tone(6, 500, 100);
        return 1;
    };

    // Other characters //
    // tone(6, 100, 10);

    // Add to screen
    tft.drawChar((*buff_pos) * CLI_FONT_WIDTH, SCR_HEIGHT - CLI_FONT_HEIGHT - 1, c, COLOR_GREEN);

    // Add to buffer
    buff[(*buff_pos)++] = c;

    return 1;
}

#endif