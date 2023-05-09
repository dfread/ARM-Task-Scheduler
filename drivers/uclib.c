#include <stdint.h>
#include "uclib.h"
//#include "uart0_irq.h"

static uint32_t random_number_state;

uint16_t rand()
{
	random_number_state= (1103515245*random_number_state+12345)&0x7fffffff;
	return random_number_state>>16;
}
void srand(uint32_t x)
{
	random_number_state=x;
}
uint16_t to_hex_ascii8( uint8_t data)
{
	uint16_t retval;
	retval=to_hex_ascii(data);
	retval= retval | (to_hex_ascii(data>>4) << 8); 
	return retval;
}
char to_hex_ascii( char data )
{
	return ((data&0x0f)) < 10 ? ((data&0x0f)+'0') : ((data&0x0f)+'a'-10);
}
//void print_hex8( uint8_t data)
//{
	//send_data_to_uart0( to_hex_ascii8(data) >>8);
	//send_data_to_uart0( to_hex_ascii8(data));
//}

void *memcpy( void *dest, void *src, unsigned n)
{
	uint8_t *d=dest, *s=src;
	while( n--)
		*d++=*s++;
	return dest;
}
