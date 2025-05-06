#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define Cap 640000
#define ALIGN4(x) ((x + 3) & ~0x3) // Chinh len thanh boi so cua 4
#define BLOCK_SIZE sizeof(block_header)

char heap[Cap] = {0};
size_t heap_size = 0;

typedef struct block_header
{
    size_t size;
    int free; // 1 is free
    struct block_header *next;
} block_header;

static block_header *head = NULL; // Con tro toi vung nho dau tien

block_header *find_free_block(size_t size) {
    block_header *curr = head;
    while (curr) {
        if (curr->free && size <= curr->size) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

block_header *request_space(block_header *last, size_t size) {
    void *ptr = sbrk(0); // Lay dia chi hien tai cua heap
    void *request = sbrk(size + BLOCK_SIZE); // Request size moi 

    if (request == (void*)-1) {
        return NULL;
    }

    block_header *block = (block_header*)ptr; // Con tro toi vung nho moi xin

    block->size = size;
    block->free = 0;
    block->next = NULL;

    if (last) {
        last->next = block; // Cap nhat vung nho cu tro toi vung nho moi
    }

    return block;
}

void *my_alloc(size_t size) {
    size = ALIGN4(size);
    block_header *block;

    if (head == NULL) { // Neu chua co vung nho nao duoc cap phat
        block = request_space(NULL, size);
        if (block == NULL) {
            return NULL;
        }
        head = block; // Tro den vung nho moi
    } else {
        block_header *last = head;
        block = find_free_block(size);
        if (block == NULL) {
            block = request_space(last, size);
            if (block == NULL) {
                return NULL;
            }
        } else {
            block->free = 0;
        }
    }

    printf("Block start: %p\n", block);
    printf("Block size: %ld\n", block->size);
    void *end = (void*)((char*)block + BLOCK_SIZE + block->size);
    printf("Block end: %p\n\n", end);

    return (void*)(block + 1); // Tra ve dia chi sau header
}


void heap_free(void *ptr) {
    if (!ptr) {
        return;
    }

    block_header *block = (block_header*)ptr - 1; // Tro ve header
    block->free = 1; // Danh dau la free

    // Co the gop cac block lien ke
    block_header *curr = head;
    while (curr) {
        // Block hien tai free, block lien ke tiep theo khac null va free thi gop
        uint8_t *expect_next = (uint8_t*) curr + curr->size + BLOCK_SIZE;
        if (curr->free && curr->next && curr->next->free && expect_next == (uint8_t*) curr->next) {
            curr->size += BLOCK_SIZE + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

// void heap_collect() {

// }

void print_mem_usage() {
    FILE* f = fopen("/proc/self/status", "r");
    if (!f) return;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            printf("%s", line);
        }
    }
    fclose(f);
}

int main() {

    int *a = my_alloc(sizeof(int) * 10);
    int *b = my_alloc(sizeof(int) * 10);
    // for (int i = 0; i < 6; i++) {
    //     printf("%p\n", &a[i]);
    // }
    // int *a = malloc(sizeof(int) * 10);
    // for (int i = 0; i < 5; i++) {
    //     a[i] = i;
    // }
    print_mem_usage();
    heap_free(a);
    print_mem_usage();
    // printf("a address: %p\n", a);
    // printf("b address: %p\n", b);

    // block_header *block = (block_header*)a - 1;
    // size_t size = block->size;
    // void *start = (void*)block;
    // void *end = (void*)((char*)block + BLOCK_SIZE + size);
    
    // printf("Block start: %p\n", start);
    // printf("Block end: %p\n", end);
    // printf("Block size: %zu\n", size);
    // printf("Block address: %p\n", block);

    return 0;
}