/*
 * This file is part of Betaflight.
 *
 * Betaflight is free software. You can redistribute this software
 * and/or modify this software under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 * version.
 *
 * Betaflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Common  UART hardware functions
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#if defined(USE_UART)

#include "build/build_config.h"

#include "drivers/nvic.h"
#include "drivers/rcc.h"
#include "drivers/inverter.h"
#include "drivers/serial.h"
#include "drivers/serial_impl.h"
#include "drivers/serial_uart.h"
#include "drivers/serial_uart_impl.h"
#include "drivers/dma.h"
#include "drivers/dma_reqmap.h"

#include "pg/serial_uart.h"

extern int sl_client_config_uart(uint8_t port_number, uint32_t speed);

USART_TypeDef hexagon_uart[3];

const uartHardware_t uartHardware[UARTDEV_COUNT] = {
    {
        .identifier = SERIAL_PORT_USART1,
        .reg = USART1,
        .txBuffer = uart1TxBuffer,
        .rxBuffer = uart1RxBuffer,
        .txBufferSize = sizeof(uart1TxBuffer),
        .rxBufferSize = sizeof(uart1RxBuffer),
    },
    {
        .identifier = SERIAL_PORT_USART2,
        .reg = USART2,
        .txBuffer = uart1TxBuffer,
        .rxBuffer = uart1RxBuffer,
        .txBufferSize = sizeof(uart1TxBuffer),
        .rxBufferSize = sizeof(uart1RxBuffer),
    },
    {
        .identifier = SERIAL_PORT_USART3,
        .reg = USART3,
        .txBuffer = uart1TxBuffer,
        .rxBuffer = uart1RxBuffer,
        .txBufferSize = sizeof(uart1TxBuffer),
        .rxBufferSize = sizeof(uart1RxBuffer),
	}
};

uartPort_t *serialUART(uartDevice_t *uartdev, uint32_t baudRate, portMode_e mode, portOptions_e options)
{
	(void)mode;
	(void)options;

    uartPort_t *s = &uartdev->port;

	// TODO: Use correct port
	int fd = sl_client_config_uart(2, baudRate);

	printf("====== In serialUART. baudRate %lu", baudRate);

	// TODO: Use correct hardware record index
    uartHardware[0].reg->fd = fd;

    s->USARTx = uartHardware[0].reg;

	return s;
}

void uartEnableTxInterrupt(uartPort_t *uartPort)
{
	(void) uartPort;
	printf("====== In uartEnableTxInterrupt");
}

#endif /* USE_UART */
