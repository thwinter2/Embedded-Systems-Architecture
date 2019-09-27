/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "i2c.h"
#include "mma8451.h"
#include "delay.h"

#define FLASH_DELAY 10
#define ACC_SENSITIVITY 70

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	int16_t prev_acc_X, prev_acc_Y, prev_acc_Z;
	uint8_t rf, gf, bf;
	
	Init_RGB_LEDs();
	i2c_init();																/* init i2c	*/
	if (!init_mma()) {												/* init mma peripheral */
		Control_RGB_LEDs(1, 0, 0);							/* Light red error LED */
		while (1)																/* not able to initialize mma */
			;
	}
	Control_RGB_LEDs(1, 1, 0);							
	
	Delay(100);
	
	while (1) {
		Delay(50);
		prev_acc_X = acc_X;
		prev_acc_Y = acc_Y;
		prev_acc_Z = acc_Z;
		
		rf = gf = bf = 0;
		
		read_full_xyz();
		
		rf = abs(prev_acc_X - acc_X) > ACC_SENSITIVITY ? 1 : 0;
		gf = abs(prev_acc_Y - acc_Y) > ACC_SENSITIVITY ? 1 : 0;
		bf = abs(prev_acc_Z - acc_Z) > ACC_SENSITIVITY ? 1 : 0;

		Control_RGB_LEDs(rf, gf, bf);
		Delay(FLASH_DELAY);
		Control_RGB_LEDs(0, 0, 0);							
		Delay(FLASH_DELAY*2);		
	}
	
}

