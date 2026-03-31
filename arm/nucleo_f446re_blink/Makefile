# ==========================================
# Настройки проекта для STM32F446RE (NUCLEO)
# ==========================================
TARGET   = nucleo_f446re_blink
MCU      = cortex-m4
CPU      = cortex-m4
FPU      = fpv4-sp-d16
FLOAT_ABI = hard

# Директории
SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin
INC_DIR  = include

# Поиск исходников
SRCS     = $(wildcard $(SRC_DIR)/*.c) \
           $(wildcard $(SRC_DIR)/*.s)
OBJS     = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS)) \
           $(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(SRCS))
DEPS     = $(OBJS:.o=.d)

# ==========================================
# Утилиты
# ==========================================
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
OBJDUMP  = arm-none-eabi-objdump
SIZE     = arm-none-eabi-size
GDB      = arm-none-eabi-gdb
OPENOCD  = openocd

# ==========================================
# Настройки программатора
# ==========================================
OPENOCD_INTERFACE = stlink-v2-1
OPENOCD_TARGET    = stm32f4x
OPENOCD_SCRIPT    = board/st_nucleo_f4.cfg

# ==========================================
# Флаги компиляции и линковки
# ==========================================
CFLAGS   = -mcpu=$(CPU) -mthumb -mfpu=$(FPU) -mfloat-abi=$(FLOAT_ABI) \
           -DUSE_HAL_DRIVER -DSTM32F446xx \
           -Wall -Wextra -Werror \
           -ffunction-sections -fdata-sections \
           -fno-common -fstack-usage \
           -std=gnu11 \
           -I$(INC_DIR) \
           -I$(CMSIS_DIR) \
           -I$(HAL_DIR) \
           -I$(STM32F4_DIR)

# Оптимизация
ifeq ($(BUILD_MODE), release)
    CFLAGS += -O3 -flto
else
    CFLAGS += -O0 -g3 -gdwarf-2
endif

ASFLAGS  = -mcpu=$(CPU) -mthumb -mfpu=$(FPU) -mfloat-abi=$(FLOAT_ABI)

LDFLAGS  = -mcpu=$(CPU) -mthumb -mfpu=$(FPU) -mfloat-abi=$(FLOAT_ABI) \
           -specs=nano.specs -specs=nosys.specs \
           -T$(LINKER_SCRIPT) \
           -Wl,-Map=$(BIN_DIR)/$(TARGET).map \
           -Wl,--gc-sections \
           -Wl,--print-memory-usage

# ==========================================
# Пути к библиотекам STM32
# ==========================================
STM32_CUBE_PATH = $(HOME)/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1
CMSIS_DIR       = $(STM32_CUBE_PATH)/Drivers/CMSIS/Include
HAL_DIR         = $(STM32_CUBE_PATH)/Drivers/STM32F4xx_HAL_Driver/Inc
STM32F4_DIR     = $(STM32_CUBE_PATH)/Drivers/CMSIS/Device/ST/STM32F4xx/Include
STARTUP_DIR     = $(STM32_CUBE_PATH)/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc
LINKER_SCRIPT   = $(STM32_CUBE_PATH)/Projects/STM32F446RE-Nucleo/Templates/SW4STM32/STM32F446RE_Nucleo/STM32F446RETx_FLASH.ld

# ==========================================
# Основные цели
# ==========================================
.PHONY: all clean flash debug size disasm help

all: $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).bin $(BIN_DIR)/$(TARGET).hex size

# Создание директорий
$(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# Генерация файлов
$(BIN_DIR)/$(TARGET).bin: $(BIN_DIR)/$(TARGET).elf
	@echo "Creating BIN file..."
	$(OBJCOPY) -O binary $< $@

$(BIN_DIR)/$(TARGET).hex: $(BIN_DIR)/$(TARGET).elf
	@echo "Creating HEX file..."
	$(OBJCOPY) -O ihex $< $@

# Линковка
$(BIN_DIR)/$(TARGET).elf: $(OBJS) | $(BIN_DIR)
	@echo "Linking..."
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	@echo "ELF created: $@"

# Компиляция C файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Ассемблерные файлы
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s | $(OBJ_DIR)
	@echo "Assembling $<..."
	$(CC) $(ASFLAGS) -c $< -o $@

# Подключение файлов зависимостей
-include $(DEPS)

# ==========================================
# Программирование и отладка
# ==========================================
flash: $(BIN_DIR)/$(TARGET).bin
	@echo "Programming $(MCU) via OpenOCD..."
	$(OPENOCD) -f interface/$(OPENOCD_INTERFACE).cfg \
	           -f target/$(OPENOCD_TARGET).cfg \
	           -c "program $< verify reset exit"

openocd:
	@echo "Starting OpenOCD server..."
	$(OPENOCD) -f interface/$(OPENOCD_INTERFACE).cfg \
	           -f target/$(OPENOCD_TARGET).cfg

gdb: $(BIN_DIR)/$(TARGET).elf
	@echo "Starting GDB..."
	$(GDB) -ex "target extended-remote :3333" \
	       -ex "monitor reset halt" \
	       -ex "load" \
	       -ex "monitor reset halt" \
	       $<

debug: openocd gdb

# ==========================================
# Анализ
# ==========================================
size: $(BIN_DIR)/$(TARGET).elf
	@echo "Memory usage:"
	$(SIZE) $<

disasm: $(BIN_DIR)/$(TARGET).elf
	@echo "Generating disassembly..."
	$(OBJDUMP) -h -S $< > $(BIN_DIR)/$(TARGET).lst
	@echo "Disassembly saved to $(BIN_DIR)/$(TARGET).lst"

# ==========================================
# Очистка
# ==========================================
clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) *.map *.lst

# ==========================================
# Справка
# ==========================================
help:
	@echo "STM32F446RE NUCLEO Makefile"
	@echo "==========================="
	@echo "Available targets:"
	@echo "  all      : Build project (default)"
	@echo "  clean    : Remove build artifacts"
	@echo "  flash    : Program MCU via OpenOCD"
	@echo "  openocd  : Start OpenOCD server"
	@echo "  gdb      : Start GDB and connect to OpenOCD"
	@echo "  debug    : Start OpenOCD + GDB for debugging"
	@echo "  size     : Show memory usage"
	@echo "  disasm   : Generate disassembly listing"
	@echo ""
	@echo "Configuration:"
	@echo "  MCU      : $(MCU)"
	@echo "  BUILD_MODE: $(or $(BUILD_MODE),debug)"
	@echo "  SRC_DIR  : $(SRC_DIR)"
	@echo "  INC_DIR  : $(INC_DIR)"
