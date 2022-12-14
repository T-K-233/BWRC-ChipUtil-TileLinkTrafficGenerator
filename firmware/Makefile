
TARGET ?= firmware

#################################
# RISCV Toolchain
#################################

# SiFive Toolchain
#PREFIX = riscv64-unknown-elf-

# Nuclei Toolchain
#PREFIX = riscv-nuclei-elf-

# xPack Toolchain
PREFIX = riscv-none-elf-


CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
DG = $(PREFIX)gdb
SIZE = $(PREFIX)size


#################################
# Working directories
#################################
VENDOR_ROOT = bsp/
USR_DIR = core/
SRC_DIR = $(USR_DIR)src/
INC_DIR = $(USR_DIR)inc/
BUILD_DIR = build/


#################################
# Source Files
#################################
A_SOURCES  = $(wildcard $(SRC_DIR)*.S) $(wildcard $(SRC_DIR)*/*.S)

C_SOURCES  = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)*/*.c)

A_SOURCES += $(USR_DIR)startup/startup.S

C_SOURCES += $(VENDOR_ROOT)oscibear/src/oscibear_hal.c
C_SOURCES += $(VENDOR_ROOT)oscibear/src/oscibear_hal_core.c
C_SOURCES += $(VENDOR_ROOT)oscibear/src/oscibear_hal_clint.c
C_SOURCES += $(VENDOR_ROOT)oscibear/src/oscibear_hal_gpio.c
C_SOURCES += $(VENDOR_ROOT)oscibear/src/oscibear_hal_plic.c
C_SOURCES += $(VENDOR_ROOT)oscibear/src/oscibear_hal_rcc.c
C_SOURCES += $(VENDOR_ROOT)oscibear/src/oscibear_hal_uart.c

INCLUDES  = -I$(INC_DIR)
INCLUDES += -I$(INC_DIR)hal/
INCLUDES += -I$(VENDOR_ROOT)oscibear/inc


#################################
# Object List
#################################
A_OBJECTS = $(addsuffix .o,$(addprefix $(BUILD_DIR),$(basename $(A_SOURCES))))
C_OBJECTS = $(addsuffix .o,$(addprefix $(BUILD_DIR),$(basename $(C_SOURCES))))

OBJECTS = $(A_OBJECTS) $(C_OBJECTS)

#################################
# Target Output Files
#################################
TARGET_ELF = $(BUILD_DIR)$(TARGET).elf
TARGET_BIN = $(BUILD_DIR)$(TARGET).bin
TARGET_HEX = $(BUILD_DIR)$(TARGET).hex
TARGET_VERILOG = $(BUILD_DIR)$(TARGET).out


#################################
# Flags
#################################

# MCU Settings
ARCH = rv32imafc_zicsr
ABI = ilp32f
CODEMODEL = medany
LD_SCRIPT = $(SRC_DIR)oscibear.ld

ARCHFLAGS = -march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CODEMODEL) -fno-pie
SPECFLAGS = --specs="nano.specs"

# compiler Flags
CFLAGS  = -g -std=gnu11 -O0 -Wall -Wextra -Warray-bounds -Wno-unused-parameter
CFLAGS += -fno-common -fno-builtin-printf
CFLAGS += $(ARCHFLAGS)
CFLAGS += $(SPECFLAGS)
CFLAGS += $(INCLUDES)

# linker Flags
LFLAGS  = -static
# LFLAGS += -nostartfiles -nostdlib
LFLAGS += -nostartfiles
LFLAGS += -T $(LD_SCRIPT)


#################################
# Build
#################################

# default target
all: $(TARGET_ELF)

$(TARGET_BIN): $(TARGET_ELF)
	$(CP) -O binary $< $@

$(TARGET_HEX): $(TARGET_ELF)
	$(CP) -O ihex $< $@

$(TARGET_VERILOG): $(TARGET_ELF)
	$(CP) -O verilog $< $@

$(TARGET_ELF): $(OBJECTS)
	@echo "[LD] linking $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@
	$(SIZE) $(TARGET_ELF)

$(A_OBJECTS): $(BUILD_DIR)%.o: %.S
	@echo "[CC] compiling $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

$(C_OBJECTS): $(BUILD_DIR)%.o: %.c
	@echo "[CC] compiling $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@


#################################
# Recipes
#################################

clean:
	@rm -rf $(BUILD_DIR)

bin: $(TARGET_BIN)

hex: $(TARGET_HEX)

verilog: $(TARGET_VERILOG)

# for debugging Makefile
list:
	@echo "A_SOURCES: $(A_SOURCES)"
	@echo "C_SOURCES: $(C_SOURCES)"
	@echo "A_OBJECTS: $(A_OBJECTS)"
	@echo "C_OBJECTS: $(C_OBJECTS)"