#ifdef TEST_MODULE

#include <tm_debug.h>
#include <execs.h>

int tm_exec_scrambler_open( String full_command )
{
    String arg = execs_get_arg( full_command, 1); 
    uint8_t scramblesize = arg.toInt();
    if(scramblesize==0) return 1;

    uint8_t* scrambles = (uint8_t*)malloc(scramblesize);

    tm_io.print(F("Transmitting ")); 
    tm_io.print(String(scramblesize)+" random bytes\n");  

    for(uint8_t i=0;i<scramblesize;i++)
    {
        scrambles[i]=(uint8_t)rand();
    }        
    bool success = tm_rf.transmit_raw(scrambles, scramblesize, true);

    free(scrambles);
    


    return !success;
}

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
        // Break when any key is pressed
        if(tm_io.get_char(false)!='\0') break;

        int rec = tm_rf.receive_raw_packet(false, &packet);
        if(rec)
        {
            tm_prog_receiver_print_packet(packet);
        }

        tm_sys.yield();
    }
    

    return 0;
}
#endif