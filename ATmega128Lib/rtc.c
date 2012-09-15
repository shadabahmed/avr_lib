/****************************************************
* Module: Real Time Counter
* 
* The Real Time Counter is clocked by a 32.768Khz watch crystal and hence very accurate for real time keeping .The 
* functions are defined for setting delays and for setting interrupt handler function to which to jump to when 
* certain amount of time has elasped .
*
****************************************************/

void (*rtcInterrupt)();
volatile unsigned int rtcCount;
unsigned int rtcMax;
/* Functions */

/*
*
* Name : RTC_Init
*
* Initializes *Real Time Counter* registers .This function does not return a value . 
*
* E.g. Usage :
*
* /RTC_Init ();/ - Initializes *RTC* registers 
*/
void RTC_Init()
{
    TCCR0=0;
    TCNT0=0;
    TIMSK&=~_BV(OCIE0)|~_BV(TOIE0);
    OCR0=0;
    ASSR=_BV(AS0);
    rtcInterrupt=NULL;
    rtcCount=0;
}

/*
*
* Name : RTC_SetInterrupt
*
* Sets the function which is to be called when the *RTC* completes the specified number of ticks .You have to provide the
* name of the function which will be your interrupt handler function and the number of ticks .One tick of the RTC equals
* 1/128th of a second.
*
* Parameters :
* 
* /fptr/ - Function pointer of (void)(*)() type .You just have to give the name of a function of void return type with no
           parameters to which to jump to when the *RTC* completes the specified number of ticks
* 
* /count/ - Number of ticks which the *RTC* must complete before calling the interrupt handler .
*
* E.g. Usage :
*  
* An example external interrupt handler function -
*          
* /void RTCInterruptHandler()/ - Interrupt Handler function definition
* { - Parenthesis Open                                                                                
*  timerCount++; - Incrementing of a global count variable                  
* } - Parenthesis Close                                                 
*
* /RTC_SetInterrupt (RTCInterruptHandler,128); / - Sets TimerInterruptHandler as the *TIMER2* interrupt handler function and jumps to that function every 1 second
*/   
void RTC_SetInterrupt(void (*fptr)(),unsigned int count)
{
  TIMSK|=_BV(TOIE0);
  rtcInterrupt=fptr;
  rtcMax=rtcCount=count; 
}
/*
* Name : RTC_ResetInterrupt
* 
* This function disables the *RTC* overflow interrupt .
*
* E.g. Usage :
*
* /RTC_ResetInterrupt (); / - Disables *RTC* overflow interrupt
*/  
void RTC_ResetInterrupt()
{
  TIMSK&=~_BV(TOIE0);
  rtcInterrupt=NULL; 
}
/*
* Name : RTC_Delay
* 
* Goes into a loop for specified number of ticks where one tick equals to 1/128th of a second .
*
* E.g. Usage :
*
* /RTC_Delay (256); / - Goes into a delay loop for 2 seconds
*/ 
void RTC_Delay(unsigned int delayUnits)
{
  void (*fptr)();
  TIMSK|=_BV(TOIE0);
  fptr=rtcInterrupt;
  rtcInterrupt=NULL;
  rtcCount=delayUnits;
  RTC_Start();
  while(rtcCount>0);
  RTC_Pause();
}
/*
* Name : RTC_Pause
* 
* This function pauses the *Real Time Counter* .
*
* E.g. Usage :
*
* /RTC_Pause (); / - Pauses the *RTC* 
*
* void RTC_Pause()
*/

/*
* Name : RTC_Start
* 
* This function starts or resumes the *RTC* from a pause without resetting the timer registers .
*
* E.g. Usage :
*
* /RTC_Start (); / - Resumes *TIMER2* operation 
*
* void RTC_Start()
*/ 
SIGNAL(SIG_OVERFLOW0)
{
    if((rtcInterrupt!=NULL) && ((--rtcCount)==0))
    {
        rtcCount=rtcMax;
        rtcInterrupt();
    }
}
