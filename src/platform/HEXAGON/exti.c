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

#include "drivers/nvic.h"
#include "drivers/io_impl.h"
#include "drivers/exti.h"

void EXTIInit(void)
{

}


void EXTIHandlerInit(extiCallbackRec_t *cb, extiHandlerCallback *fn)
{
	(void) cb;
	(void) fn;
}

void EXTIConfig(IO_t io, extiCallbackRec_t *cb, int irqPriority, ioConfig_t config, extiTrigger_t trigger)
{
	(void) io;
	(void) cb;
	(void) irqPriority;
	(void) config;
	(void) trigger;
}

void EXTIRelease(IO_t io)
{
	(void) io;
}

void EXTIEnable(IO_t io)
{
	(void) io;
}

void EXTIDisable(IO_t io)
{
	(void) io;
}


