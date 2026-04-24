#include "mem.h"

#define HEAP_MAX_SIZE 0x100000
static uint8_t kernel_heap_area[HEAP_MAX_SIZE];

typedef struct memory_block {
    uint32_t size;
    uint8_t is_free;
    struct memory_block *next;
} memory_block_t;

static memory_block_t *heap_start = (memory_block_t*)kernel_heap_area;
static uint8_t heap_initialized = 0;

void heap_init() {
    heap_start->size = HEAP_MAX_SIZE - sizeof(memory_block_t);
    heap_start->is_free = 1;
    heap_start->next = 0;
    heap_initialized = 1;
}

void* malloc(uint32_t size) {
    if (!heap_initialized) heap_init();

    memory_block_t *best_fit = 0;
    memory_block_t *curr = heap_start;

    while (curr) {
        if (curr->is_free && curr->size >= size) {
            if (!best_fit || curr->size < best_fit->size) {
                best_fit = curr;
            }
        }
        curr = curr->next;
    }

    if (best_fit) {
        if (best_fit->size > size + sizeof(memory_block_t) + 4) {
            memory_block_t *new_block = (memory_block_t*)((uint8_t*)best_fit + sizeof(memory_block_t) + size);
            new_block->size = best_fit->size - size - sizeof(memory_block_t);
            new_block->is_free = 1;
            new_block->next = best_fit->next;

            best_fit->size = size;
            best_fit->next = new_block;
        }
        best_fit->is_free = 0;
        return (void*)((uint8_t*)best_fit + sizeof(memory_block_t));
    }

    return 0;
}

void free(void* ptr) {
    if (!ptr) return;

    memory_block_t *block = (memory_block_t*)((uint8_t*)ptr - sizeof(memory_block_t));
    block->is_free = 1;

    memory_block_t *curr = heap_start;
    while (curr) {
        if (curr->is_free && curr->next && curr->next->is_free) {
            curr->size += curr->next->size + sizeof(memory_block_t);
            curr->next = curr->next->next;
            continue;
        }
        curr = curr->next;
    }
}

