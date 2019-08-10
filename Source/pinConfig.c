#ifndef F_CPU
#define F_CPU 12000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#define delay 0.05

/**
 * - Initializing the pins of port B and port D in which the pins (B1 & D3) is used for 
 *   clock and (B2 & D2) is used for sending and recieveing Data.
 */

void pinConfiguration()
{
    OCR1A = (delay*(F_CPU/1024))-1;;   /// -> Calculating the time frequency using delay(0.05ms)
    
    DDRB |= _BV(PB1) | _BV(PB2);  /// -> Use pin PB1 and PB2 as O/P.
    DDRC |= _BV(PD2)| _BV(PD3);   /// -> Use pin PD2 and PD3 as I/P.  
		
    PCMSK0 |= (1<<PCINT1);
    PCICR |= (1<<PCIE0);

    PCMSK2 |= (1<<PCINT19);
    PCICR |= (1<<PCIE2);
    
   [timers]: https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328  "Timers on the ATmega168/328"

    TCCR1B |= (1<<WGM12);             /// -> Mode 4, CTC on OCR1A
    TIMSK1 |= (1<<OCIE1A);            /// -> Set interrupt on compare match
    TCCR1B |= (1<<CS12)|(1<<CS10);    /// -> set prescaler to 1024 and start the timer   
}