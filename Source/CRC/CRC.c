#include <stdio.h>
#include "uart.h"
#include "PhysicalLayer.h"

/**
 * - This function calculates 32 bit CRC, which takes the payload compute and returns 
 *   it. The returned 32 bit CRC is used to check whether the recieved CRC is correct
 *   or not.
 */

uint32_t crc32b(uint8_t *message, int Psize)
{
   uint32_t polynomial = 0x04C11DB7; /// -> Take the polynomial to a 32 bit variable.
   uint32_t crc1=0; /// -> CRC will be null initially. 
   int n = 0;   /// -> Variable to check CRC is calculated for all characters in the message.
   while ((message[n] != 0)|| (Psize !=0))  /// -> When message has a character or Psize is not zero.
   {
        unsigned char b = message[n];  
        crc1 ^= ((uint32_t)b<<24);  /// -> Perform XOR operation of CRC with character in the message converted to 32 bit character.  
        for (int m = 0; m <8; m++)  /// -> Run the loop 8 times.
        {             
            if((crc1 & 0x80000000)!= 0) /// -> If MSB of CRC is 1, right shift crc by 1 and XOR with polynomial.
            {
                crc1 = (uint32_t)((crc1<<1)^polynomial); 
            }
            else /// -> If MSB of CRC is 0, just right shift CRC by 1.
            {
                crc1<<=1;
            }
        }
        n = n + 1; /// -> Increment n by 1 to take the next character of the message.
        Psize--;
  	}
    return crc1; /// -> Return the final computed 32 bit CRC.
}