/****************************************************
* Module: Serial Port 2(UART1)
* 
* This module contains various functions to handle
* serial port communication effectively with a PC or 
* with another microcontroller through the UART1 on the 
* ATmega128 microcontroller. The functions for *UART1* and 
*  *UART0* are similar except the respective functions are
* prefixed by the prefix /Uart1_/ or /Uart0_/ .
*
****************************************************/


/* 
*
* Buffer Variables 
* 
* The buffer variables are used for temporary storage of data while
* sending and receiving of data.
*/
byte tx1Buffer[TX1BUFFERSIZE];
byte rx1Buffer[RX1BUFFERSIZE];

/* Function pointer declaration for receive interrupt */

void (* rx1Interrupt)(byte);

/* Functions */

/*
*
* Name : Uart1_Init
*
* Initialize *UART1* with specified baud rate for sending and receiving data .This function does not return a value . 
* 
* Parameters :
*
* /baudRate/ - Baudrate for *UART1* . 
*
* E.g. Usage :
*
* /Uart1_Init (19200);/ - Initializes *UART1* at 19200bps baudrate 
*/
void Uart1_Init(unsigned long baudRate)
{
 cli();
 UBRR1H=UBRR1H_VAL(baudRate);
 UBRR1L=UBRR1L_VAL(baudRate); 
 #ifndef _U2X1_
  UCSR1A=_BV(UDRE1);
 #else
  UCSR1A=_BV(UDRE1)|_BV(U2X1);
 #endif
 UCSR1B=_BV(RXEN1)|_BV(TXEN1);
 UCSR1C=_BV(UCSZ11)|_BV(UCSZ10);
 sei();
}

/*
*
* Name : Uart1_WriteByte
*
* Send a single byte of data or a character from *UART1* .This function does not return a value .  
* 
* Parameters :
*
* /data/ - A byte or character. 
*
* E.g. Usage :
* 
* /Uart1_WriteByte (0x55) ;/ - Sends out a byte with 0x55(85 in decimal) value    
*
* /Uart1_WriteByte ('c') ;/ - Sends out character 'c' 
*
*  
*/
void Uart1_WriteByte(byte data)
{
  while ( !(UCSR1A & _BV(UDRE1)));
  UDR1=data;
}

/*
*
* Name : Uart1_WriteBytes
*
* Send out an array of bytes or characters from *UART1* .This function does not return a value .  
* 
* Parameters :
*
* /txData/ - Pointer to a byte or character array .
*
* /size/ - Size of the array with maximum value of upto the size of tx1Buffer array . 
*
* E.g. Usage :
* 
* 
* byte txData[4]={0x32,0x38,0x12,0xFA} ; - Byte array initialization 
*
* /Uart1_WriteBytes (txData,4) ;/ - Sends out first four bytes in the *txData* array   
*
* char txChars[5]={"Hello"} ;  - Character array initialization
*
* /Uart1_WriteBytes (txChars,5) ;/ - Sends out first five characters in the *txChars* array  
* 
* /Uart1_WriteBytes ("Hello! World",5) ;/ - Sends out first five characters in the string *"Hello! World"*  
*
* Alternatively /Uart1_WriteString/ can also be used for sending out strings.
*
*/
void Uart1_WriteBytes(byte * txData,byte size)
{
 int i;
 for(i=0;i<size;i++)
 {
  while (!(UCSR1A & _BV(UDRE1)));
  UDR1=txData[i];
 }
}

/*
*
* Name : Uart1_WriteString
*
* Send out an array of characters from *UART1* .This function does not return a value .  
* 
* Parameters :
*
* /txChars/ - Pointer to a byte or character array.
*
* E.g. Usage :
* 
* /Uart1_WriteString ("Hello! World")/ - Sends out a string. 
*
* /Uart1_WriteString (charArray)/ - Sends out a string terminated in null '\0' character.
*
*/
void Uart1_WriteString(char * txChars)
{
 int i;
 for(i=0;i<strlen(txChars);i++)
 {
  while (!(UCSR1A & _BV(UDRE1)));
  UDR1=txChars[i];
 }
}

/*
*
* Name : Uart1_ReadByte
*
* Returns a single byte of data or a character read from *UART1* . This function waits until it receives a byte
* from *UART1* .
* 
* Output:
*
* 0-255 - A byte or character value. 
*
* E.g. Usage :
* 
* /byte rxByte = Uart1_ReadByte () ;/ - Initalizes rxByte with a byte read from *UART1*   
*
* /char rxChar = Uart1_ReadByte () ;/ - Initalizes rxChar with a character read from *UART1* 
*
*  
*/
byte Uart1_ReadByte( )
{
  while(!(UCSR1A & _BV(RXC1)));
  return _MMIO_BYTE(_SFR_ADDR(UDR1));
}


/*
*
* Name : Uart1_ReadBytes
*
* Returns an array of bytes or characters read from *UART1* . This function waits until all the byte are received
* from *UART1* as specified by the /size/ parameter .
*
* Parameters :
*
* /size/ - Size of the array with maximum value of upto the size of rx1Buffer array . 
*
* Output :
*
* Pointer to the first byte or character read from *UART1* .
*
* E.g. Usage :
* 
* /byte char * rxData = Uart1_ReadBytes (4) ;/ - Initializes a byte array with four bytes read from *UART1* 
*
* /char * rxChars = Uart1_ReadBytes (4) ;    / - Initializes a character array with four characters read from *UART1* 
*
*/
byte * Uart1_ReadBytes(byte size)
{
   byte i;
   for(i=0;i<size && i<RX1BUFFERSIZE;i++)
   {
       while(!(UCSR1A & _BV(RXC1)));
       rx1Buffer[i]=UDR1;
   }
   return rx1Buffer;
}


/*
*
* Name : Uart1_ReadString
*
* Returns an array of bytes or characters read from *UART1* . This function takes a termination character as parameter
* and it waits until that character is encountered .
* 
* Parameters :
*
* /termChar/ - Termination character as when to stop reading . 
*
* Output :
*
* Pointer to the first byte or character read from *UART1* . The termination character is not part of the output.
*
* E.g. Usage :
* 
* /char * rxChars = Uart1_ReadString ('\r') ;    / - Initializes a character array with with a string before the '\r' termination character.
*/
char * Uart1_ReadString(char termChar)
{
    byte data;
    byte rxPos=0;
    while(1)
    {
     while(!(UCSR1A & _BV(RXC1)));
     data=UDR1;
     if(data==termChar || rxPos==RX1BUFFERSIZE)
     {
         rx1Buffer[rxPos]='\0';
         return rx1Buffer;
     }
     else
       rx1Buffer[rxPos++]=data; 
    }
}

/*
*
* Name : Uart1_SetReceiveInterrupt
*
* This function assigns the receive interrupt handler and enables the receive interrupt .The parameter is the name of the function which you want to jump
* to when the receive interrupt occurs .No explicit knowledge of function pointers is necessary .See example for usage .
*
* Parameters :
*
* /fptr/ - Function pointer to a function of the (void)(*)(byte) type .You have to enter the name of a function which has void return type and byte value as parameter .   
*
*
* E.g. Usage :
*  
* An example receive interrupt handler function -
*          
* /void ReceiveInterruptHandler(byte receivedData)/ - Interrupt Handler function definition
* { - Parenthesis Open                                             
* if(receivedData=='$') - Comparison of data which arrived at *UART1*                                     
*  globalCount++; - Incrementing of a global count variable                  
* } - Parenthesis Close                                                 
*
* /Uart1_SetReceiveInterrupt (ReceiveInterruptHandler); / - Sets ReceiveInterruptHandler as the receive interrupt handler fucntion 
*/
void Uart1_SetReceiveInterrupt(void (* fptr)(byte))
{
    UCSR1B|=_BV(RXCIE1);
    rx1Interrupt=fptr;
}

/*
*
* Name : Uart1_ResetReceiveInterrupt
*
* This function disables the receive interrupt .
*
* E.g. Usage :
*  
* /Uart1_ResetReceiveInterrupt (ReceiveInterruptHandler); / - Disables the receive interrupt.
*/
void Uart1_ResetReceiveInterrupt()
{
  UCSR1B&=~_BV(RXCIE1);
  rx1Interrupt=NULL;
}

/*
*
* Name : Uart1_printf
*
* This is a C macro with similar functionality as printf in standard C except the output goes to *UART1* . This macro
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
* /Uart1_printf ("%d %c %s %f \n",213,'d',"hello",3.142); / - Prints the string in exact same way as printf function would but on *UART1* 
*/
//{{The following line is for document generator}}
//void Uart1_printf(char * format,...)

/*
*
* Name : Uart1_scanf
*
* This is a C macro with similar functionality as scanf in standard C except the input comes from *UART1* . This macro
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
* /Uart1_scanf ("%d %c %s %f \n",&intData,&charData,stringData,&floatData); / - Inputs the variables
* 
*/
//{{The following line is for document generator}}
//void Uart1_scanf(char termChar,char * format,...)

/* UART1 Send Interrupt */
SIGNAL(SIG_UART1_RECV)
{
 byte data;
 if(rx1Interrupt!=NULL)
   {
     data=UDR1;
     rx1Interrupt(data);
   }
}

/* UART1 Send Interrupt */

SIGNAL(SIG_UART1_TRANS)
{

}