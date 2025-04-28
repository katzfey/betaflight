

#include <stdio.h>
#include "drivers/motor_impl.h"
#include "pg/motor.h"

#define HEXAGON_MAX_MOTORS 4

static bool motorEnabled[HEXAGON_MAX_MOTORS];
static float motorSpeed[HEXAGON_MAX_MOTORS];

static bool hexagonMotorEnabled(unsigned index) {
	printf("In hexagonMotorEnabled, index: %u", index);
	if (index < HEXAGON_MAX_MOTORS)	return motorEnabled[index];
	return false;
}

bool hexagonMotorEnable(void) {
	printf("In hexagonMotorEnable");
	for (int i = 0; i < HEXAGON_MAX_MOTORS; i++) {
		motorEnabled[i] = true;
	}
	return true;
}

void hexagonMotorDisable(void) {
	printf("In hexagonMotorDisable");
	for (int i = 0; i < HEXAGON_MAX_MOTORS; i++) {
		motorEnabled[i] = false;
	}
}

static uint32_t motorDebug;

void hexagonMotorWrite(uint8_t index, float value) {
	if (index < HEXAGON_MAX_MOTORS)	{
		motorSpeed[index] = value;
	}
}

void hexagonMotorUpdateComplete(void) {
	motorDebug++;
	if (motorDebug == 10000) {
		printf("Motor values: %f, %f, %f, %f", (double) motorSpeed[0],
				(double) motorSpeed[1], (double) motorSpeed[2], (double) motorSpeed[3]);
		motorDebug = 0;
	}
}

static const motorVTable_t vTable = {
    .postInit = NULL,
    .convertExternalToMotor = NULL,
    .convertMotorToExternal = NULL,
    .enable = hexagonMotorEnable,
    .disable = hexagonMotorDisable,
    .isMotorEnabled = hexagonMotorEnabled,
    .decodeTelemetry = NULL,
    .write = hexagonMotorWrite,
    .writeInt = NULL,
    .updateComplete = hexagonMotorUpdateComplete,
    .shutdown = NULL,
    .requestTelemetry = NULL,
    .isMotorIdle = NULL,
    .getMotorIO = NULL,
};

bool motorPwmDevInit(motorDevice_t *device, const motorDevConfig_t *motorConfig, uint16_t _idlePulse)
{
	(void) motorConfig;
	(void) _idlePulse;

    if (!device) {
        return false;
    }

    device->vTable = &vTable;
    // const uint8_t motorCount = device->count;
    printf("Initialized motor count %d\n", device->count);
    // pwmRawPkt.motorCount = motorCount;
	
    // idlePulse = _idlePulse;
	
    // for (int motorIndex = 0; motorIndex < MAX_SUPPORTED_MOTORS && motorIndex < motorCount; motorIndex++) {
    //     pwmMotors[motorIndex].enabled = true;
    // }

    return true;
}