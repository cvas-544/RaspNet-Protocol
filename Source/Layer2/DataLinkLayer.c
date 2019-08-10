#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"
#include "PhysicalLayer.h"
#include "NetworkLayer.h"
#include "crc.h"

uint8_t *sendBuffer; 
uint8_t *RecieveBuffer;
uint8_t *tempBuffer;
int sendBitCounter = 0;
int recieveBitCounter = 0;
int bytesToSend = 0;
int bytesToRecieve = 6;
int sendByteCounter = 0;
int recieveByteCounter = 0;
uint8_t destination = 0; 
uint8_t source = 2;
int SendLock = 0;
uint8_t prcheck;
int preeableMatch = 0;
uint8_t tempVar = 0;
int forwardFlag = 0;

/**
 * - This function is used to call the isframevalid() to get the send frame, 
 *   and we are also initializng the recieve buffer.
 */

void initialize()
{
	sendBuffer = isFrameValid();  /// -> Calling the isFrameValid() where we are initializing the send frame. 
	RecieveBuffer = (uint8_t*)malloc(bytesToRecieve*sizeof(uint8_t));
	bytesToSend = 8 + *(sendBuffer+5);
	SendLock = 1; 
}

/**
 * - This function is used to send the data bit by bit to the given destination address.
 * - The sendByteCounter is compared with the bytesToSend if it doesn't match than the 
 *   sendByteCounter is incremented, if it is matched then the sendlock is initialized 
 *   to 0 which means we are stopping the transmission. 
 * - This function also checks the forwardflag if it is 1, which means there a message
 *   which we have to forward so we need to give priority to the forward message and then 
 *   the data which is in tempBuffer is copied to the sendBuffer and it is forwarded. 
 */

void Send()
{
	if(SendLock==1) 
	{
		if(sendByteCounter<bytesToSend) 
		{
            if(sendBitCounter<=8) 
			{	
                if((*(sendBuffer+sendByteCounter) & 0x80) != 0 )
				{
					PORTB |= (1<<PB2);
					*(sendBuffer+sendByteCounter) = *(sendBuffer+sendByteCounter) <<1 ;
				}
				else
				{
					PORTB &= ~(1<<PB2);
					*(sendBuffer+sendByteCounter) = *(sendBuffer+sendByteCounter) <<1 ;	
				}
				sendBitCounter++;
			}
			if(sendBitCounter>=8)
			{			
				sendBitCounter=0;
				sendByteCounter++;
			}
			if(sendByteCounter==bytesToSend)
			{
                SendLock = 0;
				if(forwardFlag == 1)  /// -> Checking if there is any data to forward or not. 
				{
					sendBuffer = (uint8_t*)malloc((6+(*(tempBuffer+5)))*sizeof(uint8_t));
					for(int i=0; i<(6+(*(tempBuffer+5))); i++)
					{
						*(sendBuffer+i) = *(tempBuffer+i);  /// -> Copying the tempbuffer to the sendBuffer.
					}
					free(tempBuffer);
					forwardFlag = 0;
					sendByteCounter=0;
					sendBitCounter = 0;
					SendLock = 1;	
				}
			}
		}
	}	
}

/**
 * - This function first keeps on checking the preamble unitl it matches, if the preamble
 *   matches then we write it to the recieve buffer and then we are increasing the byte
 *   counter. 
 * - If the recieveByteCounter is greater than the bytestorecieve then it indicates that 
 *   the we recieved all the bytes of data. Now after recieving all the bytes of data we
 *   are performing the data validation by calling the crc32b() by passing the recieved 
 *   payload and the size of the data. 
 * - This function compares the validated CRC value with the recieved CRC this is performed
 *   by calling the crc_check() which is in the Network Layer by passing the crcCheck,
 *   source and the recieveBuffer. 
 * - If the returned value of the crc_check() is 1 then the 
 *   crcOkRecieve() function is called by passing the rcvPayloadData and the sizeRecieved
 *   as the parameters.
 * - If the returned value of the crc_check() is 2 then the message is broadcasted by 
 *   calling the forward(). 
 * - If the result is something other than 1 or 2 then this indicates that there is some 
 *   error in the recieved data. And it can be a broadcasting message, if the source address
 *   is 0.
 */

void recieve()
{
    if(recieveByteCounter<bytesToRecieve)   ///  -> Condition to check if all bytes of data are recieved or not.
	{	
		if(recieveBitCounter<8)  ///  -> Condition to check whether all the bits of particular byte are recieved or not.
		{
            preambleCheck();
		}
		if(recieveBitCounter>=8)
		{
			*(RecieveBuffer+recieveByteCounter) = tempVar;
			tempVar = 0;	
			uart_putChar('\r');
			uart_putChar('\n');
			if(recieveByteCounter==5) /// -> Once after recieving the size of the data field, here we are reallocating the recieve buffer frame
			{
				bytesToRecieve = bytesToRecieve + *(RecieveBuffer+5);
				RecieveBuffer = (uint8_t*)realloc(RecieveBuffer, bytesToRecieve*sizeof(uint8_t));
			}
			recieveByteCounter++;
			recieveBitCounter=0;
			if(recieveByteCounter==bytesToRecieve)   ///  -> When the condition satisfied, CRC validation function is called.  
			{
                uint8_t *rcvPayloadData;
				rcvPayloadData = payload_data(RecieveBuffer);
				uint8_t sizeRecieved = *(RecieveBuffer+5);
				uint32_t crcCheck;
                
				crcCheck = crc32b(rcvPayloadData,sizeRecieved); /// -> My CRC check value.
				int crcResult = crc_check(crcCheck,source,RecieveBuffer);
				if(crcResult==1)
				{
                    crcOkRecieve(rcvPayloadData,sizeRecieved); /// -> Calling crcOkRecieve() when the validated CRC matched with the recieved CRC.
				}
                else if (crcResult==2)
				{
				    forward();  /// -> This function is called when there is a message to broadcast or when the destination address is not matched with source.
				}
				else
				{
                    RecieveBuffer = (uint8_t*)malloc(bytesToRecieve*sizeof(uint8_t));
				    bytesToRecieve = 6;
				    recieveBitCounter = 0;
				    recieveByteCounter=0;
				    preeableMatch=0;
				}
			}				
		}
	}
}


void preambleCheck()
{
    if(preeableMatch == 0) 
			{
				if(PIND & (1<<PD2))
				{	
					tempVar = tempVar | 0x01;			
				}
				else
				{	
					tempVar = tempVar & (~0x01);	
				}
				if(tempVar == 0b01111110)   ///  -> If the preamble matches then write it to the recvieve buffer and then increase the byte counter.
				{
					preeableMatch = 1;
					recieveBitCounter = 9;
					uart_putChar(tempVar);
				}
				if(preeableMatch == 0)
				tempVar = tempVar << 1;
			}
			else if(preeableMatch == 1)  ///  -> Keep on checking the preamble until it matches.
			{
				if(PIND & (1<<PD2))
				{	
					tempVar = tempVar | 0x01;
					uart_putChar('1');
				}
				else
				{	
					tempVar = tempVar & (~0x01);
					uart_putChar('0');
				}
				if(recieveBitCounter<7)
				{
					tempVar = tempVar << 1;
				}
				recieveBitCounter++;
			}
}

/**
 * - This function is called when the returned value of crc_check() is 1, which means 
 *   the validated CRC is matched with the recieved CRC.
 *   This function prints the entire recieved message on the minicom console.
 */

void crcOkRecieve(uint8_t *p, uint8_t sizeRecieved)
{
    RecieveBuffer = (uint8_t*)realloc(RecieveBuffer, sizeRecieved*sizeof(uint8_t));
    while (*(p+recieveBitCounter)!=0)
    {
        *(RecieveBuffer+recieveBitCounter) = *(p+recieveBitCounter+2);
        uart_putChar(' ');
        recieveBitCounter++;
    }
    uart_putChar('\r');
    uart_putChar('\n');
    free(p);
	free(RecieveBuffer);
	bytesToRecieve = 6;
	RecieveBuffer = (uint8_t*)malloc(bytesToRecieve*sizeof(uint8_t));
	recieveBitCounter = 0;
    recieveByteCounter=0;
    preeableMatch=0;
}


/**
 * - This function is called when the returned value of crc_check() is 2, which means 
 *   that there is a message to forward.
 */

void forward()
{
    bytesToSend = 6 + *(RecieveBuffer+5);
	bytesToRecieve = 6;
    if(SendLock == 0)  /// -> If sendLock is 0 then it means the send buffer is free, so copy the data from recieve buffer to send buffer .
    {
        sendBuffer = (uint8_t*)malloc(bytesToSend*sizeof(uint8_t));
        int g;
        for(g=0; g<bytesToSend; g++)
        {
            *(sendBuffer+g) = *(RecieveBuffer+g);
        }
        free(RecieveBuffer);
        RecieveBuffer = (uint8_t*)malloc(bytesToRecieve*sizeof(uint8_t));
        recieveBitCounter = 0;
        sendBitCounter = 0;
        recieveByteCounter=0;
        sendByteCounter=0;
        preeableMatch=0;
        SendLock = 1;
    }
    else  /// -> If sendLock is 1 then that means the send buffer is not free, so copy the data to the tempBuffer and when the sendlock is free copy the data to           sendBuffer from tempbuffer. 
    {
        tempBuffer = (uint8_t*)malloc(bytesToSend*sizeof(uint8_t));  ///  -> Allocating memory for the tempBuffer. 
        int g;
        for(g=0; g<bytesToSend; g++)
        {
            *(tempBuffer+g) = *(RecieveBuffer+g);
        }
        free(RecieveBuffer);
        RecieveBuffer = (uint8_t*)malloc(bytesToRecieve*sizeof(uint8_t));
        recieveBitCounter = 0;
        preeableMatch=0;
        forwardFlag = 1;	
    }
}




