/**
 * - This function call is used to calculate 32 bit CRC, which takes the payload compute 
 *   and returns it. The returned 32 bit CRC is used to check whether the recieved CRC 
 *   is correct or not.
 */

 uint32_t crc32b(uint8_t *message, int pSize);