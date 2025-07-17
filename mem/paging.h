#pragma once
#include <stdint.h>

// Флаги страниц
#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER (1 << 2)

// Размеры структур
#define PAGE_SIZE 4096
#define ENTRIES_PER_TABLE 1024

// Типы записей
typedef uint32_t page_entry_t;

// Инициализация paging
void paging_init();

// Функции работы со страницами
void page_map(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);
void page_unmap(uint32_t virt_addr);
