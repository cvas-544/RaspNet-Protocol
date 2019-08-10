#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include "crc.h"

uint8_t source = 0x01;
uint8_t *SendBuffer;
volatile int sendFrameInitalized =0;
int sendFrameSize;

/**
 * - This function is used for initialzing the send frame, with PREAMBLE as 1st byte,
 *   next 4 bytes as CRC, 5th byte as size of data, 6th byte as destination address, 7th
 *   byte as source address, and the next number of bytes are for data that depends on 
 *   the user. The function frameInitializer() is called with destination address, the 
 *   payload and the size of the data as parameters from main.c. 
 */

void *frameInitializer(uint8_t destination,uint8_t *sendData, int size)
{
    uint32_t crc;
    uint8_t preamble = 0b01111110;
    sendFrameSize = size;
    *SendBuffer = (uint8_t*)malloc((8+size)*sizeof(uint8_t));   /// -> Dynamically alocating the frame size for send buffer. 
    uint8_t *Payload = (uint8_t*)malloc(size*sizeof(uint8_t));  /// -> Dynamically alocating frame for the user entered data. 
    
    *Payload = destination;  
    *(Payload+1) = source;
    
    for(int j=0; j<size; j++)
    {
        *(Payload+2+j) = *(sendData+j);    
    }

    crc = crc32b(Payload,size);  /// -> Calling the crc32b() with the playload and the size as parameters to get the CRC value for the Payload. 
    
/**
 * - Creating the send frame byte by byte. 
 */

    *SendBuffer = preamble;             
    *(SendBuffer+1) = (crc>>24);        
    *(SendBuffer+2) = (crc>>16);        
    *(SendBuffer+3) = (crc>>8);         
    *(SendBuffer+4) = (crc);        
    *(SendBuffer+5) = size;             
    *(SendBuffer+6) = *Payload;         
    *(SendBuffer+7) = *(Payload+1);     
     for(int g=0; g<size; g++)
    {
        *(SendBuffer+(8+g)) = *(Payload+(2+g));  
    }
    
    sendFrameInitialized = 1; /// -> The sendFrameInitialized is initialized to 1 if the frame is ready.
    
}

/**
 * - This function is used to copy the sendBuffer frame to tempBuffer frame and we are
 *   returing the tempBuffer.
 */

uint8_t isFrameValid()
{
        if(sendFrameInitialized == 1)
        {
            uint8_t *tempBuffer = (uint8_t*)malloc((8+sendFrameSize)*sizeof(uint8_t));
            int g;
            for(g=0; g<8+sendFrameSize; g++)
            {
                *(tempBuffer+g) = *(SendBuffer+g);
            }
            return tempBuffer;
        }
}
