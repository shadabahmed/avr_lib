/****************************************************
* Module: Servo Motor Control
* 
* The *SERVO* module can control upto 8 hobby servo motors with a precision of upto 0.036 degrees .    
*
****************************************************/

/* Variables */
 unsigned const int servoTimerConstants[8]={200,5200,10200,15200,20200,25200,30200,35200};
 unsigned int servoValues[8]={(END_VALUE-START_VALUE)/2,(END_VALUE-START_VALUE)/2,(END_VALUE-START_VALUE)/2,(END_VALUE-START_VALUE)/2,(END_VALUE-START_VALUE)/2,(END_VALUE-START_VALUE)/2,(END_VALUE-START_VALUE)/2,(END_VALUE-START_VALUE)/2};
 byte servoFlag;
 byte servoNumber;


/* Functions */

/*
*
* Name : Servo_Init
*
* Initalizes the *SERVOS* port and timer which controls the servos .No need to call this function unless you want to 
* reset the *SERVO* port and timer .This function is implicitly called by the /MegaBoard_Init()/ function . This 
* function does not return a value . 
*
* E.g. Usage :
*
* /Servo_Init ();/ - Initliazed the *SERVO* motor controller
*/
void Servo_Init(void)
{
 byte count;
 _SERVO_DIR_PORT_=0xFF;
 _SERVO_PORT_=0;
 servoFlag=1;
 servoNumber=0;
#ifdef _SERVO_TIMER1_
  TIMSK|=_BV(OCIE1B);
  TCNT1=0x0;
  TCCR1A=0x0;
  TCCR1B=_BV(WGM12);
  TCCR1C=0x0;
  OCR1A=40500;
  OCR1B=200;	
#elif defined _SERVO_TIMER3_
  ETIMSK|=_BV(OCIE3B);
  TCNT3=0x0;
  TCCR3A=0x0;
  TCCR3B=_BV(WGM12);
  TCCR3C=0x0;
  OCR3A=40500;
  OCR3B=200; 
#else 
 #error No timer available for servo motor controller
#endif
}

/* Functions */

/*
*
* Name : Servo_Start
*
* Starts the timer for the *SERVO* motors .This function must be called to start the *SERVO* motors .You can set the
* initial values of the angles of the *SERVO* motors by calling /Servo_SetAngles/ function .This function does not   
* return any value. 
*
* E.g. Usage :
*
* /Servo_Start ();/ - Starts the *SERVO* motors 
*/
void Servo_Start()
{
    #ifdef _SERVO_TIMER1_
     TCCR1B|=_BV(CS11); 
    #elif defined _SERVO_TIMER3_
      TCCR3B|=_BV(CS11); 
    #endif
}


/*
*
* Name : Servo_SetAngles
*
* This function sets the angles of the *SERVO* motors simulatneously .This function can also be used to initialize the
* starting angles of the *SERVO* motors before you call the /Servo_Start/ function .This function does not return any
* value .
*
* Parameters : 
*
* servoAngle1 - Range 0-180 floating point value for the angle for *SERVO* motor 1
* servoAngle2 - Range 0-180 floating point value for the angle for *SERVO* motor 2
* servoAngle3 - Range 0-180 floating point value for the angle for *SERVO* motor 3
* servoAngle4 - Range 0-180 floating point value for the angle for *SERVO* motor 4
* servoAngle5 - Range 0-180 floating point value for the angle for *SERVO* motor 5
* servoAngle6 - Range 0-180 floating point value for the angle for *SERVO* motor 6
* servoAngle7 - Range 0-180 floating point value for the angle for *SERVO* motor 7
* servoAngle8 - Range 0-180 floating point value for the angle for *SERVO* motor 8
*
* E.g. Usage :
*
* /Servo_SetAngles (90,10.5,23.5,32.5,66.8,140.3,114,45);/ - sets the angles for all the *SERVO* motors
*/
void Servo_SetAngles(float servoAngle1,float servoAngle2,float servoAngle3,float servoAngle4,float servoAngle5,float servoAngle6,float servoAngle7,float servoAngle8)
{
 servoValues[0]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle1);   
 servoValues[1]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle2);
 servoValues[2]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle3);
 servoValues[3]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle4);
 servoValues[4]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle5);
 servoValues[5]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle6);
 servoValues[6]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle7);
 servoValues[7]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle8);
}

/*
*
* Name : Servo_CenterAll
*
* This function sets the angles of the *SERVO* motors simulatneously to the center position i.e 90 degrees .The motors will
* only move once the /Servo_Start/ has been called .If /Servo_Start/ function has already been called then the motor will directly move 
* to the specified angle .This function does not return any value .
*
* E.g. Usage :
*
* /Servo_CenterAll ();/ - moves all the *SERVO* motors to the center position
*/
void Servo_CenterAll()
{
 byte i;
 for(i=0;i<8;i++)
     servoValues[i]=(END_VALUE-START_VALUE)/2;
}

/*
*
* Name : Servo_SetAngle
*
* This function moves a desired *SERVO* motor to the specified angle .The motor will only move once the Servo_Start has 
* been called .If /Servo_Start/ function has already been called then the motor will directly move to the specified 
* angle .This function does not return any value .
*
* Parameters : 
*
* /servoAngle/ - Range 0-180 floating point value for the angle for the desired motor
*
* /servoMotorNumber/ - Range 1-8 byte value for the desired *SERVO* motor number
*
* E.g. Usage :
*
* /Servo_SetAngle (78.5,7);/ - moves the 7th *SERVO* motor to 78.5 degrees from the exterem position
*/
void Servo_SetAngle(float servoAngle,byte servoMotorNumber)
{
 if(servoMotorNumber>0 && servoMotorNumber<9) 
 servoValues[servoMotorNumber]=START_VALUE+(((float)(END_VALUE-START_VALUE)/180)*servoAngle);
}



#ifdef _SERVO_TIMER1_
 SIGNAL(SIG_OUTPUT_COMPARE1B)
 {
 if(servoFlag==0)
   {
    _SERVO_PORT_&=~_BV(servoNumber);
    servoFlag=1;
    servoNumber++;
    servoNumber%=8;
    OCR1B=servoTimerConstants[servoNumber];
   }
  else
   {
    OCR1B+=servoValues[servoNumber];
    _SERVO_PORT_|=_BV(servoNumber);
    servoFlag=0;
  }
 }	  	
#elif defined _SERVO_TIMER3_
 SIGNAL(SIG_OUTPUT_COMPARE3B)
 {
 if(servoFlag==0)
   {
    _SERVO_PORT_&=~_BV(servoNumber);
    servoFlag=1;
    servoNumber++;
    servoNumber%=8;
    OCR3B=servoTimerConstants[servoNumber];
   }
  else
   {
    OCR3B+=servoValues[servoNumber];
    _SERVO_PORT_|=_BV(servoNumber);
    servoFlag=0;
  }
 }
#endif