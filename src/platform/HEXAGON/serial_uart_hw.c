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

// TODO: split this function into mcu-specific UART files ?
// static void enableRxIrq(const uartHardware_t *hardware)
// {
// #if defined(USE_HAL_DRIVER)
//         HAL_NVIC_SetPriority(hardware->irqn, NVIC_PRIORITY_BASE(hardware->rxPriority), NVIC_PRIORITY_SUB(hardware->rxPriority));
//         HAL_NVIC_EnableIRQ(hardware->irqn);
// #elif defined(STM32F4)
//         NVIC_InitTypeDef NVIC_InitStructure;
//         NVIC_InitStructure.NVIC_IRQChannel = hardware->irqn;
//         NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PRIORITY_BASE(hardware->rxPriority);
//         NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_PRIORITY_SUB(hardware->rxPriority);
//         NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//         NVIC_Init(&NVIC_InitStructure);
// #elif defined(AT32F4)
//         nvic_irq_enable(hardware->irqn, NVIC_PRIORITY_BASE(hardware->rxPriority), NVIC_PRIORITY_SUB(hardware->rxPriority));
// #elif defined(APM32F4)
//         DAL_NVIC_SetPriority(hardware->irqn, NVIC_PRIORITY_BASE(hardware->rxPriority), NVIC_PRIORITY_SUB(hardware->rxPriority));
//         DAL_NVIC_EnableIRQ(hardware->irqn);
// #else
// # error "Unhandled MCU type"
// #endif
// }

uartPort_t *serialUART(uartDevice_t *uartdev, uint32_t baudRate, portMode_e mode, portOptions_e options)
{
	(void)uartdev;
	(void)baudRate;
	(void)mode;
	(void)options;
	return NULL;
}

void uartEnableTxInterrupt(uartPort_t *uartPort)
{
	(void) uartPort;
}

#endif /* USE_UART */
