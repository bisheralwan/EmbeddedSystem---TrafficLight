#include <stdlib.h>
#include <stdio.h>
#include "msp.h"

static int light = 0;

int main(void) {
	
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
	
	//GPIO configuration for both Ports	
	P1->SEL0 &= ~((uint8_t)((1<<4) | (1<<1) | (1<<0)));
	P1->SEL1 &= ~((uint8_t)((1<<4) | (1<<1) | (1<<0)));
	P2->SEL0 &= ~((uint8_t)((1<<2) | (1<<1) | (1<<0)));
	P2->SEL1 &= ~((uint8_t)((1<<2) | (1<<1) | (1<<0)));
	
	//DIR, REN, and OUT for inputs
	P1->DIR &= ~((uint8_t)((1<<4) | (1<<1)));
	P1->REN |= (uint8_t)((1<<4) | (1<<1));
	P1->OUT |= (uint8_t)((1<<4) | (1<<1));
	
	//DIR and OUT for outputs
	P1->DIR |= (uint8_t)(1<<0);
	P2->DIR |= (uint8_t)((1<<4) | (1<<1) | (1<<0));
	P2->OUT |= ((uint8_t)((1<<2) | (1<<1) | (1<<0)));
	P1->OUT |= ((uint8_t)(1<<0));
	
	//Interrupt Configuration for inputs
	P1IES |= (uint8_t)((1<<4) | (1<<1));	
	P1IFG &= ~((uint8_t)((1<<4) | (1<<1)));	
	P1IE |= (uint8_t)((1<<4) | (1<<1));

	//Setting priority, clearing pending IRQ's, enabling IRQ's for NVIC, and enabling interrupts
	NVIC_SetPriority(PORT1_IRQn, 2); //sets PORT1_IRQn handler with a priority of 2
	NVIC_ClearPendingIRQ(PORT1_IRQn); //clears pending interrupts
	NVIC_EnableIRQ(PORT1_IRQn); //enables interrupts in IRQ
	__ASM("CPSIE I"); //enable global interrupts
	
	//Timer Configuration
	TA0CTL &= ~((uint16_t)((1<<5) | (1<<4))); //Set timers mode as: stopped
	TA0CTL &= ~((uint16_t)(1<<0)); //clear the interrupt flag
	TA0CTL |= (uint16_t)((1<<1)); //enable timer's interrupt
	//Set TASSEL as ACLK with a 32.768kHZ frequency
	TA0CTL &= ~((uint16_t)(1<<9)); //set TASSEL bit 9 as 0
	TA0CTL |= (uint16_t)(1<<8); //SET TASSEL bit 8 as 1
	
	//Set Up Mode
	TA0CTL |= (uint16_t)(1<<4); //bit 4 is 1
	
	TA0CCR0 = (uint16_t)(8192); //0.5 seconds for ACLK 
	
	
	NVIC_SetPriority(TA0_N_IRQn, 3);
	NVIC_ClearPendingIRQ(TA0_N_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn);
	
	
	//Timer 1 configuration
	TA1CTL &= ~((uint16_t)((1<<5) | (1<<4))); //Set timers mode as: stopped
	TA1CTL &= ~((uint16_t)(1<<0)); //clear the interrupt flag
	TA1CTL |= (uint16_t)((1<<1)); //enable timer's interrupt
	//Set TASSEL as ACLK with a 32.768kHZ frequency
	TA1CTL &= ~((uint16_t)(1<<9)); //set TASSEL bit 9 as 0
	TA1CTL |= (uint16_t)(1<<8); //SET TASSEL bit 8 as 1
	
	//Set Up Mode
	TA1CTL |= (uint16_t)(1<<4); //bit 4 is 1
	
	TA1CCR0 = (uint16_t)(16384); //Timer interrupts every 1 second
	
	
	NVIC_SetPriority(TA1_N_IRQn, 3);
	NVIC_ClearPendingIRQ(TA1_N_IRQn);
	NVIC_EnableIRQ(TA1_N_IRQn);
	
	while(1) {
		__ASM("WFI");
	}
}

void PORT1_IRQHandler(void) {
	P1IFG &= ~((uint8_t)(1<<1)); //clear flag
	if (light < 5) {
		light = 5;
	}
}

void TA0_N_IRQHandler() {
	TA0CTL &= ~((uint16_t)(1<<0)); //clear flag
	P1OUT ^= (uint8_t)(1<<0);
}

void TA1_N_IRQHandler() {
	TA1CTL &= ~((uint16_t)(1<<0)); //clear flag
	
	if (light < 5) {
		//set to Green
		P2OUT = ((uint8_t)((0<<2) | (1<<1) | (0<<0)));
	}

	if (light == 5) {
		//set to Yellow
		P2OUT = ((uint8_t)((0<<2) | (1<<1) | (1<<0)));
	}

	if ((light > 5) && (light < 9)) {
		//set to Red
		P2OUT = ((uint8_t)((0<<2) | (0<<1) | (1<<0)));

		if (light == 8) {
			light = 0;
		}
	}
	
	light++;
}
