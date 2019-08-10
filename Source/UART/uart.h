/**
 * - This function call is used to initialize UART configuration. 
 */

void uart_init (void);

/**
 * - This function call is to put data to the minicom console. 
 */

void uart_putChar (char c);

/**
 * - This function call is to get array of all the character. 
 */

void uart_String(char* string);
