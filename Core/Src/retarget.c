/*
 * retarget.c
 *
 *  Created on: 12. tra 2026.
 *      Author: ANTONIA
 */

#include "usart.h"
#include <stdio.h>

#ifdef __GNUC__
	/* With GCC, small printf (option LD Linker->Libraries->Small printf set to
	 * 'Yes') calls __io_putchar() */
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}
