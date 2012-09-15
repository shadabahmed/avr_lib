/****************************************************
* Module: Stepper Motors Control
* 
* The *STEPPER* module can control upto 2 stepper motors with current rating of upto 1.5 amperes per phase .
* This module provides easy to use functions which control the stepper motors accurately .This library has been     
* designed with differential drive in mind i.e. two stepper motors for two wheels on opposite sides .This drive    
* is used in most robots with 2 wheels and 3 point of contact as a castor.
*
****************************************************/

/* Stepper Ramping Array */
 const int rampArray[RAMPSTAGES]= RAMPARRAY;
/* Variables */
static struct{
unsigned int stepsToTake;
unsigned int stepsTaken;
unsigned char rampStage;
unsigned char stepperFilter;
}stepperStruct;

/* Functions */
/*
*
* Name : Stepper_Init
* 
* Initializes the *STEPPER* drivers,port and the stepping mode .This function is already called by the /MegaBoardInit/  
* function so no need to call it explicitly unless you want to change the stepping mode .This function does not return 
* any value.
*
* Parameters :
*
* /steppingMode/ -  takes the values *_HALFSTEPPINGMODE_* or *_FULLSTEPPINGMODE_* . Half stepping mode provides smoother movements but 
  lesser torque while full stepping mode provides less smooth movemets but more torque .Remember that in half stepping 
  mode ,the stepping angle is reduced to half its value      
*
* E.g. Usage :
*
* /Stepper_Init (_HALFSTEPPINGMODE_);/ - Initliazed the *STEPPER* motors controller in half stepping mode
*/
void Stepper_Init(byte steppingMode)
{
 cli();
 DDRA=0xFF;
 if(steppingMode==_HALFSTEPPINGMODE_)
  PORTA=0;
 else 
  PORTA=0x90;
 stepperStruct.stepsToTake=0;
 stepperStruct.rampStage=0;
 stepperStruct.stepsTaken=0;
 stepperStruct.stepperFilter=0xff;
 ICR1=rampArray[0];
 OCR1A=ICR1/2;
 TCNT1=0;
 TCCR1A=0;
 TCCR1B=_BV(WGM13)|_BV(WGM12);
 TIMSK|=_BV(OCIE1A);
 sei();
}

/*
*
* Name : Stepper_MoveStraight
* Move the robot straight for the specified number of steps .Remember that the direction is dependent on the connections 
* of the stepper motors made to the stepper motors port so it is likely that /Stepper_MoveStraight/ and /Stepper_RoatateAboutCenter/
* functions might be intechanged on the robot .Please first calibrate using these functions so that the functions reflect  
* the correct motion of the robot .The function returns back as soon as the robot starts moving .If you want to wait till the robot
* stops then use the function /Stepper_WaitForStop/ in the next line of calling this function .This function does not return
* any value .
*
* Parameters :
*
* /steps/ -  Range 0-65536 .Number of steps to take .Each stepper motor has specified rotation per step and from that value       
*            the number of steps can be related to the lenght ,the robot moves ,in centimeters or meters can be calculated.
*
* /dir/   -  Can take values *FORWARD* OR *BACKWARD* for straight line motion
*
* E.g. Usage :
*
* /Stepper_MoveStraight (100,FORWARD);/ - Moves the robot 100 steps forward
*/
void Stepper_MoveStraight(unsigned int steps,byte dir)
{
  if(dir==FORWARD)
 {
     PORTA|=_BV(5);
     PORTA&=~_BV(2);
 }
 else
 {
     PORTA|=_BV(2);
     PORTA&=~_BV(5);
 }
 stepperStruct.stepperFilter=0xFF;
 stepperStruct.stepsTaken=0;
 stepperStruct.stepsToTake=steps;
 StartSteppers();
}

/*
*
* Name : Stepper_RotateAboutCenter
* Rotates the robot in clockwise or anticlockwise for the specified number of steps .Remember that the direction is dependent 
* on the connections of the stepper motors made to the stepper motors port so it is likely that /Stepper_MoveStraight/ and 
* /Stepper_RoatateAboutCenter/ functions might be intechanged on the robot .Please first calibrate using these functions so that the functions reflect  
* the correct motion of the robot .The function returns back as soon as the robot starts moving .If you want to wait till the robot
* stops then use the function /Stepper_WaitForStop/ in the next line of calling this function .This function does not return
* any value .
*
* Parameters :
*
* /steps/ -  Range 0-65536 .Number of steps to take .Each stepper motor has specified rotation per step and from that value       
*            the number of steps can be related to the lenght ,the robot moves ,in centimeters or meters can be calculated.
*
* /dir/   -  Can take values *CLOCKWISE* AND *ANTICLOCKWISE* for rotation
*
* E.g. Usage :
*
* /Stepper_RotateAboutCenter (200,CLOCKWISE);/ - Rotates the robot 200 steps in the clockwise direction
*/
void Stepper_RotateAboutCenter(unsigned int steps,byte dir)
{
  if(dir==0)
  PORTA|=_BV(5)|_BV(2);
  else
   PORTA&=~(_BV(5)|_BV(2));
 stepperStruct.stepperFilter=0xFF;
 stepperStruct.stepsTaken=0;
 stepperStruct.stepsToTake=steps;
 StartSteppers();
}

/*
*
* Name : Stepper_RotateAboutWheel
* Rotates the robot in clockwise or anticlockwise about one wheel for the specified number of steps .In this mode one of the 
* motor is stopped while the other rotates hence the robot rotates about the stopped wheel .Remember that the direction 
* is dependent on the connections of the stepper motors made to the stepper motors port so please first calibrate using these functions so that 
* the functions reflect the correct motion of the robot .The function returns back as soon as the robot starts moving .If you 
* want to wait till the robot stops then use the function /Stepper_WaitForStop/ in the next line of calling this function
* .This function does not return any value .
*
* Parameters :
*
* /steps/ -  Range 0-65536 .Number of steps to take .Each stepper motor has specified rotation per step and from that value       
*            the number of steps can be related to the lenght ,the robot moves ,in centimeters or meters can be calculated.
*
* /dir/   -  Can take values *CLOCKWISE* OR *ANTICLOCKWISE* for rotation
*
* /wheel/ -  Can take values *LEFT* OR *RIGHT* which inply which motor to be stopped or in other term about which 
*            wheel robot is to be rotated  
*
* E.g. Usage :
*
* /Stepper_RotateAboutWheel (200,CLOCKWISE,LEFT);/ - Rotates the robot 200 steps in the clockwise direction about the left wheel
*/
void Stepper_RotateAboutWheel(unsigned int steps,byte wheel,byte dir)
{
  if(dir==0)
  PORTA|=_BV(5)|_BV(2);
  else
   PORTA&=~(_BV(5)|_BV(2));

 if(wheel==LEFT)
  stepperStruct.stepperFilter=0x40;
 else
  stepperStruct.stepperFilter=0x08;

 stepperStruct.stepsTaken=0;
 stepperStruct.stepsToTake=steps;
 StartSteppers();
}

/*
*
* Name : Stepper_WaitForStop
* This function waits until the robot stops and then returns .It does not return any value.
*
* E.g. Usage :
*
* /Stepper_WaitForStop ();/ - Waits until the robot stops
*/
void Stepper_WaitForStop()
{
    while( TCCR1B&(_BV(CS11)|_BV(CS10)));
}

SIGNAL(SIG_OUTPUT_COMPARE1A)
{

 if(stepperStruct.stepsToTake==0)
  StopSteppers();
 
 if(PORTA & 0x48)
  PORTA&=~(_BV(6)|_BV(3));
 else
 {
  PORTA|=(0x48 & stepperStruct.stepperFilter);
  stepperStruct.stepsToTake--;
  stepperStruct.stepsTaken++;
 }
 if(stepperStruct.stepsTaken<RAMPDURATION && stepperStruct.stepsTaken<stepperStruct.stepsToTake)
 {
  stepperStruct.rampStage=stepperStruct.stepsTaken/RAMPINTERVAL;
  ICR1=rampArray[stepperStruct.rampStage];
  OCR1A=ICR1/2;
 }
 else if(stepperStruct.stepsToTake<RAMPDURATION && stepperStruct.stepsToTake<=stepperStruct.stepsTaken)
 {
  stepperStruct.rampStage=stepperStruct.stepsToTake/RAMPINTERVAL;
  ICR1=rampArray[stepperStruct.rampStage];
  OCR1A=ICR1/2;
 }
}

