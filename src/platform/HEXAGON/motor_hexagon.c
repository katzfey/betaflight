

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "drivers/motor_impl.h"
#include "pg/motor.h"
#include "sl_client.h"

#define HEXAGON_MAX_MOTORS 4

#define ESC_BAUDRATE 921600
// #define ESC_BAUDRATE 2000000

// ESC specific definitions
#define ESC_PACKET_TYPE_PWM_CMD 1
#define ESC_PACKET_TYPE_FB_RESPONSE 128
#define ESC_PACKET_TYPE_FB_POWER_STATUS 132

// IO board specific definitions
#define IO_PACKET_TYPE_PWM_HIRES_CMD 6

// Generic definitions
#define PKT_HEADER 0xAF
#define PACKET_TYPE_VERSION_EXT_REQUEST 24
#define PACKET_TYPE_VERSION_EXT_RESPONSE 131

static bool motorEnabled[HEXAGON_MAX_MOTORS];
static float motorSpeed[HEXAGON_MAX_MOTORS];
static int motor_fd = -1;

// TODO: For requesting feedback
// static uint8_t last_fb_idx;
// static uint32_t last_fb_req_ms;

// Calculate Modbus CRC16 for array of bytes
uint16_t calc_crc_modbus(const uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t pos = 0; pos < len; pos++) {
        crc ^= (uint16_t) buf[pos]; // XOR byte into least sig. byte of crc
        for (uint8_t i = 8; i != 0; i--) { // Loop over each bit
            if ((crc & 0x0001) != 0) { // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            } else {
                // Else LSB is not set
                crc >>= 1; // Just shift right
            }
        }
    }
    return crc;
}

// Send a packet with CRC to the ESC or IO board
static void send_packet(uint8_t type, uint8_t *data, uint16_t size)
{
    uint16_t packet_size = size + 5;
    uint8_t out[packet_size];

    out[0] = PKT_HEADER;
    out[1] = packet_size;
    out[2] = type;

    memcpy(&out[3], data, size);

    uint16_t crc = calc_crc_modbus(&out[1], packet_size - 3);

    memcpy(&out[packet_size - 2], &crc, sizeof(uint16_t));

    sl_client_uart_write(motor_fd, (const char *)out, packet_size);
}

// Convert 1000 to 2000 PWM to 0 to 800 for ModalAI ESCs
static int16_t pwm_to_esc(uint16_t pwm)
{
    float p = (pwm-1000)*0.001;
	if (p < 0.0f) p = 0.0f;
	else if (p > 1.0f) p = 1.0f;

    return (int16_t)(800.0f * p);
}

static void send_esc_command(void)
{
    int16_t data[5];

    // We don't set any LEDs
    data[4] = 0;

    for (uint8_t i = 0; i < HEXAGON_MAX_MOTORS; i++) {
        data[i] = pwm_to_esc(motorSpeed[i]);

        // Make sure feedback request bit is cleared for all ESCs
        data[i] &= 0xFFFE;
    }

	// TODO: Add in feedback request for battery status and RPM
    // const uint32_t now_ms = millis();
    // if (now_ms - last_fb_req_ms > 5) {
    //     last_fb_req_ms = now_ms;
    //     // request feedback from one ESC
    //     last_fb_idx = (last_fb_idx+1) % 4;
    //     data[last_fb_idx] |= 1;
    // }

    send_packet(ESC_PACKET_TYPE_PWM_CMD, (uint8_t *)data, sizeof(data));

    // check_response();
}

bool hexagonMotorEnabled(unsigned index) {
	// printf("In hexagonMotorEnabled, index: %u", index);
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
		if (motorEnabled[index]) {
			motorSpeed[index] = value;
		}
	}
}

void hexagonMotorUpdateComplete(void) {
	motorDebug++;
	// if (motorDebug == 10000) {
	if (motorDebug == 4000) {
		printf("Motor values: %f, %f, %f, %f", (double) motorSpeed[0],
				(double) motorSpeed[1], (double) motorSpeed[2], (double) motorSpeed[3]);
		motorDebug = 0;
	}

	send_esc_command();
}

float hexagonConvertExternalToMotor(uint16_t externalValue)
{
	float motor_range = (float) (externalValue - 1000);
	motor_range /= 1000.0f;
	// printf(">>>>>>>>>>>>>>> External convert from %u to %f", externalValue, (double) motor_range);
	return motor_range;
}


uint16_t hexagonConvertMotorToExternal(float motorValue)
{
	uint16_t motor_pwm = (uint16_t) ((motorValue * 1000.0f) + 1000.0f);
	// printf(">>>>>>>>>>>>>>> Motor convert from %f to %u", (double) motorValue, motor_pwm);
	return motor_pwm;
}

static const motorVTable_t vTable = {
    .postInit = NULL,
    .convertExternalToMotor = hexagonConvertExternalToMotor,
    .convertMotorToExternal = hexagonConvertMotorToExternal,
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

	motor_fd = sl_client_config_uart(2, ESC_BAUDRATE);

	if (motor_fd == -1) {
		printf("ERROR: Failed to open ESC serial port");
		return false;
	}

	// TODO: Handshake with ESC, check for IO board

	printf("Initialized motor count %d", device->count);

	return true;
}