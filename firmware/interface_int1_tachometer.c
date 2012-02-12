#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usbtenki_cmds.h"
#include "interface.h"

#define LED_ON()	PORTC |= 1
#define LED_OFF() 	PORTC &= 0xFE
#define LED_TOGGLE()	PORTC ^= 1

#define INT1_OFF()	GICR &= ~(1<<INT1)
#define INT1_ON()	GICR |= (1<<INT1)

#define DEBOUNCE	117 // 10ms at /1024 prescaler

// Theses clocks are running at 11.71875 kHz
static volatile unsigned short timing_instant = 0;

ISR(INT1_vect, ISR_NOBLOCK)
{
	INT1_OFF();

	cli();
	LED_TOGGLE();
	sei();

	timing_instant = TCNT1;
	TCNT1=0;
		
	GIFR |= 1<<INT1;
	INT1_ON();
}

int sensors_init(void)
{
	DDRC |= 0x01;
	LED_ON();
	usbtenki_delay_ms(100);	
	LED_OFF();

	// enable pull-up on INT1 pin. This
	// goes to the open collector output of an opto.
	DDRD &= ~(1<<3);
	PORTD |= (1<<3);

	// Configure INT1 for falling edge
	INT1_OFF();
	MCUCR |= (1<<ISC11);
	MCUCR &= ~(1<<ISC10);
	INT1_ON();

	TCCR1A = 0;
	TCCR1B = (1<<CS12) | (1<<CS10); // /1024

	// /1024 : 11.71875 kHz : 83.3333 uS period 
	// /256  : 46.875 kHz

	return 0;
}

int sensors_getNumChannels(void)
{
	return 1;
}

int sensors_getChipID(unsigned char id)
{
	return USBTENKI_CHIP_TACHOMETER;
}

/** 
 * \brief Called to get a raw value from a sensor. 
 * \return Number of bytes returned (max 6)
 **/
int sensors_getRaw(unsigned char id, unsigned char *dst)
{
	memset(dst, 0, 6);
	dst[0] = timing_instant >> 8;
	dst[1] = timing_instant;

	return 2;
}

