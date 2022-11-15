//Switch LED Library
//Processor: MC9S12XDP512
//Crystal: ???????
//by Bigoa Gach
//Feb. 04, 2022

#include "derivative.h" /* common defines and macros */
#include "Clock.h" /* Clock Library */

unsigned long busspeed = 0;

void Clock_Set24MHZ(void)
{
    //these settings help acheive 24[MHz]
    SYNR = 2;
    REFDV = 1;

    //turning on the PLL
    PLLCTL = PLLCTL_PLLON_MASK | PLLCTL_AUTO_MASK;

    //Waiting for PLL to Lock into the Frequency
    while(!(CRGFLG & CRGFLG_LOCK_MASK));

    //Selecting the PLL as Clock Source
    CLKSEL |= CLKSEL_PLLSEL_MASK;

    busspeed = DEF_BUS_CLOCK * 3;
}

void Clock_Set20MHZ(void)
{
    //these settings help acheive 20[MHz]
    SYNR = 4;
    REFDV = 3; 

    //turning on the PLL
    PLLCTL = PLLCTL_PLLON_MASK | PLLCTL_AUTO_MASK;

    //Waiting for PLL to Lock into the Frequency
    while(!(CRGFLG & CRGFLG_LOCK_MASK));

    //Selecting the PLL as Clock Source
    CLKSEL |= CLKSEL_PLLSEL_MASK;

    busspeed = DEF_BUS_CLOCK * 2.5;
}

void Clock_EnableOutput(unsigned char ch)
{
    ECLKCTL |= ch;
}

unsigned long Clock_GetBusSpeed(void)
{
    unsigned long updBusSpeed;

    updBusSpeed = busspeed / 2;

    return updBusSpeed;
}

float Clock_GetFactor(void)
{

    float factor = 0.0;

    factor = Clock_GetBusSpeed() / (2 * DEF_BUS_CLOCK);

    return factor;

}
