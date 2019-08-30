/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"

#include <cmsis_os2.h>
#include "threads.h"

#include "LCD.h"
#include "LCD_driver.h"
#include "font.h"

#include "LEDs.h"
#include "timers.h"
#include "sound.h"
#include "DMA.h"
#include "I2C.h"
#include "mma8451.h"
#include "delay.h"
#include "profile.h"
#include "math.h"

void HBLED_Test(void) {

	// Enable GPIO to drive HBLED briefly
	// Jumper must be on J11 to provide power
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PTE->PSOR = MASK(31);
	PTE->PDDR |= MASK(31);
	
	PORTE->PCR[31] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[31] |= PORT_PCR_MUX(1);
	
	// Send a short pulse (active low)
	PTE->PCOR = MASK(31);
	Delay(1);	
	PTE->PSOR = MASK(31);
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	Init_Debug_Signals();
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,1);			
	
	Sound_Init();	
	Play_Tone();
	Sound_Disable_Amp();

	HBLED_Test();
	
	LCD_Init();
	LCD_Text_Init(1);
	LCD_Erase();
	
#if 0
	Graphics_Test(); // Works
#endif
	
#if 0
	LCD_Text_Test();
	LCD_Refresh();
	while (1);
#endif

	
	
	LCD_Erase();
	LCD_Text_PrintStr_RC(0,0, "Test Code");

#if 0
	// LCD_TS_Calibrate();
	LCD_TS_Test();
#endif

#if 1
	LCD_Text_PrintStr_RC(1,0, "Accel...");
	i2c_init();											// init I2C peripheral
	if (!init_mma()) {							// init accelerometer
		Control_RGB_LEDs(1,0,0);			// accel initialization failed, so turn on red error light
		while (1)
			;
	}
	LCD_Text_PrintStr_RC(1,9, "Done");
#endif
	Delay(70);
	LCD_Erase();

	osKernelInitialize();
	Create_OS_Objects();
	osKernelStart();	
}
