/****************************************************
* Module: LCD
* 
* This module contains various functions for the character
* LCD display on the MegaBoard .You can display upto 32 characters 
* at once .You can display debug messages,variables etc using on the LCD .
* For most purposes Lcd_printf () functions is more than enough unless you 
* want to do low level exploration of the *LCD* functions .
*
****************************************************/

/* Lcd Buffer */

byte lcdBuffer[LCDBUFFERSIZE];

/* Function Definitions */
/*
*
* Name : Lcd_Init
*
* Initialize the *LCD* in 16 characters and 2 lines mode with cursor type as selected in the /MegaIDE/ .This function does not return a value . 
* 
* E.g. Usage :
*
* /Lcd_Init ();/ - Initializes the *LCD*
*/
void Lcd_Init()
{
 volatile int i;
 DDRC=0xFF;
 DDRG=0x1F;
 PORTC=0;
 Lcd_WriteCommand(0x38);
 for(i=8000;i>0;i--);
 Lcd_WriteCommand(0x01);
 for(i=8000;i>0;i--);
 Lcd_WriteCommand(LCDCURSOR);
 #ifdef LCD_WELCOME_MESSAGE
  Lcd_printf(LCD_WELCOME_MESSAGE);
#endif
}

/*
*
* Name : Lcd_WriteCommand
*
* Gives a command to the *LCD* .This function does not return a value . 
* 
* Parameters : 
*
* /command/ - 0-255 byte value. 
*
* Here are some commands : 
*
* 28(0x1E) - Scroll display one character right (all lines)        
* 24(0x18) - Scroll display one character left (all lines)          
* 02(0x02) - Home (move cursor to top/left character position)
* 16(0x10) - Move cursor one character left 
* 20(0x14) - Move cursor one character right 
* 14(0x0E) - Turn on visible underline cursor 
* 15(0x0F) - Turn on visible blinking-block cursor 
* 12(0x0C) - Make cursor invisible
* 08(0x08) - Blank the display (without clearing) 
* 12(0x0C) - Restore the display (with cursor hidden) 
*
* E.g. Usage :
*
* /Lcd_WriteCommand (0x1E);/ - Scrolls the display on *LCD* one character right
*/
void Lcd_WriteCommand(byte command)
{
 volatile int i;
 PORTC=command;
 PORTG&=~_BV(0);
 PORTG|=_BV(1);
 PORTG&=~_BV(1);
 for(i=35;i>0;i--);
}

/*
*
* Name : Lcd_putchar
*
* Displays a character on the *LCD* .This function does not return a value . 
* 
* Parameters : 
*
* /lcdChar/ - character variable to be displayes
*
* E.g. Usage :
*
* /Lcd_puthchar ('C');/ - Displays character 'C' on the *LCD*
*/
void Lcd_putchar(char lcdChar)
{
 volatile int i;
 if(lcdChar>19 && lcdChar<127)
 {
  PORTC=lcdChar;
  PORTG|=_BV(0);
  PORTG|=_BV(1);
  PORTG&=~_BV(1);  
  for(i=35;i>0;i--); 
 }
}

/*
*
* Name : Lcd_GotoXY
*
* Moves the cursor to a specified position .This function does not return a value . 
* 
* Parameters : 
*
* /row/ - 1 or 2 (row number)
* 
* /column/ - 1-16 (column number)
*
* E.g. Usage :
*
* /Lcd_GotoXY (1,1);/ - Moves the cursor to the first row and the first column 
*
*/
void Lcd_GotoXY(byte row,byte column)
{
 if(row==1 && column<16)
  Lcd_WriteCommand(0x7F+column);
 else if(row==2 && column<16)
  Lcd_WriteCommand(0xBF+column);
}

/*
*
* Name : Lcd_ClearDisplay
*
* Clears the *LCD* screen and moves the cursor to the starting position .This function does not return a value . 
* 
* E.g. Usage :
*
* /Lcd_ClearDisplay ();/ - Clears the *LCD* and moves the cursor to beginning position
*/
void Lcd_ClearDisplay()
{
  volatile int i;
  Lcd_WriteCommand(0x0C);
  Lcd_WriteCommand(0x80);
  for(i=16;i>0;i--)
      Lcd_putchar(' ');
  Lcd_WriteCommand(0xC0);
  for(i=16;i>0;i--)
      Lcd_putchar(' ');
  Lcd_WriteCommand(0x80); 
  Lcd_WriteCommand(LCDCURSOR);
}

/*
*
* Name : Lcd_PrintString
*
* Displays a string on the *LCD* starting from the beginning position on the screen .If the number of characters is more than 16 then the cursor is automatically moved to 
* the second row and if the string length exceeds 32 characters then the screen is automatically cleared and cursor moved 
* to the beginning position .This function does not return a value . 
*
* Parameters : 
* 
* /lcdString/ - Pointer to the string to be displayed
*
* E.g. Usage :
*
* /Lcd_PrintString ("Hello! World");/ - Displays "Hello! World on the *LCD*
*/
void Lcd_PrintString(char * lcdString)
{
 int i;
 Lcd_ClearDisplay(); 
 for(i=0;i<strlen(lcdString);i++)
  {
     if(i==16)
      Lcd_WriteCommand(0xC0); 
     else if(i==32)
      Lcd_ClearDisplay();
     Lcd_putchar(lcdString[i]);
  }
}
/*
*
* Name : Lcd_printf
*
* This is a macro similar in functionality  to printf in standard C except that escape sequences such as '\n' etc have no 
* effect on the display .The output is displayed starting from the beginning position on the screen .If the number of characters 
* in the output is more than 16 then the cursor is automatically moved to the second row and if the string length exceeds 
* 32 characters then the screen is automatically cleared and cursor moved to the beginning position .This function does not 
* return a value . 
*
* Parameters :
*
* /format/ - The format string for printf .  
*
* /.../ - This parameter means it can take a list of varibles with the size of the list not being fixed .
*
* E.g. Usage :
* 
* /Lcd_printf ("%d %c %s",213,'d',"hello"); / - Prints the string on the *LCD* similar to printf  
*/
//void Lcd_printf(char * format,...)