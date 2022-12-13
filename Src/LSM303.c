/* CODE SUPPLIED BY SIMON WALKER!!!! */
#include <hidef.h>
#include "derivative.h"
#include "LSM303.h"
#include "i2c.h"

int LSM303_Init (void)
{
  if (!I2C_SendAddressRW(LSM303_ADDR_MAG, I2C_WRITE, I2C_WAIT))
  {
    // register 0x00 - CRA_REG_M
    (void)I2C_WriteByte (0x00, I2C_NOSTOP);
    // temperature sensor on, 3Hz
    (void)I2C_WriteByte (0b10001000, I2C_STOP);
  }
  else
    return -1;
  
  if (!I2C_SendAddressRW(LSM303_ADDR_MAG, I2C_WRITE, I2C_WAIT))
  {
    // register 0x02 - MR_REG_M
    (void)I2C_WriteByte (0x02, I2C_NOSTOP);
    // continuous conversion mode
    (void)I2C_WriteByte (0b00000000, I2C_STOP);
  }
  else
    return -1;

  if (!I2C_SendAddressRW(LSM303_ADDR_ACC, I2C_WRITE, I2C_WAIT))
  {
    // register 0x20 - CTRL_REG1_A
    (void)I2C_WriteByte (0x20, I2C_NOSTOP);
    // setup for 10Hz, normal mode, XYZ enable
    (void)I2C_WriteByte (0b00100111, I2C_STOP);
  }
  else
    return -1;

  if (!I2C_SendAddressRW(LSM303_ADDR_ACC, I2C_WRITE, I2C_WAIT))
  {
    // register 0x23 - CTRL_REG4_A
    (void)I2C_WriteByte (0x23, I2C_NOSTOP);
    // setup for block update, MSB @ lower address, 2G, high resolution
    (void)I2C_WriteByte (0b11001000, I2C_STOP);
  }
  else
    return -1;

  return 0;  
}

unsigned char LSM303_GetMagStatus (void)
{
  unsigned char retVal = 0;

  // read status register (is data available?)
  if (!I2C_SendAddressRW (LSM303_ADDR_MAG, I2C_WRITE, I2C_WAIT))
  {
    // status register
    (void)I2C_WriteByte (0x09, I2C_NOSTOP);
    I2C_IssueRestart();
    if (!I2C_SendAddressRW (LSM303_ADDR_MAG, I2C_READ, I2C_NOWAIT))
    {
      (void)I2C_RXByte (&retVal, I2C_NACK, I2C_STOP);
      return retVal;
    }
  }  
  else
    return 0;  
}


unsigned char LSM303_GetAccStatus (void)
{
  // see get mag status! rework for acc
  unsigned char retVal = 0;

  // read status register (is data available?)
  if (!I2C_SendAddressRW (LSM303_ADDR_ACC, I2C_WRITE, I2C_WAIT))
  {
    // status register
    (void)I2C_WriteByte (0x27, I2C_NOSTOP);
    I2C_IssueRestart();
    if (!I2C_SendAddressRW (LSM303_ADDR_ACC, I2C_READ, I2C_NOWAIT))
    {
      (void)I2C_RXByte (&retVal, I2C_NACK, I2C_STOP);
      return retVal;
    }
  }  
  else
    return 0;  
}

int LSM303_GetXYZ (unsigned char * pTarget)
{
  // get status
  unsigned char retVal = LSM303_GetAccStatus();
  
  // is XYZ data available?
  if (retVal & 0x08)
  {
    // have xyz data
    if (!I2C_SendAddressRW (LSM303_ADDR_ACC, I2C_WRITE, I2C_WAIT))
    {
      // start at register 0x28, read two bytes (note:
      //  this device is strange - to do multi-byte
      //  reads, it wants the MSB of the register
      //  set, but *also* needs regular ACK/NACK signals)
      (void)I2C_WriteByte (0x28 | 0x80, I2C_NOSTOP);
      I2C_IssueRestart();
      if (!I2C_SendAddressRW (LSM303_ADDR_ACC, I2C_READ, I2C_NOWAIT))
      {
        (void)I2C_RXByte (pTarget + 0, I2C_ACK, I2C_NOSTOP); // 28
        (void)I2C_RXByte (pTarget + 1, I2C_ACK, I2C_NOSTOP); // 29
        (void)I2C_RXByte (pTarget + 2, I2C_ACK, I2C_NOSTOP); // 2A
        (void)I2C_RXByte (pTarget + 3, I2C_ACK, I2C_NOSTOP); // 2B
        (void)I2C_RXByte (pTarget + 4, I2C_ACK, I2C_NOSTOP); // 2C
        (void)I2C_RXByte (pTarget + 5, I2C_NACK, I2C_STOP);  // 2D
        return 0;
      }
    }                
  }
  else
  {
    return -1;
  }  
}
