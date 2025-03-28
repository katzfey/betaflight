
HEXAGON_SDK_DIR = /opt/hexagon-sdk/4.1.0.4-lite
TOOLCHAIN_DIR = $(HEXAGON_SDK_DIR)/tools/HEXAGON_Tools/8.4.05/Tools

CC = $(TOOLCHAIN_DIR)/bin/hexagon-clang

CFLAGS = -MD -mv66 -fPIC -mcpu=hexagonv66 -G0 -fdata-sections -ffunction-sections
CFLAGS += -fomit-frame-pointer -fmerge-all-constants -fno-signed-zeros -fno-trapping-math
CFLAGS += -freciprocal-math -fno-math-errno -fno-strict-aliasing -fvisibility=hidden -fno-rtti -fmath-errno
CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

LINK = $(TOOLCHAIN_DIR)/bin/hexagon-link

LINKFLAGS = -march=hexagon -mcpu=hexagonv66 -shared -call_shared -G0
LINKFLAGS += $(TOOLCHAIN_DIR)/target/hexagon/lib/v66/G0/pic/initS.o
LINKFLAGS += -L$(TOOLCHAIN_DIR)/target/hexagon/lib/v66/G0/pic
LINKFLAGS += -Bsymbolic
LINKFLAGS += $(TOOLCHAIN_DIR)/target/hexagon/lib/v66/G0/pic/libgcc.a
LINKFLAGS += --wrap=malloc --wrap=calloc --wrap=free --wrap=realloc --wrap=printf
LINKFLAGS += --wrap=strdup --wrap=__stack_chk_fail -lc

#vpath %.c src/main \
#          src/main/io \
#          src/platform/HEXAGON \
#          src/platform/HEXAGON/target/HEXAGONV66

SRC = src/main/main.c \
      src/main/common/printf_serial.c \
      src/platform/HEXAGON/target/HEXAGONV66/hexagon_main.c
#      src/main/common/printf.c
#      src/main/io/serial.c

TARGET_BIN_DIR = bin
TARGET_OBJ_DIR = obj
TARGET_OBJS = $(addsuffix .o,$(addprefix $(TARGET_OBJ_DIR)/, $(basename $(SRC))))

$(info "Target objects: $(TARGET_OBJS)")

INCLUDE_DIRS = src/main \
               src/platform/HEXAGON \
               src/platform/HEXAGON/target/HEXAGONV66

INCLUDE_DIRS += $(HEXAGON_SDK_DIR)/rtos/qurt/computev66/include/qurt \
                $(HEXAGON_SDK_DIR)/rtos/qurt/computev66/include/posix

all: betaflight

$(TARGET_OBJ_DIR):
	mkdir -p obj

$(TARGET_BIN_DIR):
	mkdir -p bin

$(TARGET_OBJ_DIR)/%.o: %.c $(TARGET_OBJ_DIR) $(TARGET_BIN_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

betaflight: $(TARGET_OBJS)
	$(LINK) $(LINKFLAGS) -o $(TARGET_BIN_DIR)/$@ $^
	