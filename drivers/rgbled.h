/* RGBLED Driver Header file */
/* Daniel Fread, Binghamton University */

#ifndef RGBLED_H
#define RGBLED_H

typedef enum {RGB_COLOR_BLACK, RGB_COLOR_BLUE, RGB_COLOR_GREEN, RGB_COLOR_CYAN, RGB_COLOR_RED, RGB_COLOR_MAGENTA, RGB_COLOR_YELLOW, RGB_COLOR_WHITE} color_t;

void initialize_rgbled(); //Initialize the three cathodes and make initial color black
void turn_on_red_led(); 
void turn_on_green_led();
void turn_on_blue_led(); 
void turn_off_red_led(); 
void turn_off_green_led();
void turn_off_blue_led(); 
void set_rgbled_color_to(color_t color); //Use the functions above to set the color to one of the color values (below)
void turn_off_rgbled(); //Turn off all LEDs (equivalent to set color to black)
void toggle_red_led();
void toggle_green_led();
void toggle_blue_led();

#endif