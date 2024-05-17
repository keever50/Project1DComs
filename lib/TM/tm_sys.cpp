#include <tm_sys.h>
#include <avr/wdt.h>
#include <Arduino.h>

Tm_sys tm_sys;

void Tm_sys::init()
{
    wdt_enable(WDTO_4S);
}

void Tm_sys::yield()
{
    wdt_reset();

}