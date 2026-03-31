# @	Имя цели
# $<	Первая зависимость
# $^	Все зависимости
# $?	Зависимости новее цели	(если изменились)
# $*	Stem (часть до %)
# $(@D)	Директория цели
# $(<F)	Имя файла зависимости
# ==========================================
# Настройки проекта
# ==========================================
TARGET   = avr_main
MCU      = atmega328p
CC       = avr-gcc
F_CPU    = 16000000
SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin
INC_DIR  = include
DOC_DIR  = doc

# Поиск исходников
SRCS     = $(wildcard $(SRC_DIR)/*.c)
OBJS     = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS     = $(OBJS:.o=.d)

# ==========================================
# Утилиты
# ==========================================
OBJCOPY  = avr-objcopy
OBJDUMP  = avr-objdump
SIZE     = avr-size
CHECKER  = cppcheck
DOXYGEN  = doxygen
AVRDUDE  = avrdude

# ==========================================
# Настройки программатора
# ==========================================
# Для Arduino Uno/Nano
PROGRAMMER_TYPE = arduino
AUTO_PORT := $(shell ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null | head -n1)
ifeq ($(AUTO_PORT),)
    PROGRAMMER_PORT = /dev/ttyACM0
else
    PROGRAMMER_PORT = $(AUTO_PORT)
endif
PROGRAMMER_BAUD = 57600

# ==========================================
# Флаги компиляции и линковки
# ==========================================
# -mmcu=$(MCU) - целевой микроконтроллер
# -DF_CPU=$(F_CPU)UL - define для макросов задержки
# -Os - оптимизация по размеру (важно для МК!)
# -Wall -Wextra -Werror - все предупреждения = ошибки
# -ffunction-sections -fdata-sections - раздельная компиляция для линкера,
#					каждая функция в отдельной секции →
#						линкер может удалить неиспользуемые
# -funsigned-char -funsigned-bitfields - строгая типизация явная
# -	std=c89 / -std=c90 - классический ANSI C
# 	std=c99 - строгий C99 без расширений
# 	std=gnu99 - C99 с расширениями GNU
# 	std=c11 / -std=gnu11 - более новые стандарты
# -I$(INC_DIR) - путь к заголовкам
CFLAGS   = -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -Os \
           -Wall -Wextra -Werror \
           -ffunction-sections -fdata-sections \
           -funsigned-char -funsigned-bitfields \
           -fpack-struct -fshort-enums \
           -std=gnu99 \
           -I$(INC_DIR)

# -Wl,-Map,file.map - генерация карты памяти (анализ распределения памяти)
# -Wl,--gc-sections - удаление неиспользуемых секций (уменьшение размера)
LDFLAGS  = 	-mmcu=$(MCU) \
		-Wl,--gc-sections

# ==========================================
# Основные цели (Rules)
# ==========================================
.PHONY: all clean flash check doc size disasm fuses help

# all → hex → elf → объектные файлы → исходники
all: $(BIN_DIR)/$(TARGET).hex size

# Создание директорий
$(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# Генерация hex файла
# -O ihex - формат Intel HEX
# -R .eeprom - исключение секции EEPROM
$(BIN_DIR)/$(TARGET).hex: $(BIN_DIR)/$(TARGET).elf
	@echo "Creating HEX file..."
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@echo "HEX file created: $@"

# Линковка
$(BIN_DIR)/$(TARGET).elf: $(OBJS) | $(BIN_DIR)
	@echo "Linking..."
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	@echo "ELF created: $@"

# Компиляция с генерацией зависимостей
# % - wildcard (подстановка)
# $< - исходный файл (.c)
# -MMD: Создать файл зависимостей (исключая системные заголовки)
# -MP: Добавить phony-таргеты для заголовков
# -MF: Указать имя файла зависимостей
# -MT: Указать имя цели
# -c - только компиляция (без линковки)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Подключение файлов зависимостей (.d)
-include $(DEPS)

# ==========================================
# Программирование
# ==========================================
#     -p $(MCU) - тип микроконтроллера
#     -c $(PROGRAMMER_TYPE) - тип программатора
#     -P $(PORT) - порт подключения
#     -b $(BAUD) - скорость
#     -U flash:w:file.hex:i - операция записи
# Формат операции: -U <memtype>:r|w|v:<filename>[:format]
#     memtype - flash, eeprom, hfuse, lfuse, efuse
#     r - read, w - write, v 	- verify
#     :i - формат Intel HEX
flash: $(BIN_DIR)/$(TARGET).hex
	@echo "Programming $(MCU)..."
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER_TYPE) \
	  -P $(PROGRAMMER_PORT) -b $(PROGRAMMER_BAUD) \
	  -U flash:w:$<:i

# Для самодельных программаторов
isp:
	@echo "Programming via ISP..."
	$(AVRDUDE) -p $(MCU) -c usbasp \
	  -U flash:w:$(BIN_DIR)/$(TARGET).hex:i

# Установка фьюзов (пример для ATmega328P с внутренним RC 8MHz)
fuses:
	@echo "Setting fuses for $(MCU)..."
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER_TYPE) \
	  -P $(PROGRAMMER_PORT) -b $(PROGRAMMER_BAUD) \
	  -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m

# ==========================================
# Анализ и отладка
# ==========================================
size: $(BIN_DIR)/$(TARGET).elf
	@echo "Memory usage:"
	$(SIZE) --format=avr --mcu=$(MCU) $<

# -h - заголовки секций
# -S - перемешивание с исходным кодом (если есть отладочная информация)
disasm: $(BIN_DIR)/$(TARGET).elf
	@echo "Generating disassembly..."
	$(OBJDUMP) -h -S $< > $(BIN_DIR)/$(TARGET).lst
	@echo "Disassembly saved to $(BIN_DIR)/$(TARGET).lst"

# ==========================================
# Очистка
# ==========================================
clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DOC_DIR) *.map *.lst

# Полная очистка (включая бэкапы)
distclean: clean
	@find . -name "*~" -o -name "*.bak" -o -name "*.backup" | xargs rm -f

# ==========================================
# Статический анализ
# ==========================================
# --enable=all - все проверки
# --suppress=... - подавление ложных срабатываний
# -I - пути для включения заголовков
check:
	@echo "Running static analysis..."
	$(CHECKER) --enable=all \
	  --suppress=missingIncludeSystem \
	  -I$(INC_DIR) \
	  $(SRC_DIR)/*.c

# ==========================================
# Документация
# ==========================================
doc:
	@echo "Generating documentation..."
	@mkdir -p $(DOC_DIR)
	@if [ -f Doxyfile ]; then \
		$(DOXYGEN) Doxyfile; \
		if [ -d "$(DOC_DIR)/latex" ]; then \
			echo "Generating PDF..."; \
			$(MAKE) -C $(DOC_DIR)/latex >/dev/null 2>&1; \
			cp $(DOC_DIR)/latex/refman.pdf \
				$(DOC_DIR)/$(TARGET)_documentation.pdf; \
			echo "PDF created: $(DOC_DIR)/$(TARGET)_documentation.pdf"; \
		fi; \
	else \
		echo "Warning: Doxyfile not found. Using default config..."; \
		$(DOXYGEN) -g Doxyfile >/dev/null 2>&1; \
		sed -i 's/^PROJECT_NAME.*/PROJECT_NAME = "$(TARGET)"/' Doxyfile; \
		sed -i 's|^OUTPUT_DIRECTORY.*|OUTPUT_DIRECTORY = $(DOC_DIR)|' Doxyfile; \
		sed -i 's/^INPUT.*/INPUT = $(SRC_DIR) $(INC_DIR)/' Doxyfile; \
		$(DOXYGEN) Doxyfile; \
	fi

# ==========================================
# Справка
# ==========================================
help:
	@echo "AVR Bare-metal Makefile"
	@echo "========================"
	@echo "Available targets:"
	@echo "  all      : Build project (default)"
	@echo "  clean    : Remove build artifacts"
	@echo "  distclean: Clean + remove backups"
	@echo "  flash    : Program MCU via serial"
	@echo "  isp      : Program via ISP programmer"
	@echo "  fuses    : Set fuse bits (configure first!)"
	@echo "  check    : Static code analysis"
	@echo "  doc      : Generate documentation"
	@echo "  size     : Show memory usage"
	@echo "  disasm   : Generate disassembly listing"
	@echo "  help     : Show this help"
	@echo ""
	@echo "Configuration:"
	@echo "  MCU      : $(MCU)"
	@echo "  F_CPU    : $(F_CPU)"
	@echo "  SRC_DIR  : $(SRC_DIR)"
	@echo "  INC_DIR  : $(INC_DIR)"
