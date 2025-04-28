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
 * Authors:
 * jflyper - Refactoring, cleanup and made pin-configurable
 * Dominic Clifton - Serial port abstraction, Separation of common STM32 code for cleanflight, various cleanups.
 * Hamasaki/Timecop - Initial baseflight code
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#include "build/build_config.h"
#include "build/atomic.h"

#include "common/utils.h"
#include "drivers/inverter.h"
#include "drivers/nvic.h"
#include "drivers/rcc.h"

#include "drivers/serial.h"
#include "drivers/serial_uart.h"
#include "drivers/serial_uart_impl.h"

extern int sl_client_register_uart_callback(int fd, serialReceiveCallbackPtr cb, void *arg);
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

    uartPort_t *uart_port = &uartdev->port;

	serialPort_t serial_port = uart_port->port;

	serialPortIdentifier_e port_number = serial_port.identifier;

	uint8_t sl_port_number = 0;
	int hw_index = 0;
	switch(port_number) {
	case SERIAL_PORT_USART1:
		sl_port_number = 2;
		hw_index = 0;
		break;
	case SERIAL_PORT_USART2:
		sl_port_number = 6;
		hw_index = 1;
		break;
	case SERIAL_PORT_USART3:
		sl_port_number = 7;
		hw_index = 2;
		break;
	default:
		printf("ERROR: Invalid port identifier");
		break;
	}

	int fd = sl_client_config_uart(sl_port_number, baudRate);

	printf("====== In serialUART. id %u port %u baudRate %lu", port_number, sl_port_number, baudRate);

    uartHardware[hw_index].reg->fd = fd;

    uart_port->USARTx = uartHardware[hw_index].reg;

	return uart_port;
}

void uartEnableTxInterrupt(uartPort_t *uartPort)
{
	int fd = uartPort->USARTx->fd;

	printf("====== In uartEnableTxInterrupt, fd %d", fd);
}

void uartReconfigure(uartPort_t *uartPort)
{
	int fd = uartPort->USARTx->fd;

	printf("====== In uartReconfigure, fd %d", fd);

    (void) sl_client_register_uart_callback(fd, uartPort->port.rxCallback, uartPort->port.rxCallbackData);
}

void uartIrqHandler(uartPort_t *s)
{
	int fd = s->USARTx->fd;

	printf("====== In uartIrqHandler, fd %d", fd);
}

