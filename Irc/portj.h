//PORT J Library
//Processor: MC9S12XDP512
//Crystal: 16 MHz
//by Bigoa
//September 20th, 2022

#define PJ0 0b00000001
#define PJ1 0b00000010

// setup port J for interrupts
void PortJ_Init(char rising, char channels); 