#ifdef TEST_CM
//Dirk's Code
#define Recieve_Pin 3
#define Transmit_Pin 11

#define CM_Groep3_Adres 0x2C

void setup()
{
Serial.begin(9600);
}

void loop()
{

}

void Functiecode_Recieve()
{
    uint8_t* FunctieCode = &
    switch (FunctieCode)
    {
    case 0xC0:
    break;

    case 0xC3:
    break;

    case 0xD0:
    break;

    case 0xD3:
    break;
    
    case 0xD4:
    case 0xD7:
    Error(1);
    break;

    case 0xD8:
    break;

    case 0xDB:
    break;

    case 0xDC:
    break;

    case 0xDF:
    break;

    case 0xE4:
    break;
    
    case 0xE7:
    break;
    
    case 0xE8:
    case 0xEB:
    Error(1);
    break;

    case 0xEC:
    break;
    
    case 0xEF:
    break;
    
    case 0xF0:
    break;

    case 0xF3:
    break;
    
    default:
    Error(0);
    break;
    }
}

Error(uint8_t* Error_nummer)
{
    Serial.print("Error:");

    switch (Error_nummer)
    {
    case 0:
    Serial.print("geen goede functiecode");
    break;
    case 1:
    Serial.print("functiecode is onlogisch voor comunicatie module")
    
    default:

    break;
    }
}

#endif