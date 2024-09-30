#include "stm32g0xx.h"

void GPIO_Config(void);
void USART2_Config(void);
void uart_tx(uint8_t c);
uint8_t uart_rx(void);
int _print(int f, char *ptr, int len);
void print(char *s);

int main(void){

	GPIO_Config();
	USART2_Config();

	while(1){
		uart_tx(uart_rx()); // received characters sent via console are printed via transmitter
	}

	return 0;
}

void printChar(uint8_t c){
	while(!(USART2->ISR & USART_ISR_TXE_TXFNF)); // when messages are sent.
	USART2->TDR = c; // Transmit data register is taken character to send a message.
}

int _print(int f, char *ptr, int len)
{
	/*in for loop, i of is increasing until equal to len
	 * and meanwhile, chars of 2nd parameter of _print function is writen
	 * into the printChar character by character increasing ptr of 2nd parameter
	 * of _print function  */
	for(volatile int i = f; i<len; i++){
		printChar(*ptr);
		ptr++;
	}
	return len; // return length
}

void print(char *s){
	int length = 0; // to count length of character
	/* i is pointer of string and length is increasing until i equals NULL character*/
	for(char *i = s; *i != NULL; i++) length++;
	_print(0, s, length);
}

uint8_t uart_rx(void){
	while(!( USART2->ISR & USART_ISR_RXNE_RXFNE)); // when messages are detected.
	return (uint8_t)USART2->RDR; // RDR[8:0]: Receive data value
}

void uart_tx(uint8_t c){
	printChar(c); // printing character by character
}

void GPIO_Config(void){
	// input-output A port clock enable
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	/* modes of GPIOA PA2 and PA3 pins are selected as alternate function.
	 * like that 0b1111_1010_1111;*/
	GPIOA->MODER &= ~((3U << 2*2) | (3U << 2*3));
	GPIOA->MODER |= (2U << 2*2) | (2U << 2*3);

	/* PA2 and PA3 pins used for USART2_TX and USART2_RX are selected
	 * with GPIOx_AFRL = AFRL_AFSELy(Alternate Function register -
	 * Alternate function selection for port x pin y)
	 * AF1 -->> USART2_RX, USART2_TX*/
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0;
}

void USART2_Config(void){
	RCC->APBENR1 |= RCC_APBENR1_USART2EN; // RCC APB peripherals clock enable for USART2
	USART2->CR1 = 0x00; // clear all
	USART2->CR1 |= USART_CR1_UE; // UE: USART enable

	/* Baud rate of 9600, PCLK1 at 16 MHz
	 * TX/RX baud rate = f_clk/(16*USARTDIV)
	 * 9600 = 16MHz/(16*USARTDIV) --->>> USARTDIV = 104.1666667
	 * IEEE754 floating-point --->>> mantissa = 104, fraction = 0.167*16 = 2.672 ≈ 3*/
	USART2->BRR |= (3 << 0) | (104 << 4);

	USART2->CR1 |= USART_CR1_RE; // RE: Receiver enable
	USART2->CR1 |= USART_CR1_TE; // TE: Transmitter enable
}
