#include <stdbool.h>
#include <stdint.h>
#include "uart0_irq.h"
#include <MKL25Z4.h>

#ifndef UART0_IRQ_PRIORITY
#define UART0_IRQ_PRIORITY 2
#endif

#ifndef TXBUF_SIZE
#define TXBUF_SIZE 128 //should be power of 2
#endif

static uint8_t tx_buffer[TXBUF_SIZE];
static uint32_t txhead, txtail;
static uint32_t txhead_next() { return (txhead+1)%TXBUF_SIZE; }
static uint32_t txtail_next() { return (txtail+1)%TXBUF_SIZE; }
static _Bool txbuf_is_empty() { return txhead == txtail; }
static _Bool txbuf_is_full() { return txhead_next() == txtail; }
//TDRE flag in status register (while)
void configure_uart0() {

	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SOPT2 |= ( SIM_SOPT2_UART0SRC(1) | SIM_SOPT2_PLLFLLSEL(1) );
	PORTA->PCR[1] = (PORTA->PCR[1] &~ PORT_PCR_MUX_MASK) |
			PORT_PCR_MUX(2);
	PORTA->PCR[2] = (PORTA->PCR[2] &~ PORT_PCR_MUX_MASK) |
			PORT_PCR_MUX(2);
	UART0->BDH = 0;
	UART0->BDL = 26; //115.2kBd
	UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
	UART0->C1 =  0;
	NVIC_SetPriority( UART0_IRQn, UART0_IRQ_PRIORITY );
	NVIC_ClearPendingIRQ( UART0_IRQn );
	NVIC_EnableIRQ( UART0_IRQn );
}

_Bool receive_byte_from_uart0( uint8_t *data ) {
	if( UART0->S1 & UART0_S1_RDRF_MASK ) { //UART0_SI_TDRE_MASK
		*data = UART0->D; //UART0->D = data*;
		return true;
	}
	UART0->S1 |= 0x1F;
	return false;
}

_Bool send_byte_to_uart0( uint8_t data ) { //If while(!TDRE) do nothing
	if( !txbuf_is_full() ) {
		tx_buffer[txhead] = data;
		txhead = txhead_next();
		UART0->C2 |= UART0_C2_TIE_MASK;
		return true;
	}

	return false;
}

void UART0_IRQHandler() {
	asm("CPSID I");
	if( !txbuf_is_empty() ) {
		UART0->D = tx_buffer[txtail];
		txtail = txtail_next();
	}else{
		UART0->C2 &= ~UART0_C2_TIE_MASK;
	}
	asm("CPSIE I");
	return;
}

_Bool print_str( char *p ) {

	_Bool retval = true;
	while( *p ) {
		retval = send_byte_to_uart0(*p++);
	}

	return retval;
}

void print_hex(uint8_t x){
     x = (x & 0x0f);
     if(x < 10)
          send_byte_to_uart0('0' + x);
     else
          send_byte_to_uart0('A' + x - 10);
}
void print_hex8(uint8_t x){
     print_hex(x>>4);
     print_hex(x);
}
void print_hex16(uint16_t x){
     print_hex8(x>>8);
     print_hex8(x);
}
void print_hex32(uint32_t x){
	print_hex16(x>>16);
	print_hex16(x);
}