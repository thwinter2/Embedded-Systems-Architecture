/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "i2c.h"
#include "mma8451.h"
#include "delay.h"
#include "RTCS.h"

#define FLASH_DELAY 2
#define ACC_SENSITIVITY 90
#define TASK_MOTION_SENSOR_FREQ_HZ (50) 

void Init_Debug_Signals(void) {
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

	PORTB->PCR[DBG_0] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_1] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_2] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_3] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_4] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_5] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_6] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_7] |= PORT_PCR_MUX(1);          

	PTB->PDDR |= MASK(DBG_0) | MASK(DBG_1) | MASK(DBG_2) | MASK(DBG_3) | MASK(DBG_4) | MASK(DBG_5) | MASK(DBG_6) | MASK(DBG_7);
	PTB->PCOR = MASK(DBG_0) | MASK(DBG_1) | MASK(DBG_2) | MASK(DBG_3) | MASK(DBG_4) | MASK(DBG_5) | MASK(DBG_6) | MASK(DBG_7);

}


void Init_Config_Signals(void) {
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	PORTE->PCR[CONFIG1_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[CONFIG1_POS] |= PORT_PCR_MUX(1);          
	PTE->PDDR &= ~MASK(CONFIG1_POS);

	PORTE->PCR[CONFIG2_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[CONFIG2_POS] |= PORT_PCR_MUX(1);          
	PTE->PDDR &= ~MASK(CONFIG2_POS);

	PORTE->PCR[CONFIG3_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[CONFIG3_POS] |= PORT_PCR_MUX(1);          
	PTE->PDDR &= ~MASK(CONFIG3_POS);
}

void Task_Motion_Sensor(void) {
	static int16_t prev_acc_X=0, prev_acc_Y=0, prev_acc_Z=0;
	int16_t acc_X=0, acc_Y=0, acc_Z=0;
	uint8_t rf, gf, bf;

	read_full_xyz(&acc_X, &acc_Y, &acc_Z);

	rf = abs(prev_acc_X - acc_X) > ACC_SENSITIVITY ? 1 : 0;
	gf = abs(prev_acc_Y - acc_Y) > ACC_SENSITIVITY ? 1 : 0;
	bf = abs(prev_acc_Z - acc_Z) > ACC_SENSITIVITY ? 1 : 0;

	Control_RGB_LEDs(rf, gf, bf);
	Delay(FLASH_DELAY);
	Control_RGB_LEDs(0, 0, 0);							
	Delay(FLASH_DELAY*2);		

	prev_acc_X = acc_X;
	prev_acc_Y = acc_Y;
	prev_acc_Z = acc_Z;
}

	/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	
	Init_RGB_LEDs();
	Init_Debug_Signals();
	// Init_Config_Signals();

	Control_RGB_LEDs(1, 1, 0);								/* yellow: starting up */
	i2c_init();																/* init i2c	*/
	Delay(200);
	
	if (!init_mma()) {												/* init mma peripheral */
		Control_RGB_LEDs(1, 0, 0);							/* Light red error LED */
		while (1)																/* not able to initialize mma */
			;
	}
	Control_RGB_LEDs(0, 0, 0);							
	
	RTCS_Init(SCHED_FREQ_HZ);
	RTCS_Add_Task(Task_Motion_Sensor, 0, TICKS(TASK_MOTION_SENSOR_FREQ_HZ)); // Run periodically
	RTCS_Run_Scheduler();
	
}

