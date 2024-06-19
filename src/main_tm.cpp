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
execs_function_t mainexecs[] =
    {
        {"test", Etest, "TEST"},
        {"rec", tm_exec_receiver_open, "Receives HU packets"},
        {"scram", tm_exec_scrambler_open, "(Scram)bler.\n     Usage: scram (bytes)"},
        {"help", tm_exec_help, ""},
        {"redraw", tm_exec_redraw, "Redraws the screen"},
        {"gen", tm_exec_packet_generator_open, "Opens HU packet (gen)eration menu"},
        {"send", tm_exec_packet_send_open, "Transmits a packet"},
        {"getf", tm_exec_packet_getf, "gets a float in a data address"},
        {"data", tm_exec_packet_data_open, "Opens the data editor for packets"},
        {"view", tm_exec_packet_view, "views the last received packet"},
        {"sendf", tm_exec_packet_send_full_open, "Send full transmission flow"},
        {"sleep", tm_exec_sleep, "Sleeps the device"},
        {"sendrec", tm_exec_packet_sendrec_open, "sends and receives"},
        {"recsend", tm_exec_packet_recsend, "receives and sends"},
        {"", NULL, ""} // Terminator
};

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
        String command = mainexecs[i].name;
        command.toUpperCase();
        String desc = mainexecs[i].description;
        tm_io.print(command + ":\"" + desc + "\"\n");
    }
    tm_io.set_auto_flush(true);
    tm_io.flush();
    return 0;
}

void setup()
{
    delay(500);
    Serial.begin(SER_BAUD);
    tm_sys.init();
    tm_rf.init();
    tm_io.init();
    tm_io.set_color(0b11111111);
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