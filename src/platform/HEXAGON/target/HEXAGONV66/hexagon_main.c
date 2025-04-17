#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "platform.h"
// #include "io/serial.h"
// #include "common/printf_serial.h"

extern void HAP_debug(const char *msg, int level, const char *filename, int line);

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

#define __EXPORT __attribute__ ((visibility ("default")))

// Called by the SLPI LINK server to initialize and start AP
int slpi_link_client_init(void) __EXPORT;

// Called by the SLPI LINK server when there is a new message for AP
int slpi_link_client_receive(const uint8_t *data, int data_len_in_bytes) __EXPORT;

// extern int betaflight_main(int argc, char * argv[]);

void *main_thread_trampoline(void *arg)
{
	(void) arg;
	// betaflight_main(0, NULL);
    return NULL;
}

int slpi_link_client_init(void)
{
    HAP_printf("About to call betaflight_main");
    // HAP_printf("About to call betaflight_main %p", &betaflight_main);

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