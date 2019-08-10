#include <stdio.h>
#include "uart.h"

/**
 * - This function is used to extract the payload from the recieved buffer, which is 
 *   sent to the data link layer from where it is being called.
 */

uint8_t *payload_data(uint8_t *RBuffer)
{
    int x = 6; 
	int y = 0;
	uint8_t *Buffer = (uint8_t*)malloc(*(RBuffer+5)*sizeof(uint8_t));
	for(y=0;y<=(*(RBuffer+5));y++)
	*(Buffer+y) = 0;
        for(y=0;y<(*(RBuffer+5));y++)
		{ 
			*(Buffer+y) = *(RBuffer+x);  /// -> Copying the payload to the temporary buffer called Buffer.
			x++;
		}
	    return Buffer;
}

/**
 * - This function is used to check whether the data recieved is correct or not. This 
 *   function takes the computed CRC value, recieved buffer and the source address as 
 *   parameters and returns the numerical values (0,1,2) when the soruce and destination 
 *   address are compared to each other. The returned values are sent to the data link
 *   layer. 
 */

int crc_check(uint32_t CRCcheck, uint8_t source1, uint8_t *RecBuffer)
{
    if(*(RecBuffer+6) == source1) 
    {
        uint32_t crc = 0x00000000;
        for(int s = 0;s<=3;s++)      /// -> Copying the crc which is recieved to the tempCRC
        {
            uint32_t tempCRC = 0x00000000;
            tempCRC =  *(RecBuffer+(s+1));
            crc = crc | tempCRC<<(24-(s*8));
        }
        if(CRCcheck == crc)
        {
            uart_String("CRC ok");
            uart_putChar('\n');
            uart_putChar('\r');
            return 1;
        }
        else 
        {			
            uart_String("CRC Error");
            uart_putChar('\n');
            return 0;
        }		
    }
	else if(*(RecBuffer+6) == 0) 
    {
        if(*(RecBuffer+7) == source1)   /// -> If the recieved destination address matches my address then it means broadcast is completed. 
        {
            uart_String("Broadcast Completed");
            uart_putChar('\n');
            uart_putChar('\r');
            return 0;
        }
        else     /// -> If the recieved destination address is 0 then the message has to be forwarded.
        {
            uart_String("Broadcasting your message..");
            uart_putChar('\n');
            uart_putChar('\r');
            return 2;
        }
			 
    }   
    else if(*(RecBuffer+6) != source1)  /// -> If the recieved destination address is not matching with the source address then the message has to be forwarded.
    {
        uart_String("Forwading....");
        uart_putChar('\n');
        uart_putChar('\r');	
        return 2;	
    }
}
