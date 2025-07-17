#include "kernel.h"
#include "io/serial.h"
#include "mem/paging.h"
#include "screen.h"
#include <stdbool.h>

#define WHITE_TEXT 0x07

char *video_memory = (char *)0xb8000;
unsigned int current_line = 0;

extern char stack_top;

void kernel_main() {
  serial_print(COM1, "[Kernel] Started!\n");
  serial_print(COM1, "[Kernel] Enabling Paging...\n");
  __asm__ volatile("mov %0, %%esp" : : "r"(&stack_top));
  paging_init();
  page_map(0x100000, 0x100000, PAGE_PRESENT | PAGE_WRITABLE);

  page_map(0xC0000000, 0x100000, PAGE_PRESENT | PAGE_WRITABLE);
  serial_print(COM1, "[Kernel] Success!\n");

  serial_print(COM1, "[Kernel] Test...\n");
  uint32_t *ptr = (uint32_t *)0xC0000000;
  *ptr = 0x12345678;
  serial_print(COM1, "[Kernel] Success!\n");

  serial_print(COM1, "[Kernel] Unmapping...\n");
  page_unmap(0xC0000000);
  serial_print(COM1, "[Kernel] Success!\n");

  clear_screen();
  print_screen("Hello, World!");
  serial_print(COM1, "Hello, World!");
  for (;;) {
    __asm__ volatile("hlt");
  }
}

void clear_screen() {
  unsigned int i = 0;
  while (i < (80 * 25 * 2)) {
    video_memory[i] = ' ';
    i++;
    video_memory[i] = WHITE_TEXT;
    i++;
  }
}

unsigned int print_screen(char *str) {
  unsigned int i = 0;
  i = (current_line * 80 * 2);

  while (*str != 0) {
    if (*str == '\n') {
      current_line++;
      i = (current_line * 80 * 2);
      *str++;
    } else {
      video_memory[i] = *str;
      *str++;
      i++;
      video_memory[i] = WHITE_TEXT;
      i++;
    }
  }
  current_line++;

  return 1;
}
