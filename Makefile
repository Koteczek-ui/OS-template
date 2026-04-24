# directories config
BIN_DIR = bin
TMP_DIR = $(BIN_DIR)/tmp
SRC_DIR = src
C_CODE_DIR = $(SRC_DIR)/c
ASM_CODE_DIR = $(SRC_DIR)/asm

# names config
IMG_NAME = $(BIN_DIR)/os-img.bin
HDD_NAME = $(BIN_DIR)/harddrive.img
KERNEL_BIN = $(BIN_DIR)/kernel.bin
BOOT_NAME = $(ASM_CODE_DIR)/boot.asm
INTERUPPTS_NAME = $(ASM_CODE_DIR)/interrupts.asm

# operating system for tools
OS_TYPE := $(shell uname -s 2>/dev/null || echo Windows_NT)

ifeq ($(OS_TYPE),Windows_NT)
    MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
    RM = rmdir /s /q
else
    MKDIR = mkdir -p $(1)
    RM = rm -rf
endif

ifneq ($(filter clean,$(MAKECMDGOALS)),)
    ifeq ($(OS_TYPE),Windows_NT)
        CLEAN_INFO = @echo "--- Cleaning Windows build files ---"
    else
        CLEAN_INFO = @echo "--- Cleaning Linux build files ---"
    endif
endif

# sources & flags config
C_SOURCES = $(shell find $(C_CODE_DIR) -name "*.c")
INC_FLAGS = -I$(SRC_DIR)

# objects config
OBJECTS = $(patsubst $(C_CODE_DIR)/%.c,$(TMP_DIR)/c/%.o,$(C_SOURCES))
ASM_INTERRUPTS_OBJ = $(TMP_DIR)/asm/interrupts.o

all: check_tools build_dirs $(HDD_NAME) run

# tools checking
check_tools:
	@echo "--- Checking dependencies ---"
	@for tool in nasm gcc ld qemu-system-i386; do \
		if ! command -v $$tool > /dev/null 2>&1; then \
			echo "ERROR: $$tool is not installed."; \
			if [ "$(OS_TYPE)" = "Windows_NT" ]; then \
				echo "Please install $$tool manually on Windows."; exit 1; \
			else \
				sudo apt install python3-$$tool; \
			fi; \
		fi; \
	done

# compilation
# create directories
build_dirs:
	@$(call MKDIR,$(BIN_DIR))
	@$(call MKDIR,$(TMP_DIR))
	@$(call MKDIR,$(TMP_DIR)/c)
	@$(call MKDIR,$(TMP_DIR)/asm)

# compile C files
$(TMP_DIR)/c/%.o: $(C_CODE_DIR)/%.c
	@$(call MKDIR,$(dir $@))
	@echo "Compiling: $<"
	gcc -ffreestanding -m32 -fno-pie -fno-stack-protector -c $< -o $@ $(INC_FLAGS) || (echo "GCC failed on $< with Error Level $$?"; exit 1)

# interrupts.asm compiling
$(ASM_INTERRUPTS_OBJ): $(INTERUPPTS_NAME)
	@$(call MKDIR,$(dir $@))
	@echo "--- Compiling ASM Interrupts ---"
	nasm -f elf32 $< -o $@ || (echo "NASM failed on $<"; exit 1)

# kernel linking - checks if objects in TMP_DIR exist
$(KERNEL_BIN): $(OBJECTS) $(ASM_INTERRUPTS_OBJ)
	@echo "--- Linking Kernel ---"
	ld -m elf_i386 -T linker.ld $^ -o $@ || (echo "Linker failed"; exit 1)

# bootloader compiling
$(BIN_DIR)/boot.bin: $(BOOT_NAME)
	@echo "--- Compiling Bootloader ---"
	nasm -f bin $< -o $@ || (echo "NASM failed"; exit 1)

# updating hard drive image without wiping data sectors
$(HDD_NAME): $(BIN_DIR)/boot.bin $(KERNEL_BIN)
	@echo "--- Updating harddrive image ---"
	@if [ ! -f $(HDD_NAME) ]; then \
		echo "Creating new 512MB image..."; \
		dd if=/dev/zero of=$(HDD_NAME) bs=1M count=512; \
	fi
	@dd if=$(BIN_DIR)/boot.bin of=$(HDD_NAME) conv=notrunc
	@dd if=$(KERNEL_BIN) of=$(HDD_NAME) seek=1 conv=notrunc

# run QEMU
run: $(HDD_NAME)
	@echo "--- Starting QEMU ---"
	qemu-system-i386 -drive format=raw,file=$< -m 256

clean:
	$(CLEAN_INFO)
ifeq ($(OS_TYPE),Windows_NT)
	@if exist $(BIN_DIR) $(RM) $(subst /,\,$(BIN_DIR))
else
	@$(RM) $(BIN_DIR)
endif

.PHONY: all check_tools build_dirs run clean

