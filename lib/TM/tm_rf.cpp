#ifdef TEST_MODULE
#include <tm_rf.h>

//static RH_ASK rh_ask(RA_BITRATE, RA_RX, RA_TX, RA_TR, false);

int Tm_rf::init(int bitrate)
{
    static RH_ASK rh_ask(bitrate, RA_RX, RA_TX, RA_TR, false);
    rh_ask.init();
    driver = &rh_ask;

    return 0;
}

int Tm_rf::receive_raw_packet(bool blocking, hu_packet_t *packet)
{
    uint8_t len = sizeof(temp_buffer);
    do
    {
        tm_sys.yield();

        bool rec = driver->recv(temp_buffer, &len);
        if (rec)
        {
            hu_protocol_decode_raw(packet, temp_buffer);

            return 1;
        }
    } while (blocking);

    return 0;
}

int Tm_rf::transmit_raw(uint8_t *raw, uint8_t len, bool blocking)
{
    bool success = driver->send(raw, len);

    if (blocking)
        driver->waitPacketSent(1000);
    return success;
}

RHGenericDriver::RHMode Tm_rf::get_mode()
{
    return driver->mode();
}

Tm_rf tm_rf;

#endif