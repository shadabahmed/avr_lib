/****************************************************
* Module: DC Motors control using PWM
* 
* The *DCMOTORS* module can control upto 2 DC motors with current rating of upto 2 amperes per motor .
* This module provides easy to use functions which control the speed and direction of rotation of the DC motors .  
*
*/

/* Functions */
/*
*
* Name : DCmotors_Init
* 
* Initializes the *DCMOTORS* module .This function is already called by the /MegaBoardInit/ function so no need to call 
* it explicitly unless you want to reinitialize the module .This function does not return any value.
*
* E.g. Usage :
*
* /DCmotors_Init ();/ - Initliazes the *DCMOTORS* module
*/
void DCmotors_Init()
{
 DDRD=0xF0;
 TCCR3A=_BV(COM3B1)|_BV(COM3C1)|_BV(WGM30);
 TCCR3B=_BV(WGM33);
 TCNT3=0;
 OCR3A=PWMREGISTER;
 DDRE=_BV(4)|_BV(5);
}


/* Functions */
/*
*
* Name : DCmotors_ChangeDir
* 
* Changes the direction of DC motors .Note that the direction is dependent on the connections made between the motors and 
* the DC motors port .Please first calibrate the directions by switching between various connection settings .This function
* does not return any value .
*
* Parameters :
*
* /leftMotorDir/ - takes the values _CLOCKWISE_ or _ACLOCKWISE_
*
* /rightMotorDir/ - takes the values _CLOCKWISE_ or _ACLOCKWISE_
*
* E.g. Usage :
*
* /DCmotors_ChangeDir (_CLOCKWISE_,_ACLOCKWISE_);/ - Changes the direction of both the motors
*/
void DCmotors_ChangeDir(byte leftMotorDir,byte rightMotorDir)
{
    PORTD=((leftMotorDir<<6)&0xC0)|((rightMotorDir<<4)&0x30);
}

/* Functions */
/*
*
* Name : DCmotors_ChangeSpeed
* 
* Changes the speed of the DC motors .Note that DC motors require starting torque therefore there is a threshold speed
* value below which motors remain stalled .This value is dependent on load,gearing,motor contruction etc so you have to 
* determine that yourself .This function does not return any value.
* 
* Parameters :
*
* /leftMotorSpeed/ - takes the values between 0-100 (0 - stopped and 100 - full speed)
*
* /rightMotorSpeed/ - takes the values between 0-100 (0 - stopped and 100 - full speed)
*
* E.g. Usage :
*
* /DCmotors_ChangeSpeed (100,100);/ - Change the speed of both the motors
*/
void DCmotors_ChangeSpeed(unsigned int leftMotorSpeed,unsigned int rightMotorSpeed)
{
  if(leftMotorSpeed<=100 && rightMotorSpeed<=100)
  {
   leftMotorSpeed=(unsigned int)((MAXPWM/100)*(float)leftMotorSpeed); 
   rightMotorSpeed=(unsigned int)((MAXPWM/100)*(float)rightMotorSpeed);  
   OCR3B=leftMotorSpeed;
   OCR3C=rightMotorSpeed;
  }
}

/*
*
* Name : DCmotors_SetLeftMotor
* 
* Changes the speed and direction of the left DC motor individually .
* 
* Parameters :
*
* /motorSpeed/ - takes the values between 0-100 (0 - stopped and 100 - full speed)
*
* /motorDir/ - takes the values _CLOCKWISE_ or _ACLOCKWISE_
*
* E.g. Usage :
*
* /DCmotors_SetLeftMotor (90,_CLOCKWISE_);/ - Rotate the left motor clockwise with 90% speed
*/
void DCmotors_SetLeftMotor(unsigned int motorSpeed,byte motorDir)
{
  if(motorSpeed<=100)
  {
   motorSpeed=(unsigned int)((MAXPWM/100)*(float)motorSpeed);  
   OCR3B=motorSpeed;
  }
   PORTD&=(motorDir<<6)&0xC0;
}

/*
*
* Name : DCmotors_SetRightMotor
* 
* Changes the speed and direction of the right DC motor individually .
* 
* Parameters :
*
* /motorSpeed/ - takes the values between 0-100 (0 - stopped and 100 - full speed)
*
* /motorDir/ - takes the values _CLOCKWISE_ or _ACLOCKWISE_
*
* E.g. Usage :
*
* /DCmotors_SetRightMotor (100,100);/ - Rotate the right motor clockwise with 90% speed
*/
void DCmotors_SetRightMotor(unsigned int motorSpeed,byte motorDir)
{
  if(motorSpeed<=100)
  {
   motorSpeed=(unsigned int)((MAXPWM/100)*(float)motorSpeed);  
   OCR3C=motorSpeed;
  }
  PORTD&=(motorDir<<4)&0x30;
}

/*
*
* Name : DCmotors_ApplyBrakes
* 
* Applies brakes to both the motors .
* 
* E.g. Usage :
*
* /DCmotors_ApplyBrakes ();/ - Applies brakes to both the motors
*/
void DCmotors_ApplyBrakes()
{
 OCR3B=MAXPWM;
 OCR3C=MAXPWM;
 PORTD|=0xF0;
}