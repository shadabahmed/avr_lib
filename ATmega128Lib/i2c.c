/****************************************************
* Module: I2C Bus
* 
* The *I2C* module provides simple functions to communicate with *I2C* devices such as sonar sensors ,eeproms etc .To use   
* the the functions provided there shud be atleast one *I2C* device connected on the *I2C* port .
*
*/

/* Functions */

/*
*
* Name : I2C_Init
* 
* Initializes the *I2C* module .This function is already called by the /MegaBoardInit/ function so no need to call 
* it explicitly unless you want to reinitialize the module .This function does not return any value.
*
* E.g. Usage :
*
* /I2C_Init ();/ - Initliazes the *I2C* module
*/
void I2C_Init()
{
 TWCR=_BV(TWEN)|_BV(TWEA);
 TWSR=0;
 TWBR=28;
}

/*
*
* Name : I2C_WriteData
* 
* This function writes an array of bytes to an i2c device .The return value is an integer which is negative for error and
* has value 1 if the operation succeeds.
*
* Parameters
*
* /i2cSlaveAdd/ - Range 1-127 . *I2C* slave address 
* /i2cData/ - Array of bytes to be written
* /i2cDataSize/ - Range 0-255 .Number of bytes to be written
* /stopFlag/ - Range 0 or 1 . If 0 then *STOP* condition is not written on the bus else it is written
*
* Return Error Codes :
*
*  -1      - Start Condition problem
*  -2      - Address Mismatch
*  -3      - Data Transmission Error 
* 
* E.g. Usage :
*
* /I2C_WriteData (0x70,"\x00\x50",2,1);/ - Writes bytes 0x00 and 0x50 to an *I2C* device with address 0x70 
*/
int I2C_WriteData(byte i2cSlaveAdd,byte * i2cData,byte i2cDataSize,byte stopFlag)
{
  byte i;
  GEN_START();
  I2C_Wait();
  if(I2C_SCODE!=I2C_START && I2C_SCODE!=I2C_REP_START)
  {
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_START_ERROR;
  }

  TWDR=I2C_SLA_W;
  CLR_TWINT();  
  I2C_Wait();
  if(I2C_SCODE!=I2C_MT_SLA_ACK)
  {   
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_SLAVEACK_ERROR;
  }

  for(i=0;i<i2cDataSize;i++)
  {
   TWDR=i2cData[i];
   CLR_TWINT();  
   I2C_Wait();
   if(I2C_SCODE!=I2C_MT_DATA_ACK)
   {
       GEN_STOP();
       I2C_WaitForStop();
       return I2C_SLAVEDATA_ERROR;
   }
  }
  
  if(stopFlag)
  {
   GEN_STOP();
   I2C_WaitForStop();
  }

 return I2C_SUCCESS;  
}

/*
*
* Name : I2C_WriteRegister
* 
* This function writes an array of bytes to an i2c device register .The return value is an integer which is negative for error and
* has value 1 if the operation succeeds.
*
* Parameters
*
* /i2cSlaveAdd/ - Range 1-127 . *I2C* slave address 
* /registerAddress/ - Range 0-255 . *I2C* device register address
* /i2cData/ - Array of bytes to be written
* /i2cDataSize/ - Range 0-255 .Number of bytes to be written
* /stopFlag/ - Range 0 or 1 . If 0 then *STOP* condition is not written on the bus else it is written
*
* Return Error Codes :
*
*  -1      - Start Condition problem
*  -2      - Address Mismatch
*  -3      - Data Transmission Error 
* 
* E.g. Usage :
*
* /I2C_WriteRegister (0x70,0,"\x50",1,1);/ - Writes byte 0x50 to the register 0 of an *I2C* device with address 0x70 
*/
int I2C_WriteRegister(byte i2cSlaveAdd,byte registerAddress,byte * i2cData,byte i2cDataSize,byte stopFlag)
{
  byte i;
  GEN_START();
  I2C_Wait();
  if(I2C_SCODE!=I2C_START && I2C_SCODE!=I2C_REP_START)
  {
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_START_ERROR;
  }

  TWDR=I2C_SLA_W;
  CLR_TWINT();  
  I2C_Wait();
  if(I2C_SCODE!=I2C_MT_SLA_ACK)
   {   
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_SLAVEACK_ERROR;
   }

  TWDR=registerAddress;
  CLR_TWINT();  
  I2C_Wait();
  if(I2C_SCODE!=I2C_MT_DATA_ACK)
   {
       GEN_STOP();
       I2C_WaitForStop();
       return I2C_SLAVEDATA_ERROR;
   }

  for(i=0;i<i2cDataSize;i++)
  {
   TWDR=i2cData[i];
   CLR_TWINT();  
   I2C_Wait();
   if(I2C_SCODE!=I2C_MT_DATA_ACK)
   {
       GEN_STOP();
       I2C_WaitForStop();
       return I2C_SLAVEDATA_ERROR;
   }
  }
  
  if(stopFlag)
  {
   GEN_STOP();
   I2C_WaitForStop();
  }

 return I2C_SUCCESS;  

}


/*
*
* Name : I2C_ReadData
* 
* This function reads an array of bytes from an *I2C* device .The return value is an integer which is negative for error and
* has value 1 if the operation succeeds.
*
* Parameters
*
* /i2cSlaveAdd/ - Range 1-127 . *I2C* slave address 
* /registerAddress/ - Range 0-255 . *I2C* device register address
* /i2cData/ - Pointer to the array of bytes to which data will be read
* /i2cDataSize/ - Range 0-255 .Number of bytes to be written
* /stopFlag/ - Range 0 or 1 . If 0 then *STOP* condition is not written on the bus else it is written
*
* Return Error Codes :
*
*  -1      - Start Condition problem
*  -2      - Address Mismatch
*  -3      - Data Transmission Error 
*
* E.g. Usage :
*
* /I2C_ReadRegister (0x70,2,inputArray,1,1);/ - Reads one byte from the register number 2 of an *I2C* device with address 0x70 and stores the byte in inputArray 
*/
int I2C_ReadData(byte i2cSlaveAdd,byte * i2cData,byte i2cDataSize,byte stopFlag)
{
  byte i;
  GEN_START();
  I2C_Wait();
  if(I2C_SCODE!=I2C_START && I2C_SCODE!=I2C_REP_START)
   {
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_START_ERROR;
   }
  
  TWDR=I2C_SLA_R;
  CLR_TWINT(); 
  I2C_Wait();
  if(I2C_SCODE!=I2C_MR_SLA_ACK)
   {  
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_SLAVEACK_ERROR;
   }
  
  for(i=0;i<i2cDataSize;i++)
   {
    CLR_TWINT();  
    I2C_Wait();
    if(I2C_SCODE!=I2C_MR_DATA_ACK)
     {
       GEN_STOP();
       I2C_WaitForStop();      
       return I2C_SLAVEDATA_ERROR;
     }
    i2cData[i]=TWDR;
   }
  
  if(stopFlag)
   {
    GEN_STOP();
    I2C_WaitForStop();
   }
 return I2C_SUCCESS;  
} 

/*
*
* Name : I2C_ReadRegister
* 
* This function reads an array of bytes from a register of an *I2C* device .The return value is an integer which is negative for error and
* has value 1 if the operation succeeds.
*
* Parameters
*
* /i2cSlaveAdd/ - Range 1-127 . *I2C* slave address 
* /i2cData/ - Pointer to the array of bytes to which data will be read
* /i2cDataSize/ - Range 0-255 .Number of bytes to be written
* /stopFlag/ - Range 0 or 1 . If 0 then *STOP* condition is not written on the bus else it is written
*
* Return Error Codes :
*
*  -1      - Start Condition problem
*  -2      - Address Mismatch
*  -3      - Data Transmission Error 
*
* E.g. Usage :
*
* /I2C_ReadData (0x70,inputArray,2,1);/ - Reads two bytes from an *I2C* device with address 0x70 and stores them in the inputArray 
*/
int I2C_ReadRegister(byte i2cSlaveAdd,byte registerAddress,byte * i2cData,byte i2cDataSize,byte stopFlag)
{
  byte i;

  GEN_START();
  I2C_Wait();
  if(I2C_SCODE!=I2C_START && I2C_SCODE!=I2C_REP_START)
  {
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_START_ERROR;
  }

  TWDR=I2C_SLA_W;
  CLR_TWINT();  
  I2C_Wait();
  if(I2C_SCODE!=I2C_MT_SLA_ACK)
   {   
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_SLAVEACK_ERROR;
   }

  TWDR=registerAddress;
  CLR_TWINT();  
  I2C_Wait();
  if(I2C_SCODE!=I2C_MT_DATA_ACK)
   {
       GEN_STOP();
       I2C_WaitForStop();
       return I2C_SLAVEDATA_ERROR;
   }

  GEN_START();
  I2C_Wait();
  if(I2C_SCODE!=I2C_START && I2C_SCODE!=I2C_REP_START)
   {
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_START_ERROR;
   }
  
  TWDR=I2C_SLA_R;
  CLR_TWINT(); 
  I2C_Wait();
  if(I2C_SCODE!=I2C_MR_SLA_ACK)
   {  
      GEN_STOP();
      I2C_WaitForStop();
      return I2C_SLAVEACK_ERROR;
   }
  
  for(i=0;i<i2cDataSize;i++)
   {
    CLR_TWINT();  
    I2C_Wait();
    if(I2C_SCODE!=I2C_MR_DATA_ACK)
     {
       GEN_STOP();
       I2C_WaitForStop();      
       return I2C_SLAVEDATA_ERROR;
     }
    i2cData[i]=TWDR;
   }
  
  if(stopFlag)
   {
    GEN_STOP();
    I2C_WaitForStop();
   }
 return I2C_SUCCESS;  
} 