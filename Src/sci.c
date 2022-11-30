//PWM Library
//Processor: MC9S12XDP512
//Crystal: 20 MHz
//by Bigoa Gach
//November 04th, 2022

//libraries
#include "derivative.h"
#include "sci.h"
#include <stdio.h>
#include <stdlib.h>

unsigned long sci0_Init (unsigned long ulBusClock, unsigned long ulBaudRate, int iRDRF_Interrupt) 
{
    //do SCI startup (11.3.2.1)
    SCI0BD = (unsigned int)(ulBusClock / (ulBaudRate * 16)); 

    //turn on TX/RX
    SCI0CR2 = 0b00001100;

    //set up interrupt
    SCI0CR2_RIE = (char)iRDRF_Interrupt;

    return(0);
}

void sci0_txStr (char const * straddr) {
    //loops through string while not null (end of the string)
    while (*straddr)
    {
        sci0_txByte(*straddr);
        straddr++;
    }  
}

int sci0_read (unsigned char * pData) {
    //byte not read?
    if(!SCI0SR1_RDRF) 
        return 0;
    
    *pData = SCI0DRL;

    return 1;
}

unsigned char sci0_bread (void) {
    //waiting for byte to arrive
    while(!SCI0SR1_RDRF);
    
    return SCI0DRL;
}

void sci0_txByte (unsigned char data) {
    while(!(SCI0SR1_TDRE /*&& SWL_Transition (SWL_CTR)*/));

    SCI0DRL = data;
}

int sci0_txNonByte (unsigned char data) {
    if(!SCI0SR1_TDRE)
        return 0;
    else {
        SCI0DRL = data;

        return 1;
    }
}