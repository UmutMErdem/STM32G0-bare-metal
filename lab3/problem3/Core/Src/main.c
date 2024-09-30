#include "stm32g0xx.h"

void clearSSD(void){ // Clear display
	GPIOB -> ODR |= (1U << 0); //PB0 -> A
	GPIOB -> ODR |= (1U << 1); //PB1 -> B
	GPIOB -> ODR |= (1U << 2); //PB2 -> C
	GPIOB -> ODR |= (1U << 3); //PB3 -> D
	GPIOB -> ODR |= (1U << 4); //PB4 -> E
	GPIOB -> ODR |= (1U << 5); //PB5 -> F
	GPIOB -> ODR |= (1U << 6); //PB6 -> G
}

void setSSD(int x){ // choose number we want and its leds are turned on.
	clearSSD();
	switch(x){
		case 0:
			GPIOB->ODR &= ~(0x3F); // A,B,C,D,E,F is on
			break;
		case 1:
			GPIOB->ODR &= ~(0x6); // B,C is on
			break;
		case 2:
			GPIOB->ODR &= ~(0x5B); // A,B,D,E,G is on
			break;
		case 3:
			GPIOB->ODR &= ~(0x4F); // A,B,C,D,G is on
			break;
		case 4:
			GPIOB->ODR &= ~(0x66); // B,C,F,G is on
			break;
		case 5:
			GPIOB->ODR &= ~(0x6D); // A,C,D,F,G is on
			break;
		case 6:
			GPIOB->ODR &= ~(0x7D); // A,C,D,E,F,G is on
			break;
		case 7:
			GPIOB->ODR &= ~(0x7); // A,B,C is on
			break;
		case 8:
			GPIOB->ODR &= ~(0x7F); // A,B,C,D,E,F,G is on
			break;
		case 9:
			GPIOB->ODR &= ~(0x6F); //A,B,C,D,F,G is on; E is off
			break;
	}
}

void counter(void){
	SetZero(); // leds show us 0000 value.
	delay(1000000);
	for(int i=0; i<=9999; i++){ // count up timer until 10000.
		int thousand, hundred, decimal, unit;
		thousand=(i/1000); // thousand digit of i
		hundred=((i-thousand*1000)/100); // hundred digit of i
		decimal=((i- thousand*1000 - hundred*100)/10); // decimal digit of i
		unit=(i- thousand*1000 - hundred*100 - decimal*10); // unit digit of i

		/* unit digit we want is set to 1 and the others are set to 0*/
		GPIOA ->ODR &= ~(1U << 7); // off D1 - PA7
		GPIOA ->ODR &= ~(1U << 6); // off D2 - PA6
		GPIOA ->ODR &= ~(1U << 0); // off D3 - PA0
		GPIOA ->ODR |= (1U << 5); // on D4 - PA5
		setSSD(unit);
		delay(300);

		/* decimal digit we want is set to 1 and the others are set to 0*/
		GPIOA ->ODR &= ~(1U << 7); // D1 - PA7
		GPIOA ->ODR &= ~(1U << 6); // D2 - PA7
		GPIOA ->ODR |= (1U << 0); // D3 - PA7
		GPIOA ->ODR &= ~(1U << 5);
		setSSD(decimal);
		delay(300);

		/* hundred digit we want is set to 1 and the others are set to 0*/
		GPIOA ->ODR &= ~(1U << 7); // D1 - PA7
		GPIOA ->ODR |= (1U << 6); // D2 - PA7
		GPIOA ->ODR &= ~(1U << 0); // D3 - PA7
		GPIOA ->ODR &= ~(1U << 5);
		setSSD(hundred);
		delay(300);

		/* thousand digit we want is set to 1 and the others are set to 0*/
		GPIOA ->ODR |= (1U << 7); // D1 - PA7
		GPIOA ->ODR &= ~(1U << 6); // D2 - PA7
		GPIOA ->ODR &= ~(1U << 0); // D3 - PA7
		GPIOA ->ODR &= ~(1U << 5);
		setSSD(thousand);
		delay(300);
	}

	GPIOB->ODR |= (1U << 8); // PB8 - D8 LED turn on
	delay(1000000);
	GPIOB->BRR |= (1U << 8); // led turn off
	SetZero();
	delay(1000000);
}
void ButtonInit(void){
	/* rising edge, selection register and mask register */
	// PA1 is button
	EXTI->RTSR1 |= (1U << 1); // Rising Trigger Selection Register
	EXTI->EXTICR[0] |= (0U << 8*1); // External Interrupt Configuration Register | for port selection
	EXTI->IMR1 |= (1U << 1); // Interrupt Mask Register

	/* enable NVIC and set interrupt priority */
	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void EXTI0_1_IRQHandler(void){ // EXTI for button
	counter(); // when button pressed, counter is started.
	EXTI->RPR1 |= (1U << 1); // Rising Pending Register
}

void GPIOA_Init(void){
	/* enable required GPIOA registers and RCC register */
	/*PA7 -> D1 digit, PA6 -> D2 digit, PA0 -> D3 digit, PA5 -> D4 digit,*/
	RCC->IOPENR |= (1U << 0);
	for(int k=0; k<9; k++){
		if (k==0 || k==1 || k==5 || k==6 || k==7 || k==8){
			GPIOA->MODER &= ~(3U << 2*k);
			GPIOA->MODER |= (1U << 2*k);
		}
	}
}

void GPIOB_Init(void){
	/* enable required GPIOB registers and RCC register */
	/*PB0-PB6 output pins are assigned from A to G respectively*/
	RCC->IOPENR |= (1U << 1);
	for(int k=0; k<9; k++){
		if (k==0 || k==1 || k==2 || k==3 || k==4 || k==5 || k==6 || k==8){
			GPIOB->MODER &= ~(3U << 2*k);
			GPIOB->MODER |= (1U << 2*k);
		}
	}
}

void SetZero(){
	GPIOA ->ODR |= (1U << 7); // D1 digit -> PA7
	GPIOA ->ODR |= (1U << 6); // D2 digit -> PA6
	GPIOA ->ODR |= (1U << 0); // D3 digit -> PA0
	GPIOA ->ODR |= (1U << 5); // D4 digit -> PA5
	setSSD(0);
}

void delay(uint32_t time) {
	for(; time>0; time--);
}

int main(void) {
	GPIOA_Init();
	GPIOB_Init();
	SetZero();
	ButtonInit();

	while(1) {
	}

	return 0;
}
