BINARY_DIR = bin
TEMP_DIR = bin/tmp
SRC_DIR = src
IMAGE_NAME = $(BINARY_DIR)/os-img.bin
HDD_NAME = $(BINARY_DIR)/harddrive.img
KERNEL_BIN = $(BINARY_DIR)/kernel.bin

OS_TYPE := $(shell uname -s 2>/dev/null || echo Windows_NT)

ifeq ($(OS_TYPE),Windows_NT)
    MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
    RM = rmdir /s /q
else
    MKDIR = mkdir -p $(1)
    RM = rm -rf
endif

C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")
INC_FLAGS = -I$(SRC_DIR)

OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(TEMP_DIR)/%.o,$(C_SOURCES))

all: check_tools build_dirs $(HDD_NAME) run

check_tools:
	@echo "--- Checking dependencies ---"
	@for tool in nasm gcc ld qemu-system-i386; do \
		if ! command -v $$tool > /dev/null 2>&1; then \
			echo "ERROR: $$tool is not installed."; \
			echo -n "Do you want to install $$tool? (y/n): "; \
			read choice; \
			if [ "$$choice" = "y" ]; then \
				if [ "$(OS_TYPE)" = "Windows_NT" ]; then \
					echo "Please install $$tool manually on Windows."; exit 1; \
				else \
					if [ "$$tool" = "nasm" ]; then sudo apt install nasm; \
					elif [ "$$tool" = "gcc" ]; then sudo apt install gcc-multilib; \
					elif [ "$$tool" = "qemu-system-i386" ]; then sudo apt install qemu-system-x86; \
					else sudo apt install $$tool; \
					fi; \
				fi; \
			else \
				echo "Aborting: $$tool is required."; exit 1; \
			fi; \
		fi; \
	done

build_dirs:
	@$(call MKDIR,$(BINARY_DIR))
	@$(call MKDIR,$(TEMP_DIR))

$(TEMP_DIR)/%.o: $(SRC_DIR)/%.c
	@$(call MKDIR,$(dir $@))
	@echo "Compiling: $<"
	gcc -ffreestanding -m32 -fno-pie -fno-stack-protector -c $< -o $@ $(INC_FLAGS) || (echo "GCC failed on $< with Error Level $$?"; exit 1)

$(KERNEL_BIN): $(OBJECTS)
	@echo "--- Linking Kernel ---"
	ld -m elf_i386 -Ttext 0x1000 $^ -e main --oformat binary -o $@ || (echo "Linker failed with Error Level $$?"; exit 1)

$(IMAGE_NAME): $(KERNEL_BIN) boot.asm
	@echo "--- Compiling Bootloader (incorporating kernel) ---"
	nasm -f bin boot.asm -o $@ || (echo "NASM failed with Error Level $$?"; exit 1)

$(HDD_NAME): $(IMAGE_NAME)
	@echo "--- Creating 512MB Hard Drive Image ---"
	dd if=/dev/zero of=$@ bs=1M count=512 status=none
	dd if=$< of=$@ conv=notrunc status=none || (echo "DD failed with Error Level $$?"; exit 1)

run: $(HDD_NAME)
	@echo "--- Starting QEMU ---"
	qemu-system-i386 -drive format=raw,file=$< -m 256

clean:
	@$(RM) $(BINARY_DIR)

.PHONY: all check_tools build_dirs run clean

