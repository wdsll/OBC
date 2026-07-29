TARGET := obc_1p5kw_initial
BUILD_DIR ?= build/gd32f303

PREFIX ?= arm-none-eabi-
CC := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size

CPU_FLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS := $(CPU_FLAGS) -std=c11 -Os -g3 \
	-ffunction-sections -fdata-sections \
	-Wall -Wextra -Werror \
	-Iinclude -Itarget/gd32f303/baremetal
LDFLAGS := $(CPU_FLAGS) \
	-Ttarget/gd32f303/baremetal/gd32f303re.ld \
	-Wl,--gc-sections \
	-Wl,-Map,$(BUILD_DIR)/$(TARGET).map \
	--specs=nano.specs --specs=nosys.specs

SOURCES := \
	src/main.c \
	src/app.c \
	src/can_service.c \
	src/safety_manager.c \
	target/gd32f303/baremetal/board_baremetal.c \
	target/gd32f303/baremetal/startup_gd32f303.c \
	target/gd32f303/baremetal/system_gd32f303.c

OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean size

all: \
	$(BUILD_DIR)/$(TARGET).elf \
	$(BUILD_DIR)/$(TARGET).hex \
	$(BUILD_DIR)/$(TARGET).bin \
	size

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $<

clean:
	$(RM) -r $(BUILD_DIR)
