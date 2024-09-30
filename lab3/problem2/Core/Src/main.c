#include "stm32g0xx.h"

int PSC_val = 1;
uint8_t flag = 0;


void delay(uint32_t);
void LedInit(void);
void LedToggle(void);
void ButtonInit(void);
void InitTimer(void);
void EXTI0_1_IRQHandler(void);

int main(void) {

	LedInit();
	ButtonInit();
	InitTimer();

	TIM3->PSC = 1000*PSC_val; // in the beginning, PSC= 1000 for 1s.

	while(1){

		if(flag){ // When the button is pressed, the flag will be equal to 1 and will enter the if block.
			PSC_val++; //PSC_val is increasing once.
			delay(2000000);
			TIM3->PSC = 1000*PSC_val; // according to PSC_val, PSC is increasing.

			if(PSC_val == 11){ //PSC_val returns to 1 after 11
				PSC_val = 1;
			}
			flag = 0; // When the button is pressed again, the flag is set to 0 to enter the if block.
		}
	}

	return 0;
}

void InitTimer(void){

	RCC->APBENR1 |= (1U<<1); //

	TIM3->CR1 = 0; //TIM3 control register 1 for enabling Counter enable
	TIM3->CR1 |= (1<<7); // for Auto-reload preload enable
	TIM3->CNT = 0; // TIMx counter

	TIM3->PSC = 1000; // TIMx prescaler
	TIM3->ARR = 16000; // TIMx auto-reload register

	TIM3->DIER |= (1<<0); // TIMx DMA/Interrupt enable register
	TIM3->CR1 |= (1<<0); // for Counter enable

	NVIC_SetPriority(TIM3_IRQn,0); // TIM3 priority is 0
	NVIC_EnableIRQ(TIM3_IRQn); // interrupt is enabled.
}

void TIM3_IRQHandler (){
	LedToggle();
	TIM3->SR &= ~(1U << 0);
	/* TIMx status register - Update interrupt flag
	This bit is set by hardware on an update event. It is cleared by software.*/
}

void delay(uint32_t time){
	for(; time>0 ; time--);
}

void LedInit(void){
	/* Enable GPIOC clock */
	RCC->IOPENR |= (1U << 2);

	/* Setup PC6 as output */
	GPIOC->MODER &= ~(3U << 2*6);
	GPIOC->MODER |= (1U << 2*6);

    /* Clear PC6 */
    GPIOC->BRR |= (1U << 6);
}

void LedToggle(void){
	GPIOC->ODR ^= (1U << 6); // using XOR logic, output is changing.
}

void ButtonInit(){

	RCC->IOPENR |= (1U << 0U);
	GPIOA->MODER &= ~(3U << 0);
	GPIOA->PUPDR &= ~(3U << 0); // GPIO port pull-up/pull-down register
	GPIOA->PUPDR |= (1U << 0);

	RCC->APBENR2 |= (1U<<0); // SYSCFGRST: SYSCFG, COMP and VREFBUF reset
	/* EXTI Rising Trigger Selection Register 1
	 * Each bit enables/disables the rising edge trigger for the event and interrupt on the
	corresponding line.
	 */
	EXTI->RTSR1 |= (1U<<0);
	/* EXTI Interrupt Mask Register 1
	 * Each bit enables/disables the rising edge trigger for the event and interrupt on the
	corresponding line.
	 */
	EXTI->IMR1 |= (1U<<0);

	EXTI->FTSR1 |= (1U<<0); // EXTI falling trigger selection register 1
	EXTI->RTSR1 &= ~(1U<<0);

	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void EXTI0_1_IRQHandler(void){
	flag=1; // when button pressed, flag sets 1.
	EXTI->FPR1 |= (1<<0); // EXTI Falling Pending Register 1
}
