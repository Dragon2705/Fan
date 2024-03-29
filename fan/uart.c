#include "RccConfig.h"

#include "uart.h"

extern uint8_t RxBuffer[10];

void Uart2Config(void) {

        // 1. Enable the UART CLOCK and GPIO CLOCK
        RCC -> APB1ENR |= (1 << 17); // Enable UART2 CLOCK
        RCC -> AHB1ENR |= (1 << 0); // Enable GPIOA CLOCK

        // 2. Configure the UART PINs for ALternate Functions
        GPIOA -> MODER |= (2 << 4); // Bits (5:4)= 1:0 --> Alternate Function for Pin PA2
        GPIOA -> MODER |= (2 << 6); // Bits (7:6)= 1:0 --> Alternate Function for Pin PA3

        GPIOA -> OSPEEDR |= (3 << 4) | (3 << 6); // Bits (5:4)= 1:1 and Bits (7:6)= 1:1 --> High Speed for PIN PA2 and PA3

        GPIOA -> AFR[0] |= (7 << 8); // Bytes (11:10:9:8) = 0:1:1:1  --> AF7 Alternate function for USART2 at Pin PA2
        GPIOA -> AFR[0] |= (7 << 12); // Bytes (15:14:13:12) = 0:1:1:1  --> AF7 Alternate function for USART2 at Pin PA3

        // 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
        USART2 -> CR1 = 0x00; // clear all
        USART2 -> CR1 |= (1 << 13); // UE = 1... Enable USART
        USART2 -> CR1 |= (1 << 3); // TE=1.. Enable Transmitter
        USART2 -> CR1 |= (1 << 2);
        // 4. Program the M bit in USART_CR1 to define the word length.
        USART2 -> CR1 &= ~(1 << 12); // M =0; 8 bit word length
        USART2 -> CR1 &= ~(1 << 10);
        // 5. Select the desired baud rate using the USART_BRR register.
        USART2 -> BRR = (13 << 0) | (22 << 4); // Baud rate of 115200, PCLK1 at 42MHz

        // 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register
        USART2 -> CR1 |= (1 << 5); // Enable the Receiver Interrupt

        NVIC_EnableIRQ(USART2_IRQn);
}

void UART2_SendChar(uint8_t c)

{

        USART2 -> DR = c; // load the data into DR register
        while (!(USART2 -> SR & (1 << 6))); // Wait for TC to SET.. This indicates that the data has been transmitted
}

void UART2_SendString(char * string) {
        while ( * string) UART2_SendChar( * string++);
}

uint8_t UART2_GetChar(void) {
        uint8_t temp;

        while (!(USART2 -> SR & (1 << 5))); // wait for RXNE bit to set
        temp = USART2 -> DR; // Read the data. This clears the RXNE also
        return temp;
}