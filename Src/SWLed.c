//Switch LED Library
//Processor: 
//Crystal: 16 MHz
//by Bigoa Gach
//Jan. 24, 2022

#include "derivative.h"
#include "SwLed.h"

void SwLED_Init(void)
{
    PT1AD1 = 0;  //Clear port
    DDR1AD1 = 0b11100000;  //LEDs as output, buttons as input
    ATD1DIEN1 |= 0b00011111; //Enable GPIOs for buttons
}

void LED_On(char _led)
{
    PT1AD1 |= _led;
}

void LED_Off(char _led)
{
    PT1AD1 &= ~_led;
}

void LED_Tog(char _led)
{
    PT1AD1 ^= _led;
}

void Sw_Process(SwState* _state, unsigned char _mask)
{
    if(PT1AD1 & _mask)
    { //Switch active
      if(*_state == Idle)
      { //just pressed
        *_state = Pressed;
      }
      else
      {
        *_state = Held;
      }
    }
    else
    {//Switch not-active
      if(*_state == Held)
      {//just released
        *_state = Released;
      }
      else
      {
        *_state = Idle;
      }
    }
}

unsigned int SwPressed(void)
{
  int i, count = 0;
  for(i = 0; i < 5; i++)
  {
    if(PT1AD1 & 0b00000001<<i)
    {
      ++count;
    }
  }
  return count;
}

unsigned char SwCk(void)
{
  unsigned char cSample1 = 1;
  unsigned char cSample2 = 0;

  while(cSample1 != cSample2)
  {
    cSample1 = PT1AD1 & 0b00011111;
  }
  return cSample1;
}