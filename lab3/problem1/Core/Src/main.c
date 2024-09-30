#include "stm32g0xx.h"
#include "stm32g0xx_it.h"

uint32_t counter; // parameter called counter is defined as unsigned int 32 bits type.
#define LEDDELAY    1000 // 1 second

// presign of function
void led_toggle(void);
void SysInitial(void);
void delay_ms(uint32_t s);
void led_init(void);

int main(void) {

	SysInitial(); // system initial function is activated.

    while(1) {
    	led_toggle(); // led is toggled by using XOR.
        delay_ms(LEDDELAY); // delay for LEDDELAY miliseconds
    }

    return 0;
}

void SysInitial(void){
	SysTick_Config(SystemCoreClock/1000); // SysTick_Handler() function's time is defined in function parameter.
	led_init(); //led initilize
}

void SysTick_Handler(void)
{
  if(counter != 0){ // if counter is not 0.
	  counter--; // counter is decreasing once in each cycle.
  }
}

void delay_ms(uint32_t s) {
	counter = s;
	while(counter);
}

void led_init(void){
	/* Enable GPIOC clock */
	RCC->IOPENR |= (1U << 2);

	/* Setup PC6 as output */
	GPIOC->MODER &= ~(3U << 2*6);
	GPIOC->MODER |= (1U << 2*6);
}

void led_toggle(void){
	// Toggle LED
	GPIOC->ODR ^= (1U << 6);
}
