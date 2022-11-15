/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     20 MHz (Requires Active PLL)
// Author:        Bigoa Gach
// Details:       Bring up the PWM CLock A or B at the desired 
//                interval, channel X
// Revision History
//      Created:  OCT 2022
/////////////////////////////////////////////////////////////////////////////
#include "derivative.h"
/////////////////////////////////////////////////////////////////////////////
// Enummerations
/////////////////////////////////////////////////////////////////////////////

//PWM DivideFactor value
typedef enum PWM_DivideFactor
{
  PWM_DivideFactor_1 = 0b00000000,
  PWM_DivideFactor_2 = 0b00000001,
  PWM_DivideFactor_4 = 0b00000010,
  PWM_DivideFactor_8 = 0b00000011,
  PWM_DivideFactor_16 = 0b00000100,
  PWM_DivideFactor_32 = 0b00000101,
  PWM_DivideFactor_64 = 0b00000110,
  PWM_DivideFactor_128 = 0b00000111
}PWM_DivideFactor;

//Channels
typedef enum PWM_CH_BIT
{
  PWM_CH0_BIT = 0b00000001, //0
  PWM_CH1_BIT = 0b00000010, //1
  PWM_CH2_BIT = 0b00000100, //2
  PWM_CH3_BIT = 0b00001000, //3
  PWM_CH4_BIT = 0b00010000, //4
  PWM_CH5_BIT = 0b00100000, //5
  PWM_CH6_BIT = 0b01000000, //6
  PWM_CH7_BIT = 0b10000000  //7
}PWM_CH_BIT;

//Polatiry High or Low
typedef enum BWM_Polarity
{
  StartLow,
  StartHigh
}BWM_Polarity;

//Clock A or B
typedef enum BWM_ClockSelect
{
  ClockA = 0b11111000,
  ClockB = 0b10001111
}BWM_ClockSelect;

/////////////////////////////////////////////////////////////////////////////
// Library Prototypes
/////////////////////////////////////////////////////////////////////////////

// frequency  : desired PWM frequency
// clockSpeed : clock speed in seconds
// channel    : desired channel bit
// poles      : determines whether the pulse states HIGH or LOW (0-1 as enum)
// period     : number of ticks for the length of the frequency
// duty       : number of ticks that the pulse is high relative to the period (duty / period * 100 = duty cycle)
void PWM_8Bit_Init(unsigned long frequency, unsigned long clockSpeed, PWM_CH_BIT channel, BWM_Polarity poles, unsigned int period, unsigned int duty);

// frequency  : desired PWM frequency
// clockSpeed : clock speed in seconds
// channel    : desired channel bit
// poles      : determines whether the pulse states HIGH or LOW (0-1 as enum)
// period     : number of ticks for the length of the frequency
// duty       : number of ticks that the pulse is high relative to the period (duty / period * 100 = duty cycle)
void PWM_16Bit_Init(unsigned long frequency, unsigned long clockSpeed, PWM_CH_BIT channel, BWM_Polarity poles, unsigned int period, unsigned int duty);

// enables PWM channel
// channel    : desired channel bit
void PWM_Enable(PWM_CH_BIT channel);

// disables PWM channel
// channel    : desired channel bit
void PWM_Disable(PWM_CH_BIT channel);