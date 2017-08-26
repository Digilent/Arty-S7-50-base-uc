
/************************************************************************/
/*																		*/
/*	led_game.c	--	LED Game for Digilent Boards						*/
/*																		*/
/************************************************************************/
/*	Author: Sam Bobrowicz												*/
/*	Copyright 2017, Digilent Inc.										*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*		A Game that is played using the buttons and switches to try to  */
/*      match a random color displayed on the RGB LED in a limited      */
/*      of guesses. Attach a terminal with baud 115200, no parity for   */
/*      instructions on how to play.                                    */
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		8/25/2017(SamB): Created										*/
/*																		*/
/************************************************************************/

#include <stdio.h>
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"
#include "PWM.h"
#include "xtmrctr.h"
#include "rgb_led.h"
#include "xuartlite_l.h"

/*
 * XPAR redefinitions
 */
#define LED_BASEADDR XPAR_AXI_GPIO_LED_BASEADDR
#define INPUT_BASEADDR XPAR_AXI_GPIO_INPUT_BASEADDR
#define RGB_BASEADDR XPAR_PWM_0_PWM_AXI_BASEADDR
#define TIMER_ID XPAR_AXI_TIMER_0_DEVICE_ID
#define UART_BASEADDR XPAR_AXI_UARTLITE_0_BASEADDR

//Should always be zero
#define TIMER_NUMBER 0

//Number of RGB LEDs on the board
#define NUM_RGB_LEDS 2

//Number of guesses the user gets
#define NUM_GUESSES 5

XTmrCtr TimerCounter;
uint32_t rGuesses, gGuesses, bGuesses;

void Print_Guess(u32 guess, rgb_t prevRgb);

void Flush_UART();


int main()
{
    XTmrCtr *TmrCtrInstancePtr = &TimerCounter;
	uint32_t led;
	uint32_t tmrVal;
	uint32_t guess = 0;
	char userInput = 'a';
	rgb_t userColor = {0,0,0};
	rgb_t randColor = {0,0,0};
	uint8_t hasWon = 0;

	rGuesses = 0;
	gGuesses = 0;
	bGuesses = 0;

	led = 0;
    Xil_Out32(LED_BASEADDR + 0x0, led);
    Xil_Out32(LED_BASEADDR + 0x4, 0x0);

    RGB_LED_Init(RGB_BASEADDR, NUM_RGB_LEDS);

	XTmrCtr_Initialize(TmrCtrInstancePtr, TIMER_ID);
	XTmrCtr_SetOptions(TmrCtrInstancePtr, TIMER_NUMBER, XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(TmrCtrInstancePtr, TIMER_NUMBER, 0);
	XTmrCtr_Start(TmrCtrInstancePtr, TIMER_NUMBER);

	xil_printf("\x1B[H"); //Set cursor to top left of terminal
	xil_printf("\x1B[2J"); //Clear terminal
	xil_printf("**************************************************\n\r");
	xil_printf("*         Digilent RGB LED Guessing Game         *\n\r");
	xil_printf("**************************************************\n\r");
	xil_printf("Press any key to Begin...\n\r");

	Flush_UART();
	/* Wait for data on UART */
	XUartLite_RecvByte(UART_BASEADDR);

	/*
	 * Get random number based on entropy of user input. Use that number
	 * to generate the random color displayed on LD0. There are 1000
	 * possible colors.
	 */
	tmrVal = XTmrCtr_GetValue(TmrCtrInstancePtr, TIMER_NUMBER);
	randColor.r = ((tmrVal % 1000) / 100) * 25;
	randColor.g = ((tmrVal % 100) / 10) * 25;
	randColor.b = (tmrVal % 10) * 25;
	RGB_LED_Set(RGB_BASEADDR, 0, randColor);

	while (guess < NUM_GUESSES && !hasWon)
	{
		Print_Guess(guess, userColor);

		/*
		 * Get valid user input for Red Guess
		 */
		xil_printf("Guess Red Magnitude (0-9):");
		Flush_UART();
		userInput = 'a';
		while (userInput < '0' || userInput > '9')
		{
			userInput = XUartLite_RecvByte(UART_BASEADDR);
		}
		xil_printf("%c\n\r", userInput);
		//Quick and dirty ASCII to hex conversion
		userColor.r = (((uint8_t) userInput) - 48) * 25;

		/*
		 * Get valid user input for Green Guess
		 */
		xil_printf("Guess Green Magnitude (0-9):");
		Flush_UART();
		userInput = 'a';
		while (userInput < '0' || userInput > '9')
		{
			userInput = XUartLite_RecvByte(UART_BASEADDR);
		}
		xil_printf("%c\n\r", userInput);
		//Quick and dirty ASCII to hex conversion
		userColor.g = (((uint8_t) userInput) - 48) * 25;

		/*
		 * Get valid user input for Blue Guess
		 */
		xil_printf("Guess Blue Magnitude (0-9):");
		Flush_UART();
		userInput = 'a';
		while (userInput < '0' || userInput > '9')
		{
			userInput = XUartLite_RecvByte(UART_BASEADDR);
		}
		xil_printf("%c\n\r", userInput);
		//Quick and dirty ASCII to hex conversion
		userColor.b = (((uint8_t) userInput) - 48) * 25;

		/*
		 * Set LEDs with info and check if won
		 */
		RGB_LED_Set(RGB_BASEADDR, 1, userColor);
		led = 0;
		if (userColor.r == randColor.r)
			led = led | (1 << 2);
		if (userColor.g == randColor.g)
			led = led | (1 << 1);
		if (userColor.b == randColor.b)
			led = led | 1;
		if (led == 7) //This is true if won
		{
			led = led | (1 << 3);
			hasWon = 1;
		}
	    Xil_Out32(LED_BASEADDR + 0x0, led);
		guess++;
	}
	xil_printf("\x1B[H"); //Set cursor to top left of terminal
	xil_printf("\x1B[2J"); //Clear terminal
	xil_printf("**************************************************\n\r");
	xil_printf("*                    YOU %s                    *\n\r", hasWon ? "WON!" : "LOST");
	xil_printf("**************************************************\n\r");
	xil_printf("*Number of Guesses: %28d *\n\r", guess);
	xil_printf("*Last Guess:              Red=%d  Green=%d  Blue=%d *\n\r", userColor.r / 25,userColor.g / 25,userColor.b / 25);
	xil_printf("*Correct color:           Red=%d  Green=%d  Blue=%d *\n\r", randColor.r / 25,randColor.g / 25,randColor.b / 25);
	xil_printf("**************************************************\n\r");
	xil_printf("\n\r");
	xil_printf("     PRESS THE RED RESET BUTTON TO PLAY AGAIN     \n\r");
	xil_printf("\n\r");

	while (1)
	{}

	return 0;
}

void Print_Guess(u32 guess, rgb_t prevRgb)
{
	rGuesses = rGuesses * 10 + prevRgb.r / 25;
	gGuesses = gGuesses * 10 + prevRgb.g / 25;
	bGuesses = bGuesses * 10 + prevRgb.b / 25;

	xil_printf("\x1B[H"); //Set cursor to top left of terminal
	xil_printf("\x1B[2J"); //Clear terminal
	xil_printf("**************************************************\n\r");
	xil_printf("*         Digilent RGB LED Guessing Game         *\n\r");
	xil_printf("**************************************************\n\r");
	xil_printf("*Instructions:                                   *\n\r");
	xil_printf("*   Try to guess the color currently displayed   *\n\r");
	xil_printf("*   on LD0 by entering the color's Red, Green,   *\n\r");
	xil_printf("*   and Blue magnitudes. You have %d guesses.     *\n\r", NUM_GUESSES);
	xil_printf("*   The other LEDs provide the following info:   *\n\r");
	xil_printf("*                                                *\n\r");
	xil_printf("*   LD1: Color of previous guess                 *\n\r");
	xil_printf("*   LD2: Lit when previous blue guess was right  *\n\r");
	xil_printf("*   LD3: Lit when previous green guess was right *\n\r");
	xil_printf("*   LD4: Lit when previous red guess was right   *\n\r");
	xil_printf("*   LD5: Illuminated if you win!                 *\n\r");
	xil_printf("**************************************************\n\r");
	xil_printf("*Remaining Guesses: %28d *\n\r", NUM_GUESSES - guess);
	if (guess != 0)
	{
		xil_printf("*Previous Guess:          Red=%d  Green=%d  Blue=%d *\n\r", prevRgb.r / 25,prevRgb.g / 25,prevRgb.b / 25);
		xil_printf("*Red Values Guessed: %27d *\n\r", rGuesses);
		xil_printf("*Green Values Guessed: %25d *\n\r", gGuesses);
		xil_printf("*Blue Values Guessed: %26d *\n\r", bGuesses);
	}
	xil_printf("**************************************************\n\r");

	return;
}

void Flush_UART()
{
	while (!XUartLite_IsReceiveEmpty(UART_BASEADDR))
	{
		XUartLite_ReadReg(UART_BASEADDR, XUL_RX_FIFO_OFFSET);
	}

	return;
}
