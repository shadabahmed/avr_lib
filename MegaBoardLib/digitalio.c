/****************************************************
* Module: Digital I/O and Interrupts
* 
* This module provides simple functions to read from and write to the digital I/O ports on the *MegaBoard* .There are two
* digital ports on *MegaBoard* ,*PORT1* and *PORT2*(also serves and the external interrupts port) .Functions for external 
* interrupts are also provided .
*
****************************************************/

/* Variables */

void (* extInterrupt0)();
void (* extInterrupt1)();
void (* extInterrupt2)();
void (* extInterrupt3)();


byte port1Mode=_MODE_INPUT_;
byte port2Mode=_MODE_INPUT_;

/* Functions */


/*
* Name : IO_ReadPort1
*
* Reads the 8 bit port ,*PORT1* , and returns the value ranging from 0-255 .
* 
* E.g. Usage :
*
* /byte value=IO_ReadPort1 ();/ - Reads the *PORT1* and initializes a variable to its value 
*/
byte IO_ReadPort1()
{
 if(port1Mode!=_MODE_INPUT_)
 {
  port1Mode=_MODE_INPUT_;
  DDRB&=0xFC;
  DDRE&=0xF3;
  DDRD&=0x0F;
 }
 return (byte)((PINB&0x03)|(PINE&0x0C)|(PINF&0xF0));
}

/*
* Name : IO_ReadPort1Bit
*
* Reads the value of an individual pin of the 8 bit port ,*PORT1* , and returns the values 0 or 1 .The 8 bits of the
* *PORT1* are numbered 0 to 7 .  
*
* Parameters :
* 
* /bitNumber/ - Range 0-7 .The bit number to be read 
* 
* E.g. Usage :
*
* /if(IO_ReadPort1Bit (7)) Foo();/ - Reads the 7th bit on *PORT1* and if the bit is set then calls some funtion .
*/
byte IO_ReadPort1Bit(byte bitNumber)
{
    port1Mode=_MODE_BITMODE_;
    if(bitNumber<2)
    {
     DDRB&=~_BV(bitNumber);
     return (PINB & _BV(bitNumber)>>(bitNumber)); 
    }
    else if(bitNumber<4)
    {
     DDRE&=~_BV(bitNumber);
     return (PINE & _BV(bitNumber)>>(bitNumber)); 
    }
    else if(bitNumber<8)
    {
     DDRD&=~_BV(bitNumber);
     return (PIND & _BV(bitNumber)>>(bitNumber));  
    }
    return 0;
}
/*
* Name : IO_WritePort1
*
* Writes an 8 bit value on the *PORT1* .
* 
* Parameters
*
* /value/ - Range 0-255 .Value to be written on the *PORT1*
*
* E.g. Usage :
*
* /IO_WritePort1 (0xF2);/ - Writes value 0xF2(242) on *PORT1*
*/
void IO_WritePort1(byte value)
{
 if(port1Mode!=_MODE_OUTPUT_)
 {
  port1Mode=_MODE_OUTPUT_;
  DDRB|=0x03;
  DDRE|=0x0C;
  DDRD|=0xF0;
 }
 PORTB=(PORTB&0xFC)|(value&0x03);
 PORTE=(PORTE&0xF3)|(value&0x0C);
 PORTD=(PORTD&0x0F)|(value&0xF0);
}

/*
* Name : IO_WritePort1Bit
*
* Sets or restes the bit value of an individual pin of the 8 bit port ,*PORT1* .The 8 bits of the *PORT1* are 
* numbered 0 to 7 .  
*
* Parameters :
*  
* /bitValue/ - 0 or 1 to set or reset the bit 
*
* /bitNumber/ - Range 0-7 .The bit number to be read 
* 
* E.g. Usage :
*
* /IO_WritePort1Bit (1,7)) / - Sets the 7th bit on *PORT1* 
*/
void IO_WritePort1Bit(byte bitValue,byte bitNumber)
{
    port1Mode=_MODE_BITMODE_;
    if(bitNumber<2)
    {
     DDRB|=_BV(bitNumber);
     if(bitValue!=0)
      PORTB|=_BV(bitNumber);
     else
      PORTB&=~_BV(bitNumber);
    }
    else if(bitNumber<4)
    {
     DDRE|=_BV(bitNumber);
     if(bitValue!=0)
      PORTE|=_BV(bitNumber);
     else
      PORTE&=~_BV(bitNumber);
    }
    else if(bitNumber<8)
    {
     DDRD|=_BV(bitNumber);
     if(bitValue!=0)
      PORTD|=_BV(bitNumber);
     else
      PORTD&=~_BV(bitNumber);
    }
}


/*
* Name : IO_ReadPort2
*
* Reads the 4 bit port ,*PORT2* , and returns the value ranging from 0-15 .
* 
* E.g. Usage :
*
* /byte value=IO_ReadPort2 ();/ - Reads the *PORT2* and initializes a variable to its value 
*/
byte IO_ReadPort2()
{
 if(port2Mode!=_MODE_INPUT_)
 { 
     DDRE&=0x0F;
     port2Mode=_MODE_INPUT_;
 }
 return (byte)((PINE)>>4);
}

/*
* Name : IO_ReadPort2Bit
*
* Reads the value of an individual pin of the 4 bit port ,*PORT2* , and returns the values 0 or 1 .The 4 bits of the
* *PORT2* are numbered 0 to 3 .  
*
* Parameters :
* 
* /bitNumber/ - Range 0-3 .The bit number to be read 
* 
* E.g. Usage :
*
* /if(IO_ReadPort2Bit (3)) Foo();/ - Reads the 3rd bit on *PORT2* and if the bit is set then calls some funtion .
*/
byte IO_ReadPort2Bit(byte bitNumber)
{
    port2Mode=_MODE_BITMODE_;
    if(bitNumber<4)
    {
     DDRE&=~_BV(bitNumber+4);
     return ((PINB & _BV(bitNumber+4))>>(bitNumber+4)); 
    }
    return 0;
}

/*
* Name : IO_WritePort2
*
* Writes a 4 bit value on the *PORT2* .
* 
* Parameters
*
* /value/ - Range 0-15 .Value to be written on the *PORT2*
*
* E.g. Usage :
*
* /IO_WritePort2 (0x0C);/ - Writes value 0x0C(12) on *PORT2*
*/
void IO_WritePort2(byte value)
{
 if(port2Mode!=_MODE_OUTPUT_)
 {
  DDRE|=0xF0;
  port2Mode=_MODE_OUTPUT_;
 }
 PORTE=((value&0x0F)<<4);
}

/*
* Name : IO_WritePort2Bit
*
* Sets or restes the bit value of an individual pin of the 4 bit port ,*PORT2* .The 3 bits of the *PORT1* are 
* numbered 0 to 3 .  
*
* Parameters :
*  
* /bitValue/ - 0 or 1 to set or reset the bit 
*
* /bitNumber/ - Range 0-3 .The bit number to be read 
* 
* E.g. Usage :
*
* /IO_WritePort2Bit (1,3)) / - Sets the 3rd bit on *PORT2* 
*/
void IO_WritePort2Bit(byte value,byte bitNumber)
{
    port2Mode=_MODE_BITMODE_;
    if(bitNumber<4)
    {
     DDRE|=_BV(bitNumber+4);
     if(value==0)
      PORTE&=~_BV(bitNumber+4);
     else
      PORTE|=_BV(bitNumber+4); 
    }
}


/*
* Name : IO_SetExtInterrupt
*
* Sets the *PORT2* bits in external interrupt mode .The mode of interrupt can also be set .Only those pins which have 
* interrupt functions set will work as interrupt pins while rest of the pins will work in normal I/O mode .You also
* have to supply a function name as a parameter which will be called when the interrupt occurs .See example for usage .
*
* Parameters :
*  
* /interruptNumber/ - pin number to be set in interrupt mode (same as pin number when reading in bit mode)
*
* /interruptMode/ - Three Modes -
                  - INT_POS_EDGE (for positive edge triggered interrupt)
                  - INT_NEG_EDGE (for negative edge triggered interrupt) 
                  - INT_LOW_LEVEL(for low level triggered interrupt) 
*
* /fptr/ - Function pointer of (void)(*)() type .You just have to give the name of a function of void return type with no
           parameters to which to jump to when the external interrupt occurs.
* 
* E.g. Usage :
*  
* An example external interrupt handler function -
*          
* /void ExtInterruptHandler(byte receivedData)/ - Interrupt Handler function definition
* { - Parenthesis Open                                                                                
*  pulseCount++; - Incrementing of a global count variable                  
* } - Parenthesis Close                                                 
*
* /IO_SetExtInterrupt (0,INT_POS_EDGE,ExtInterruptHandler); / - Sets ExtInterruptHandler as the external interrupt handler fucntion for external interrupt 0 in positive edge triggered mode 
*/    
void IO_SetExtInterrupt(byte interruptNumber,byte interruptMode,void (* fptr)())
{
    if(interruptNumber==1)
    {
      EICRB=(EICRB&0xFC)|(interruptMode&0x03);
      DDRE&=~_BV(interruptNumber+4);
      extInterrupt0=fptr;
      EIMSK|=_BV(interruptNumber+4);
    }
    else if(interruptNumber==2)
    {
      EICRB=(EICRB&0xF3)|((interruptMode&0x03)<<2);
      DDRE&=~_BV(interruptNumber+4);
      extInterrupt1=fptr;
      EIMSK|=_BV(interruptNumber+4);
    }
    else if(interruptNumber==3)
    {
      EICRB=(EICRB&0xCF)|((interruptMode&0x03)<<4);
      DDRE&=~_BV(interruptNumber+4);
      extInterrupt2=fptr;
      EIMSK|=_BV(interruptNumber+4);
    }
    else if(interruptNumber==4)
    {
      EICRB=(EICRB&0x3F)|((interruptMode&0x03)<<6);
      DDRE&=~_BV(interruptNumber+4);
      extInterrupt3=fptr;
      EIMSK|=_BV(interruptNumber+4);
    }
}
/*
* Name : IO_ResetExtInterrupt
* 
*  This function disables the external interrupt for the pin number specified .
*
* Parameters :
*  
* /interruptNumber/ - Interrupt number to be disabled (same as pin number when reading in bit mode)
*
* E.g. Usage :
*
* /IO_ResetExtInterrupt (0); / - Disables the external interrupt number 0 
*/  
void IO_ResetExtInterrupt(byte interruptNumber)
{
 if(interruptNumber<4)
      EIMSK&=~_BV(interruptNumber+4);
}
SIGNAL(SIG_INTERRUPT4)
{
 if(extInterrupt0!=NULL)
    extInterrupt0();
}
SIGNAL(SIG_INTERRUPT5)
{
 if(extInterrupt1!=NULL)
    extInterrupt1();
}
SIGNAL(SIG_INTERRUPT6)
{
 if(extInterrupt2!=NULL)
    extInterrupt2();
}
SIGNAL(SIG_INTERRUPT7)
{
 if(extInterrupt3!=NULL)
    extInterrupt3();
}