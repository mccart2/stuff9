/*
 * GccApplication9.c
 *
 * Created: 4/13/2019 5:49:10 PM
 * Author : Student
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define ADC0 0x0 // We will be reading in here
#define AVCC 5000 // Our 5v
#define PRECISION 1024
#define scale (AVCC/PRECISION)// 5000/1024


uint16_t readADC(uint8_t ADCchannel);
int main(void)
{
	uint16_t volt; // declare variable
	DDRB = 0xFF;
	
	//This block sets up the ADC
	ADMUX |= (1<<REFS0); // AVCC with ext capacitor at AREF pin and input ADC0
	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Turns on ADC, and engages prescaler of 128
	
	//This block lets us set the external interrupt
	PCMSK1 = (1<<0)|(1<<1);
	PCICR = (1<<PCIE1);
	
	//This block lets us set the timer
	TCCR1A = (1<<WGM11);// Fast PWM mode
	TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS12) | (1<<CS10);
	OCR1A = 3124;
	OCR1B = 12499;
	sei();
	
	while (1)
	{
		volt = readADC(ADC0); //Accepts voltage from this pin and processes it
		volt = ((scale*volt)/10) + 2; //Converts the voltage
		if (volt <= 20)
		{
			TCCR1A &= ~(1<<COM1A1);
			TCCR1A |= (1<<COM1B1);
		}
		else
		{
			TCCR1A &= ~(1<<COM1B1);
			TCCR1A |= (1<<COM1A1);
		}
	}
}

ISR(PCINT1_vect)
{
	PORTB ^= (1<<2); //Begin
}

uint16_t readADC(uint8_t ADCchannel)
{
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F); //Resets ADMUX
	ADCSRA |= (1 << ADSC); //This is where we do our conversion
	while(ADCSRA & (1<<ADSC));  //Delay until conversion is done, will be zero after operation
	return ADC; //Return the value
}