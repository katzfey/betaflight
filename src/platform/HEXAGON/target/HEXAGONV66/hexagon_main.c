#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "platform.h"
// #include "io/serial.h"
// #include "common/printf_serial.h"

extern void HAP_debug(const char *msg, int level, const char *filename, int line);

#include "pg/pg.h"
#include "fc/tasks.h"

const pgRegistry_t __pg_registry_start[32];
const pgRegistry_t __pg_registry_end[32];
const uint8_t __pg_resetdata_start[32];
const uint8_t __pg_resetdata_end[32];

// void HAP_printf(const char *file, int line, const char *format, ...)
void HAP_printf(const char *format, ...)
{
	va_list ap;
	char buf[300];

	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	HAP_debug(buf, 0, "test.c", 1);
	//usleep(20000);
}

uint32_t micros(void) {
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);

	uint32_t result = (uint32_t)(tp.tv_sec) * 1000000;
	result += (uint32_t)(tp.tv_nsec / 1000);
	return result;
}

void init(void)
{
    HAP_printf("betaflight init");

// #if SERIAL_PORT_COUNT > 0
//     HAP_printf("printfSerialInit");
//     printfSerialInit();
// #endif

	return;
}

static FAST_DATA_ZERO_INIT bool gyroEnabled;

void scheduler(void)
{
	static int debug_print = 0;

	if (debug_print == 100000) {
    	HAP_printf("betaflight scheduler");
		debug_print = 0;
	}
	debug_print++;

	/* Start actual scheduler */
    static uint32_t checkCycles = 0;
    static uint32_t scheduleCount = 0;
    const timeUs_t schedulerStartTimeUs = micros();

    timeUs_t currentTimeUs;
    uint32_t nowCycles;
    timeUs_t taskExecutionTimeUs = 0;
    task_t *selectedTask = NULL;
    uint16_t selectedTaskDynamicPriority = 0;
    uint32_t nextTargetCycles = 0;
    int32_t schedLoopRemainingCycles;
    bool firstSchedulingOpportunity = false;

    if (gyroEnabled) {
        // Realtime gyro/filtering/PID tasks get complete priority
        task_t *gyroTask = getTask(TASK_GYRO);
	}

	/* End actual scheduler */

	qurt_timer_sleep(10);

	return;
}

#define __EXPORT __attribute__ ((visibility ("default")))

// Called by the SLPI LINK server to initialize and start AP
int slpi_link_client_init(void) __EXPORT;

// Called by the SLPI LINK server when there is a new message for AP
int slpi_link_client_receive(const uint8_t *data, int data_len_in_bytes) __EXPORT;

extern int betaflight_main(int argc, char * argv[]);

void *main_thread_trampoline(void *arg)
{
	betaflight_main(0, NULL);
    return NULL;
}

int slpi_link_client_init(void)
{
    HAP_printf("About to call betaflight_main %p", &betaflight_main);

    struct sched_param param = { .sched_priority = 128 };
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setschedparam(&attr, &param);

    const uint32_t stack_size = 4096U;
    pthread_attr_setstacksize(&attr, stack_size);

	pthread_t ctx = 0;
    pthread_create(&ctx, &attr, &main_thread_trampoline, NULL);
    pthread_attr_destroy(&attr);

    return 0;
}

int slpi_link_client_receive(const uint8_t *data, int data_len_in_bytes)
{
	(void) data;
	(void) data_len_in_bytes;

    // if (data_len_in_bytes < QURT_RPC_MSG_HEADER_LEN) {
    //     return 0;
    // }
    // const auto *msg = (struct qurt_rpc_msg *)data;
    // if (msg->data_length + QURT_RPC_MSG_HEADER_LEN != data_len_in_bytes) {
    //     return 0;
    // }
	// 
    // switch (msg->msg_id) {
    // case QURT_MSG_ID_MAVLINK_MSG: {
    //     if ((msg->inst < MAX_MAVLINK_INSTANCES) && (mav_cb[msg->inst])) {
    //         mav_cb[msg->inst](msg, mav_cb_ptr[msg->inst]);
    //     }
    //     break;
    // }
    // default:
    //     HAP_PRINTF("Got unknown message id %d", msg->msg_id);
    //     break;
    // }

    return 0;
}