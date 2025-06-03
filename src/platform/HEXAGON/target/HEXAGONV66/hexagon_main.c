#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>
#include <pthread.h>

#include "platform.h"
// #include "io/serial.h"
// #include "common/printf_serial.h"

// Binary log file support
static const char dir_path[] = "/data/betaflight";
static const char logfile[] = "log.bin";
char full_log_path[128];

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

int __wrap_printf(const char * format, ...)
{
	va_list ap;
	char buf[300];

	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	HAP_debug(buf, 0, "hexagon_main.c", 1);
	return 0;
}

// Called by the SLPI LINK server to initialize and start AP
int slpi_link_client_init(void) __attribute__ ((visibility ("default")));



extern int betaflight_main(int argc, char * argv[]);

void *main_thread_trampoline(void *arg)
{
	(void) arg;
	betaflight_main(0, NULL);
    return NULL;
}

int slpi_link_client_init(void)
{
	// Setup the logging directory and the log file
	full_log_path[0] = 0;
	struct stat statbuf;
    if (stat(dir_path, &statbuf) == 0) {
		if (S_ISDIR(statbuf.st_mode)) {
			strcpy(full_log_path, dir_path);
			strcat(full_log_path, "/");
			strcat(full_log_path, logfile);
    		HAP_printf("Setting up log file %s", full_log_path);
		    FILE *fp = fopen(full_log_path, "wb"); // Open the file in binary write mode
		    if (fp == NULL) {
    			HAP_printf("Error opening the log file");
				full_log_path[0] = 0;
		    } else {
		    	fclose(fp); // Close the file
			}
		} else {
    		HAP_printf("ERROR: %s exists but is not a directory", dir_path);
		}
    }

    // HAP_printf("About to call betaflight_main");
    int betaflight_thread_priority = sched_get_priority_max(SCHED_FIFO) - 20;
    HAP_printf("Setting pthread priority to %d", betaflight_thread_priority);

    struct sched_param param = { .sched_priority = betaflight_thread_priority };
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
