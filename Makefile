# ===== Конфигурация =====
KERNEL_NAME := PonOS
ISO_NAME := $(KERNEL_NAME).iso
KERNEL_BIN := kernel.bin

ASM := nasm
ASM_FLAGS :=  -f elf32
CC := gcc
CC_FLAGS := -m32 -c -ffreestanding -nostdlib -fno-pic -Wall -Wextra -g
LD := ld
LD_FLAGS := -melf_i386 -T linker.ld -nostdlib

# ===== Сборка =====
.PHONY: all clean run

all: $(ISO_NAME)

kernel.o: kernel.c
	$(CC) $(CC_FLAGS) $< -o $@

boot.o: boot.asm
	$(ASM) $(ASM_FLAGS) $< -o $@

serial.o: io/serial.c
	$(CC) $(CC_FLAGS) $< -o $@

mem.o: mem/paging.c
	$(CC) $(CC_FLAGS) $< -o $@

$(KERNEL_BIN): boot.o kernel.o serial.o mem.o
	$(LD) $(LD_FLAGS) $^ -o $@

$(ISO_NAME): $(KERNEL_BIN)
	mkdir -p iso/boot/grub
	cp $(KERNEL_BIN) iso/boot/
	
	echo 'menuentry "$(KERNEL_NAME)" {' > iso/boot/grub/grub.cfg
	echo '  multiboot /boot/$(KERNEL_BIN)' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg

	echo 'menuentry Reboot {' >> iso/boot/grub/grub.cfg
	echo '  reboot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg

	echo 'menuentry Shutdown {' >> iso/boot/grub/grub.cfg
	echo '  halt' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg

	grub-mkrescue -o $(ISO_NAME) iso

clean:
	rm -f *.o $(KERNEL_BIN) $(ISO_NAME)
	rm -rf iso

run: $(ISO_NAME)
	qemu-system-i386 -cdrom $(ISO_NAME) -serial stdio -no-reboot -d guest_errors
