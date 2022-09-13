
TARGET ?= firmware


#################################
# RISCV SiFive Toolchain
#################################
CC = riscv64-unknown-elf-gcc
CXX = riscv64-unknown-elf-g++
CP = riscv64-unknown-elf-objcopy
OD = riscv64-unknown-elf-objdump
DG = riscv64-unknown-elf-gdb
SIZE = riscv64-unknown-elf-size


#################################
# Working directories
#################################
VENDOR_ROOT = bsp/
SRC_DIR = core/src/
INC_DIR = core/inc/
BUILD_DIR = build/


#################################
# Source Files
#################################
A_SOURCES  = $(wildcard $(SRC_DIR)*.S) $(wildcard $(SRC_DIR)*/*.S)

C_SOURCES  = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)*/*.c)
C_SOURCES += $(VENDOR_ROOT)oscibear/src/oscibear_hal.c
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
ARCH = rv32i
ABI = ilp32
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
LFLAGS += -nostartfiles -nostdlib
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