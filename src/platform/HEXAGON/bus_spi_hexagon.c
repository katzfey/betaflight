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

void spiInitDevice(SPIDevice device)
{
	(void) device;
}

// DMA transfer setup and start
void spiSequenceStart(const extDevice_t *dev)
{
	(void) dev;
}

void spiPinConfigure(const spiPinConfig_t *pConfig)
{
	(void) pConfig;
}

uint16_t spiCalculateDivider(uint32_t freq)
{
	(void) freq;
	return 1;
}
