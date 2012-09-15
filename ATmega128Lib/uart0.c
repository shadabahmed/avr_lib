/****************************************************
* Module: Serial Port 1(UART0)
* 
* This module contains various functions to handle
* serial port communication effectively with a PC or 
* with another microcontroller through the UART0 on the 
* ATmega128 microcontroller. The functions for *UART0* and 
*  *UART1* are similar except the respective functions are
* prefixed by the prefix /Uart0_/ or /Uart1_/ .
*
****************************************************/


/* 
*
* Buffer Variables 
* 
* The buffer variables are used for temporary storage of data while
* sending and receiving of data.
*/
byte tx0Buffer[TX0BUFFERSIZE];
byte rx0Buffer[RX0BUFFERSIZE];

/* Function pointer declaration for receive interrupt */

void (* rx0Interrupt)(byte);

/* Functions */

/*
*
* Name : Uart0_Init
*
* Initialize *UART0* with specified baud rate for sending and receiving data .This function does not return a value . 
* 
* Parameters :
*
* /baudRate/ - Baudrate for *UART0* . 
*
* E.g. Usage :
*
* /Uart0_Init (19200);/ - Initializes *UART0* at 19200bps baudrate 
*/
void Uart0_Init(unsigned long baudRate)
{
 cli();
 UBRR0H=UBRR0H_VAL(baudRate);
 UBRR0L=UBRR0L_VAL(baudRate); 
 #ifndef _U2X0_
  UCSR0A=_BV(UDRE0);
 #else
  UCSR0A=_BV(UDRE0)|_BV(U2X0);
 #endif
 UCSR0B=_BV(RXEN0)|_BV(TXEN0);
 UCSR0C=_BV(UCSZ01)|_BV(UCSZ00);
 sei();
}

/*
*
* Name : Uart0_WriteByte
*
* Send a single byte of data or a character from *UART0* .This function does not return a value .  
* 
* Parameters :
*
* /data/ - A byte or character. 
*
* E.g. Usage :
* 
* /Uart0_WriteByte (0x55) ;/ - Sends out a byte with 0x55(85 in decimal) value    
*
* /Uart0_WriteByte ('c') ;/ - Sends out character 'c' 
*
*  
*/
void Uart0_WriteByte(byte data)
{
  while ( !(UCSR0A & _BV(UDRE0)));
  UDR0=data;
}

/*
*
* Name : Uart0_WriteBytes
*
* Send out an array of bytes or characters from *UART0* .This function does not return a value .  
* 
* Parameters :
*
* /txData/ - Pointer to a byte or character array .
*
* /size/ - Size of the array with maximum value of upto the size of tx0Buffer array . 
*
* E.g. Usage :
* 
* 
* byte txData[4]={0x32,0x38,0x12,0xFA} ; - Byte array initialization 
*
* /Uart0_WriteBytes (txData,4) ;/ - Sends out first four bytes in the *txData* array   
*
* char txChars[5]={"Hello"} ;  - Character array initialization
*
* /Uart0_WriteBytes (txChars,5) ;/ - Sends out first five characters in the *txChars* array  
* 
* /Uart0_WriteBytes ("Hello! World",5) ;/ - Sends out first five characters in the string *"Hello! World"*  
*
* Alternatively /Uart0_WriteString/ can also be used for sending out strings.
*
*/
void Uart0_WriteBytes(byte * txData,byte size)
{
 int i;
 for(i=0;i<size;i++)
 {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0=txData[i];
 }
}

/*
*
* Name : Uart0_WriteString
*
* Send out an array of characters from *UART0* .This function does not return a value .  
* 
* Parameters :
*
* /txChars/ - Pointer to a byte or character array.
*
* E.g. Usage :
* 
* /Uart0_WriteString ("Hello! World")/ - Sends out a string. 
*
* /Uart0_WriteString (charArray)/ - Sends out a string terminated in null '\0' character.
*
*/
void Uart0_WriteString(char * txChars)
{
 int i;
 for(i=0;i<strlen(txChars);i++)
 {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0=txChars[i];
 }
}

/*
*
* Name : Uart0_ReadByte
*
* Returns a single byte of data or a character read from *UART0* . This function waits until it receives a byte
* from *UART0* .
* 
* Output:
*
* 0-255 - A byte or character value. 
*
* E.g. Usage :
* 
* /byte rxByte = Uart0_ReadByte () ;/ - Initalizes rxByte with a byte read from *UART0*   
*
* /char rxChar = Uart0_ReadByte () ;/ - Initalizes rxChar with a character read from *UART0* 
*
*  
*/
byte Uart0_ReadByte( )
{
  while(!(UCSR0A & _BV(RXC0)));
  return _MMIO_BYTE(_SFR_ADDR(UDR0));
}


/*
*
* Name : Uart0_ReadBytes
*
* Returns an array of bytes or characters read from *UART0* . This function waits until all the byte are received
* from *UART0* as specified by the /size/ parameter .
*
* Parameters :
*
* /size/ - Size of the array with maximum value of upto the size of rx0Buffer array . 
*
* Output :
*
* Pointer to the first byte or character read from *UART0* .
*
* E.g. Usage :
* 
* /byte char * rxData = Uart0_ReadBytes (4) ;/ - Initializes a byte array with four bytes read from *UART0* 
*
* /char * rxChars = Uart0_ReadBytes (4) ;    / - Initializes a character array with four characters read from *UART0* 
*
*/
byte * Uart0_ReadBytes(byte size)
{
   byte i;
   for(i=0;i<size && i<RX0BUFFERSIZE;i++)
   {
       while(!(UCSR0A & _BV(RXC0)));
       rx0Buffer[i]=UDR0;
   }
   return rx0Buffer;
}


/*
*
* Name : Uart0_ReadString
*
* Returns an array of bytes or characters read from *UART0* . This function takes a termination character as parameter
* and it waits until that character is encountered .
* 
* Parameters :
*
* /termChar/ - Termination character as when to stop reading . 
*
* Output :
*
* Pointer to the first byte or character read from *UART0* . The termination character is not part of the output.
*
* E.g. Usage :
* 
* /char * rxChars = Uart0_ReadString ('\r') ;    / - Initializes a character array with with a string before the '\r' termination character.
*/
char * Uart0_ReadString(char termChar)
{
    byte data;
    byte rxPos=0;
    while(1)
    {
     while(!(UCSR0A & _BV(RXC0)));
     data=UDR0;
     if(data==termChar || rxPos==RX0BUFFERSIZE)
     {
         rx0Buffer[rxPos]='\0';
         return rx0Buffer;
     }
     else
       rx0Buffer[rxPos++]=data; 
    }
}

/*
*
* Name : Uart0_SetReceiveInterrupt
*
* This function assigns the receive interrupt handler and enables the receive interrupt .The parameter is the name of the function which you want to jump
* to when the receive interrupt occurs .No explicit knowledge of function pointers is necessary .See example for usage .
*
* Parameters :
*
* /fptr/ - Function pointer to a function of the (void)(*)(byte) type .You have to enter the name of a function which has void return type and byte value as parameter .   
*
* E.g. Usage :
*  
* An example receive interrupt handler function -
*
* /void ReceiveInterruptHandler(byte receivedData)/ - Interrupt Handler function definition
* { - Parenthesis Open                                             
* if(receivedData=='$') - Comparison of data which arrived at *UART0*                                     
*  globalCount++; - Incrementing of a global count variable                  
* } - Parenthesis Close                                                 
*
* /Uart0_SetReceiveInterrupt (ReceiveInterruptHandler); / - Sets ReceiveInterruptHandler as the receive interrupt handler fucntion 
*/
void Uart0_SetReceiveInterrupt(void (* fptr)(byte))
{
    UCSR0B|=_BV(RXCIE0);
    rx0Interrupt=fptr;
}

/*
*
* Name : Uart0_ResetReceiveInterrupt
*
* This function disables the receive interrupt .
*
* E.g. Usage :
*  
* /Uart0_ResetReceiveInterrupt (ReceiveInterruptHandler); / - Disables the receive interrupt.
*/
void Uart0_ResetReceiveInterrupt()
{
  UCSR0B&=~_BV(RXCIE0);
  rx0Interrupt=NULL;
}

/*
*
* Name : Uart0_printf
*
* This is a C macro with similar functionality as printf in standard C except the output goes to *UART0* . This macro
* does not return a value .You can also use width and modifiers in the format string for e.g. "%5d" .
* 
* Parameters :
*
* /format/ - The format string for printf .  
*
* /.../ - This parameter means it can take a list of varibles with the size of the list not being fixed .
*
* E.g. Usage :
* 
* /Uart0_printf ("%d %c %s %f \n",213,'d',"hello",3.142); / - Prints the string in exact same way as printf function would but on *UART0* 
*/
//{{The following line is for document generator}}
//void Uart0_printf(char * format,...)

/*
*
* Name : Uart0_scanf
*
* This is a C macro with similar functionality as scanf in standard C except the input comes from *UART0* . This macro
* does not return a value .The function reads the string until the termination character is encountered . You can also 
* use width and modifiers in the format string for e.g. "%5d" .
*
* Parameters :
*
* /termChar/ - Termination character .
*
* /format/ - The format string for printf .  
*
* /.../ - This parameter means it can take a list of varibles with the size of the list not being fixed .
*
* E.g. Usage :
*
* int intData; - integer declaration
* char charData; - character declaration
* char * stringData; - string declaration
* float floatData;  - float declaration
* /Uart0_scanf ("%d %c %s %f \n",&intData,&charData,stringData,&floatData); / - Inputs the variables
* 
*/
//{{The following line is for document generator}}
//void Uart0_scanf(char termChar,char * format,...)

/* UART0 Send Interrupt */
SIGNAL(SIG_UART0_RECV)
{
 byte data;
 if(rx0Interrupt!=NULL)
   {
     data=UDR0;
     rx0Interrupt(data);
   }
}

/* UART0 Send Interrupt */

SIGNAL(SIG_UART0_TRANS)
{

}

