//Switch LED Library
//Processor: MC9S12XDP512
//Crystal: 8 MHz
//by Bigoa Gach
//Mar. 22, 2022

//libraries
#include "derivative.h"
#include "lcd.h"
#include "ECT.h"
#include "Clock.h"

//constants
#define lcd_RWUP DDRH = 0; PORTK |= 2; //inputs, set R/W* high
#define lcd_RWDOWN PORTK &= (~2); DDRH = 0xFF; //set R/W* lower, outputs
#define lcd_EUP PORTK |= 1;
#define lcd_EDOWN PORTK &= (~1);
#define lcd_RSUP PORTK |= 4;
#define lcd_RSDOWN PORTK &= (~4);
#define lcd_MICRODELAY { char __x = 1; while(--__x); } //RS == ADDRESS

char lcd_Busy(void)
{
    //blocking - wait for LCD to not be busy
    //service function - private

    unsigned char inValue = 0;

    lcd_RSDOWN; //instruction
    lcd_RWUP; //reading

    do
    {
        lcd_EUP;
        lcd_MICRODELAY;

        inValue = PTH;
        lcd_EDOWN;
    } while (inValue & 0x80);
    return(inValue);
}

void lcd_Ctrl (unsigned char dragonballDurag)
{

    (void)lcd_Busy(); //wait for LCD to not be busy

    lcd_RWDOWN; //write
    lcd_RSDOWN; //command

    PTH = dragonballDurag; //present data on port

    lcd_EUP; 
    lcd_EUP;
    lcd_EDOWN; //latch

}

void lcd_Init (void)
{

    if(!TSCR1_TEN) {
        Timer_Init(Timer_Prescale_128);
        TimerCH_EnableAsOutput(Timer_CH1_BIT, 0);
    }

    DDRH = 0xFF; //PH7-PH0 -> D7 - D0 (data Lines)
    DDRK |= 0b00000111; //PK2-PK0 -> Control Lines (output)
    Timer_Sleep_ms(45); //1st delay - Delay 40+ [ms]

    //Present Data on PTH
    PTH  = 0b00111000;  //Function Set
    /*         ||||||_____(don't care)
               |||||______(don't care)
               ||||_______font:  5x8 matrix (LOW)
               |||________lines: 2 (HIGH)
               ||_________data:  8-bit (HIGH)
               |__________function set of commands
    */ 

    //Write Operation - Instruction Register
    PORTK_PK1 = 0; //PK1->R/W->0 
    PORTK_PK2 = 0; //PK2->RS->0

    //Latch Instruction
    PORTK_PK0 = 1;
    Timer_Sleep_ms(1); 
    PORTK_PK0 = 0;

    Timer_Sleep_ms(5); //2nd Delay, 4.1ms+   

    //Latch same Instruction again
    PORTK_PK0 = 1;
    Timer_Sleep_ms(1); 
    PORTK_PK0 = 0;

    Timer_Sleep_ms(1); //third Delay  100uS+ 

    //Latch same Instruction again
    PORTK_PK0 = 1;
    Timer_Sleep_ms(1); 
    PORTK_PK0 = 0;    

    //Busy flag is active now**********************
    //lcd_Ctrl(0b00111000); //Function set
    //We can check for busy flag now**********************
    //5x8 dots, 2 lines
    lcd_Ctrl(0b00111000); 
    lcd_Ctrl(0b00001110);   //display controls
    /*             ||||_____Blink:   LOW for off
                   |||______Cursor:  HIGH for on
                   ||_______Display: HIGH for on
                   |________Display Control commands    
    */

    lcd_Ctrl(0b00000001);   //clear display, home position
    lcd_Ctrl(0b00000110);   //mode controls
    /*              |||_____Shift:   LOW for no display shift
                    ||______Inc/Dec: HIGH for increment (to the left)
                    |_______Entry Mode commands
    */      
}

void lcd_Data (unsigned char val)
{

    (void)lcd_Busy(); //wait for LCD to not be busy

    lcd_RWDOWN; //write
    lcd_RSUP; //command

    PTH = val; //present data on port

    lcd_EUP; 
    lcd_EDOWN; //latch

    lcd_EUP;
    lcd_EDOWN; //latch

}

//the following code is referenced from the 
//"Old" CMPE2200 Notepack on moodle

void lcd_Addr (unsigned char addr)
{

    addr |= 0b10000000;
    lcd_Ctrl(addr);

}

void lcd_AddrXY (unsigned char ix, unsigned char iy)
{

    if(ix > 3 || iy > 19)
        lcd_Addr(0);
    else
    {
        switch (ix)
        {
            case 0: lcd_Addr(iy); break;      //first row 
            case 1: lcd_Addr(iy + 64); break; //second row 
            case 2: lcd_Addr(iy + 20); break; //third row
            case 3: lcd_Addr(iy + 84); break; //forth row
        }
    }

}

void lcd_String (char const * straddr)
{

    while(*straddr != 0)      //watch for NULL terminator
        lcd_Data(*straddr++); //send next character

}

void lcd_StringXY (unsigned char ix, unsigned char iy, char const * const straddr)
{

    lcd_AddrXY(ix, iy);
    lcd_String(straddr);

}

void lcd_Clear (void)
{

    (void)lcd_Busy(); //wait for LCD to not be busy
    lcd_Ctrl(0b00000001);

}

void lcd_Home (void)
{

    (void)lcd_Busy(); //wait for LCD to not be busy
    lcd_Ctrl(0b00000010);

}

void lcd_DispControl (unsigned char curon, unsigned char blinkon)
{

    (void)lcd_Busy(); //wait for LCD to not be busy
    if(curon == 0)
    {
        if(blinkon == 0)
        {
            lcd_Ctrl(0b00001100);
        }
        else if(blinkon == 1)
        {
            lcd_Ctrl(0b00001101);
        }
    }
    else if(curon == 1)
    {
        if(blinkon == 0)
        {
            lcd_Ctrl(0b00001110);
        }
        else if(blinkon == 1)
        {
            lcd_Ctrl(0b00001111);
        }
    }

}