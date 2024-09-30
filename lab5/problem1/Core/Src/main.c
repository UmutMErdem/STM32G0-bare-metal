/* author: Umut Mehmet ERDEM | Arda DERİCİ | Serdar BAŞYEMENİCİ
* problem1.c
*/

#include "stm32g0xx.h"

uint16_t pot_value; // Potantiometer data
uint32_t led1_val; // Brightness value for LED1
uint32_t led2_val; // Brightness value for LED2

uint16_t ADC_Data(void);
void GPIO_Config(void);
void TIM2_Config(void);
void TIM3_Config(void);
void ADC_Config(void);
void Delay(volatile uint32_t);

void TIM3_IRQHandler(void){
	TIM3->SR &=  ~(TIM_SR_CC1IF); // Clear capture compare interrupt 1 flag
	TIM3->SR &= ~(TIM_SR_UIF); // Clear update status register
}

void TIM2_IRQHandler(void) {
    // PWM Duty Cycle[%] = (CCRx/ARR)*100;
	pot_value = ADC_Data();
	led1_val = (16000*pot_value)/4095;
	led2_val = 16000-led1_val;

	TIM2->CCR1 =led1_val;
	TIM3->CCR1 =led2_val;

	TIM3->SR &=  ~(TIM_SR_CC1IF); // Clear capture compare interrupt 1 flag
    TIM2->SR &= ~(TIM_SR_UIF); // Clear update status register
}

int main(void){

	GPIO_Config();
	TIM2_Config();
	TIM3_Config();
	ADC_Config();

	while(1){

	}
	return 0;
}

uint16_t ADC_Data(void){
	ADC1->CR |= ADC_CR_ADSTART; // ADC Start Conversion

	/*This bit is set by hardware at the end of each conversion(EOC) of a channel
	 * when a new data result is available in the ADC_DR register.
	 * 0: Channel conversion not complete
	 * 1: Channel conversion complete*/
	if((ADC1->ISR>>2) & (ADC_ISR_EOC>>2)){
		return ADC1->DR; // return ADC data value
	}
	return 0;
}

void GPIO_Config(void){
	// input-output A and B ports clock enable
	RCC->IOPENR |= (RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN);

	// select PA0 mode as Alternate Function
	GPIOA->MODER &= ~(3U << 2*0);
	GPIOA->MODER |= (2U << 2*0);

	/* PA0 pin used for TIM2_CH1 are selected
	 * with GPIOx_AFRL = AFRL_AFSELy(Alternate Function register -
	 * Alternate function selection for port x pin y)
	 * AF2 -->> TIM2_CH1*/
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_1;

	// select PB4 mode as Alternate Function
	GPIOB->MODER &= ~(3U << 2*4);
	GPIOB->MODER |=  (2<< 2*4) ;

	/* PB4 pin used for TIM3_CH1 are selected
	* AF1 -->> TIM3_CH1*/
	GPIOB->AFR[0]  |= GPIO_AFRL_AFSEL4_0;

	//PA1 is ADC
	GPIOA->MODER |= (3 << 2*1);
}

void ADC_Config(void){
	RCC->APBENR2 |= RCC_APBENR2_ADCEN; // ADC clock enable

	ADC1->CR |=ADC_CR_ADVREGEN; //voltage regulator enable
	Delay(500);

	ADC1->CR |=ADC_CR_ADCAL; //calibration
	while(((ADC1->CR>>31)==ADC_CR_ADCAL>>31)); // until calibration
	/* 0: Calibration complete
	 * 1: Write 1 to calibrate the ADC. Read at 1 means that a calibration is in progress.*/

	ADC1->CR |= ADC_CR_ADEN; // ADC is enabled.
	while (ADC1->ISR & ADC_ISR_ADRDY); // 1: ADC is ready to start conversion

	ADC1->CHSELR |= ADC_CHSELR_CHSEL1; //chanel selection for PA1
	ADC1->CFGR1 |= ADC_CFGR1_CONT; // contionous conversion

	ADC1->SMPR |= (6UL<<0); // 79.5 ADC clock cycles for sampling time selection 1
}

void TIM2_Config(void){
	RCC->APBENR1 |= RCC_APBENR1_TIM2EN; // Timer 2 clock enable

	TIM2->CR1 = 0; // zero out the control register just in case
	TIM2->CR1 |= TIM_CR1_ARPE; // Auto-reload preload enable

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

void TIM3_Config(void){
	RCC->APBENR1 |=	RCC_APBENR1_TIM3EN; // Timer 3 clock enable

	TIM3->CR1=0; // zero out the control register just in case
	TIM3->CR1 |= TIM_CR1_ARPE; // Auto-reload preload enable

	TIM3->CCMR1 |= (6U << 4); // PWM mode 1 is selected. -->> 0110: PWM Mode
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE; // Output Compare 1 Preload Enable

	TIM3->CCER |= TIM_CCER_CC1E; // Capture compare ch1 enable

	TIM3->CNT =0; // zero out counter

	// tim uptade frequency = TIM_CLK/((TIM_PSC+1)*TIM_ARR) for 1s interrupt
	TIM3->PSC= 9;
	TIM3->ARR= 16000;

	// Update Generation: Re-initialize the counter and generates an update of the registers.
	TIM3->EGR |= TIM_EGR_UG;

	TIM3->DIER |= TIM_DIER_UIE; // Update interrupt enable

	TIM3->CR1 |= TIM_CR1_CEN; // TIM3 Counter enable

	NVIC_SetPriority(TIM3_IRQn, 1);
	NVIC_EnableIRQ(TIM3_IRQn);
}

void Delay(volatile uint32_t time){
    for(; time>0; time--);
}
