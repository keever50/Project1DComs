#pragma once
#ifdef TEST_MODULE
#include <RH_ASK.h>
#include <hu_protocol.h>
#include <test_module_hardware.h>
#include <tm_sys.h>


class Tm_rf
{
public:
    int init();
    // Return 1 on receive
    int receive_raw_packet( bool blocking, hu_packet_t* packet );
    int transmit_raw( uint8_t* raw, uint8_t len, bool blocking );
private:
    RH_ASK* driver;
    uint8_t temp_buffer[RH_ASK_MAX_PAYLOAD_LEN];
};

extern Tm_rf tm_rf;
#endif