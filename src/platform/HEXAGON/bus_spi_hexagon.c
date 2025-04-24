/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"

#include "common/utils.h"
#include "common/maths.h"

#include "drivers/bus.h"
#include "drivers/bus_spi.h"
#include "drivers/bus_spi_impl.h"
#include "drivers/dma.h"
#include "drivers/io.h"
#include "drivers/rcc.h"
#include "pg/bus_spi.h"

SPI_TypeDef hexagon_spi_bus;

int sl_client_config_spi_bus(void);
int sl_client_spi_transfer(int fd, const uint8_t *send, uint8_t *recv, const unsigned len);

void spiInitDevice(SPIDevice device)
{
	printf("In spiInitDevice: %d", device);

	spiDevice[device].dev = &hexagon_spi_bus;

	hexagon_spi_bus.fd = sl_client_config_spi_bus();
}

// DMA transfer setup and start
void spiSequenceStart(const extDevice_t *dev)
{
	if (dev == NULL) {
		printf("**** NULL dev pointer");
		return;
	}

	busDevice_t *bus = dev->bus;

	if (bus == NULL) {
		printf("**** NULL bus pointer");
		return;
	}

	SPI_TypeDef *instance = bus->busType_u.spi.instance;

	if (instance == NULL) {
		printf("**** NULL instance pointer");
		return;
	}

	volatile struct busSegment_s *volatile curSegment = bus->curSegment;

	if (curSegment == NULL) {
		printf("**** NULL curSegment pointer");
		return;
	}

	if (curSegment[0].negateCS == true) {
		printf("**** Only a single record");
		return;
	}

	if (curSegment[0].len != 1) {
		printf("**** Register address is not length 1");
		return;
	}

	if (curSegment[1].negateCS != true) {
		printf("**** more than 2 records");
		return;
	}

	if (curSegment[1].len != 1) {
		printf("**** Data is not length 1");
		return;
	}

	// TODO: Add more checks

	uint8_t send[2];
	uint8_t recv[2];
	unsigned len = 2;
	bool is_read = false;

	// Send[0] has register address
	send[0] = *curSegment[0].u.buffers.txData;

	// A write transaction will have a non-NULL txData buffer
	// and a read transaction will have a non-NULL rxData buffer
	if (curSegment[1].u.buffers.txData) {
		recv[1] = *curSegment[1].u.buffers.txData;
		// printf("Writing 0x%0.2x to register 0x%0.2x", recv[1], send[0]);
	} else if (curSegment[1].u.buffers.rxData) {
		is_read = true;
	}

	int spi_fd = bus->busType_u.spi.instance->fd;

	sl_client_spi_transfer(spi_fd, &send[0], &recv[0], len);

	if (is_read) {
		curSegment[1].u.buffers.rxData[0] = recv[1];
		// printf("Read 0x%0.2x from register 0x%0.2x", recv[1], (send[0] & 0x7F));
	}

	// This marks the transaction as completed
	dev->bus->curSegment = (busSegment_t *)BUS_SPI_FREE;
}

void spiPinConfigure(const spiPinConfig_t *pConfig)
{
	// printf("In spiPinConfigure");
	(void) pConfig;
}

uint16_t spiCalculateDivider(uint32_t freq)
{
	// printf("In spiCalculateDivider");
	(void) freq;
	return 1;
}
