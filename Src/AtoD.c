//AtoD Library
//Processor: MC9S12XDP512
//Crystal: 20 MHz
//by Bigoa Gach
//December 4th, 2022

//libraries
#include "derivative.h"
#include "AtoD.h"

void AtoD_Init(int iEnableInterrupt) 
{
    DDR1AD0 = 0b00000000; //enabling all channels as inputs
    ATD0DIEN = 0b00000000; //all analog

    //powering up ATD
    //fast clear (clears on read)
    //ATD continues in wait mode
    //all external triggers disabled
    //enabling interrupt if user wants t0
    //(5.3.2.3)
    if(iEnableInterrupt)
        ATD0CTL2 = 0b11100010;
    else
        ATD0CTL2 = 0b11100000;

    //50[us] delay
    asm LDX #134;
    asm DBNE X,*;

    //configuring AtoD to complete 8 conversions per sequence (5.3.2.4)
    ATD0CTL3 = 0b01000000;

    //determining the clock parameters for the A/D (5.3.2.5)
    ATD0CTL4 = 0b00000100;

    //determines how the data is presented, and how the conversion operate
    //(5.3.2.6)
    ATD0CTL5 = 0b10110000;

}

unsigned int AtoD_CH_Read(int channel)
{
    //wait for conversion completion flag
    while(!ATD0STAT0_SCF);

    #define read(x) \
        if(channel == x) \
            return ATD0DR##x; 
    read(0); read(1); read(2); read(3); read(4); read(5); read(6); read(7); 
}