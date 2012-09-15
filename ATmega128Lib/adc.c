/****************************************************
* Module: Analog to Digital Converter
* 
* This module contains various functions for reading analog analog voltage
* at the *ADC* input port .You can take input readings one at a time or read all the inputs 
* at once or run the *ADC* in free running mode in which *ADC* runs in interrupt mode 
* and updates the adcInputs data variables automatically to the latest values.All the readings
* are 10 bit reading meaning the value returned will be between 0 & 1024.
****************************************************/

/* Buffer variable to store the conversion results */
unsigned short int adcInputs[8];

/*
*
* Name : Adc_Init
*
* Initalizes the *ADC* .This function does not return a value . 
*
* E.g. Usage :
*
* /Adc_Init ();/ - 
*/
void Adc_Init()
{ 
 MCUCSR=_BV(JTD);
 MCUCSR=_BV(JTD);
 ADCSRA=_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1);
 ADMUX=0;
}

/*
*
* Name : Adc_ReadInput
*
* Takes a readinf from the specified channel .This function does not return a value . 
* 
* Parameters : 
*
* /channelNumber/ - character variable to be displayes
*
* E.g. Usage :
*
* /Adc_ReadInput (7);/ - 
*/
int Adc_ReadInput(byte channelNumber)
{
 int adcReading;
 ADCSRA=_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1);
 if(channelNumber>7)
     return -1;
 ADMUX=channelNumber;
 ADCSRA|=_BV(ADSC);
 while(ADCSRA & _BV(ADSC));
 adcReading = (int)(ADCL | (ADCH << 8));
 return adcReading;
}

/*
*
* Name : Adc_ReadAllInputs
*
* Takes reading from all the 8 channels of the *ADC* .The readings are stored in /adcInputs/ array .The /adcInputs/ array is a short unsigned int type with values varying from 0-1024 .This function does not return a value . 
*
* E.g. Usage :
*
* /Adc_ReadAllInputs ();/ - takes readings from all 8 channels and stores them in adcInputs array
*
* /unsigned short int reading=adcInputs[0];/ - initializes a variable with value from the *ADC* reading of the channel 0 
*/
void Adc_ReadAllInputs()
{
 byte muxValue=0;
 ADCSRA=_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1);
 for(muxValue=0;muxValue<8;muxValue++)
 {
  ADMUX=(muxValue&0x07);
  ADCSRA|=_BV(ADSC);
  while(ADCSRA & _BV(ADSC));
  adcInputs[muxValue]= (unsigned short int)(ADCL | (ADCH << 8));
 }
}

/*
*
* Name : Adc_TakeContinousReadings
*
* This functions starts the *ADC* running in interrupt mode .You need to call this function only once unlike the /Adc_ReadAllInputs/
* function .The /adcInputs/ array is automatically updated when the adc interrupts occur .This mode is useful when you 
* want the *ADC* readings and dont want to wait for the conversions to be complete .Reading are taken from all the 8 channels 
* of the *ADC* .The adcInputs array is a short unsigned int type with values varying from 0-1024 .This function does not
* return a value . 
*
* E.g. Usage :
*
* /Adc_TakeContinousReadings ();/ - starts the *ADC* in interrupt mode 
*
* /Uart1_printf ("%d \n",(int)adcInputs[0]);/ - prints the *ADC* channel 0 reading on *UART1*
*/
void Adc_TakeContinousReadings()
{
 ADMUX=0;
 ADCSRA|=_BV(ADIE);
 ADCSRA|=_BV(ADSC);
}

SIGNAL(SIG_ADC)
{
 byte muxValue=ADMUX;
 adcInputs[muxValue]=(unsigned short int)(ADCL | (ADCH << 8));
 ADMUX=((++muxValue)&0x07);
 if(muxValue==7)
     muxValue=0;
 ADCSRA|=_BV(ADSC);
}
