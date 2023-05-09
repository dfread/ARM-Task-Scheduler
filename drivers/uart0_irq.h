#ifndef UART0_IRQ_H
#define UART0_IRQ_H
#include <stdbool.h>
#include <stdint.h>
void configure_uart0();
_Bool receive_byte_from_uart0( uint8_t *data );
_Bool send_byte_to_uart0( uint8_t data );
_Bool print_str( char * );
void print_hex32(uint32_t x);
#endif