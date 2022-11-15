//Switch LED Library
//Processor: MC9S12XDP512
//Crystal: ???????
//by Bigoa Gach
//Feb. 13, 2022

#include "derivative.h" /* common defines and macros */
#include "ECT.h" /* Clock Library */
#include "math.h" /* Math Library */
#include "Clock.h" /* Clock Library */

unsigned int prescaler;

void Timer_Init(Timer_Prescale tmr_pre)
{
    TSCR2 |= (byte)tmr_pre;  
    TSCR1 |= TSCR1_TEN_MASK; //Enables timer
    TFLG1 = TFLG1_C0F_MASK;

    prescaler = (unsigned char)(1 << (unsigned int)tmr_pre);
}

void TimerCH_EnableAsOutput(Timer_CH_BIT tmr_ch_bit, unsigned int interval)
{
    TIOS |= tmr_ch_bit;
    
    if(tmr_ch_bit == Timer_CH0_BIT)
      TC0 = TCNT + interval;
    if(tmr_ch_bit == Timer_CH6_BIT)
      TC6 = TCNT + interval;
}

void TimerCH_EnableAsInput(Timer_CH_BIT tmr_ch_bit)
{
    TIOS &= ~(tmr_ch_bit); //enables inputs
}

void TimerCH_EnableToggle(Timer_CH_BIT tmr_ch_bit) {
    //toggle action TC0
    #define toggle1(x) \
        if(tmr_ch_bit & Timer_CH##x##_BIT) \
            TCTL2 = TCTL2_OL##x##_MASK; 
    #define toggle2(x) \
        if(tmr_ch_bit & Timer_CH##x##_BIT) \
            TCTL1 = TCTL1_OL##x##_MASK; 
    toggle1(0); toggle1(1); toggle1(2); toggle1(3);
    toggle2(4); toggle2(5); toggle2(6); toggle2(7);
}

void TimerCH_IntEnable(Timer_CH_BIT tmr_ch_bit)
{
    TIE |= tmr_ch_bit;
}

void Timer_SleepCounts(unsigned int val)
{
    TimerCH_EnableAsOutput(Timer_CH6_BIT, val);
    while(!(TFLG1 & TFLG1_C6F_MASK));
    TFLG1 = TFLG1_C6F_MASK;
}

// void Timer_SleepCounts(unsigned int val, Timer_CH_BIT channel, unsigned int* channelLoc)
// {
//     TimerCH_EnableAsOutput(channel);
//     *channelLoc = TCNT + val;
//     //TFLG1 = TFLG1_C0F_MASK;
//     while(!(TFLG1 & channel));
// }

void Timer_Sleep_ms(unsigned int ms)
{
    int val;
    unsigned int count = (unsigned int)(Clock_GetBusSpeed() / 1000);
    count /= prescaler;

    for(val = 0; val < ms; val++)
    {
        Timer_SleepCounts((unsigned int)count);
    }
}


