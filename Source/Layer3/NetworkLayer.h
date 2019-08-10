/**
 * - This function call is to extract the payload from the recieved buffer, which is 
 *   sent to the data link layer from where it is being called.
 */

uint8_t *payload_data(uint8_t *RBuffer);

/**
 * - This function call is to check whether the data recieved is correct or not. This 
 *   function takes the computed CRC value, recieved buffer and the source address as 
 *   parameters and returns the numerical values (0,1,2) when the soruce and destination 
 *   address are compared to each other. The returned values are sent to the data link
 *   layer. 
 */

crc_check(uint32_t CRCcheck, uint8_t source1, uint8_t *RecBuffer);