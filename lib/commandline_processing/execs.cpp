#include <execs.h>

String execs_get_arg(String full_input, unsigned int arg)
{
    bool inquote=false;
    unsigned int start=0;
    unsigned int end=0;
    unsigned int current_arg=0;
    for(unsigned int i=0;i<full_input.length()+1;i++)
    {
        char c = full_input[i];

        // When quote is detected, toggle ignore white spaces.
        if(c=='\'' || c=='\"')
        {
            inquote=!inquote;
        }       


        if(!inquote)
        {
            // When whitespace or end is detected, update start/end
            if(c==' ' || c=='\0' || c=='\n' || c=='\r')
            {
                end=i;

                // Check if this is the argument we want. Otherwise move starting of argument
                if(current_arg==arg)
                {
                    String str = full_input.substring(start, end);
                    // When in quotes, remove quotes
                    if((*str.begin() == '\'' || *str.begin() == '\"') && (*(str.end()-1) == '\'' || *(str.end()-1)  == '\"'))
                    {
                        return full_input.substring(start+1, end-1);
                    }else{
                        return str;
                    }
                }else{
                    start=i+1;
                    current_arg++;
                }
            }
        }
        


    }

    return "";
}

int execs_run(String full_input, execs_function_t* functions)
{
    String command = execs_get_arg(full_input, 0);

    // For each entry in functions
    for(int i=0;;i++)
    {
        execs_function_t entry = functions[i];

        // Break when function is a null. This is the end of the list.
        if(entry.function==NULL) break;

        // Check if it matches
        if(entry.name == command)
        {
            // Then run and return its return
            return entry.function(full_input);
        }
        
    }

    // Not found
    return -1;
}

// test 1 'A B C' 2