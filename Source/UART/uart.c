#ifndef F_CPU
#define F_CPU 12000000UL                     
#endif
#include <stdio.h>
#include <avr/io.h>
#include <util/setbaud.h>
#define BAUD 9600                            /// baud is defined here
#define BAUDRATE ((F_CPU/16/BAUD)-1)	     /// set baudrate value for UBRR
  

/**
 * - This function is used for UART initialization. 
 */

[Configuring UART]: https://appelsiini.net/2011/simple-usart-with-avr-libc "Configuration of UART"

void uart_init (void)
{
	UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
	UCSR0C = _BV(UCSZ01)|_BV(UCSZ00);  /// -> 8-bit data
	UCSR0B = _BV(RXEN0)|_BV(TXEN0);    /// -> Enable RX and TX
}

/**
 * - This function is used for transmitting data to the Minicom console. 
 */

void uart_putChar (char c)
{
    loop_until_bit_is_set(UCSR0A,UDRE0);    /// -> Wait until data register empty
    UDR0 = c ;
}

/**
 * - This function is used for recieveing data to the Minicom console. 
 */

char uart_getchar(void)
{
	
	loop_until_bit_is_set(UCSR0A, RXC0);   /// -> Wait until unread data exists
	return UDR0;
}

/**
 * - This function is used to get the character from User and to place it in a char array 
 *   named as string.
 */

void uart_String(char* string)
{
	int index;
	for(index = 0;index < strlen(string); index++)
	{
		uart_putChar(string[index]);
	}
}
