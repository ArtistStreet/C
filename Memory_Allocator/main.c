#include <stdio.h>
#include <assert.h>
#include <unistd.h>

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

static block_header *head = NULL;

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
        last->next = block;
    }

    return block;
}

void *my_alloc(size_t size) {
    void *ptr = sbrk(size);
    if (ptr == (void*)-1) 
        return NULL;

    return ptr;
}


void heap_free(void *ptr) {

}

void heap_collec() {

}

int main() {
    char *root = heap_alloc(26);
    for (int i = 0; i < 26; i++) {
        root[i] = i + 'A';
    }

    int *a = my_alloc(sizeof(int) * 4);
    printf("int: %ld\n", sizeof(int));
    printf("a address: %p", a);

    return 0;
}