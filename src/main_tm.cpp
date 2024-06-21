#ifdef TEST_MODULE

#include <tm_io.h>
#include <Arduino.h>
#include <execs.h>
#include <RH_ASK.h>
#include <tm_sys.h>
#include <avr/sleep.h>

// Programs
#include <tm_debug.h>
int Etest(String full_input);
int tm_exec_help(String full_input);
int tm_exec_redraw(String full_input);
int tm_exec_sleep(String full_input);
int tm_exec_reset(String full_input);
execs_function_t mainexecs[] =
    {
        {"rec", tm_exec_receiver_open, "Receives packets"},
        {"scram", tm_exec_scrambler_open, "send random bytes\nEx: scram 50 sends 50 random bytes"},
        {"help", tm_exec_help, ""},
        {"redraw", tm_exec_redraw, "Redraws the screen"},
        {"gen", tm_exec_packet_generator_open, "Build a packet"},
        {"data", tm_exec_packet_data_open, "Build data for packet"},
        {"send", tm_exec_packet_send_open, "Transmits a packet"},
        {"getf", tm_exec_packet_getf, "read float from data\nEx: getf 4 gets float on pos 4"},
        {"view", tm_exec_packet_view, "view the last packet"},
        //{"sendf", tm_exec_packet_send_full_open, "Send full transmission flow"},
        {"sleep", tm_exec_sleep, "Sleeps the device"},
        {"sendrec", tm_exec_packet_sendrec_open, "sends and receives"},
        {"recsend", tm_exec_packet_recsend, "receives and sends"},
        {"reset", tm_exec_reset, "resets the testmodule"},
        {"", NULL, ""} // Terminator
};

int tm_exec_reset(String full_input)
{
    while (true)
        ;
}

int tm_exec_sleep(String full_input)
{
    tm_io.brightness(0);
    digitalWrite(LED_BUILTIN, LOW);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    wdt_disable();
    sleep_mode();
    wdt_enable(WDTO_2S);
    tm_io.brightness(255);
}

int tm_exec_redraw(String full_input)
{
    tm_io.redraw();
}

int Etest(String full_input)
{
    tm_io.print(">");
    tm_io.print(execs_get_arg(full_input, 1));
    tm_io.print("<\n");
    return 1;
}

int tm_exec_help(String full_input)
{
    tm_io.set_auto_flush(false);
    for (int i = 0;; i++)
    {
        if (mainexecs[i].function == NULL)
            break;
        tm_io.set_color(0b11111000);
        String command = mainexecs[i].name;
        command.toUpperCase();
        String desc = mainexecs[i].description;
        tm_io.print(command + ": ");
        tm_io.set_color(0b11010110);
        tm_io.print(desc + "\n");
    }
    tm_io.set_color(0b11111111);
    tm_io.set_auto_flush(true);
    tm_io.flush();
    return 0;
}

int bitrate_select()
{
    tm_io.print("Select RF bitrate (Default 2000)");
    tm_io.get_char(false);
    String str = tm_io.input(true);
    return str.toInt();
}

void setup()
{
    delay(500);
    Serial.begin(SER_BAUD);
    tm_sys.init();

    tm_io.init();
    tm_io.set_color(0b11111111);

    // Let user select RF bitrate for initialization
    tm_rf.init(bitrate_select());

    tm_io.print(F("Welcome\nEnter help to list commands\n"));
    tm_io.print("Connect to a serial monitor on\n" + String(SER_BAUD) + " baud for easier control\n");

    hu_protocol_set_address(hu_protocol_encode_address("2AG3TM"));
}

void loop()
{
    String in = tm_io.input(true);

    // Try execute command
    int err = execs_run(in, mainexecs);
    if (err)
    {
        tm_io.set_color(0b11100000);
        if (err == -1)
        {
            tm_io.print(F("Command not found\n"));
        }
        else
        {
            tm_io.print("\nReturned " + String(err) + "\n");
        }
    }
    else
    {
        // tm_io.set_color(0b00011000);
        // tm_io.print(F("\nProgram exitted\n"));
    }
    tm_io.set_color(0b11111111);
    tm_sys.yield();
}

#endif