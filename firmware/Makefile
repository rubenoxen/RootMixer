CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

INC_DIR = inc
STARTUP_DIR = startup
LINKER_DIR = linker
BUILD_DIR = build
DRIVERS_DIR = drivers
APP_DIR = app
TUSB_DIR = lib/tinyusb

TARGET = $(BUILD_DIR)/mixer_firmware

SOURCES = $(wildcard $(APP_DIR)/*.c) $(wildcard $(DRIVERS_DIR)/*.c) $(wildcard $(STARTUP_DIR)/*.c)

SOURCES += \
	$(TUSB_DIR)/src/tusb.c \
	$(TUSB_DIR)/src/common/tusb_fifo.c \
	$(TUSB_DIR)/src/device/usbd.c \
	$(TUSB_DIR)/src/device/usbd_control.c \
	$(TUSB_DIR)/src/class/midi/midi_device.c \
	$(wildcard $(TUSB_DIR)/src/portable/synopsys/dwc2/*.c) \
	$(wildcard $(TUSB_DIR)/src/portable/st/synopsys/*.c)

SOURCES := $(filter-out %_os.c, $(SOURCES))

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

VPATH = $(APP_DIR):$(DRIVERS_DIR):$(STARTUP_DIR):$(TUSB_DIR)/src:$(TUSB_DIR)/src/common:$(TUSB_DIR)/src/device:$(TUSB_DIR)/src/class/midi:$(TUSB_DIR)/src/portable/synopsys/dwc2:$(TUSB_DIR)/src/portable/st/synopsys

CFLAGS = $(MCU) -Wall -Wextra -g -O0 -std=c11
CFLAGS += -I$(INC_DIR) -I$(TUSB_DIR)/src 
CFLAGS += -I$(TUSB_DIR)/hw/mcu/st/cmsis_device_f4/Include 
CFLAGS += -I$(TUSB_DIR)/hw/mcu/arm/CMSIS_5/CMSIS/Core/Include
CFLAGS += -I$(TUSB_DIR)/hw/mcu/arm/cmsis_5/CMSIS/Core/Include
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_STM32F4 -DBOARD_TUD_RHPORT=0 -DSTM32F411xE

LDFLAGS = -T $(LINKER_DIR)/stm32f411.ld -nostdlib -Wl,-Map=$(TARGET).map

all: $(BUILD_DIR) $(TARGET).elf $(TARGET).bin size

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET).elf: $(OBJECTS)
	$(CC) $(MCU) $(LDFLAGS) $^ -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

size: $(TARGET).elf
	@echo "--- Memory Usage ---"
	@$(SIZE) -A -x $<

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean size
