/* RGBLED Driver C file */
/* Daniel Fread, Binghamton University */

#include "rgbled.h"
#include <MKL25Z4.h>

#define RED_LED_LOC 18
#define GREEN_LED_LOC 19
#define BLUE_LED_LOC 1

void initialize_rgbled(){ //Initialize the three cathodes and make initial color black
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED_LOC] =
		PORT_PCR_MUX(1) |
		PORT_PCR_IRQC(0) |
		PORT_PCR_ISF(1);
	PTB->PDDR |= (1<<RED_LED_LOC);
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[GREEN_LED_LOC] =
		PORT_PCR_MUX(1) |
		PORT_PCR_IRQC(0) |
		PORT_PCR_ISF(1);
	PTB->PDDR |= (1<<GREEN_LED_LOC);
	
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[BLUE_LED_LOC] = 
		PORT_PCR_MUX(1) |
		PORT_PCR_IRQC(0) |
		PORT_PCR_ISF(1);
	PTD->PDDR |= (1<<BLUE_LED_LOC);
}
void turn_on_red_led(){
	PTB->PCOR |= (1<<RED_LED_LOC);
}	
void turn_on_green_led(){
	PTB->PCOR |= (1<<GREEN_LED_LOC);
}
void turn_on_blue_led(){
	PTD->PCOR |= (1<<BLUE_LED_LOC);
}	
void turn_off_red_led(){
	PTB->PSOR |= (1<<RED_LED_LOC);
}	
void turn_off_green_led(){
	PTB->PSOR |= (1<<GREEN_LED_LOC);
}
void turn_off_blue_led(){
	PTD->PSOR |= (1<<BLUE_LED_LOC);
}	
void set_rgbled_color_to(color_t color){ //Use the functions above to set the color to one of the color values (below)
	switch(color){
		case RGB_COLOR_BLACK :
			turn_off_red_led();
			turn_off_green_led();
			turn_off_blue_led();
			break;
		case RGB_COLOR_BLUE :
			turn_off_red_led();
			turn_off_green_led();
			turn_on_blue_led();
			break;
		case RGB_COLOR_GREEN :
			turn_off_red_led();
			turn_on_green_led();
			turn_off_blue_led();
			break;
		case RGB_COLOR_CYAN :
			turn_off_red_led();
			turn_on_green_led();
			turn_on_blue_led();
			break;
		case RGB_COLOR_RED :
			turn_on_red_led();
			turn_off_green_led();
			turn_off_blue_led();
			break;
		case RGB_COLOR_MAGENTA :
			turn_on_red_led();
			turn_off_green_led();
			turn_on_blue_led();
			break;
		case RGB_COLOR_YELLOW :
			turn_on_red_led();
			turn_on_green_led();
			turn_off_blue_led();
			break;
		case RGB_COLOR_WHITE :
			turn_on_red_led();
			turn_on_green_led();
			turn_on_blue_led();
			break;
	}
}
void turn_off_rgbled(){ //Turn off all LEDs (equivalent to set color to black)
	turn_off_red_led();
	turn_off_green_led();
	turn_off_blue_led();
}
void toggle_red_led(){
	PTB->PTOR |= (1<<RED_LED_LOC);
}	
void toggle_green_led(){
	PTB->PTOR |= (1<<GREEN_LED_LOC);
}
void toggle_blue_led(){
	PTD->PTOR |= (1<<BLUE_LED_LOC);
}