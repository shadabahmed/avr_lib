/****************************************************
* Module: I2C Sensors
* 
* The *I2C SENSORS* module uses the *I2C* module providing simple functions for *I2C* sensors .
* To use the the functions provided there shud be atleast one *I2C* device connected on the *I2C* port .
*
* Functions for the following I2C sensors are provided : 
*
* + SRF08 Ultrasonic Sensor
* + CMPS03 Compass Module
*
*/

/* Functions */

/* SRF08 Functions */
#define SRF08_INCHES 0x50
#define SRF08_CM     0x51
/*
*
* Name : Srf08_Ping
* 
* Initializes the *SRF08* sonar for taking readings and also sets whether the sonar should return the value in inches or  
* centimeters .The /Srf08_ReadDistance already calls this function so no need to explicitly call unless you want to check 
* the status of sonar .This function returns a negative value if there is some problem else returns 1 .For details on error
* codes see the *I2C* documentaion .
*
* Parameters :
*
* /deviceAddress/ - address of *SRF08* . Default factory set address is 0x70(112) 
*
* /readingUnit/ - Takes values SRF08_INCHES for readings in inches or SRF08_CM for readings in centimetres 
* 
* E.g. Usage :
*
* /Srf08_Ping (SRF08_CM);/ - Initializes *SRF08* sonar with reading unit as centimetres
*/
int Srf08_Ping(byte deviceAddress,byte readingUnit)
{
  return I2C_WriteRegister(deviceAddress,0,&readingUnit,1,1);
}
/*
*
* Name : Srf08_ReadLightSensor
* 
* Takes reading from the light sensor present on *SRF08* sonar sensor .This function returns a negative value if there 
* is some problem else returns the value of intensity of light ranging from 0-255 .For details on error codes see the 
* *I2C* documentaion .
* 
* /deviceAddress/ - address of *SRF08* . Default factory set address is 0x70(112) 
*
* E.g. Usage :
*
* /Srf08_ReadLightSensor ();/ - Returns reading taken from light sensor on *SRF08*
*/
int Srf08_ReadLightSensor(byte deviceAddress)
{
    byte reading=SRF08_CM;
    volatile long int loopCount;
    int returnValue;
    
    if((returnValue=I2C_WriteRegister(deviceAddress,0,&reading,1,1))<0)
        return returnValue;
    
    for(loopCount=32000;loopCount>0;loopCount--);
    
    returnValue=I2C_ReadRegister(deviceAddress,1,&reading,1,0);
    I2C_WriteData(deviceAddress,NULL,0,1);
    
    if(returnValue<0)
        return returnValue;
    else
        return (int)reading;         
}

/*
*
* Name : Srf08_ReadDistance
* 
* Takes distance reading from the *SRF08* sonar sensor .This function returns a negative value if there 
* is some problem else returns the value of distance in centimetres or inches .For details on error codes 
* see the *I2C* documentaion .
* 
* /deviceAddress/ - address of *SRF08* . Default factory set address is 0x70(112) 
*
* /readingUnit/ - Takes values SRF08_INCHES for readings in inches or SRF08_CM for readings in centimetres 
*
* E.g. Usage :
*
* /Srf08_ReadDistance (0x70,SRF08_CM);/ - Returns reading taken from light sensor on *SRF08*
*/
int Srf08_ReadDistance(byte deviceAddress,byte readingUnit)
{
    byte reading[2];
    volatile long int loopCount;
    int returnValue;
    
     if((returnValue=I2C_WriteRegister(deviceAddress,0,&readingUnit,1,1))<0)
        return returnValue;

    for(loopCount=32000;loopCount>0;loopCount--);
  
    returnValue=I2C_ReadRegister(deviceAddress,2,reading,2,0);
    I2C_WriteData(deviceAddress,NULL,0,1);
    
    if(returnValue<0)
        return returnValue;
    else
        return (int)(reading[0]<<8|reading[1]);           
}

/*
*
* Name : Srf08_ChangeAddress
* 
* If you have multiple *SRF08* sensors and wish to interface them all then you need to assign a different address to 
* each of the sensors .Remember that at the time of assigning address only one *SRF08* sensor should be interfaced to
* the *I2C* bus .This function returns 1 if succesful else on failure returns a negative value .For details on error codes 
* see the *I2C* documentaion .The red LED on *SRF08* will glow on success and you will have to restart *MegaBoard* to use 
* the new address .Read your *SRF08* documentation for more details .
* 
* Parameters : 
*
* /oldAddress/ - old address of *SRF08* . Default factory set address is 0x70(112) 
*
* /newAddress/ - new address to be set .Takes values between 0x70(112) to 0x7F(127)   
*
* E.g. Usage :
*
* /Srf08_ChangeAddress (0x70,0x7F);/ - Changed address of *SRF08* from 0x70(112) to 0x7F(127)
*/
int Srf08_ChangeAddress(byte oldAddress,byte newAddress)
{
 int returnValue;
 if((oldAddress>0x6F && oldAddress<0x80) && (newAddress>0x6F && newAddress<0x80))
  {

    if((returnValue=I2C_WriteRegister(oldAddress,0,"\xA0",1,1))<0)
      return returnValue;

    if((returnValue=I2C_WriteRegister(oldAddress,0,"\xAA",1,1))<0)
      return returnValue;

    if((returnValue=I2C_WriteRegister(oldAddress,0,"\xA5",1,1))<0)
      return returnValue;

    newAddress=(newAddress<<1);
    return I2C_WriteRegister(oldAddress,0,&newAddress,1,1);
  }
 else 
     return -2; 
}

/*
*
* Name : Srf08_SetMaxRange
* 
* Changes the maximum range of the *SRF08* sensor .The dafult value is 11.008 meters .Use this function in conjunction
* with /Srf08_SetMaxAnalogGain/ if it improves the accuracy in your readings . The value is reset on restart so you have 
* to call this function everytime the sensor is powered up .This function returns 1 if succesful else on failure returns 
* a negative value .For details on error codes see the *I2C* documentaion .
*
* Parameters : 
*
* /deviceAddress/ - address of *SRF08* . Default factory set address is 0x70(112) 
*
* /range/ - Range in milimetres .Default value is the maximum possible value of 11008mm   
*
* E.g. Usage :
*
* /Srf08_SetMaxRange (0x70,6000);/ - Sets the max range of *SRF08* with address 0x70 to 6000mm
*/
int Srf08_SetMaxRange(byte deviceAddress,unsigned int range)
{
 byte command;    
 if(range>11008)
     return -2;
 command=(range-43)/43;
 return I2C_WriteRegister(deviceAddress,2,&command,1,1);
}
/*
*
* Name : Srf08_SetMaxAnalogGain
* 
* Changes the maximum analog gain for the readings of the *SRF08* sensor .The dafult value is 31 and it varies between 
* 0-255 .Use this function in conjunction with /Srf08_SetMaxRange/ if it improves the accuracy in your readings .The  
* value is reset on restart so you have to call this function everytime the sensor is powered up .This function returns 1 
* if succesful else on failure returns a negative value .For details on error codes see the *I2C* documentaion .
*
* Parameters : 
*
* /deviceAddress/ - address of *SRF08* . Default factory set address is 0x70(112) 
*
* /gain/ - Range 0-255 .Defualt Value on startup is 31  
*
* E.g. Usage :
*
* /Srf08_SetMaxRange (0x70,6000);/ - Sets the max range of *SRF08* with address 0x70 to 6000mm
*/
int Srf08_MaxGain(byte deviceAddress,byte gain)
{
 return I2C_WriteRegister(deviceAddress,1,&gain,1,1);
}

/*
*
* Name : Cmps03_GetReading
* 
* Takes the angle reading from the *CMPS03* compass .The reading returned is angle between the direction to which the
* compass is pointing and the north direction .If the readings are not accurate enough then use the function /Cmps03_Calibrate/
* and follow the instructions displayed at the *LCD* .
* 
* Parameters :
*
* /deviceAddress/ - address of *CMPS03* . Default factory set address is 0x60(96) 
*
*
* E.g. Usage :
*
* /Cmps03_GetReading (0x60);/ - Returns reading taken from the *CMPS03* compass 
*/
int Cmps03_GetReading(byte deviceAddress)
{
 byte highByte,lowByte,dataBytes[2];
 int returnValue;
 if((returnValue=I2C_ReadRegister(deviceAddress,2,dataBytes,2,0))<0) 
     return returnValue;
 I2C_WriteData(deviceAddress,NULL,0,1);
 return ((dataBytes[0]<<8)|dataBytes[1]);
}

/*
*
* Name : Cmps03_Calibrate
* 
* Calibration is important for *CMPS03* compass module as the readings given out it are dependent upon the latitude of 
* the of reading .Calibration just needs to be done once for for your location .This function returns a negative value
* if not successful and 1 is successful along with providing visual feedback on the *LCD* .You will need to remove and 
* reapply power to use the compass with new settings .
*
* Parameters :
*
* /deviceAddress/ - address of *CMPS03* . Default factory set address is 0x60(96) 
*
*
* E.g. Usage :
*
* /Cmps03_Calibrate (0x60);/ - Starts the calibration procedure of the *CMPS03* compass 
*/
int Cmps03_Calibrate(byte deviceAddress)
{
 byte highByte,lowByte,dataBytes[2];
 volatile unsigned long loopCount;
 int returnValue;
 Lcd_printf("CMPS03   Compass--Calibration--");
 for(loopCount=900000;loopCount>0;loopCount--);
 
 Lcd_printf("Point Compass toNorth within 4s");
 Lcd_GotoXY(2,15);
 for(loopCount=500000;loopCount>0;loopCount--);
 Lcd_GotoXY(2,14);
 Lcd_putchar('3');
 for(loopCount=500000;loopCount>0;loopCount--);
 Lcd_GotoXY(2,14);
 Lcd_putchar('2');
 for(loopCount=500000;loopCount>0;loopCount--);
  Lcd_GotoXY(2,14);
 Lcd_putchar('1');
 for(loopCount=500000;loopCount>0;loopCount--);
 if((returnValue=I2C_WriteRegister(deviceAddress,15,"\xFF",1,1))<0) 
 {
     Lcd_printf("  Calibration      Failed!    ");
     return returnValue;
 }
 Lcd_printf("North Direction Calibration Done");
 for(loopCount=900000;loopCount>0;loopCount--);
 
 Lcd_printf("Point Compass toEast within 4s");
 Lcd_GotoXY(2,14);
 for(loopCount=500000;loopCount>0;loopCount--);
 Lcd_GotoXY(2,13);
 Lcd_putchar('3');
 for(loopCount=500000;loopCount>0;loopCount--);
 Lcd_GotoXY(2,13);
 Lcd_putchar('2');
 for(loopCount=500000;loopCount>0;loopCount--);
  Lcd_GotoXY(2,13);
 Lcd_putchar('1');
 for(loopCount=500000;loopCount>0;loopCount--);
  if((returnValue=I2C_WriteRegister(deviceAddress,15,"\xFF",1,1))<0) 
 {
     Lcd_printf("  Calibration      Failed!    ");
     return returnValue;
 }
 Lcd_printf("East Direction  Calibration Done");
 for(loopCount=900000;loopCount>0;loopCount--);
 
 Lcd_printf("Point Compass toSouth within 4s");
 Lcd_GotoXY(2,15);
 for(loopCount=500000;loopCount>0;loopCount--);
 Lcd_GotoXY(2,14);
 Lcd_putchar('3');
 for(loopCount=500000;loopCount>0;loopCount--);
 Lcd_GotoXY(2,14);
 Lcd_putchar('2');
 for(loopCount=500000;loopCount>0;loopCount--);
  Lcd_GotoXY(2,14);
 Lcd_putchar('1');
 for(loopCount=500000;loopCount>0;loopCount--);
  if((returnValue=I2C_WriteRegister(deviceAddress,15,"\xFF",1,1))<0) 
 {
     Lcd_printf("  Calibration      Failed!    ");
     return returnValue;
 }
 Lcd_printf("South Direction Calibration Done");
 for(loopCount=900000;loopCount>0;loopCount--);
 
 Lcd_printf("Point Compass toWest within 4s");
 Lcd_GotoXY(2,14);
 for(loopCount=500000;loopCount>0;loopCount--);
 Lcd_GotoXY(2,13);
 Lcd_putchar('3');
 for(loopCount=500000;loopCount>0;loopCount--);
 Lcd_GotoXY(2,13);
 Lcd_putchar('2');
 for(loopCount=500000;loopCount>0;loopCount--);
  Lcd_GotoXY(2,13);
 Lcd_putchar('1');
 for(loopCount=500000;loopCount>0;loopCount--);
 if((returnValue=I2C_WriteRegister(deviceAddress,15,"\xFF",1,1))<0) 
 {
     Lcd_printf("  Calibration      Failed!    ");
     return returnValue;
 }
 Lcd_printf("West Direction  Calibration Done");
 for(loopCount=900000;loopCount>0;loopCount--);
 Lcd_printf("  Calibration      Complete!    ");
 return I2C_SUCCESS;
}

