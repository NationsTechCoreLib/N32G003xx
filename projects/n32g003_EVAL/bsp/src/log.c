/**
*     Copyright (c) 2022, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file log.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "log.h"

#if LOG_ENABLE

#include "n32g003.h"
#include "n32g003_gpio.h"
#include "n32g003_uart.h"
#include "n32g003_rcc.h"

#define LOG_UARTx       UART1
#define LOG_PERIPH      RCC_APB_PERIPH_UART1
#define LOG_GPIO        GPIOA
#define LOG_PERIPH_GPIO RCC_APB_PERIPH_IOPA
#define LOG_TX_PIN      GPIO_PIN_2
#define LOG_TX_AF       GPIO_AF5_UART1
#define LOG_RX_PIN      GPIO_PIN_3
#define LOG_RX_AF       GPIO_AF5_UART1

void log_init(void)
{
	GPIO_InitType GPIO_InitStructure;
	UART_InitType UART_InitStructure;
    
    RCC_APB_Peripheral_Clock_Enable(LOG_PERIPH | RCC_APB_PERIPH_AFIO | LOG_PERIPH_GPIO);

	GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = LOG_TX_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = LOG_TX_AF;
    GPIO_Peripheral_Initialize(LOG_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.Pin             = LOG_RX_PIN;
    GPIO_InitStructure.GPIO_Alternate  = LOG_RX_AF;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);

    UART_InitStructure.BaudRate        = 115200;
    UART_InitStructure.WordLength      = UART_WL_8B;
    UART_InitStructure.StopBits        = UART_STPB_1;
    UART_InitStructure.Parity          = UART_PE_NO;
    UART_InitStructure.Mode            = UART_MODE_RX | UART_MODE_TX;
    UART_Initializes(LOG_UARTx, &UART_InitStructure);

    /* Enable uart1 */
    UART_Enable(LOG_UARTx);
}

static int is_lr_sent = 0;

int fputc(int ch, FILE* f)
{
    if (ch == '\r')
    {
        is_lr_sent = 1;
    }
    else if (ch == '\n')
    {
        if (!is_lr_sent)
        {
            
            /* Loop until the end of transmission */
            while (UART_Flag_Status_Get(LOG_UARTx, UART_FLAG_TXC) == RESET)
            {
            }
						UART_Data_Send(LOG_UARTx, (uint8_t)'\r');
        }
        is_lr_sent = 0;
    }
    else
    {
        is_lr_sent = 0;
    }
    
    /* Loop until the end of transmission */
    while (UART_Flag_Status_Get(LOG_UARTx, UART_FLAG_TXC) == RESET)
    {
    }
		UART_Data_Send(LOG_UARTx, (uint8_t)ch);
    return ch;
}

#endif // LOG_ENABLE
