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

USART_TypeDef hexagon_uart[NUM_HEXAGON_UART];

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
	},
    {
        .identifier = SERIAL_PORT_UART4,
        .reg = USART4,
        .txBuffer = uart1TxBuffer,
        .rxBuffer = uart1RxBuffer,
        .txBufferSize = sizeof(uart1TxBuffer),
        .rxBufferSize = sizeof(uart1RxBuffer),
	}
};

uint32_t hexagonSerialTotalRxWaiting(const serialPort_t *instance) {
	(void) instance;
	printf("In hexagonSerialTotalRxWaiting");
	return 0;
}

uint8_t hexagonSerialRead(serialPort_t *instance) {
	(void) instance;
	printf("In hexagonSerialRead");
	return 0;
}


static struct serialPortVTable hexagon_uart_vtable = {
    // void (*serialWrite)(serialPort_t *instance, uint8_t ch);
    // uint32_t (*serialTotalRxWaiting)(const serialPort_t *instance);
    // uint32_t (*serialTotalTxFree)(const serialPort_t *instance);
    // uint8_t (*serialRead)(serialPort_t *instance);
    // void (*serialSetBaudRate)(serialPort_t *instance, uint32_t baudRate);
    // bool (*isSerialTransmitBufferEmpty)(const serialPort_t *instance);
    // void (*setMode)(serialPort_t *instance, portMode_e mode);
    // void (*setCtrlLineStateCb)(serialPort_t *instance, void (*cb)(void *instance, uint16_t ctrlLineState), void *context);
    // void (*setBaudRateCb)(serialPort_t *instance, void (*cb)(serialPort_t *context, uint32_t baud), serialPort_t *context);
    // void (*writeBuf)(serialPort_t *instance, const void *data, int count);
    // void (*beginWrite)(serialPort_t *instance);
    // void (*endWrite)(serialPort_t *instance);
	.serialWrite = NULL,
	.serialTotalRxWaiting = hexagonSerialTotalRxWaiting,
	.serialTotalTxFree = NULL,
	.serialRead = hexagonSerialRead,
	.serialSetBaudRate = NULL,
	.isSerialTransmitBufferEmpty = NULL,
	.setMode = NULL,
	.setCtrlLineStateCb = NULL,
	.setBaudRateCb = NULL,
	.writeBuf = NULL,
	.beginWrite = NULL,
	.endWrite = NULL
};

uartPort_t *serialUART(uartDevice_t *uartdev, uint32_t baudRate, portMode_e mode, portOptions_e options)
{
	(void)mode;
	(void)options;

    uartPort_t *uart_port = &uartdev->port;

	serialPort_t serial_port = uart_port->port;

	serialPortIdentifier_e port_number = serial_port.identifier;

	uint8_t sl_port_number = 0;
	int hw_index = -1;
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
	case SERIAL_PORT_UART4:
		sl_port_number = 8;
		hw_index = 3;
		break;
	default:
		printf("ERROR: Invalid port identifier");
		break;
	}

	if ((hw_index > -1) && (hw_index < 4)) {
		if (hw_index == 3) {
			printf("Configuring MSP port");
		} else {
			int fd = sl_client_config_uart(sl_port_number, baudRate);

			printf("====== In serialUART. id %u port %u baudRate %lu", port_number, sl_port_number, baudRate);

		    uartHardware[hw_index].reg->fd = fd;
		}

	    uart_port->USARTx = uartHardware[hw_index].reg;
	} else {
		printf("ERROR: Invalid port number %u", port_number);
		return NULL;
	}

	uartdev->port.port.vTable = &hexagon_uart_vtable;

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

