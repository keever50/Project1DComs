#ifdef TEST_MODULE
#include <tm_rf_debugger.h>
#include <hu_protocol.h>

#define TM_RF_MAX_ERRORS    10

#define TM_RF_DEBUG_EXECS 1
hu_packet_t* tm_rf_debug_packet;
uint8_t tm_rf_debug_packet_buffer[HU_PROTOCOL_BUFFER_SIZE];

static cli_function tm_rf_debug_funcs[TM_RF_DEBUG_EXECS]
{

};
static const char* tm_rf_debug_names[TM_RF_DEBUG_EXECS]=
{

};

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
        sprintf(sprint_buf, "S%X ", start);
        cli_print(term, sprint_buf);

        // Check length byte
        uint8_t length = tm_rf_debug_packet_buffer[i++];
        if(length<=HU_PROTOCOL_MAX_PACKET_SIZE && length>=HU_PROTOCOL_MIN_PACKET_LEN) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "L%X ", length);
        cli_print(term, sprint_buf);

        // Check function byte
        uint8_t function = tm_rf_debug_packet_buffer[i++];
        if(function<=HU_PROTOCOL_FUNCTION_RANGE) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "F%X ", function);
        cli_print(term, sprint_buf);

        // Check source byte
        uint8_t source = tm_rf_debug_packet_buffer[i++];
        if(source!=0) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "s%X ", source);
        cli_print(term, sprint_buf);

        // Check destination byte
        uint8_t destination = tm_rf_debug_packet_buffer[i++];
        if(destination!=0) term->current_color=TM_GREEN;
        else term->current_color=TM_RED;
        sprintf(sprint_buf, "d%X ", destination);
        cli_print(term, sprint_buf);

        cli_print(term, "\n");
        cli_draw(term);
        memset(tm_rf_debug_packet_buffer, 0, HU_PROTOCOL_BUFFER_SIZE);
    }

}

int rf_debug_open(cli_terminal_t* term, const char* full_command)
{
    // Memory allocation
    char* command_line = (char*)malloc(31); 
    //tm_rf_debug_packet_buffer = (uint8_t*)malloc(HU_PROTOCOL_BUFFER_SIZE);

    if(!command_line || !tm_rf_debug_packet_buffer) return 1;
    cli_clear(term);
    cli_print(term, "---RF Debugger---\n");
    cli_print(term, "enter help\nfor commands\n");
    cli_draw(term);

    cli_executables_t execs;
    execs.executable_functions=tm_rf_debug_funcs;
    execs.executable_names=tm_rf_debug_names;
    execs.number_of_executables=TM_RF_DEBUG_EXECS;
    // Command loop
    while(1==1)
    {
        static int entry_pos=0;
        char t = tm_entry(tft, command_line, &entry_pos, 30);
        if(!t)
        {
            cli_print(term, command_line);
            cli_put(term, '\n');
            cli_draw(term);
            cli_execute(term, command_line, &execs);
        }

        rf_debug_receive(term);
        wdt_reset();
        delay(10);
    }

    free(command_line);
    //free(tm_rf_debug_packet_buffer);
    return 0;
}

#endif