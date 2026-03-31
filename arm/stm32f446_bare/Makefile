# @	Имя цели
# $<	Первая зависимость
# $^	Все зависимости
# $?	Зависимости новее цели	(если изменились)
# $*	Stem (часть до %)
# $(@D)	Директория цели
# $(<F)	Имя файла зависимости

TARGET   = stm32f446
MCU      = STM32F446RET6
CC       = arm-none-eabi-gcc
OBJCOPY  = arm-none-eabi-objcopy
OBJDUMP  = arm-none-eabi-objdump
SIZE     = arm-none-eabi-size
OPENOCD  = openocd
GDB      = gdb-multiarch

SRC_DIR  = src
INC_DIR  = inc
OBJ_DIR  = obj
BIN_DIR  = bin

# Исходники проекта (убраны старые драйверы)
SRCS     = $(wildcard $(SRC_DIR)/*.c)
OBJS     = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
# Стартап-файл (лежит в SRC_DIR)
OBJS    += $(OBJ_DIR)/startup.o
DEPS     = $(OBJS:.o=.d)

# Флаги для Cortex-M4 с FPU
ARCH_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS   = $(ARCH_FLAGS) \
           -DSTM32F446xx \
           -I$(INC_DIR)

CFLAGS_DEBUG   = -g3 -Og -DDEBUG
CFLAGS_RELEASE = -O2 -DNDEBUG

LDFLAGS  = $(ARCH_FLAGS) \
           -TSTM32F446RETx_FLASH.ld \
           -Wl,--gc-sections \
           -Wl,--print-memory-usage \
           -Wl,-Map=$(BIN_DIR)/$(TARGET).map
LIBS     = -lc -lm -lnosys

.PHONY: all clean flash openocd debug size disasm help

all: $(BIN_DIR)/debug/$(TARGET).elf $(BIN_DIR)/debug/$(TARGET).hex $(BIN_DIR)/debug/$(TARGET).bin size

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR)/debug $(BIN_DIR)/release:
	mkdir -p $@

# Компиляция C-файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) -MMD -MP -c $< -o $@

# Ассемблирование стартап-файла
$(OBJ_DIR)/startup.o: $(SRC_DIR)/startup_stm32f446xx.s | $(OBJ_DIR)
	@echo "Assembling $<..."
	$(CC) $(ARCH_FLAGS) -c $< -o $@

# Линковка debug
$(BIN_DIR)/debug/$(TARGET).elf: $(OBJS) | $(BIN_DIR)/debug
	@echo "Linking $@..."
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@
	@echo "ELF created: $@"

# Создание HEX из ELF
$(BIN_DIR)/debug/$(TARGET).hex: $(BIN_DIR)/debug/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# Создание BIN из ELF (чистый бинарный образ)
$(BIN_DIR)/debug/$(TARGET).bin: $(BIN_DIR)/debug/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Релизная сборка
$(BIN_DIR)/release/$(TARGET).elf: CFLAGS += $(CFLAGS_RELEASE)
$(BIN_DIR)/release/$(TARGET).elf: $(OBJS) | $(BIN_DIR)/release
	@echo "Linking release $@..."
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(BIN_DIR)/release/$(TARGET).hex: $(BIN_DIR)/release/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(BIN_DIR)/release/$(TARGET).bin: $(BIN_DIR)/release/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

release: $(BIN_DIR)/release/$(TARGET).elf $(BIN_DIR)/release/$(TARGET).hex $(BIN_DIR)/release/$(TARGET).bin size

# Включение файлов зависимостей
-include $(DEPS)

size: $(BIN_DIR)/debug/$(TARGET).elf
	$(SIZE) $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(TARGET).map $(TARGET).lst

# Прошивка через OpenOCD (используем BIN для чистоты)
flash: $(BIN_DIR)/debug/$(TARGET).bin
	$(OPENOCD) -f openocd.cfg -c "program $< 0x08000000 verify reset exit"

# Ручной запуск OpenOCD в фоне с выводом в терминал
openocd:
	$(OPENOCD) -f openocd.cfg

# Автоматический запуск OpenOCD (в фоне) и подключение GDB
debug: $(BIN_DIR)/debug/$(TARGET).elf
	@echo "Starting OpenOCD in background..."
	@$(OPENOCD) -f openocd.cfg > /tmp/openocd.log 2>&1 &
	@echo "Waiting for OpenOCD to start (port 3333)..."
	@for i in 1 2 3 4 5 6 7 8 9 10; do \
		(netstat -tln 2>/dev/null | grep -q ':3333') && break || sleep 1; \
		if [ $$i -eq 10 ]; then \
			echo "ERROR: OpenOCD did not start. Check /tmp/openocd.log"; \
			exit 1; \
		fi; \
	done
	@echo "OpenOCD is ready. Launching GDB..."
	$(GDB) -ex "target remote localhost:3333" $<

disasm: $(BIN_DIR)/debug/$(TARGET).elf
	$(OBJDUMP) -h -S $< > $(BIN_DIR)/$(TARGET).lst

help:
	@echo "Available targets:"
	@echo "  all      : Build debug version (default)"
	@echo "  release  : Build release version"
	@echo "  clean    : Remove build artifacts"
	@echo "  flash    : Flash debug binary to MCU via OpenOCD"
	@echo "  openocd  : Start OpenOCD server (foreground, manual)"
	@echo "  debug    : Auto-start OpenOCD (background) and launch GDB"
	@echo "  size     : Show size of debug binary"
	@echo "  disasm   : Generate disassembly listing"
	@echo "  help     : Show this help"
