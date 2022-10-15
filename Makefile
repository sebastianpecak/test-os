AS = $(TARGET)-as
CC = $(TARGET)-gcc
FORCE_INCLUDE = sys_config_flags.h sys_compiler_guard.h
# Sources for which there is no optimization enabled, i.e. -O0.
NO_OPT_SRC = sys_entry_point.c
KERNEL_OUTPUT_NAME = system
TMP_ISO_DIR = .tmpIsoDir
TMP_ISO_BOOT_DIR = "$(TMP_ISO_DIR)/boot"
TMP_ISO_GRUB_DIR = "$(TMP_ISO_BOOT_DIR)/grub"
GRUB_CFG_FILE = grub.cfg
SOURCES = $(shell ls *.c)
OBJECTS = $(subst .c,.o,$(SOURCES))
#DEFINES = -DASM_DEFINED_ENTRY_POINT
LINKER_SCRIPT_FILE = kernel_linker_script.ld
CFLAGS = -c -std=gnu99 -ffreestanding -Wall -Wextra -O0

# Build bootable kernel ISO as default target.
default: $(KERNEL_OUTPUT_NAME).iso

# Make bootable ISO file.
$(KERNEL_OUTPUT_NAME).iso: $(KERNEL_OUTPUT_NAME).bin
	@rm -rf $(TMP_ISO_DIR)
	@mkdir -p $(TMP_ISO_GRUB_DIR)
	@cp $(KERNEL_OUTPUT_NAME).bin $(TMP_ISO_BOOT_DIR)
	@cp $(GRUB_CFG_FILE) $(TMP_ISO_GRUB_DIR)
	@grub-mkrescue -o $(KERNEL_OUTPUT_NAME).iso $(TMP_ISO_DIR)
	@rm -rf $(TMP_ISO_DIR)

# Link kernel binary.
$(KERNEL_OUTPUT_NAME).bin: $(OBJECTS)
	$(CC) -T $(LINKER_SCRIPT_FILE) -o $@ -ffreestanding -O2 -nostdlib -lgcc $^
	@if grub-file --is-x86-multiboot $@; then echo MULTIBOOT VERIFICATION: OK; else echo MULTIBOOT VERIFICATION: NOK; exit; fi

# Compile source files.
%.o : %.c
	$(CC) $< -o $@ $(CFLAGS) $(addprefix -include, $(FORCE_INCLUDE))

# Assemble preprocessed asm.
# entry_point.o: entry_point.s
# 	$(AS) $< -o $@

clean:
	@rm -f *.o
	@rm -f *.bin
	@rm -f *.iso

debug_print:
	@echo "AS                : `which $(AS)`"
	@echo "CC                : `which $(CC)`"
	@echo "FORCE_INCLUDE     : $(FORCE_INCLUDE)"
	@echo "NO_OPT_SRC        : $(NO_OPT_SRC)"
	@echo "KERNEL_OUTPUT_NAME: $(KERNEL_OUTPUT_NAME)"
	@echo "TMP_ISO_DIR       : $(TMP_ISO_DIR)"
	@echo "TMP_ISO_BOOT_DIR  : $(TMP_ISO_BOOT_DIR)"
	@echo "TMP_ISO_GRUB_DIR  : $(TMP_ISO_GRUB_DIR)"
	@echo "GRUB_CFG_FILE     : $(GRUB_CFG_FILE)"
	@echo "LINKER_SCRIPT_FILE: $(LINKER_SCRIPT_FILE)"
	@echo "SOURCES           : $(SOURCES)"
	@echo "OBJECTS           : $(OBJECTS)"
