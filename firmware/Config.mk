
# build setup

CPU_OPTS = -mcpu=cortex-m3 -mthumb
LIBSTM32_OPTS = -DUSB_PULLUP_PORT=GPIOA -DUSB_PULLUP_PIN=GPIO_Pin_8
AUX_OPTS = -Wall -Werror -g -O3
COMPILE_OPTS = $(CPU_OPTS) -fshort-wchar $(AUX_OPTS) $(LIBSTM32_OPTS)
INCLUDE_DIRS = -I.
LIBRARY_DIRS = -L.

ENV_BIN_PATH := /opt/arm-none-eabi-v3/bin:/opt/openocd/bin

export PATH := $(ENV_BIN_PATH):$(PATH)

CC = arm-none-eabi-gcc
CFLAGS = $(COMPILE_OPTS) $(INCLUDE_DIRS)

CXX = arm-none-eabi-g++
CXXFLAGS = $(COMPILE_OPTS) $(INCLUDE_DIRS)

AS = arm-none-eabi-gcc
ASFLAGS = $(COMPILE_OPTS) -c

LD = arm-none-eabi-gcc
LDFLAGS = $(CPU_OPTS) -Wl,--gc-sections,-Map=$@.map,-cref $(INCLUDE_DIRS) $(LIBRARY_DIRS)

OBJCP = arm-none-eabi-objcopy
OBJCPFLAGS = -O binary

AR = arm-none-eabi-ar
ARFLAGS = cr

GDB = arm-none-eabi-gdb

