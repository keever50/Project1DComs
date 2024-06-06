#ifdef TEST_MODULE

#include <tm_debug.h>
#include <execs.h>

hu_packet_t debug_packetbuffer;
hu_packet_t debug_packetbuffer_receive;

bool debug_packetbuffer_override_LRC=false;

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

void tm_prog_receiver_print_data(hu_packet_t& packet)
{
    char buff[10];

    // Decimal ruler
    tm_io.set_color(0b11111000);
    tm_io.print("DEC");
    for(uint8_t i=0;i<10;i++)
    {
        sprintf(buff, "0%d|", i);    
        tm_io.print(buff);
    }

    uint8_t row=0;
    for(uint8_t i=0;i<packet.length-4;i++)
    {
        memset(buff, 0, sizeof(buff));
        uint8_t D=packet.data[i];

        // Each beginning
        if(i%10==0)
        {
            tm_io.print("\n");
            // Vertical decimal ruler
            sprintf(buff, "%d0>", row++);   
            tm_io.set_color(0b11111000); 
            tm_io.print(buff);  
            memset(buff, 0, sizeof(buff));          
        }

        tm_io.set_color(0b11111111);
        if(D<=0x0F)
        {
            sprintf(buff, "0%X|", D);
        }else{
            sprintf(buff, "%X|", D);
        }
        
        tm_io.print(buff);
        
        
    }
}

void tm_prog_receiver_print_packet( hu_packet_t& packet, bool our_packet )
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
    tm_io.set_color(0b11111111);
    tm_io.print(F("DATA START (HEX)\n"));  
    // for( uint8_t i=0; i<packet.length-HU_PROTOCOL_LENGTH_NON_DATA; i++)
    // {
    //     if(i%5==0) tm_io.print("\n");
    //     tm_io.print(String(i) + ":" + String( packet.data[i], HEX)+"|");
    // }
    tm_prog_receiver_print_data(packet);
    tm_io.print(F("\nDATA END\n"));  
    tm_io.set_color(0b10010111);

    tm_io.print(F("End: "));  
    tm_io.print(String(packet.end)+'\n');    

    // LRC check
    tm_io.print(F("LRC: "));  
    tm_io.print(String(packet.LRC));  
    if(our_packet)
    {
        if(debug_packetbuffer_override_LRC)
        {
            tm_io.set_color(0b11110000);
            tm_io.print(F("(OVERRIDDEN)\n"));    
        }else{
            tm_io.set_color(0b00010000);
            tm_io.print(F("(AUTO)\n"));    
        }
        tm_io.set_color(0b10010111);
    }else{
        uint8_t LRC = hu_protocol_calculate_LRC(&packet);
        // Detect error
        if(LRC!=packet.LRC)
        {
            tm_io.set_color(0b11100000);
            tm_io.print(F("(ERROR)\n"));   
        }else{
            tm_io.set_color(0b00011000);
            tm_io.print(F("(OK)\n"));   
        }
    }

    tm_io.set_color(0b10010111);
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

    //hu_packet_t packet;
    for(;;)
    {
        // Break when any key is pressed
        if(tm_io.get_char(false)!='\0') break;

        int rec = tm_rf.receive_raw_packet(false, &debug_packetbuffer_receive);
        if(rec)
        {
            tm_prog_receiver_print_packet(debug_packetbuffer_receive, false);
        }

        tm_sys.yield();
    }
    

    return 0;
}

int tm_exec_packet_generator_instruction()
{
    tm_io.set_auto_flush(false);
    tm_io.print(F("---Packet generator---\n"));
    tm_io.print(F("0) Auto set\n"));
    tm_io.print(F("1) Start byte\n"));
    tm_io.print(F("2) length byte\n"));
    tm_io.print(F("3) Function byte\n"));
    tm_io.print(F("4) Source byte (EG: 2AG3TM)\n"));
    tm_io.print(F("5) Destination byte (EG: 1AG2MM)\n"));
    tm_io.print(F("6) End byte\n"));
    tm_io.print(F("7) LRC (C=calculate, number=manual)\n"));
    tm_io.print(F("8) View\n"));
    tm_io.print(F("9) Exit\n"));
    tm_io.print(F("h) this message\n"));
    tm_io.flush();
    tm_io.set_auto_flush(true);
}

int tm_exec_packet_generator_open( String full_command )
{
    
    tm_exec_packet_generator_instruction();

    for(;;)
    {
        String in;
        tm_io.print(F("option"));
        in = tm_io.input(true);

        // Exit on empty
        if(in.length()==0) break;
        uint8_t select = in[0];//execs_get_arg(in, 0).toInt();
        switch(select)
        {
            case '0':
            {
                tm_io.print(F("func"));
                in = tm_io.input(true);

                break;
            }
            case '1':
            {
                tm_io.print(F("start"));
                in = tm_io.input(true);
                debug_packetbuffer.start = execs_get_arg(in, 0).toInt();
                break;
            }
            case '2':
            {
                tm_io.print(F("length"));
                in = tm_io.input(true);
                debug_packetbuffer.length = execs_get_arg(in, 0).toInt();
                break;
            }
            case '3':
            {
                tm_io.print(F("func"));
                in = tm_io.input(true);
                debug_packetbuffer.function = execs_get_arg(in, 0).toInt();
                break;
            }                
            case '4':
            {
                tm_io.print(F("src(EG 2AG2TM)"));
                in = tm_io.input(true);
                debug_packetbuffer.source = hu_protocol_encode_address(execs_get_arg(in, 0).c_str()); // Add checks for wrong address
                break;
            }        
            case '5':
            {
                tm_io.print(F("dest(EG 2AG2TM)"));
                in = tm_io.input(true);
                debug_packetbuffer.destination = hu_protocol_encode_address(execs_get_arg(in, 0).c_str()); // Add checks for wrong address
                break;
            }        
            case '6':
            {
                tm_io.print(F("end"));
                in = tm_io.input(true);
                debug_packetbuffer.end = execs_get_arg(in, 0).toInt();
                break;
            }        
            case '7':
            {
                tm_io.print(F("LRC (c=auto)"));
                in = tm_io.input(true);
                if(in[0]=='C')
                {
                    debug_packetbuffer_override_LRC=false;
                    debug_packetbuffer.LRC=0;
                }else{
                    debug_packetbuffer_override_LRC=true;
                    debug_packetbuffer.LRC=in.toInt();
                }

                break;
            }     
            case '8':
            {
                tm_prog_receiver_print_packet( debug_packetbuffer, true );
                tm_io.set_color(0b11111111);
                tm_io.print(F("press any key to continue..."));
                tm_io.input(true);
                break;
            }
            case '9':
            {
                tm_io.print("exitted\n");
                return 0;
            }
            case 'h':
            {
                tm_exec_packet_generator_instruction();
                break;
            }
        }
    }

}

int tm_exec_packet_send_open( String full_command )
{
    tm_io.print(F("Sending packet"));
    //int err = hu_protocol_transmit(tm_rf.get_driver(), &debug_packetbuffer);
    int err = hu_protocol_transmit_manual(tm_rf.get_driver(), &debug_packetbuffer, false, !debug_packetbuffer_override_LRC);

    if(err)
    {
        tm_io.print("\nTransmit error\n");
        return err;
    }
    
    while(tm_rf.get_mode()==RHGenericDriver::RHModeTx)
    {
        tm_io.print(F("."));   
        delay(250);
    }
    tm_io.print(F("\nDone\n"));  
    return 0;
}

int _tm_exec_state_send( hu_packet_t &packet )
{
    tm_io.print(F("Sending packet"));   
    int err = hu_protocol_transmit_manual(tm_rf.get_driver(), &packet, false, true);
    if(err)
    {
        tm_io.print("\nTransmit error\n");
        return 1;
    }
    uint8_t lcount=0;
    while(tm_rf.get_mode()==RHGenericDriver::RHModeTx)
    {
        // Prevent new lines from rendering. These take too long
        if(lcount<15)
        {
            tm_io.print(F(">"));   
            lcount++;
            delay(50);
        }

    }      
    
    return 0;
}

// Return 0 on success, 1 on timeout, 2 on packet error
int _tm_exec_state_wait_receive(uint8_t src)
{
    hu_prot_receive_err_t ret;
    unsigned long start_time=millis();

    // Attempt to read till packet received
    for(;;)
    {
        tm_sys.yield();
        // Time out
        if(millis()-start_time>=TM_TRANSFLOW_RECEIVE_TIMEOUT) return TM_TRANSFLOW_RECEIVE_TIMEOUT_ERR;

        // Receive and check if we are still listening. (Listening means no packet)
        ret = hu_protocol_receive(tm_rf.get_driver(), &debug_packetbuffer_receive);
        if(ret != HU_PROT_RECEIVE_LISTENING && ret != HU_PROT_RECEIVE_IGNORE) break;

        // Report traffic
        if( ret == HU_PROT_RECEIVE_IGNORE )
        {
            tm_io.print("\nwrong dest\n");
            memset(&debug_packetbuffer_receive,0,sizeof(debug_packetbuffer_receive));
        }

        else if(debug_packetbuffer_receive.source != src && ret != HU_PROT_RECEIVE_LISTENING)
        {
            tm_io.print("\nwrong src\n");  
            memset(&debug_packetbuffer_receive,0,sizeof(debug_packetbuffer_receive));         
        }
    }
    
    // Check if the packet received successfully
    if(ret != HU_PROT_RECEIVE_RECEIVED) return TM_TRANSFLOW_RECEIVE_CORRUPT_ERR;

    return 0;

}

int tm_exec_packet_send_full_open( String full_command )
{
    hu_packet_t retranspacket;
    memset(&retranspacket, 0, sizeof(retranspacket));
    retranspacket.function=HU_PROTOCOL_FUNCTION_RETRANSMIT;
    retranspacket.start=HU_PROTOCOL_START_BYTE;
    retranspacket.end=HU_PROTOCOL_END_BYTE;
    retranspacket.length=HU_PROTOCOL_LENGTH_NON_DATA;


    uint8_t retry=1;
    int err=0;
    const uint8_t state_start=0;
    const uint8_t state_wait_rec=1;
    const uint8_t state_wait_ack=1;
    const uint8_t state_retrans=2;
    const uint8_t state_retry=3;
    const uint8_t state_done=4;
    uint8_t state=state_start;
    for(;;)
    {
        tm_sys.yield();

        switch(state)
        {
            // Start transmission
            case state_start:
            {
                err = _tm_exec_state_send(debug_packetbuffer);
                if(err)
                {
                    return 1;
                }
                state = state_wait_ack;
                break;
            }

            // Receive acknowledge
            case state_wait_rec:
            {
                tm_io.print(F("Ack?"));  
                memset(&debug_packetbuffer_receive,0,sizeof(debug_packetbuffer_receive));
                err = _tm_exec_state_wait_receive(debug_packetbuffer.source);
                tm_io.print(F("\n"));  
                // When ack timed out
                if(err == TM_TRANSFLOW_RECEIVE_TIMEOUT_ERR)
                {
                    tm_io.print(F("Timed out\n"));   
                    state = state_retry;
                    break;
                
                // When ack is corrupted. Send retransmission request
                }else if(err == TM_TRANSFLOW_RECEIVE_CORRUPT_ERR)
                {
                    tm_io.print(F("Corrupted\n"));   
                    state = state_retrans;
                    break;                    
                }

                // When ack is retrans
                else if(debug_packetbuffer_receive.function==HU_PROTOCOL_FUNCTION_RETRANSMIT)
                {
                    tm_io.print(F("Retransmission\n"));   
                    state = state_retry;
                    break;
                }

                // Check if ack has function ack 
                else if(debug_packetbuffer_receive.function==HU_PROTOCOL_FUNCTION_ACKNOWLEDGE)
                {
                    tm_io.print(F("Acknowledged\n"));   
                    state = state_done;
                    break;
                }

                // Otherwise unexpected
                else{
                    tm_io.print(F("Unexpected packet\n"));   
                    state = state_retry;
                    break;                    
                }

                return 1;
            }

            case state_retrans:
            {
                if(retry>=TM_TRANSFLOW_RECEIVE_RETRIES) return 1;
                tm_io.print(F("Retry(Retrans)\n"));  
                retranspacket.destination=debug_packetbuffer.destination;
                retranspacket.source=debug_packetbuffer.source;
                err = _tm_exec_state_send(retranspacket);
                if(err) return 1;
                retry++;
                state = state_wait_rec;

                break;
            }

            case state_retry:
            {
                if(retry>=TM_TRANSFLOW_RECEIVE_RETRIES) return 1;
                tm_io.print(F("Retry\n"));  
                retry++;
                state = state_start;
                break;
            }

            case state_done:
            {
                tm_io.print(F("Success\n"));   
                return 0;
            }
        }
    }
}

int tm_exec_packet_data_open( String full_command )
{
    tm_io.set_auto_flush(false);
    
    tm_io.print(F("---- Data sequenced editor ----\nUsage:\n"));    
    tm_io.print(F("a[n] = point to address\n"));   
    tm_io.print(F("b[n...] = set bytes\n"));   
    tm_io.print(F("f[f...] = set floats\n")); 
    tm_io.print(F("r[n] = random,amount\n")); 
    tm_io.print(F("c = clear\n")); 
    tm_io.print(F("v = view data\n")); 
    tm_io.print(F("q = quit\n")); 
    tm_io.print(F("EG:\nc a 10 b 1 2 3 f 3.14 a 20 r 2 v q\n"));     
    tm_io.flush();

    tm_sys.yield();
    uint8_t addr=0;

    while(true)
    {
        // Get new line
        uint8_t index=0;

        tm_io.set_auto_flush(true);
        tm_io.print("data");
        String in = tm_io.input(true);
        tm_io.set_auto_flush(false);

        while(true)
        {
            // Check if packet needs to be resized
            if(debug_packetbuffer.length<addr)
            {
                debug_packetbuffer.length=addr;
                tm_io.print("packet has been resized\n");
                tm_io.flush();
            }

            // Get new argument till empty argument
            String arg = execs_get_arg(in, index++);
            if(arg.length()==0) break;

            char C = arg[0]; // Extract first character as instruction
            switch(C)
            {
                case 'a': // Address select
                {
                    arg = execs_get_arg(in, index++);
                    addr=arg.toInt();
                    // Limit size
                    if(addr>=sizeof(hu_packet_s)) addr=sizeof(hu_packet_s)-1;

                    //index++;
                    continue; 
                }
                case 'b': // Byte
                {
                    // Get all the bytes to add
                    while(true)
                    {
                        // Get next (after command) byte
                        arg = execs_get_arg(in, index++);

                        // Check if this argument is empty
                        if(arg.length()==0) break;

                        // Check if this is a number
                        char N = arg[0];
                        if(N<'0'||N>'9')
                        {
                            // Pass this letter to the next command
                            index--;
                            break;
                        }

                        // Stop overflow
                        if(addr>=sizeof(hu_packet_s)) break;

                        // Write
                        debug_packetbuffer.data[addr]=arg.toInt();

                        // Next
                        addr++;
                    }
                    continue;
                }
                case 'f':
                {
                    // Get all the floats to add
                    while(true)
                    {
                        // Get next (after command) float
                        arg = execs_get_arg(in, index++);

                        // Check if this argument is empty
                        if(arg.length()==0) break;

                        // Check if this is a number
                        char N = arg[0];
                        if(N<'0'||N>'9')
                        {
                            // Pass this letter to the next command
                            index--;
                            break;
                        }

                        // Stop overflow
                        if(addr+4U>=sizeof(hu_packet_s)) break;

                        // Write
                        *(float*)(debug_packetbuffer.data+addr)=arg.toFloat();

                        // Next
                        addr=addr+4;
                    }
                    continue;                    
                }
                case 'v':
                {
                    tm_prog_receiver_print_data(debug_packetbuffer);
                    tm_io.print("\n");
                    tm_io.set_color(0b11111111);
                    tm_io.flush();
                    continue;
                }
                case 'q':
                {
                    tm_io.set_auto_flush(true);
                    return 0;
                }
            }

        }
    }

    tm_io.set_auto_flush(true);
    return 0;
}

int tm_exec_packet_getf( String full_command )
{
    String arg = execs_get_arg(full_command, 1);
    uint8_t addr = arg.toInt();
    static uint8_t fbuff[4];

    for(uint8_t i=0;i<4;i++)
    {
        fbuff[i] = debug_packetbuffer_receive.data[addr+i];
    }

    tm_io.print("float: "+String(*(float*)fbuff)+"\n");

    return 0;
}

int tm_exec_packet_view( String full_command )
{
    tm_prog_receiver_print_packet( debug_packetbuffer_receive, false );
    return 0;    
}

#endif