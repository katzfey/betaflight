#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define _PROVIDE_POSIX_TIME_DECLS 1
#include <time.h>

#include "pg/pg.h"
#include "drivers/serial.h"
#include "pg/bus_spi.h"
#include "drivers/system.h"

#include "config/feature.h"
#include "config/config.h"
#include "config/config_streamer.h"
#include "config/config_streamer_impl.h"
#include "config/config_eeprom_impl.h"

void *dmaDescriptors;

uint32_t SystemCoreClock;

const pgRegistry_t __pg_registry_end[1];
const pgRegistry_t __pg_registry_start[1];
const uint8_t __pg_resetdata_start[1];
const uint8_t __pg_resetdata_end[1];

char _estack;
char _Min_Stack_Size;

serialPinConfig_t serialPinConfig_System;
serialPinConfig_t serialPinConfig_Copy;

spiPinConfig_t spiPinConfig_SystemArray[0];

const mcuTypeInfo_t *getMcuTypeInfo(void)
{
    static const mcuTypeInfo_t info = { .id = MCU_TYPE_SIMULATOR, .name = "SIMULATOR" };
    return &info;
}

static struct timespec start_time;

uint64_t micros64(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return 1.0e6*((ts.tv_sec + (ts.tv_nsec*1.0e-9)) - (start_time.tv_sec + (start_time.tv_nsec*1.0e-9)));
}

uint64_t millis64(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return 1.0e3*((ts.tv_sec + (ts.tv_nsec*1.0e-9)) - (start_time.tv_sec + (start_time.tv_nsec*1.0e-9)));
}

uint32_t millis(void)
{
    return millis64() & 0xFFFFFFFF;
}

uint32_t getCycleCounter(void)
{
    return (uint32_t) (micros64() & 0xFFFFFFFF);
}

int32_t clockCyclesToMicros(int32_t clockCycles)
{
    return clockCycles;
}

float clockCyclesToMicrosf(int32_t clockCycles)
{
    return (float) clockCyclesToMicros(clockCycles);
}
// virtual EEPROM
static FILE *eepromFd = NULL;

bool loadEEPROMFromFile(void)
{
    if (eepromFd != NULL) {
        fprintf(stderr, "[FLASH_Unlock] eepromFd != NULL\n");
        return false;
    }

    // open or create
    eepromFd = fopen(EEPROM_FILENAME, "r+");
    if (eepromFd != NULL) {
        // obtain file size:
        fseek(eepromFd, 0, SEEK_END);
        size_t lSize = ftell(eepromFd);
        rewind(eepromFd);

        size_t n = fread(eepromData, 1, sizeof(eepromData), eepromFd);
        if (n == lSize) {
            printf("[FLASH_Unlock] loaded '%s', size = %d / %d\n", EEPROM_FILENAME, lSize, sizeof(eepromData));
        } else {
            fprintf(stderr, "[FLASH_Unlock] failed to load '%s'\n", EEPROM_FILENAME);
            return false;
        }
    } else {
        printf("[FLASH_Unlock] created '%s', size = %d\n", EEPROM_FILENAME, sizeof(eepromData));
        if ((eepromFd = fopen(EEPROM_FILENAME, "w+")) == NULL) {
            fprintf(stderr, "[FLASH_Unlock] failed to create '%s'\n", EEPROM_FILENAME);
            return false;
        }

        if (fwrite(eepromData, sizeof(eepromData), 1, eepromFd) != 1) {
            fprintf(stderr, "[FLASH_Unlock] write failed: %s\n", strerror(errno));
            return false;
        }
    }
    return true;
}

void configUnlock(void)
{
    loadEEPROMFromFile();
}

void configLock(void)
{
    // flush & close
    if (eepromFd != NULL) {
        fseek(eepromFd, 0, SEEK_SET);
        fwrite(eepromData, 1, sizeof(eepromData), eepromFd);
        fclose(eepromFd);
        eepromFd = NULL;
        printf("[FLASH_Lock] saved '%s'\n", EEPROM_FILENAME);
    } else {
        fprintf(stderr, "[FLASH_Lock] eeprom is not unlocked\n");
    }
}

configStreamerResult_e configWriteWord(uintptr_t address, config_streamer_buffer_type_t *buffer)
{
	// (void) address;
	// (void) buffer;
    STATIC_ASSERT(CONFIG_STREAMER_BUFFER_SIZE == sizeof(uint32_t), "CONFIG_STREAMER_BUFFER_SIZE does not match written size");
	
    if ((address >= (uintptr_t)eepromData) && (address + sizeof(uint32_t) <= (uintptr_t)ARRAYEND(eepromData))) {
        memcpy((void*)address, buffer, sizeof(config_streamer_buffer_type_t));
        printf("[FLASH_ProgramWord]%p = %08lx\n", (void*)address, *((uint32_t*)address));
    } else {
        printf("[FLASH_ProgramWord]%p out of range!\n", (void*)address);
    }
    return CONFIG_RESULT_SUCCESS;
}

int __wrap_printf ( const char * format, ... )
{
	(void) format;
	return 0;
}

void failureMode(failureMode_e mode)
{
    printf("[failureMode]!!! %d\n", mode);
    while (1);
}

void debugInit(void)
{
    printf("debugInit\n");
}

void unusedPinsInit(void)
{
    printf("unusedPinsInit\n");
}

void IOHi(IO_t io)
{
    UNUSED(io);
}

void IOLo(IO_t io)
{
    UNUSED(io);
}

void IOInitGlobal(void)
{
    // NOOP
}

IO_t IOGetByTag(ioTag_t tag)
{
    UNUSED(tag);
    return NULL;
}

