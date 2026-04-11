# Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Target MCU configuration
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# Paths and Files
SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build
TARGET = $(BUILD_DIR)/mixer_firmware

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Compiler Flags
CFLAGS = $(MCU) -Wall -Wextra -g -O0 -I$(INC_DIR) -std=c11

# Build Rules
all: $(BUILD_DIR) $(TARGET).elf $(TARGET).bin

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files to objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Link objects into ELF
$(TARGET).elf: $(OBJECTS)
	$(CC) $(MCU) $^ -o $@ --specs=nosys.specs

# Generate raw binary
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Clean workspace
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean