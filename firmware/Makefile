# Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Target MCU configuration
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# Paths and Files
INC_DIR = inc
STARTUP_DIR = startup
LINKER_DIR = linker
BUILD_DIR = build
DRIVERS_DIR = drivers
APP_DIR = app

TARGET = $(BUILD_DIR)/mixer_firmware

SOURCES = $(wildcard $(APP_DIR)/*.c) $(wildcard $(DRIVERS_DIR)/*.c) $(wildcard $(STARTUP_DIR)/*.c)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

# VPATH tells make where to look for source files
VPATH = $(APP_DIR):$(DRIVERS_DIR):$(STARTUP_DIR)

# Compiler Flags
CFLAGS = $(MCU) -Wall -Wextra -g -O0 -I$(INC_DIR) -std=c11
LDFLAGS = -T $(LINKER_DIR)/stm32f411.ld -nostdlib -Wl,-Map=$(TARGET).map

# Build Rules
all: $(BUILD_DIR) $(TARGET).elf $(TARGET).bin size

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files to objects
$(BUILD_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Link objects into ELF
$(TARGET).elf: $(OBJECTS)
	$(CC) $(MCU) $(LDFLAGS) $^ -o $@

# Generate raw binary
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

size: $(TARGET).elf
	@echo "--- Memory Usage ---"
	@$(SIZE) -A -x $<

# Clean workspace
clean:
	rm -rf $(BUILD_DIR)

#ST-Link Flash
flash: $(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "reset_config connect_assert_srst" -c "program $< verify reset exit"

.PHONY: all clean flash size