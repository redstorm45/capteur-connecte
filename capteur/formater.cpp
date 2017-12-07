#include "formater.h"

char* getFormated(uint16_t* values, uint8_t length)
{
    char* tab = new char[2*length+1];
    for(int i=0; i<length; i++){
        tab[2*i] = (values[i] & 0xFF00)>>8;
        tab[2*i] = values[i] & 0x00FF;
    }
    tab[2*length] = '\0';
    return tab;
}

