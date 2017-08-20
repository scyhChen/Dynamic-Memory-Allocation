#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "heaplib.h"

/* Useful shorthand: casts a pointer to a (char *) before adding */
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))

typedef struct _block_header_t {
    unsigned int max_size; // in bytes
    unsigned int curr_size; // in bytes
    bool in_use_f; // true or false?
} block_header_t ;

/* This is an example of a helper print function useful for
 * programming and debugging purposes.  Always wrap print statments
 * inside an #ifdef so that the default compile does not include them.
 * Function calls and printfs will kill your performance and create
 * huge output files, for which we have ZERO TOLERANCE.
 */
void print_block_header(block_header_t *block) {
    printf("block starts at addr %p\n"   // cute little C printing trick.
            "max_size = %d\n"            // Notice there are no commas between these lines
            "curr_size = %d\n"
            "in use? %s\n"
            "payload starts at addr %p\n", block, block->max_size, block->curr_size, block->in_use_f ? "Yes" : "No", ADD_BYTES(block, sizeof(block_header_t)));
}

void print_block_payload(block_header_t *block) {
    int i, num_chars = block->curr_size;
    char *payload = ADD_BYTES(block, sizeof(block_header_t));
    printf("payload:\n");
    for (i = 0; i < num_chars; i++) {
        printf("\t%c\n", payload[i]);
    }
}

/* Turns the heap into one unused block.
 *
 */
int hl_init(void *heapptr, unsigned int heap_size) {

    block_header_t *block = (block_header_t *)heapptr;
    block->max_size = heap_size;
    block->curr_size = 0;
    block->in_use_f = false;

#ifdef PRINT_DEBUG
    print_block_header(block);
#endif

    return 1; // Success!

}

/* this heaplib is so lame it will only allocate one block for you
*/

void *hl_alloc(void *heapptr, unsigned int payload_size) {

    block_header_t *block = (block_header_t *)heapptr;

    if (block->in_use_f)
        return NULL;

    block->in_use_f = true;
    block->curr_size = payload_size;

#ifdef PRINT_DEBUG
    print_block_header(block);
#endif

    return ADD_BYTES(block, sizeof(block_header_t)); // Success!
}

/* but it will gladly release this block
*/
void hl_release(void *heapptr, void *payload_ptr) {
    block_header_t *block = (block_header_t *)heapptr;
    block->curr_size = 0;
    block->in_use_f = false;
}

void *hl_resize(void *heapptr, void *blockptr, unsigned int new_size) {
    block_header_t *block = (block_header_t *)heapptr;
    block->curr_size = new_size;
    return blockptr;
}
