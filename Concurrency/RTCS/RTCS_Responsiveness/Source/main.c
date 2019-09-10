/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "timers.h"
#include "delay.h"
#include "ADC.h"
#include "config.h"
#include "RTCS.h"

// software delay
uint8_t g_flash_LED=0; 			// initially don't flash LED, just do RGB sequence
uint32_t g_w_delay=W_DELAY_SLOW; 		// delay for white flash
uint32_t g_RGB_delay=RGB_DELAY_SLOW; 	// delay for RGB sequence

void Initialize_Ports(void) {
	// Enable clock to ports A, B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set LED port bits to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);

	// Select port D on pin mux, enable pull-up resistors
	PORTD->PCR[SW1_POS] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;
	PORTD->PCR[SW2_POS] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;

	// Clear switch bits to input
	PTD->PDDR &= ~MASK(SW1_POS); 
	PTD->PDDR &= ~MASK(SW2_POS); 
	
	// Turn off LEDs
	PTB->PSOR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PSOR |= MASK(BLUE_LED_POS);
}

void Init_Debug_Signals(void) {
	// Enable clock to port B
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Make 3 pins GPIO
	PORTB->PCR[DEBUG_RTCS_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DEBUG_RTCS_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DEBUG_RED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DEBUG_RED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DEBUG_GREEN_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DEBUG_GREEN_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DEBUG_BLUE_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DEBUG_BLUE_POS] |= PORT_PCR_MUX(1);          
	
	// Set ports to outputs
	PTB->PDDR |= MASK(DEBUG_RTCS_POS);
	PTB->PDDR |= MASK(DEBUG_RED_POS);
	PTB->PDDR |= MASK(DEBUG_GREEN_POS);
	PTB->PDDR |= MASK(DEBUG_BLUE_POS);
	
	// Initial values are 0
	PTB->PCOR = MASK(DEBUG_RTCS_POS);
	PTB->PCOR = MASK(DEBUG_RED_POS);
	PTB->PCOR = MASK(DEBUG_GREEN_POS);
	PTB->PCOR = MASK(DEBUG_BLUE_POS);
}	


void Task_Flash(void) {
	PTB->PSOR = MASK(DBG_2);
//	if (g_flash_LED == 1) { 	// Only run task when in flash mode
		Control_RGB_LEDs(1, 1, 1);
		Delay(g_w_delay);
		Control_RGB_LEDs(0, 0, 0);
		Delay(g_w_delay);
//	}
	PTB->PCOR = MASK(DBG_2);
}

void Task_RGB(void) {
	PTB->PSOR = MASK(DBG_3);
//	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		Control_RGB_LEDs(1, 0, 0);
		Delay(g_RGB_delay);
		Control_RGB_LEDs(0, 1, 0);
		Delay(g_RGB_delay);
		Control_RGB_LEDs(0, 0, 1);
		Delay(g_RGB_delay);
//	}
	PTB->PCOR = MASK(DBG_3);
}

void Task_Read_Switches(void) {
	PTB->PSOR = MASK(DBG_1);
	if (SWITCH_PRESSED(SW1_POS)) { // flash white
		RTCS_Enable_Task(Task_Flash, 1);
		RTCS_Enable_Task(Task_RGB, 0);
	} else {
		RTCS_Enable_Task(Task_RGB, 1);
		RTCS_Enable_Task(Task_Flash, 0);
	}
	if (SWITCH_PRESSED(SW2_POS)) {
		g_w_delay = W_DELAY_FAST;
		g_RGB_delay = RGB_DELAY_FAST;
	}	else {
		g_w_delay = W_DELAY_SLOW;
		g_RGB_delay = RGB_DELAY_SLOW;
	}
	PTB->PCOR = MASK(DBG_1);
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	Initialize_Ports();
	Init_Debug_Signals();
	Init_RGB_LEDs();
	
	RTCS_Init(100); // 100 Hz timer ticks
	
	RTCS_Add_Task(Task_Read_Switches, PRIO_RS, 10);
	RTCS_Add_Task(Task_Flash, PRIO_TF, 20);	// Period? 
	RTCS_Add_Task(Task_RGB, PRIO_TRGB, 50);	// Period?
	RTCS_Enable_Task(Task_Flash, 0);
	RTCS_Enable_Task(Task_RGB, 0);

	__enable_irq();
	RTCS_Run_Scheduler();  // This call never returns

}

// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
