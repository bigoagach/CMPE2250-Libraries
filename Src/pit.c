//PIT Library
//Processor: MC9S12XDP512
//Crystal: 8 MHz
//by Bigoa Gach
//Oct. 03, 2022

#include "derivative.h"
#include "pit.h"
#include <stdlib.h>

// requires interrupt handler for PIT Channel 0:
/*
interrupt VectorNumber_Vpit0 void PIT0Int (void)
{
  // clear flag
  PITTF = PITTF_PTF0_MASK; // can't read - clears other flags, write only

  // take action!
}
*/

/* PIT CALCULATION CODE FROM MOODLE (https://moodle.nait.ca/course/view.php?id=90722) 
 * MOST CODE TAKEN FROM SIMON WALKER'S CODE!!!!!!! */

// chan           : desired PIT channel (0-1 as enum)
// iWantInterrupt : turn on channel interrupt or not (0 == no interrupt)
// ulBusRate      : bus frequency, in Hz
// ulInterval_us  : desired interval in us (maximum is bus rate and 2^24 register limit dependent)
int PIT_Init (PIT_Channel chan, PIT_Interrupt intState, unsigned long ulBusRate, unsigned long ulInterval_us) {

    unsigned long ulBusCycles = (long)((ulBusRate / 1E6f) * ulInterval_us);
    unsigned long iPotentialLarge = 0;
    unsigned int iPotentialSmall = 0;
    unsigned short fac8;
    unsigned long closest = 4294967295ul;
    unsigned int iSmall;
    unsigned int iLarge;
    
    //check for if the interval value is too large, 
    //default interval to max (256 * 65536 * 1/ulBusRate)
    //24Mhz -> max interval of 699[ms]
    //20Mhz -> max interval of 839[ms]
    //8Mhz  -> max interval of 2.10[s]
    if((ulBusCycles / 256) > 65536) {
        iPotentialSmall = 255;
        iPotentialLarge = 65535;
    }

    //SIMON'S CODE STARTS HERE
    //cycle through best f 8-bit factors 
    for(fac8 = 1; fac8 < 257; fac8++) {
        unsigned short fac16;
        unsigned long displacement;

        //when fac8 is too small
        if(ulBusCycles / fac8 > 65536)
            continue;

        //when the value is bang-on
        fac16 = (unsigned short)(ulBusCycles / fac8);
        if(ulBusCycles % fac8 == 0 && ulBusCycles % fac16 == 0) {
            iPotentialSmall = fac8;
            iPotentialLarge = fac16;
            break;
        }

        //when the value isn't bang-on
        displacement = (unsigned long)abs((int)((fac8 * fac16) - ulBusCycles));
        if(displacement < closest) {
            closest = displacement;
            iPotentialSmall = fac8;
            iPotentialLarge = fac16;
        }
    }

    iLarge = (unsigned int)iPotentialLarge - 1u;
    iSmall = iPotentialSmall - 1u;

    PITMTLD0 = (byte)iSmall; //13.3.0.5
    PITLD0 = iLarge;   //13.3.0.3
    // should yield ulInterval_us[us] interval (PITMTLD0 * PITLD0 * ulBusRate = ulInterval_us[us] per interrupt)
    // changing the two registers above determine the PIT interval
    // enable chan 0 (we will use channel 0 and channel 1 in this course)
    PITCE |= (byte)(chan + 1u);
    // enable interrupt on chan 0
    if(intState) //on
        PITINTE |= (byte)(chan + 1u); // 13.3.0.5
    else //off
        PITINTE &= ~((byte)(chan + 1u));  // 13.3.0.5
    //PITINTE = 0b00000001;
    // finally, enable periodic interrupt, normal in wait, PIT stalled in freeze
    // PIT still runs in wait mode (relevant next course)
    // PIT stalls when debugging so PIT events don’t pile up while stepping
    PITCFLMT = 0b10100000; //13.3.0.1

    return 1;
}
// blocking sleep for the specified number of ms, using the specified channel
//  requires init to reset enable of channel to reload counts
//  sleep can be longer than expected, as it has to eat the startup time
//   for init, so shorter delays will be longer than expected (.115ms in this lib)
int PIT_Sleep (PIT_Channel chan, unsigned long ulBusRate, unsigned int ms) {
    int i;
    for(i = 0; i < ms; i++) {
        PIT_Init(PIT_Channel_0, PIT_Interrupt_Off, ulBusRate, 1000);
        PITTF |= PITTF_PTF0_MASK; 
        while(!(PITTF & PITTF_PTF0_MASK));
    }
}