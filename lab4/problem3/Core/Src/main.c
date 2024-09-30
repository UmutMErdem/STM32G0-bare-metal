/* author: Umut Mehmet ERDEM | Arda DERİCİ | Serdar BAŞYEMENİCİ
* problem3.c
*/

#include "stm32g0xx.h"

uint32_t ten_thousands,thousands,hundreds,tens;
uint32_t number;
uint32_t is_ten_digit=1;
uint8_t ten =0;
uint8_t unit =0;
uint32_t counter=0;
uint32_t dutyCycle =0;

void USART2_Config(uint32_t);
void keypad_Config(void);
void TIM3_Config(void);
uint8_t conIntToAlp(uint8_t);
void printChar(uint8_t);
void SetKeypad(void);
void ResetKeypad(void);
void delay(volatile uint32_t);

int main(void) {

	ResetKeypad();
	TIM3_Config();
	keypad_Config();
	USART2_Config(9600);
	SetKeypad();

    while(1) {
    }

    return 0;
}

uint8_t conIntToAlp(uint8_t a){
	if(a==0)
		return 48;
	else if(a==1)
		return 49;
	else if(a==2)
		return 50;
	else if(a==3)
		return 51;
	else if (a==4)
		return 52;
	else if(a==5)
		return 53;
	else if (a==6)
		return 54;
	else if(a==7)
		return 55;
	else if(a==8)
		return 56;
	else return 57;
}

void TIM3_IRQHandler(){
	counter+=10; // to write to the console every two seconds

	if(counter==2000){
		delay(2000);
		number=TIM3->CCR1;

		ten_thousands=(number)/10000;
		printChar(conIntToAlp(ten_thousands));

		number -= (ten_thousands * 10000);
		thousands=number/1000;
		printChar(conIntToAlp(thousands));

		number -= (thousands * 1000);
		printChar(conIntToAlp(hundreds));

		number -= (hundreds * 100);
		tens = (number/10);
		printChar(conIntToAlp(tens));

		number -= (tens * 10);
		printChar(conIntToAlp(number));
		counter=0;
	}

	TIM3->SR &=  ~(1U<<0);
	TIM3->SR &=  ~(1U<<1);
}

void EXTI0_1_IRQHandler(){
	ResetKeypad();

	GPIOA->ODR |=(1<<0);
	if((GPIOB->IDR>>1)&1){
		if(is_ten_digit==1){ // KEYPAD button control for 1 - first column
			ten=1;
			is_ten_digit=0;
		}
		else{
			unit=1;
			is_ten_digit=1;
		}
	}

	else{ // KEYPAD button control for 4
		GPIOA->ODR &=  ~(1U <<0);
		GPIOA->ODR |=(1<<1);
		if((GPIOB->IDR>>1)&1){

			if(is_ten_digit==1){
				ten=4;
				is_ten_digit=0;
			}
			else{
				unit=4;
				is_ten_digit=1;
			}

		}

		else {	// 	KEYPAD button control for 7
			GPIOA->ODR &=  ~(1U <<1);
			GPIOA->ODR |=(1<<4);
			if((GPIOB->IDR>>1)&1){
				if(is_ten_digit==1){
					ten=7;
					is_ten_digit=0;
				}
				else{
					unit=7;
					is_ten_digit=1;
				}
			}

			else{
				GPIOA->ODR &=  ~(1U <<4);
				GPIOA->ODR |=(1<<5);
				if((GPIOB->IDR>>1)&1){
				}
			}
		}
	}

	SetKeypad();
	EXTI->RPR1 |=(1<<1);
}

void EXTI4_15_IRQHandler(){
	ResetKeypad();

	GPIOA->ODR |=(1<<0);
	if((GPIOB->IDR>>4)&1){ // KEYPAD button control for A
	}

	else{
		GPIOA->ODR &=  ~(1U <<0);
		GPIOA->ODR |=(1<<1);
		if((GPIOB->IDR>>4)&1){ // KEYPAD button control for B
		}
		else {
			GPIOA->ODR &=  ~(1U <<1);
			GPIOA->ODR |=(1<<4);
			if((GPIOB->IDR>>4)&1){ // KEYPAD button control for C
			}
			else{
				GPIOA->ODR &=  ~(1U <<4);
				GPIOA->ODR |=(1<<5);
				if((GPIOB->IDR>>4)&1){ // KEYPAD button control for D
				}
			}
		}
	}

	SetKeypad();
	EXTI->RPR1 |=(1<<4);
}

void EXTI2_3_IRQHandler(){
	ResetKeypad();
	if((EXTI->RPR1>>2)&1){ // KEYPAD button control for number 2 - 2nd column
		GPIOA->ODR |=(1<<0);
		if((GPIOB->IDR>>2)&1){
			if(is_ten_digit==1){
				ten=2;
				is_ten_digit=0;
			}
			else{
				unit=2;
				is_ten_digit=1;
			}
		}

		else{
			GPIOA->ODR &=  ~(1U <<0);
			GPIOA->ODR |=(1<<1);
			if((GPIOB->IDR>>2)&1){  // KEYPAD button control for number 5
				if(is_ten_digit==1){
					ten=5;
					is_ten_digit=0;
				}
				else{
					unit=5;
					is_ten_digit=1;
				}
			}

			else {
				GPIOA->ODR &=  ~(1U <<1);
				GPIOA->ODR |=(1<<4);
				if((GPIOB->IDR>>2)&1){ // KEYPAD button control for number 8
					if(is_ten_digit==1){
						ten=8;
						is_ten_digit=0;
					}
					else{
						unit=8;
						is_ten_digit=1;
					}
				}

				else{
					GPIOA->ODR &=  ~(1U <<4);
					GPIOA->ODR |=(1<<5);
					if((GPIOB->IDR>>2)&1){ // 	KEYPAD button control for number 0
						if(is_ten_digit==1){
							ten=0;
							is_ten_digit=0;
						}
						else{
							unit=0;
							is_ten_digit=1;
						}
					}
				}
			}
		}

		EXTI->RPR1 |=(1<<2);
	}
	else{
		GPIOA->ODR |=(1<<0);
		if((GPIOB->IDR>>3)&1){
			if(is_ten_digit==1){ // 	KEYPAD button control for 3 - 3rd column
				ten=3;
				is_ten_digit=0;
			}
			else{
				unit=3;
				is_ten_digit=1;
			}
		}
		else{
			GPIOA->ODR &=  ~(1U <<0);
			GPIOA->ODR |=(1<<1);
			if((GPIOB->IDR>>3)&1){
				if(is_ten_digit==1){ // KEYPAD button control for 6
					ten=6;
					is_ten_digit=0;
				}
				else{
					unit=6;
					is_ten_digit=1;
				}
			}

			else {
				GPIOA->ODR &=  ~(1U <<1);
				GPIOA->ODR |=(1<<4);
				if((GPIOB->IDR>>3)&1){ // 	KEYPAD button control for 9
					if(is_ten_digit==1){
						ten=9;
						is_ten_digit=0;
					}
					else{
						unit=9;
						is_ten_digit=1;
					}
				}

				else{
					GPIOA->ODR &=  ~(1U <<4);
					GPIOA->ODR |=(1<<5);
					if((GPIOB->IDR>>3)&1){ // 	KEYPAD button control for #
						TIM3->CCR1=(16000*((ten*10)+unit)/100);
					}
				}
			}
		}

		EXTI->RPR1 |=(1<<3);
	}

	SetKeypad();// all output set
}

void printChar(uint8_t b){
	USART2->TDR =(uint16_t)b;
	while(!(USART2->ISR&(1<<6)));
}

void SetKeypad(void){
	GPIOA->ODR |= (1<<0);  // R1 set
	GPIOA->ODR |= (1<<1);  // R2 set
	GPIOA->ODR |= (1<<4);  // R3 set
	GPIOA->ODR |= (1<<5);  // R4 set

}

void ResetKeypad(void){
	GPIOA->ODR &=  ~(1U <<0); // R1 reset
	GPIOA->ODR &=  ~(1U <<1); // R2 reset
	GPIOA->ODR &=  ~(1U <<4); // R3 reset
	GPIOA->ODR &=  ~(1U <<5);  // R4 reset
}

void delay(volatile uint32_t time){
    for(; time>0; time--);
}

void keypad_Config(void){

	 GPIOA->MODER &=  ~(3U<<2*0); // PA0 as output  R1
	 GPIOA->MODER |=  (1U<<0);

	 GPIOA->MODER &=  ~(3U <<2*1); // PA1  as output R2
	 GPIOA->MODER |=  (1 << 2);

	 GPIOA->MODER &=  ~(3U <<2*4); // PA4  as output R3
	 GPIOA->MODER |=  (1 << 8);

	 GPIOA->MODER &=  ~(3U <<2*5); //PA5  as output R4
	 GPIOA->MODER |=  (1 << 10);

	 GPIOB->MODER &= ~(3U << 2*1); // PB1 as input C1
	 GPIOB->PUPDR |= (2U << 2*1);

	 GPIOB->MODER &= ~(3U << 2*2); // PB2 as input C2
	 GPIOB->PUPDR |= (2U << 2*2);

	 GPIOB->MODER &= ~(3U << 2*3); // PB3 as input C3
	 GPIOB->PUPDR |= (2U << 2*3);

	 GPIOB->MODER &= ~(3U << 2*4); // PB4
	 GPIOB->PUPDR |= (2U << 2*4);

	 EXTI->RTSR1 |=(1U<<1); // PB1 as interrupt
	 EXTI->EXTICR[0] |= (1U<<8*1);
	 EXTI->IMR1 |=(1<<1);
	 NVIC_SetPriority(EXTI0_1_IRQn,1);
	 NVIC_EnableIRQ(EXTI0_1_IRQn);

	 EXTI->RTSR1 |=(1U<<2); // PB2 as interrupt
	 EXTI->EXTICR[0] |= (1U<<8*2);
	 EXTI->IMR1 |=(1<<2);

	 EXTI->RTSR1 |=(1U<<3);// PB3 as interrupt
	 EXTI->EXTICR[0] |= (1U<<8*3);
	 EXTI->IMR1 |=(1<<3);
	 NVIC_SetPriority(EXTI2_3_IRQn,0);
	 NVIC_EnableIRQ(EXTI2_3_IRQn);

	 EXTI->RTSR1 |=(1U<<4);		// PB4 as interrupt
	 EXTI->EXTICR[1] |= (1U<<8*0);
	 EXTI->IMR1 |=(1<<4);
	 NVIC_SetPriority(EXTI4_15_IRQn,2);
	 NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void USART2_Config(uint32_t bdr){
	// enable GPIOA
	RCC->IOPENR |= (1U << 0);
	RCC->APBENR1 |= (1U << 17);

	GPIOA-> MODER &= ~(3U << 2*2);
	GPIOA-> MODER |=  (2U << 2*2);

	GPIOA-> AFR[0] &= ~(0xFU << 4*2);
	GPIOA-> AFR[0] |=  (1U << 4*2);

	GPIOA-> MODER &= ~(3U << 2*3);
	GPIOA-> MODER |=  (2U << 2*3);

	GPIOA-> AFR[0] &= ~(0xFU << 4*3);
	GPIOA-> AFR[0] |=  (1U << 4*3);

	USART2 -> CR1 = 0;
	USART2 -> CR1 |= (1U << 3); // Transmitter
	USART2 -> CR1 |= (1U << 2); // Receiver

	USART2 -> CR1 |= (1U << 5);

	USART2 -> BRR = (uint16_t)(SystemCoreClock / bdr);

	USART2 -> CR1 |= (1U << 0); // Usart Enable

}

void TIM3_Config(void){
	/* Enable GPIOB and GPIOA clock */
	RCC->IOPENR |= (3U << 0);

	/* Setup PA6 as alternate function */
	GPIOA->MODER &= ~(3U << 2*6);
	GPIOA->MODER |=  (2<< 2*6) ;

	GPIOA->AFR[0]  &= ~(0xFU<<4*6);
	GPIOA->AFR[0]  |=  (1<<4*6);

	RCC->APBENR1 |= RCC_APBENR1_TIM3EN; // Timer 3 clock enable
	TIM3->CR1 = 0;
	TIM3->CR1 |= TIM_CR1_ARPE; // Auto-reload preload enable
	TIM3->CNT = 0; // zero out counter

	// tim uptade frequency = TIM_CLK/((TIM_PSC+1)*TIM_ARR) for 1 s interrupt
	TIM3->PSC = 10;
	TIM3->ARR = (16000);
	TIM3->DIER |= TIM_DIER_UIE; // Update interrupt enable
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE; // Output Compare 1 Preload Enable

	// PWM mode 1 is selected.
	TIM3->CCMR1 &= ~ (1u<<16); //0
	TIM3->CCMR1 |=(1u<<6); //1
	TIM3->CCMR1 |= (1u<<5); //1
	TIM3->CCMR1 &= ~(1u<<4);  // 0
	TIM3->CCER |= TIM_CCER_CC1E; // Capture compare ch1 enable
	TIM3->CCR1 = 0;

	// Update Generation: Re-initialize the counter and generates an update of the registers.
	TIM3->EGR |= TIM_EGR_UG;

	TIM3->CR1 |= TIM_CR1_CEN; // TIM3 Counter enable

	NVIC_SetPriority(TIM3_IRQn,3);  // Setting Priority for timer handler
	NVIC_EnableIRQ(TIM3_IRQn); // timer handler enable
}
