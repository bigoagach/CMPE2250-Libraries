//PWM Library
//Processor: MC9S12XDP512
//Crystal: 20 MHz
//by Bigoa Gach
//October 21st, 2022

//libraries
#include "derivative.h"
#include "pwmlib.h"

// clockSpeed : clock speed in Hertz
// channel    : desired channel bit
// poles      : determines whether the pulse states HIGH or LOW (0-1 as enum)
// period     : number of ticks for the length of the frequency
// duty       : number of ticks that the pulse is high relative to the period (duty / period * 100 = duty cycle)
void PWM_8Bit_Init(unsigned long frequency, unsigned long clockSpeed, PWM_CH_BIT channel, BWM_Polarity poles, unsigned int period, unsigned int duty) {
    
    unsigned int clockVal;
    unsigned char div = PWM_DivideFactor_1; 
    BWM_ClockSelect clockX;

    //calculating what to divide the clock by
    clockVal = (int)(clockSpeed / (frequency * period)) / 2;
    
    //reducing the clock val as much as possible
    //tadeo showed me how to use multi-line macro defs :)
    #define dFac(x) \
        if(clockVal % x == 0) \
            div = PWM_DivideFactor_##x
    dFac(2); dFac(4); dFac(8); dFac(16); dFac(32); dFac(64); dFac(128);

    //set Clock A or B (8.3.2.4)
    if(channel == PWM_CH0_BIT || channel == PWM_CH1_BIT || channel == PWM_CH4_BIT || channel == PWM_CH5_BIT) {
        clockX = ClockA;
        PWMPRCLK &= clockX;
        PWMPRCLK |= div;

        //dividing the clockVal by 2^(div)
        clockVal /= 1 << div;

        //set Clock SX to divide by 50 (8.3.2.10)
        PWMSCLA = (byte)clockVal; //x2
    }
    else if(channel == PWM_CH2_BIT || channel == PWM_CH3_BIT || channel == PWM_CH6_BIT || channel == PWM_CH7_BIT) {
        clockX = ClockB;
        PWMPRCLK &= clockX;
        PWMPRCLK |= div << 4;
        
        //dividing the clockVal by 2^(div)
        clockVal /= 1 << div;

        //set Clock SX to divide by 50 (8.3.2.10)
        PWMSCLB = (byte)clockVal; //x2
    }

    //select Clock SX for channel X (8.3.2.3)
    PWMCLK |= channel;

    //set polarity (8.3.2.2)
    if(!poles)              //if StartLow
        PWMPOL &= ~channel; //set channel bit to 0
    else                    //if StartHigh
        PWMPOL |= channel;  //set channel bit to 1

    //setting program period (8.3.2.13) and duty (8.3.2.14)
    #define set(x) \
        if(channel & PWM_CH##x##_BIT) {\
            PWMPER##x = (byte)period; \
            PWMDTY##x = (byte)duty; \
        }    
    set(0); set(1); set(2); set(3); set(4); set(5); set(6); set(7);
}

// frequency  : desired PWM frequency
// clockSpeed : clock speed in Hertz
// channel    : desired channel bit
// poles      : determines whether the pulse states HIGH or LOW (0-1 as enum)
// period     : number of ticks for the length of the frequency
// duty       : number of ticks that the pulse is high relative to the period (duty / period * 100 = duty cycle)
void PWM_16Bit_Init(unsigned long frequency, unsigned long clockSpeed, PWM_CH_BIT channel, BWM_Polarity poles, unsigned int period, unsigned int duty) {
    
    unsigned int clockVal;
    unsigned char div = PWM_DivideFactor_1; 
    BWM_ClockSelect clockX;

    //calculating what to divide the clock by
    clockVal = (int)(clockSpeed / (frequency * period)) / 2;
    
    //reducing the clock val as much as possible
    //tadeo showed me how to use multi-line macro defs :)
    #define dFac(x) \
        if(clockVal % x == 0) \
            div = PWM_DivideFactor_##x
    dFac(2); dFac(4); dFac(8); dFac(16); dFac(32); dFac(64); dFac(128);

    //set Clock A or B (8.3.2.4)
    if(channel == PWM_CH0_BIT || channel == PWM_CH1_BIT || channel == PWM_CH4_BIT || channel == PWM_CH5_BIT) {
        clockX = ClockA;
        PWMPRCLK &= clockX;
        PWMPRCLK |= div;

        //dividing the clockVal by 2^(div)
        clockVal /= 1 << div;

        //set Clock SX to divide by 50 (8.3.2.10)
        PWMSCLA = (byte)clockVal; //x2
    }
    else if(channel == PWM_CH2_BIT || channel == PWM_CH3_BIT || channel == PWM_CH6_BIT || channel == PWM_CH7_BIT) {
        clockX = ClockB;
        PWMPRCLK &= clockX;
        PWMPRCLK |= div << 4;
        
        //dividing the clockVal by 2^(div)
        clockVal /= 1 << div;

        //set Clock SX to divide by 50 (8.3.2.10)
        PWMSCLB = (byte)clockVal; //x2
    }

    //select Clock SX for channel X (8.3.2.3)
    PWMCLK |= channel;

    //set polarity (8.3.2.2)
    if(!poles)              //if StartLow
        PWMPOL &= ~channel; //set channel bit to 0
    else                    //if StartHigh
        PWMPOL |= channel;  //set channel bit to 1

    //setting program period (8.3.2.13) and duty (8.3.2.14)
    #define set16(x, y) \
        if((channel & PWM_CH##x##_BIT) || (channel & PWM_CH##y##_BIT)) {\
            PWMPER##x##y = period; \
            PWMDTY##x##y = duty; \
        } \
        PWMCTL_CON##x##y = 1; //set as a 16-bit channel (concatenating the channels) 
    set16(0,1); set16(2,3); set16(4,5); set16(6,7); 

}

// enables PWM channel
// channel    : desired channel bit
void PWM_Enable(PWM_CH_BIT channel) {
    //enable channel (8.3.2.1)
    PWME |= channel;
}

// disables PWM channel
// channel    : desired channel bit
void PWM_Disable(PWM_CH_BIT channel) {
    //disable channel (8.3.2.1)
    PWME &= ~channel;
}