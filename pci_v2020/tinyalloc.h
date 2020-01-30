#include <stdbool.h>
#include <stddef.h>
#include "mcb02.h"

bool ta_init();
void *ta_alloc(size_t num);
void *ta_calloc(size_t num, size_t size);
bool ta_free(void *ptr);

size_t ta_num_free();
size_t ta_num_used();
size_t ta_num_fresh();
bool ta_check();


unsigned int ta[1<<28];
//TODO CALCULATE MINIMUM SIZE

#ifndef TA_ALIGN
#define TA_ALIGN 4
#endif

#ifndef TA_BASE
#define TA_BASE ((unsigned int)&ta)
#endif

#ifndef TA_HEAP_START
#define TA_HEAP_START (TA_BASE + sizeof(Heap))
#endif

#ifndef TA_HEAP_LIMIT
#define TA_HEAP_LIMIT (TA_HEAP_START + MCB02_RAM_LEN/2) //TODO RENAME
#endif

#ifndef TA_HEAP_BLOCKS
#define TA_HEAP_BLOCKS 256
#endif

#ifndef TA_SPLIT_THRESH
#define TA_SPLIT_THRESH 16
#endif

#ifdef TA_DEBUG
extern void print_s(char *);
extern void print_i(size_t);
#else
#define print_s(X)
#define print_i(X)
#endif

typedef struct Block Block;

struct Block {
    void *addr;
    Block *next;
    size_t size;
};

typedef struct {
    Block *free;   // first free block
    Block *used;   // first used block
    Block *fresh;  // first available blank block
    size_t top;    // top free addr
    Block blocks[TA_HEAP_BLOCKS];
} Heap;

static Heap *heap = (Heap *)TA_BASE;
