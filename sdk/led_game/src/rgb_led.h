
#ifndef RGB_LED_H
#define RGB_LED_H

#include "xil_types.h"

/*
 * Simple struct type representing a 24-bit RGB color
 */
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
}rgb_t;

//The period of the PWM window in clock cycles (clock typically 100MHz)
#define RGB_PWM_PERIOD 8192
//Maximum PWM duty cycle for the RGB LEDs. The RGB LEDs are too bright to look at 100% duty cycle, so this value dims them.
//The maximum brightness equals (RGB_MAX_PWM/RGB_PWM_PERIOD)*100% .
#define RGB_MAX_PWM 256
//Don't modify, used to convert from 8-bit color values to a PWM value that is between 0-RGB_MAX_PWM
#define RGB_COLOR_MULT (RGB_MAX_PWM / 256)

void RGB_LED_Init(u32 pwmAddr, u32 numLed);
void RGB_LED_Set(u32 pwmAddr, u32 ledIndex, rgb_t rgbVal);

#endif
