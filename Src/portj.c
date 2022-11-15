//PORT J Library
//Processor: MC9S12XDP512
//Crystal: 16 MHz
//by Bigoa
//September 20th, 2022

#include "derivative.h"
#include "portj.h"

void PortJ_Init(char rising, char channels) 
{
    PTJ &= 0b11111100; //clear port J or will interrupt once for free 
    DDRJ &= 0b11111100; //inputs 
    PPSJ |= rising; 
    PIEJ |= channels; 
}