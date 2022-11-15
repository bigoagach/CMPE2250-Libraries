//Switch LED Library
//Processor: MC9S12XDP512
//Crystal: 8MHz
//by Bigoa Gach
//Feb. 04, 2022


#include "derivative.h" /* common defines and macros */
#include "SevSeg.h" /* Seven Segment Display */

#define Segs_WLatch PORTA &= (~0x01); PORTA |= 0x01;
#define Segs_ML PORTA &= (~0x02);
#define Segs_MH PORTA |= 0x02;

void SevSeg_Init(void)
{

    PORTA |= 0x03; //ensure port pins high
    DDRA |= 0x03; //configure direction of port pins as outputs
    DDRB = 0xff; //configure direction of port pins as outputs

    SevSeg_ClearAll(); //clears seven segment displays

}

void SevSeg_ClearDigit(unsigned char digit)
{

    unsigned char clearVal = 0b10000000;

    //call SevSeg_Char()
    SevSeg_Cust(digit, clearVal);

}

void SevSeg_ClearAll(void)
{

    int address;

    for(address = 0; address < 8; address++)
    {
        SevSeg_ClearDigit((unsigned char)address);
    }

}

void SevSeg_Char(unsigned char address, unsigned char character)
{

    address &= 0x07; //limits address to position 0 to 7
    address |= 0b01011000; 
    /*           ||||||||_____Addr
                 |||||________Bank A
                 ||||_________Normal
                 |||__________Decode  
                 ||___________Decode as HEX
                 |____________No Data coming
    */

   character |= 0x80;

   PORTB = address; //configuring bits

   Segs_MH //present command w/MODE high
   Segs_WLatch //latches command

   PORTB = character; //present data

   Segs_ML //present command w/MODE low
   Segs_WLatch //latches command

}

void SevSeg_CharD(unsigned char address, unsigned char character)
{

    address &= 0x07; //limits address to position 0 to 7
    address |= 0b01011000; 
    /*           ||||||||_____Addr
                 |||||________Bank A
                 ||||_________Normal
                 |||__________Decode  
                 ||___________Decode as HEX
                 |____________No Data coming
    */

   character &= (~0x80);

   PORTB = address; //configuring bits

   Segs_MH //present command w/MODE high
   Segs_WLatch //latches command

   PORTB = character; //present data

   Segs_ML //present command w/MODE low
   Segs_WLatch //latches command

}

void SevSeg_Cust(unsigned char address, unsigned char character)
{
    
    address &= 0x07; //limits address to position 0 to 7
    address |= 0b01111000; 
    /*           ||||||||_____Addr
                 |||||________Bank A
                 ||||_________Normal
                 |||__________No Decode  
                 ||___________Decode as HEX
                 |____________No Data coming
    */

   PORTB = address; //configuring bits

   Segs_MH //present command w/MODE high
   Segs_WLatch //latches command

   PORTB = character; //present data

   Segs_ML //present command w/MODE low
   Segs_WLatch //latches command

}

void SevSeg_Error(unsigned int row) 
{ //this hoe doesn't work damn >:(
    if(row == 0) {
        SevSeg_Cust(0, 0b10011111);
        SevSeg_Cust(1, 0b10001100);
        SevSeg_Cust(2, 0b10011001);
        SevSeg_Cust(3, 0b10011110);
    }
    else if(row == 1) {
        SevSeg_Cust(4, 0b10011111);
        SevSeg_Cust(5, 0b10001100);
        SevSeg_Cust(6, 0b10011001);
        SevSeg_Cust(7, 0b10011110);
    }
}

void SevSeg_ClearTop4(void)
{

    int address;

    for(address = 0; address < 4; address++)
    {
        SevSeg_ClearDigit((unsigned char)address);
    }

}

void SevSeg_ClearBot4(void)
{

    int address;

    for(address = 4; address < 8; address++)
    {
        SevSeg_ClearDigit((unsigned char)address);
    }

}

void SevSeg_Top4(unsigned int nibble)
{

    unsigned char character = (unsigned char)nibble & (unsigned char)0b00001111;
    unsigned char address = 3;
    unsigned char count;

    for(count = 0; count < 4; count++)
    {
        character = ((unsigned char)(nibble >> (unsigned char)(4 * count))) & 0b00001111;
        SevSeg_Char(address, character);
        address--;
    }

}

void SevSeg_Bot4(unsigned int nibble)
{

    unsigned char character = ((unsigned char)nibble) & 0b00001111;
    unsigned char address = 7;
    unsigned char count;

    for(count = 0; count < 4; count++)
    {
        character = ((unsigned char)(nibble >> (unsigned char)(4 * count))) & 0b00001111;
        SevSeg_Char(address, character);
        address--;
    }

}

void SevSeg_Top4D(unsigned int index, unsigned int nibble)
{

    unsigned char character = ((unsigned char)nibble) & 0b00001111;
    unsigned char address = 3;
    unsigned char count;

    for(count = 0; count < 4; count++)
    {
        character = ((unsigned char)(nibble >> (unsigned char)(4 * count))) & 0b00001111;

        if(address == index)
            SevSeg_CharD(address, character);
        else
            SevSeg_Char(address, character);

        address--;
    }

}

void SevSeg_Bot4D(unsigned int index, unsigned int nibble)
{
    
    unsigned char character = ((unsigned char)nibble) & 0b00001111;
    unsigned char address = 7;
    unsigned char count;

    for(count = 0; count < 4; count++)
    {
        character = ((unsigned char)(nibble >> (unsigned char)(4 * count))) & 0b00001111;

        if(address == index)
            SevSeg_CharD(address, character);
        else
            SevSeg_Char(address, character);

        address--;
    }

}

unsigned int HexToBCD(unsigned int uHex)
{

    unsigned char i;
    unsigned int result = 0;
    unsigned int pow = 1;

    for(i = 0; i < 4; i++)
    {
        result += (uHex % 10) * pow;
        uHex /= 10;
        pow *= 16;
    }

    return result;

}

unsigned int BCDToHex(unsigned int uBcd)
{

    unsigned char i;
    unsigned int result = 0;
    unsigned int pow = 1;

    for(i = 0; i < 4; i++)
    {
        result += (uBcd % 16) * pow;
        uBcd /= 16;
        pow *= 10;
    }

    return result;

}