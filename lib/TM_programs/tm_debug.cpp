#ifdef TEST_MODULE

#include <tm_debug.h>

void tm_prog_receiver_print_packet( hu_packet_t& packet )
{
    tm_io.set_color(0b10010111);
    tm_io.set_auto_flush(false);
    tm_io.print(F("-----START HU PACKET-----\n"));  

    tm_io.print(F("Start: "));  
    tm_io.print(String(packet.start)+'\n');    

    tm_io.print(F("Length: "));  
    tm_io.print(String(packet.length)+'\n');    

    tm_io.print(F("Function: "));  
    tm_io.print(String(packet.function)+'\n');    

    tm_io.print(F("Source: "));  
    tm_io.print(String(packet.source)+'\n');    

    tm_io.print(F("Destination: "));  
    tm_io.print(String(packet.destination)+'\n');        

    // Data
    tm_io.set_color(0b11010110);
    tm_io.print(F("DATA START\n"));  
    for( uint8_t i=0; i<packet.length-HU_PROTOCOL_LENGTH_NON_DATA; i++)
    {
       tm_io.print("["+String(i) + ":" + String(packet.data[i])+"]");
    }
    tm_io.print(F("\nDATA END\n"));  
    tm_io.set_color(0b10010111);

    tm_io.print(F("End: "));  
    tm_io.print(String(packet.end)+'\n');    

    tm_io.print(F("LRC: "));  
    tm_io.print(String(packet.LRC)+'\n');    

    tm_io.print(F("----- END HU PACKET -----\n"));  

    tm_io.redraw();
    //tm_io.flush();
    tm_io.set_auto_flush(true);
}

int tm_exec_receiver_open( String full_command )
{
    tm_io.set_color(0b10010111);
    tm_io.print(F("------RF Receiver------\n"));
    tm_io.print(F("Listening for HU packets...\n"));

    hu_packet_t packet;
    for(;;)
    {
        int rec = tm_rf.receive_raw(false, &packet);
        if(rec)
        {
            tm_prog_receiver_print_packet(packet);
        }

        tm_sys.yield();
    }
    

    return 0;
}
#endif