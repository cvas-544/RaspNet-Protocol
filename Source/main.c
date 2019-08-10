#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"
#include "InterruptServiceRoutine.h"
#include "DataLinkLayer.h"
#include "PhysicalLayer.h"
#include "PinConfig.h"

uint8_t* sendData;


/**
 * - Here we are Calling the pinConfiguration(), uart_init(), intialize(), initvalue(). 
 *
 * - Pinconfiguration() is used for initializing the pins, uart_init() is used 
 *   for initializing the UART, initialize() is used for initializing the send 
 *   and recieve buffers, and initvalue() is used for calling functions - userInput(), 
 *   userDestinationInput(), frameInitializer().
 * 
 * - UserInput() is used for getting input message form user using Minicom console,
 *   userDestinationInput() is used for getting destination address statically from user,
 *   and framinitializer() is called for initializing the send buffer.
 * 
 */


int main(void)
{
    pinConfiguration(); /// -> Initializing the pins
    uart_init();        /// -> Initializing UART.
    sei();              /// -> Enables interrupts.
		
    initValue();
    initialize();       /// -> Intializing the send and recieve buffers.
        
    while(1)
    {
        /// -> Main loop	
    }
    return 0;
}

/**
 * - This function is used for performing function calls of userInput(),
 *   userDestinationInput(), frameInitializer().
 */

void initValue()
{
     userInput(); 
     int size = strlen(sendData);
     uint8_t destinationAddress = userDestinationInput();
     frameInitializer(destinationAddress,sendData,size);
    
}

/**
 * - This function is used for getting input message from user using Minicom console.
 */

void userInput()
{
     uart_putString("Please enter the message: ");
     int i= 0;
     while(1)
     {	
        char in = uart_getchar();
        if(in == 0x0D || i >= 256)
        break;
        uart_putchar(in);	
        *(sendData+i) = in;
        i++;
    }
    uart_putchar('\n');
}

/**
 * - This function is used for getting destination address from user using Minicom console.
 */

void userDestinationInput()
{
    uart_putString("Please enter the DestinationID: ");
    destinationAddress = uart_getchar();
    uart_putchar(destinationAddress );	
    
}
