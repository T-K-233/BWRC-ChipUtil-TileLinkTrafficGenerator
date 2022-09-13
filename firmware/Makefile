# Path you your toolchain installation, leave empty if already in system PATH
TOOLCHAIN_ROOT =

# Path to the MCU codebase
VENDOR_ROOT = ./bsp/

###############################################################################

# Project specific
TARGET = firmware.elf
SRC_DIR = src/
INC_DIR = inc/

# Toolchain
CC = riscv64-unknown-elf-gcc
DG = riscv64-unknown-elf-gdb

# MCU Settings
#ARCH = rv32imac    rv32imafc
ARCH = rv32i
ABI = ilp32
CODEMODEL = medany
LD_SCRIPT = $(SRC_DIR)/oscibear.ld

ARCHFLAGS = -march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CODEMODEL) -fno-pie
SPECFLAGS = --specs="nano.specs"

# Compiler Flags
CFLAGS  = -g -std=gnu11 -O0 -Wall -Wextra -Warray-bounds -Wno-unused-parameter
CFLAGS += -fno-common -fno-builtin-printf
CFLAGS += $(ARCHFLAGS)
CFLAGS += $(SPECFLAGS)
CFLAGS += $(INCLUDES)

# Linker Flags
LFLAGS  = -static
LFLAGS += -nostartfiles -nostdlib
LFLAGS += -T $(LD_SCRIPT)

# Project sources
SRC_FILES = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)*/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)*.s) $(wildcard $(SRC_DIR)*/*.s)

# Project includes
INCLUDES   = -I$(INC_DIR)
INCLUDES  += -I$(INC_DIR)hal/


# Vendor sources: Note that files in "Templates" are normally copied into project for customization,
# but that is not necessary for this simple project.
# ASM_FILES += $(VENDOR_ROOT)Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f767xx.s
# SRC_FILES += $(VENDOR_ROOT)Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/BSP/STM32F7xx_Nucleo_144/stm32f7xx_nucleo_144.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
# SRC_FILES += $(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c


# Vendor includes
INCLUDES += -I$(VENDOR_ROOT)Drivers/CMSIS/Core/Include
INCLUDES += -I$(VENDOR_ROOT)Drivers/CMSIS/Device/ST/STM32F7xx/Include
INCLUDES += -I$(VENDOR_ROOT)Drivers/STM32F7xx_HAL_Driver/Inc
INCLUDES += -I$(VENDOR_ROOT)Drivers/BSP/STM32F7xx_Nucleo_144


###############################################################################

CXX_OBJS = $(SRC_FILES:.c=.o)
ASM_OBJS = $(ASM_FILES:.s=.o)
ALL_OBJS = $(ASM_OBJS) $(CXX_OBJS)

all: $(TARGET)

# Compile
$(CXX_OBJS): %.o: %.c
$(ASM_OBJS): %.o: %.s
$(ALL_OBJS):
	@echo "[CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
%.elf: $(ALL_OBJS)
	@echo "[LD] $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $(ALL_OBJS) -o $@

# Clean
clean:
	@rm -f $(ALL_OBJS) $(TARGET)

# Debug
gdb-server_rvlink:
	rv-util

gdb-server_openocd:
	openocd -f ./openocd.cfg

gdb-client: $(TARGET)
	$(DB) -tui $(TARGET)
