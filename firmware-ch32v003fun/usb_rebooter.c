#include "ch32v003fun.h"
#include "ch32v003_GPIO_branchless.h"
#include "usb_rebooter.h"
#include <stdio.h>

void init_rcc(void)
{
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO;
}

void setup_uart()
{
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO;

    GPIOD->CFGLR &= ~(0xf << (4 * 6));
    GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 6);

    GPIOD->CFGLR &= ~(0xf << (4 * 5));
    GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 5);

    // PD6 as TX
    AFIO->PCFR1 &= ~(1 << 2 | 1 << 21);
    AFIO->PCFR1 |= 1 << 21;

    USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx;
    USART1->CTLR2 = USART_StopBits_1;
    USART1->CTLR3 = USART_HardwareFlowControl_None;

    USART1->BRR = UART_BRR;
    USART1->CTLR1 |= CTLR1_UE_Set;
}

#define EN_PIN GPIOv_from_PORT_PIN(GPIO_port_C, 2)
#define FLAG_PIN GPIOv_from_PORT_PIN(GPIO_port_C, 1)
#define LED_PIN GPIOv_from_PORT_PIN(GPIO_port_A, 2)

void setup_gpio()
{
    GPIO_port_enable(GPIO_port_A);
    GPIO_port_enable(GPIO_port_C);
    GPIO_port_enable(GPIO_port_D);
    GPIO_pinMode(FLAG_PIN, GPIO_pinMode_I_pullUp, GPIO_Speed_10MHz);
    GPIO_pinMode(EN_PIN, GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
    GPIO_pinMode(LED_PIN, GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
}

#define LOOP_MS 1000

#define ACTIVE_SEC 12 * 60 * 60
// #define ACTIVE_SEC 1
#define INACTIVE_SEC 1

int main()
{
    SystemInit();
    init_rcc();
    setup_uart();
    printf("start\n\r");

    setup_gpio();

    while (1)
    {
        printf("active\n\r");
        GPIO_digitalWrite_0(EN_PIN);
        GPIO_digitalWrite_1(LED_PIN);

        for (int i = 0; i < ACTIVE_SEC; i++)
        {
            printf("%d/%d\n\r", i, ACTIVE_SEC);
            Delay_Ms(LOOP_MS);
        }

        printf("unactive\n\r");
        GPIO_digitalWrite_1(EN_PIN);
        GPIO_digitalWrite_0(LED_PIN);

        for (int i = 0; i < INACTIVE_SEC; i++)
            Delay_Ms(LOOP_MS);
    }
}
