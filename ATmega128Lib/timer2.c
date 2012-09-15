/****************************************************
* Module: 8 Bit Timer and Counter
* 
* This module provides functions to control the 8 Bit Timer2 on the Atmega128 MCU .You can set the frequency of the 
* timer or even use it as a counter for external pulses .You can also define a function to jump to when the timer
* overflow interrupt occurs .
*
****************************************************/

/* Variables */
byte prescalarValue;
void (*timer2Interrupt)();

/* Functions */
/*
*
* Name : Timer2_Init
*
* Initializes *TIMER2* registers .This function does not return a value . 
*
* E.g. Usage :
*
* /Timer2_Init ();/ - Initializes *TIMER2* registers 
*/
void Timer2_Init()
{
    TCNT2=0;
    TCCR2=_BV(WGM21);
    OCR2=0;
    TIMSK&=~_BV(OCIE2);
    timer2Interrupt=NULL;
}

/*
* Name : Timer2_Start
*
* Starts the *TIMER2* with specified clock settings and overflow value .The timer can also be run in *counter* mode 
* by providing a clock from ouside on the pin nuber 7 of the *PORT1* .This function does not return a value . 
*
* Parameters :
* 
* /clockMode/ - Sets the prescalar values of the timer clock or selects external clock .It take following values 
              - PRESCALAR_1      (for timer clock = mainclock/1)
              - PRESCALAR_8      (for timer clock = mainclock/8)
              - PRESCALAR_64     (for timer clock = mainclock/64)
              - PRESCALAR_256    (for timer clock = mainclock/256)  
              - PRESCALAR_1024   (for timer clock = mainclock/1024) 
              - EXTCLK_POS_EDGE  (for external clock at pin 7 of *PORT1* .Trasition at +ve edge)
              - EXTCLK_NEG_EDGE  (for external clock at pin 7 of *PORT1* .Trasition at -ve edge)

* /topValue/  - Numbers of clock cycles to count before the overflow interrupt occurs .
* 
* Clock Frequency :
* 
* The clock frequency is related to the prescalar value and top value bye the following formula 
*
*  Timer_OverFlow_Frequency = Main_Clock_Frequency/((Prescalar Value)*(1+Top Value))
*
*  Main Clock for *MegaBoard* has frequency of 16Mhz
* 
*  *E.g. for Timer Frequency* 
* 
*  We calculate frequency when we have Prescalar value of 256 and Top Value of 24
*
*  Timer_OverFlow_Frequency = 16000000/(256*(1+24)) = 16000000/6400 = 250Hz
* 
* So here our *TIMER2* interrupt function will be called at the rate of 250Hz
* 
* E.g. Usage :
*
* /Timer2_Start (PRESCALAR_256,24);/ - Starts *TIMER2* with an interrupt frequency of 250Hz as from above example
*/
void Timer2_Start(byte clockMode,byte topValue)
{
    OCR2=topValue;
    TCCR2|=(clockMode&0x07);
    prescalarValue=(clockMode&0x07);
}

/*
*
* Name : Timer2_SetInterrupt
*
* Sets the function which is to be called when the *TIMER2* overflow occurs .You have to provide the name of the function 
* which will be your interrupt handler function .
*
* Parameters :
* 
* /fptr/ - Function pointer of (void)(*)() type .You just have to give the name of a function of void return type with no
           parameters to which to jump to when the *TIMER2* overflow interrupt occurs.
* 
* E.g. Usage :
*  
* An example external interrupt handler function -
*          
* /void TimerInterruptHandler()/ - Interrupt Handler function definition
* { - Parenthesis Open                                                                                
*  timerCount++; - Incrementing of a global count variable                  
* } - Parenthesis Close                                                 
*
* /Timer2_SetInterrupt (TimerInterruptHandler); / - Sets TimerInterruptHandler as the *TIMER2* interrupt handler function 
*/   
void Timer2_SetInterrupt(void (*fptr)())
{
    TIMSK|=_BV(OCIE2);
    timer2Interrupt=fptr;
}

/*
* Name : Timer2_ResetInterrupt
* 
* This function disables the *TIMER2* overflow interrupt .
*
* E.g. Usage :
*
* /Timer2_ResetInterrupt (0); / - Disables *TIMER2* overflow interrupt
*/  
void Timer2_ResetInterrupt()
{
     Timer2_Pause();
     TIMSK&=~_BV(OCIE2);
     timer2Interrupt=NULL;
}

/*
* Name : Timer2_Pause
* 
* This function pauses the *TIMER2* .
*
* E.g. Usage :
*
* /Timer2_Pause (); / - Pauses *TIMER2* 
*
* void Timer2_Pause()
*/ 

/*
* Name : Timer2_Resume
* 
* This function resumes the *TIMER2* without resetting the timer registers .To be used after /Timer2_Pause/ has been called .
*
* E.g. Usage :
*
* /Timer2_Resume (); / - Resumes *TIMER2* operation 
*
* void Timer2_Resume()
*/ 
SIGNAL(SIG_OUTPUT_COMPARE2 )
{
    if(timer2Interrupt!=NULL)
        timer2Interrupt();
}