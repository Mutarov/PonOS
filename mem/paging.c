#include "paging.h"

/* Таблицы (выровнены по границе 4KB) */
__attribute__((
    aligned(PAGE_SIZE))) static page_entry_t page_directory[ENTRIES_PER_TABLE];

__attribute__((aligned(
    PAGE_SIZE))) static page_entry_t first_page_table[ENTRIES_PER_TABLE];

/* Вспомогательные функции */
static inline uint32_t get_pd_index(uint32_t addr) {
  return addr >> 22; // Бит 22-31
}

static inline uint32_t get_pt_index(uint32_t addr) {
  return (addr >> 12) & 0x3FF; // Бит 12-21
}

static void tlb_flush(uint32_t virt_addr) {
  asm volatile("invlpg (%0)" : : "r"(virt_addr));
}

/* Основная функция инициализации */
void paging_init() {
  /* 1. Инициализация первой таблицы */ страниц (identity map)
  for (uint32_t i = 0; i < ENTRIES_PER_TABLE; i++) {
    first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
  }

  /* 2. Настройка каталога страниц */
  page_directory[0] = (uint32_t)first_page_table | PAGE_PRESENT | PAGE_WRITABLE;

  /* Остальные записи каталога помечаем как невалидные */
  for (uint32_t i = 1; i < ENTRIES_PER_TABLE; i++) {
    page_directory[i] = 0;
  }

  /* 3. Включение механизма страничной адресации */
  asm volatile("mov %0, %%cr3\n"
               "mov %%cr0, %%eax\n"
               "or $0x80000001, %%eax\n"
               "mov %%eax, %%cr0"
               :
               : "r"(page_directory)
               : "eax");

  /* 4. Отображаем важные системные адреса */
  page_map(0xB8000, 0xB8000, PAGE_PRESENT | PAGE_WRITABLE); // VGA буфер
  page_map(0x100000, 0x100000, PAGE_PRESENT | PAGE_WRITABLE);
}

/* Отображение виртуального адреса на физический */
void page_map(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags) {
  uint32_t pd_index = get_pd_index(virt_addr);
  uint32_t pt_index = get_pt_index(virt_addr);

  /* Если запись в каталоге невалидна - создаем новую таблицу */
  if (!(page_directory[pd_index] & PAGE_PRESENT)) {
    static page_entry_t new_table[ENTRIES_PER_TABLE]
        __attribute__((aligned(PAGE_SIZE)));
    page_directory[pd_index] = (uint32_t)new_table | flags | PAGE_PRESENT;

    /* Инициализируем новую таблицу */
    for (uint32_t i = 0; i < ENTRIES_PER_TABLE; i++) {
      new_table[i] = 0;
    }
  }

  /* Получаем указатель на таблицу страниц */
  page_entry_t *pt = (page_entry_t *)(page_directory[pd_index] & ~0xFFF);
  pt[pt_index] = phys_addr | (flags & 0xFFF) | PAGE_PRESENT;

  /* Обновляем TLB */
  tlb_flush(virt_addr);
}

/* Удаление отображения */
void page_unmap(uint32_t virt_addr) {
  uint32_t pd_index = get_pd_index(virt_addr);
  uint32_t pt_index = get_pt_index(virt_addr);

  if (page_directory[pd_index] & PAGE_PRESENT) {
    page_entry_t *pt = (page_entry_t *)(page_directory[pd_index] & ~0xFFF);
    pt[pt_index] = 0;
    tlb_flush(virt_addr);
  }
}
