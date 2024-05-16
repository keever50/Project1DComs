#include <tm_io.h>
#include <Arduino.h>
#include <execs.h>
#include <RH_ASK.h>

int Etest( String full_input )
{
    tm_io.print(">");
    tm_io.print(execs_get_arg(full_input, 1));
    tm_io.print("<\n");
    return 0;
}

execs_function_t mainexecs[]=
{
    {"test", Etest},
    {"", NULL} // Terminator
};

void setup()
{
    Serial.begin(9600);
    tm_io.init();
    
}

void loop()
{
    tm_io.set_color((uint8_t)rand());
    tm_io.print(String(rand()));
    tm_io.print(String(rand()));
    tm_io.print(String(rand()));
    tm_io.print(String(rand()));

    tm_io.print("\n");
    String in;
    in="test dsad '3 3 4' dad  ";
    execs_run(in, mainexecs);
    
}