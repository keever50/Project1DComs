#ifdef TEST_MODULE

#include <tm_io.h>
#include <Arduino.h>
#include <execs.h>
#include <RH_ASK.h>
#include <tm_sys.h>

//Programs
#include <tm_debug.h>

int Etest( String full_input )
{
    tm_io.print(">");
    tm_io.print(execs_get_arg(full_input, 1));
    tm_io.print("<\n");
    return 1;
}

execs_function_t mainexecs[]=
{
    {"test", Etest},
    {"rec", tm_exec_receiver_open},
    {"", NULL} // Terminator
};

void setup()
{
    Serial.begin(9600);
    tm_sys.init();
    tm_rf.init();
    tm_io.init();
    tm_io.set_color(0b11111111);
}

void loop()
{
    String in = tm_io.input(true);

    

    // Try execute command
    int err = execs_run(in, mainexecs);
    if(err)
    {
        tm_io.set_color(0b11100000);
        if(err==-1)
        {
            
            tm_io.print(F("Command not found\n"));
        }else{
            tm_io.print("\nReturned " + String(err) + "\n");
        }
    }else{
        tm_io.set_color(0b00011000);
        tm_io.print(F("\nProgram exitted\n"));
    }
    tm_io.set_color(0b11111111);
    tm_sys.yield();
}

#endif