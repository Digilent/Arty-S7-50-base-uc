#include "rgb_led.h"
#include "PWM.h"

void RGB_LED_Init(u32 pwmAddr, u32 numLed)
{
	int i;
    PWM_Set_Period(pwmAddr, RGB_PWM_PERIOD);

    for (i = 0; i < (numLed*3); i++)
    {
    	PWM_Set_Duty(pwmAddr, 0, i);
    }
    PWM_Enable(pwmAddr);
}

void RGB_LED_Set(u32 pwmAddr, u32 ledIndex, rgb_t rgbVal)
{
	PWM_Set_Duty(pwmAddr, rgbVal.r*RGB_COLOR_MULT, (ledIndex * 3) + 0); //Red
	PWM_Set_Duty(pwmAddr, rgbVal.g*RGB_COLOR_MULT, (ledIndex * 3) + 1); //Green
	PWM_Set_Duty(pwmAddr, rgbVal.b*RGB_COLOR_MULT, (ledIndex * 3) + 2); //Blue
}
