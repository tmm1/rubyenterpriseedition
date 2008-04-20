#include "object-heap.h"

struct RObjectHeap {
    RObjectHeapBlock *blocks;
    unsigned int blocks_used;
    unsigned int blocks_count;
    RVALUE *freelist;
};

struct RObjectHeapBlock {
    void *membase;
    void *memlimit;
    RVALUE *objects;
    unsigned int size;
    int *marks;
    unsigned int marks_size;
};


#define INITIAL_BLOCKS_COUNT 100
#define BLOCK_SIZE 1000

static void r_object_heap_block_init(RObjectHeapBlock *block, unsigned int size);
static void r_object_heap_block_deinit(RObjectHeapBlock *block);


RObjectHeap *
r_object_heap_new()
{
    RObjectHeap *heap;
    unsigned int i;
    
    heap = (RObjectHeap *) malloc(sizeof(RObjectHeap));
    heap->blocks = malloc(sizeof(RObjectHeapBlock *) * INITIAL_BLOCKS_COUNT);
    heap->blocks_used = INITIAL_BLOCKS_COUNT;
    heap->blocks_count = INITIAL_BLOCKS_COUNT;
    for (i = 0; i < INITIAL_BLOCKS_COUNT; i++) {
	r_object_heap_block_init(&heap->blocks[i], BLOCK_SIZE);
    }
    heap->freelist = &heap->blocks[0].objects[0];
    return heap;
}

void
r_object_heap_free(RObjectHeap *heap)
{
    unsigned int i;
    
    for (i = 0; i < heap->blocks_used; i++) {
	r_object_heap_block_deinit(&heap->blocks[i]);
    }
    free(heap->blocks);
    free(heap);
}

VALUE
r_object_heap_alloc(RObjectHeap *heap)
{
    VALUE result;
    
    result = heap->freelist;
    freelist = heap->freelist->as.free.next;
    RANY(result)->as.free.flags = FL_ALLOCATED;
    return result;
}

void
r_object_heap_dealloc(RObjectHeap *heap, VALUE object, RObjectHeapBlock *block)
{
    RANY(object).as.free.next = heap->freelist;
    heap->freelist = (RVALUE *) object;
}

void   r_object_heap_mark(RObjectHeap *heap, RVALUE object);
void   r_object_heap_unmark(RObjectHeap *heap, RVALUE object);
void   r_object_heap_mark_with_block(RObjectHeap *heap, RVALUE object, RObjectHeapBlock *block);
void   r_object_heap_unmark_with_block(RObjectHeap *heap, RVALUE object, RObjectHeapBlock *block);


static void
r_object_heap_block_init(RObjectHeapBlock *block, unsigned int size)
{
    block = (RObjectHeapBlock *) malloc(sizeof(RObjectHeapBlock));
    block->membase = malloc(sizeof(RVALUE) * size);
    block->memlimit = block->membase + sizeof(RVALUE) * size;
    
    /* Make sure that block->objects is aligned to sizeof(RVALUE). We'll have
     * to sacrifice a single object if malloc() didn't give us an aligned address.
     */
    if (block->membase % sizeof(RVALUE) == 0) {
	block->objects = block->membase;
	block->size = size;
    } else {
	unsigned int padding = sizeof(RVALUE) - (block->membase % sizeof(RVALUE);
	block->objects = (RVALUE *) (block->membase + padding);
	block->size = size - 1;
    }
    assert(block->objects % sizeof(RVALUE) == 0);
    
    block->marks_size = (unsigned int) (ceil(block->size / (sizeof(int) * 8.0)));
    block->marks = (int *) calloc(block->marks_size, sizeof(int));
    
    return block;
}

void *
r_object_heap_block_deinit(RObjectHeapBlock *)
{
    free(block->membase);
    free(block->marks);
}

