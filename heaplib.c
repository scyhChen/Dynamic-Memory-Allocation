#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

/* You must implement these functions according to the specification
 * given in heaplib.h. You can define any types you like to this file.
 *
 * Student 1 Name: Yuehan Chen
 * Student 1 NetID: yc598
 * Student 2 Name: Zhikun Zhao
 * Student 2 NetID: zz86
 *
 * Include a description of your approach here.
 * This is a seg-list approach to implement malloc. We define two structs: one for heap that
 * maintains the free-block-lists of different sizes available to use, one for the block.
 * We implement the split policy to provide more available blocks in the free lists.
 */

#define MY_HEAP_SIZE (2*sizeof(unsigned int) + 5*sizeof(uintptr_t))
#define BLOCK_HEADER_SIZE (sizeof(unsigned int) + sizeof(uintptr_t))

/* Useful shorthand: casts a pointer to a (char *) before adding */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))

typedef struct _heap_header_t {
    unsigned int max_size; // in bytes
    unsigned int curr_size; // in bytes
    void *class1_8; // pointer to first free block of size 1-2
    void *class9_16;
    void *class17_32;
    void *class33_64;
    void *class65_inf;
} heap_header_t ;

typedef struct _block_header_t {
    unsigned int size_inuse; // in bytes
    void *next_free;
} block_header_t ;

/* Return the appropriate list of free blocks */
void **classptr(void *heap_ptr, unsigned int size) {
    heap_header_t *heap = (heap_header_t *)heap_ptr;
    size -= BLOCK_HEADER_SIZE;
    if (size <= 0) {
      return NULL;
    } else if (size < 9) {
      return &(heap->class1_8);
    } else if (size < 17) {
      return &(heap->class9_16);
    } else if (size < 33) {
      return &(heap->class17_32);
    } else if (size < 65) {
      return &(heap->class33_64);
    } else {
      return &(heap->class65_inf);
    }
}

unsigned int align(unsigned int size) {
    unsigned int offset = ALIGNMENT - (size % ALIGNMENT);
    return (size + offset);
}


int hl_init(void *heap_ptr, unsigned int heap_size) {
    if (heap_ptr == NULL) {
      return 0;
    }
    if (heap_size < (MY_HEAP_SIZE + BLOCK_HEADER_SIZE)) {
      return 0;  //no enough space for metadata
    }

    heap_header_t *heap = (heap_header_t *)heap_ptr;
    heap->curr_size = 0;  //initialize
    heap->class1_8 = NULL;
    heap->class9_16 = NULL;
    heap->class17_32 = NULL;
    heap->class33_64 = NULL;
    heap->class65_inf = NULL;

    char *start = ADD_BYTES(heap, (MY_HEAP_SIZE + BLOCK_HEADER_SIZE));
    unsigned int offset = ALIGNMENT - (((uintptr_t)start) % ALIGNMENT);
    start = ADD_BYTES(start, offset);  //align where the first block should start
    uintptr_t size = (uintptr_t)(ADD_BYTES(heap, heap_size));
    size = size - (uintptr_t)start + BLOCK_HEADER_SIZE;
    size -= (size % ALIGNMENT);  //find the aligned max_size
    if (size <= BLOCK_HEADER_SIZE) {
      return 0;  //no enough space to align the blocks
    }

    heap->max_size = size;  //initialize

    block_header_t *block = (block_header_t *)(start - BLOCK_HEADER_SIZE);
    block->size_inuse = size;  //initialize the first block with max_size in init
    block->next_free = NULL;

    void **class = classptr(heap_ptr, size);
    *class = (void *)block;  //add first block to appropriate free list

    return 1;

}

void *hl_alloc(void *heap_ptr, unsigned int payload_size) {

    heap_header_t *heap = (heap_header_t *)heap_ptr;
    unsigned int size = payload_size + BLOCK_HEADER_SIZE;
    size = align(size);  //total size needed for the block
    if (payload_size == 0) {
      return NULL;  //return NULL is no space or requested 0 byte
    }

    void **prev = classptr(heap_ptr, size);
    unsigned int opt = (uintptr_t)prev - (uintptr_t)heap_ptr - 2*sizeof(unsigned int);
    opt = 5 - (opt/(sizeof(uintptr_t)));

    unsigned int nextsize, i;
    void *blockclass = (*prev);
    block_header_t *block, *next;
    void **class, **prevcopy;

    while (opt > 0) {
      if (blockclass == NULL) {
        prev = (void **)(ADD_BYTES(prev, sizeof(uintptr_t)));
        blockclass = *prev;
        opt -= 1;
      } else {
        block = (block_header_t *)blockclass;
        prevcopy = prev;
        for (i = 0; i < (heap->max_size - heap->curr_size); i = i + 2) {
          if (block->size_inuse >= size) {
            *prev = block->next_free;

            nextsize = block->size_inuse - size; // split the block
            if (nextsize > BLOCK_HEADER_SIZE) {
              next = (block_header_t *)(ADD_BYTES(block, size));
              next->size_inuse = nextsize;
              class = classptr(heap_ptr, nextsize);
              if ((uintptr_t)(*class) == 0) {
                next->next_free = NULL;
              } else {
                next->next_free = *class;
              }
              *class = (void *)next;
            }

            block->size_inuse = size+1;
            heap->curr_size += size;
            block->next_free = NULL;
            return (void *)(ADD_BYTES(block, BLOCK_HEADER_SIZE));
          } else {
            if (block->next_free != NULL) {
              prev = &(block->next_free);
              block = (block_header_t *)(block->next_free);
            } else {
              break;
            }
          }
        }

        prev = (void **)(ADD_BYTES(prevcopy, sizeof(uintptr_t)));
        blockclass = *prev;
        opt -= 1;
      }
    }

    return NULL;

}

void hl_release(void *heap_ptr, void *payload_ptr) {

    if (payload_ptr == NULL) {
      return;
    }

    heap_header_t *heap = (heap_header_t *)heap_ptr;
    block_header_t *block = (block_header_t *)((char *)(payload_ptr) - BLOCK_HEADER_SIZE);

    void **class;
    if ((block->size_inuse) % ALIGNMENT == 0) {
      return;
    } else {
      block->size_inuse -= 1;
      class = classptr(heap_ptr, block->size_inuse);
      if ((uintptr_t)(*class) == 0) {
        block->next_free = NULL;
      } else {
        block->next_free = *class;
      }
      *class = (void *)block;
      heap->curr_size -= block->size_inuse;
    }

    return;
}

void *hl_resize(void *heap_ptr, void *payload_ptr, unsigned int new_size) {

    void *new = hl_alloc(heap_ptr, new_size);
    if ((payload_ptr == NULL) || (new == NULL)) {
      return new;
    } else {
      block_header_t *block = (block_header_t *)((char *)(payload_ptr) - BLOCK_HEADER_SIZE);
      unsigned int size = block->size_inuse - 1 - BLOCK_HEADER_SIZE;
      if (size > new_size) {
        size = new_size;
      }
      memcpy(new, payload_ptr, size);
      hl_release(heap_ptr, payload_ptr);

      return new;
    }

}
