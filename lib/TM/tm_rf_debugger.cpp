#ifdef TEST_MODULE
#include <tm_rf_debugger.h>
#include <hu_protocol.h>
#include <biterrors.h>
#define TM_RF_MAX_ERRORS    10

char tm_rf_debug_linebuffer[32];
hu_packet_t tm_rf_debug_packet;
uint8_t tm_rf_debug_packet_buffer[HU_PROTOCOL_BUFFER_SIZE];
uint8_t tm_rf_debug_source;

const char* tm_str_ok="OK\n";
const char* tm_str_err="ERR\n";

// Commands //
#define TM_RF_DEBUG_EXECS 4
static cli_function tm_rf_debug_funcs[TM_RF_DEBUG_EXECS]
{
    rf_debug_toggle_receive,
    rf_debug_generate_packet,
    rf_debug_send_packet,
    rf_debug_ident
};
static const char* tm_rf_debug_names[TM_RF_DEBUG_EXECS]=
{
    "rec",
    "gen",
    "send",
    "ident"
};
//

void printout(cli_terminal_t* cli, char* str)
{
    Serial.print(str);
    cli_print(cli, str);
    //cli_draw(cli);

}

int rf_debug_ident(cli_terminal_t* term, const char* full_command)
{
    const uint8_t maxarglen=32;
    int iter=0, arglen=0;
    char buff[32];

    cli_get_next_argument_iterative(&iter, full_command, buff, maxarglen, &arglen);
    cli_get_next_argument_iterative(&iter, full_command, buff, maxarglen, &arglen);   

    cli_print(term, buff);
    tm_rf_debug_source = hu_protocol_encode_address(buff);
    if(tm_rf_debug_source) cli_print(term, tm_str_ok);
    else cli_print(term, tm_str_err);
    cli_draw(term);    
    
    //free(arg);
    return 0; 
}

int rf_debug_send_packet(cli_terminal_t* term, const char* full_command)
{
    const uint8_t maxarglen=32;
    int iter=0, arglen=0;
    char* arg = tm_rf_debug_linebuffer;

    cli_get_next_argument_iterative(&iter, full_command, arg, maxarglen, &arglen);
    cli_get_next_argument_iterative(&iter, full_command, arg, maxarglen, &arglen);   

    tm_rf_debug_packet.destination=hu_protocol_encode_address(arg);

    cli_print(term, "...");
    cli_draw(term);
    hu_protocol_transmit(&rh_ask, &tm_rf_debug_packet);
    cli_print(term, tm_str_ok);
    cli_draw(term);
    
    free(arg);
    return 0;
}

int rf_debug_generate_packet(cli_terminal_t* term, const char* full_command)
{
    const uint8_t maxarglen=4;
    int iter=0, arglen=0;
    char arg[maxarglen+1];
    cli_get_next_argument_iterative(&iter, full_command, arg, maxarglen, &arglen);
    cli_get_next_argument_iterative(&iter, full_command, arg, maxarglen, &arglen);

    tm_rf_debug_packet.start=HU_PROTOCOL_START_BYTE;
    tm_rf_debug_packet.function=atoi(arg);
    tm_rf_debug_packet.source=tm_rf_debug_source; // UPDATE THIS WHEN FUNCTION IS READY
    // Fill in data length depending on function
    tm_rf_debug_packet.length=HU_PROTOCOL_LENGTH_NON_DATA+2;

    // Placeholder
    tm_rf_debug_packet.data[0]=0xAA;
    tm_rf_debug_packet.data[1]=0xBB;

    tm_rf_debug_packet.end=HU_PROTOCOL_END_BYTE;

    cli_print(term, tm_str_ok);
    cli_draw(term);

    return 0;
}

bool rf_debug_receiving=false;
int rf_debug_toggle_receive(cli_terminal_t* term, const char* full_command)
{
    rf_debug_receiving=!rf_debug_receiving;
    if(rf_debug_receiving)
    {
        cli_print(term, "ON\n");
    }else
    {
        cli_print(term, "OFF\n");
    }
    cli_draw(term);
    return 0;
    
}

void rf_debug_receive(cli_terminal_t* term)
{
    //memset(tm_rf_debug_packet_buffer, 0, HU_PROTOCOL_BUFFER_SIZE);

    const int sprint_buf_len = 20;
    char sprint_buf[sprint_buf_len];

    const char* errors[TM_RF_MAX_ERRORS];
    uint8_t len = RH_ASK_MAX_MESSAGE_LEN;
    if(rh_ask.recv(tm_rf_debug_packet_buffer, &len))
    {
        int i=0, err=0;

        // Check start byte
        uint8_t start = tm_rf_debug_packet_buffer[i++];
        if(start==HU_PROTOCOL_START_BYTE) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "(S%X-", start);
        printout(term, sprint_buf);

        // Check length byte
        uint8_t length = tm_rf_debug_packet_buffer[i++];
        uint8_t data_len = length-HU_PROTOCOL_LENGTH_NON_DATA;
        if(length<=HU_PROTOCOL_MAX_DATA_SIZE+HU_PROTOCOL_LENGTH_NON_DATA && length>=HU_PROTOCOL_LENGTH_NON_DATA) term->current_color=TM_GREEN;
        else{
            term->current_color=TM_RED;
            data_len = 0;
        };
        sprintf(sprint_buf, "L%X-", length);
        printout(term, sprint_buf);

        // Check function byte
        uint8_t function = tm_rf_debug_packet_buffer[i++];
        if(function<=HU_PROTOCOL_FUNCTION_RANGE) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "F%X-", function);
        printout(term, sprint_buf);

        // Check source byte
        uint8_t source = tm_rf_debug_packet_buffer[i++];
        if(source!=0) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "s%X-", source);
        printout(term, sprint_buf);

        // Check destination byte
        uint8_t destination = tm_rf_debug_packet_buffer[i++];
        if(destination == tm_rf_debug_source) term->current_color=TM_GREEN;
        else if(destination != tm_rf_debug_source) term->current_color=0b11111000;
        sprintf(sprint_buf, "d%X", destination);
        printout(term, sprint_buf);

        // Get data
        term->current_color=TM_WHITE;
        printout(term, "[");
        for(int B=0;B<data_len;B++)
        {
            uint8_t byte = tm_rf_debug_packet_buffer[i++];
            sprintf(sprint_buf, "%X", byte);
            cli_print(term, sprint_buf);

            if(B==data_len-1) break;
            printout(term, "-");
        }
        printout(term, "]");

        // End of trans
        uint8_t end = tm_rf_debug_packet_buffer[i++];
        if(end==HU_PROTOCOL_END_BYTE) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "E%X-", end);
        printout(term, sprint_buf);

        // LRC
        uint8_t LRC = tm_rf_debug_packet_buffer[i++];
        uint8_t our_LRC = get_LRC(tm_rf_debug_packet_buffer, HU_PROTOCOL_START_LENGTH+length);
        if(LRC==our_LRC) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "X%X)", LRC);
        printout(term, sprint_buf);

        // Finish
        printout(term, "\n");
        cli_draw(term);
        memset(tm_rf_debug_packet_buffer, 0, HU_PROTOCOL_BUFFER_SIZE);
        term->current_color=TM_WHITE;

        Serial.print('\n');
    }

    

}

int rf_debug_open(cli_terminal_t* term, const char* full_command)
{
    // Memory allocation
    memset(tm_rf_debug_linebuffer, 0, sizeof(tm_rf_debug_linebuffer));

    //cli_clear(term);
    cli_print(term, "DEBUG\n");
    cli_draw(term);

    cli_executables_t execs;
    execs.executable_functions=tm_rf_debug_funcs;
    execs.executable_names=tm_rf_debug_names;
    execs.number_of_executables=TM_RF_DEBUG_EXECS;
    // Command loop
    while(1==1)
    {
        static int entry_pos=0;
        char t = tm_entry(tft, tm_rf_debug_linebuffer, &entry_pos, 30);
        if(!t)
        {
            cli_print(term, tm_rf_debug_linebuffer);
            cli_put(term, '\n');
            cli_draw(term);
            cli_execute(term, tm_rf_debug_linebuffer, &execs);
        }

        if(rf_debug_receiving) rf_debug_receive(term);

        wdt_reset();
        delay(10);
    }

    return 0;
}

#endif