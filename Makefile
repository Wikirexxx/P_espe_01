# Definir nombre del proyecto
PRJ_NAME = P_Especialidad

# Definir herramientas de compilación
CC = arm-none-eabi-gcc
SIZE = arm-none-eabi-size
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

DEVICE_PATH := $(shell cygpath -u "C:\Users\franc\Documents\WS_VSCode\STM32F405ws\Chip_headers\Drivers\CMSIS\Device\ST\STM32F4xx\Include")
CMSIS_PATH := $(shell cygpath -u "C:\Users\franc\Documents\WS_VSCode\STM32F405ws\Chip_headers\Drivers\CMSIS\Include")

# Directorios
SRC_DIR = src
IFC_DIR = if_c
INC_DIR = Inc
IFH_DIR = if_h
STARTUP_DIR = startup
BUILD_DIR = build
OUTPUT_DIR = $(BUILD_DIR)/output

# Opciones de compilación
CFLAGS = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb \
         -std=gnu11 -I$(INC_DIR) -I$(IFH_DIR) -I$(CMSIS_PATH) -I$(DEVICE_PATH) \
         -g3 -Og -ffunction-sections -fdata-sections \
         -Wall -Wextra -Wunused -Wunused-variable -Wunused-but-set-variable

LDFLAGS = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb \
          -T startup/STM32F405RGTX_FLASH.ld \
          --specs=nosys.specs -Wl,-Map=$(BUILD_DIR)/$(PRJ_NAME).map -static \
          -Wl,--start-group -lc -lm -Wl,--end-group

# ======== Fuentes (incluye src/ y if_c/) ========
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(IFC_DIR)/*.c)

# Objetos en build/ manteniendo subcarpetas
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

STARTUP_SRC = $(STARTUP_DIR)/startup_stm32f405rgtx.s
STARTUP_OBJ = $(BUILD_DIR)/startup_stm32f405rgtx.o

ELF = $(BUILD_DIR)/$(PRJ_NAME).elf
HEX = $(OUTPUT_DIR)/$(PRJ_NAME).hex
BIN = $(OUTPUT_DIR)/$(PRJ_NAME).bin
LIST = $(BUILD_DIR)/$(PRJ_NAME).list
LST = $(BUILD_DIR)/$(PRJ_NAME).elf.lst

# Regla principal
all: dirs $(HEX) $(BIN) $(LIST) $(LST) size

# Crear carpetas necesarias
dirs:
	mkdir -p $(BUILD_DIR)/$(SRC_DIR) $(BUILD_DIR)/$(IFC_DIR) $(OUTPUT_DIR)

# Compilar C (sirve para src/*.c y if_c/*.c)
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar startup en ensamblador
$(STARTUP_OBJ): $(STARTUP_SRC) | dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Generar el ELF con símbolos de depuración
$(ELF): $(OBJS) $(STARTUP_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Generar el archivo .hex
$(HEX): $(ELF) | dirs
	$(OBJCOPY) -O ihex $< $@

# Generar el archivo .bin (para flasheo rápido)
$(BIN): $(ELF) | dirs
	$(OBJCOPY) -O binary $< $@

# Generar el archivo .list (desensamblado)
$(LIST): $(ELF)
	$(OBJDUMP) -h -S $< > $@

# Generar el archivo .elf.lst con información detallada
$(LST): $(ELF)
	$(OBJDUMP) -d -S $< > $@

# Mostrar tamaño del ejecutable
size: $(ELF)
	$(SIZE) $<

# Limpiar archivos generados
clean:
	rm -rf $(BUILD_DIR)