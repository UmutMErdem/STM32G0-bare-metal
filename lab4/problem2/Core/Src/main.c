#include "stm32g0xx.h"

void GPIO_Config(void);
void TIM2_Config(void);

uint8_t limit_flag = 0;

void TIM2_IRQHandler(void) {
    // PWM Duty Cycle[%] = (CCRx/ARR)*100;

	/* capture-compare value of TIM2 is increasing until
	 *  equal to period value of TIM2, when TIM2_CCR is equal to TIM2_ARR,
	 *  limit_flag is 1 and TIM2_CRR is decreasing until equal to 0.*/
	if(!(limit_flag) && TIM2->CCR1 < TIM2->ARR){
		TIM2->CCR1+=200;
		if(TIM2->CCR1 >= TIM2->ARR) limit_flag = 1;
	}
	else if(limit_flag && TIM2->CCR1>0){
		TIM2->CCR1-=200;
		if(TIM2->CCR1<=0) limit_flag = 0;
	}

    TIM2->SR &= ~(1U << 0); // Clear update status register
}

int main(void){

	GPIO_Config();
	TIM2_Config();

	while(1){
	}

	return 0;
}

void GPIO_Config(void){
	// input-output A port clock enable
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	// select PA0 mode as Alternate Function
	GPIOA->MODER &= ~(3U << 2*0);
	GPIOA->MODER |= (2U << 2*0);

	/* PA0 pin used for TIM2_CH1 are selected
	 * with GPIOx_AFRL = AFRL_AFSELy(Alternate Function register -
	 * Alternate function selection for port x pin y)
	 * AF2 -->> TIM2_CH1*/
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_1;
}

void TIM2_Config(void){
	RCC->APBENR1 |= RCC_APBENR1_TIM2EN; // Timer 2 clock enable

	TIM2->CR1 = 0; // zero out the control register just in case
	TIM2->CR1 = TIM_CR1_ARPE; // Auto-reload preload enable

	TIM2->CCMR1 |= (6U << 4); // PWM mode 1 is selected.
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // Output Compare 1 Preload Enable

	TIM2->CCER |= TIM_CCER_CC1E; // Capture compare ch1 enable

	TIM2->CNT = 0; // zero out counter

	// tim uptade frequency = TIM_CLK/((TIM_PSC+1)*TIM_ARR) for 1 s interrupt
	TIM2->PSC = 9; // prescaler
	TIM2->ARR = 16000; // period

	TIM2->CCR1 = 0; // zero out duty for ch1 in TIM capture/compare register 1

	// Update Generation: Re-initialize the counter and generates an update of the registers.
	TIM2->EGR |= TIM_EGR_UG;

	TIM2->DIER |= TIM_DIER_UIE; // Update interrupt enable

	TIM2->CR1 |= TIM_CR1_CEN; // TIM2 Counter enable

	NVIC_SetPriority(TIM2_IRQn, 1); // Setting Priority for timer handler
	NVIC_EnableIRQ(TIM2_IRQn); // timer handler enable
}
