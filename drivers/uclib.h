#ifndef MYSTDLIB
#define MYSTDLIB
#include <stdint.h>
#include <stddef.h>

uint16_t rand();
void srand(uint32_t);

uint16_t to_hex_ascii8( uint8_t data);
char to_hex_ascii( char data );
void print_hex8( uint8_t data);
void *memcpy( void *dest, void*src, unsigned n);
#endif
