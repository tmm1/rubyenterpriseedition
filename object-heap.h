#ifndef _RUBY_OBJECT_HEAP_H_
#define _RUBY_OBJECT_HEAP_H_

#include "ruby.h"

typedef struct {
    union {
	struct {
	    unsigned long flags;	/* always 0 for freed obj */
	    struct RVALUE *next;
	} free;
	struct RBasic  basic;
	struct RObject object;
	struct RClass  klass;
	struct RFloat  flonum;
	struct RString string;
	struct RArray  array;
	struct RRegexp regexp;
	struct RHash   hash;
	struct RData   data;
	struct RStruct rstruct;
	struct RBignum bignum;
	struct RFile   file;
	struct RNode   node;
	struct RMatch  match;
	struct RVarmap varmap;
	struct SCOPE   scope;
    } as;
#ifdef GC_DEBUG
    char *file;
    int   line;
#endif
} RVALUE;

typedef struct RObjectHeap RObjectHeap;
typedef struct RObjectHeapBlock RObjectHeapBlock;
typedef struct RObjectHeapIterator RObjectHeapIterator;

RObjectHeap *r_object_heap_new();
void   r_object_heap_free(RObjectHeap *heap);

VALUE  r_object_heap_alloc(RObjectHeap *heap);
void   r_object_heap_dealloc(RObjectHeap *heap, VALUE object, RObjectHeapBlock *block);
void   r_object_heap_mark(RObjectHeap *heap, RVALUE object);
void   r_object_heap_unmark(RObjectHeap *heap, RVALUE object);
void   r_object_heap_mark_with_block(RObjectHeap *heap, RVALUE object, RObjectHeapBlock *block);
void   r_object_heap_unmark_with_block(RObjectHeap *heap, RVALUE object, RObjectHeapBlock *block);

void   r_object_heap_expand(RObjectHeap *heap);
void   r_object_heap_compact(RObjectHeap *heap);

unsigned int r_object_heap_get_block_count(RObjectHeap *heap);
unsigned int r_object_heap_get_object_count(RObjectHeap *heap);
unsigned int r_object_heap_get_free_count(RObjectHeap *heap);
void r_object_heap_get_memory_boundaries(void **himem, void **lomem);

RObjectHeapIterator *r_object_heap_get_iterator(RObjectHeap *heap);
int   r_object_heap_iterator_has_next(RObjectHeapIterator *iterator);
VALUE r_object_heap_iterator_next(RObjectHeapIterator *iterator, RObjectHeapBlock *block);

#endif /* _RUBY_OBJECT_HEAP_H_ */

