#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "DataLinkLayer.h"


/**
 * - Interrupt service routine for the timer, so that it will perform the toggling event
 *   ie. toogling PORTB, in every clock cycle.
 */

ISR(TIMER1_COMPA_vect)
{
	PORTB = PORTB ^ (1<<PB1); /// -> Toggling B1 port for every clock cycle 
}

/**
 * - Interrupt service routine for the sending data at every clock cycle. And it will 
 *   send data bitwise from the send buffer, that is done by calling send(). 
 */

ISR(PCINT0_vect)
{
	Send();
}

/**
 * - Interrupt service routine for the recieving data at every clock cycle. And it will 
 *   recieve data bitwise from the recieve buffer, that is done by calling recieve(). 
 */

ISR(PCINT2_vect)
{	
	recieve();								
}
