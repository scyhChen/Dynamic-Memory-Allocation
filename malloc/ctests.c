#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"

#define HEAP_SIZE 1024
#define ARRAY_LEN 16

const char* testDescriptions[] = {
  "init: heap should be created when enough space",
  "init: heap should not be created when not enough space",
  "alloc: block should be allocated when enough space",
  "alloc: block should not be allocated when not enough space",
  "alloc: block returned is aligned",
  "init: does not support multiple heaps",
  "resize: should be able to alloc a new block when there is enough space after resize",
  "alloc: should not allocate less than the requested size",
  "release: released memory should be able to be allocated again",
  "resize: if blockptr has the value 0 (NULL), resize should behave like hl_alloc",
  "resize: should return 0 if the request cannot be satisfied",
  "resize: the contents of the block should be preserved",
  ""
};

/* Checks whether a "heap" is created when there IS enough space.
 * THIS TEST IS COMPLETE.
 */
int test01() {

    char heap[HEAP_SIZE];

    int heap_created_f = hl_init(heap, HEAP_SIZE);

    if (heap_created_f) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Checks whether a "heap" is created when there IS NOT enough space.
 * THIS TEST IS NOT COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 */
int test02() {

  char heap[HEAP_SIZE];

  int heap_created_f = hl_init(heap, 0);

  if (heap_created_f == 0) {
      return SUCCESS;
  }
  return FAILURE;

}

/* Checks whether a block can be allocated when there is enough space.
 * THIS TEST IS NOT COMPLETE.
 */
int test03() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  // should work
  void *block = hl_alloc(heap, ARRAY_LEN);

  if (block == NULL) {
      return FAILURE;
  }
  return SUCCESS;
}

/* Checks whether a block can be allocated when there is NOT enough space.
 * THIS TEST IS COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 */
int test04() {

    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // should NOT work
    void *block = hl_alloc(heap, ARRAY_LEN * HEAP_SIZE);

    if (block == NULL) {
        return SUCCESS;
    }
    return FAILURE;
}

/* Checks whether hl_alloc returns a pointer that has the correct
 * alignment.
 * THIS TEST IS NOT COMPLETE. heaplame.c does not pass this test. Feel free to fix it!
 */
int test05() {

    char array[HEAP_SIZE * 5];
    void *block;
    bool aligned_f = false;
    int i = 0;

    hl_init(&array, HEAP_SIZE);

    while (i < 8) {
      block = hl_alloc(&array, ARRAY_LEN + i); // doesn't really matter how much we allocate here
      aligned_f = !((unsigned long)block & (ALIGNMENT - 1));
      if (aligned_f == false) {
          return FAILURE;
      }
      i++;
    }

    char *start = array + HEAP_SIZE + 3;
    hl_init(start, HEAP_SIZE);
    i = 0;

    while (i < 8) {
      block = hl_alloc(start, ARRAY_LEN + i); // doesn't really matter how much we allocate here
      aligned_f = !((unsigned long)block & (ALIGNMENT - 1));
      if (aligned_f == false) {
          return FAILURE;
      }
      i++;
    }

    return SUCCESS;
}

/* Check if init supports multiple heaps.
 */
int test06() {

    char heap1[HEAP_SIZE];

    hl_init(heap1, HEAP_SIZE);

    // should work
    void *block = hl_alloc(heap1, ARRAY_LEN);

    if (block == NULL) {
        return FAILURE;
    }

    char heap2[HEAP_SIZE];

    hl_init(heap2, HEAP_SIZE);

    block = hl_alloc(heap2, ARRAY_LEN);

    if (block == NULL) {
        return FAILURE;
    }

    return SUCCESS;

}

/* Check if a block can be allocated after a block of the same size being resized to 0.
 */
int test07() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  int i = 0;
  void *block, *last;

  while (1) {
    block = hl_alloc(heap, 5*ARRAY_LEN);
    if (block == NULL) {
      break;
    } else {
      last = block;
      i++;
    }
  }
  block = hl_resize(heap, last, 0);
  block = hl_alloc(heap, 5*ARRAY_LEN);

  if (block == NULL) {
      return FAILURE;
  }
  return SUCCESS;

}

/* Alloc is allowed to allocate more than the requested size, but never less.
 */
int test08() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  int i = 0;
  void *block;

  while (i < 6) {
    block = hl_alloc(heap, 10*ARRAY_LEN);
    i++;
  }

  if (block != NULL) {
      return FAILURE;
  }

  return SUCCESS;

}

/* released memory should be able to be allocated again.
 */
int test09() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  void *block = hl_alloc(heap, HEAP_SIZE/2);

  if (block != NULL) {
    hl_release(heap, block);
    block = hl_alloc(heap, HEAP_SIZE/2);
  } else {
    block = hl_alloc(heap, HEAP_SIZE/3);
    hl_release(heap, block);
    block = hl_alloc(heap, HEAP_SIZE/3);
  }

  if (block == NULL) {
      return FAILURE;
  }

  return SUCCESS;

}

/* If blockptr has the value 0 (NULL), resize should behave like hl_alloc.
 */
int test10() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  void *block = hl_alloc(heap, HEAP_SIZE);

  block = hl_resize(heap, block, ARRAY_LEN);

  if (block == NULL) {
      return FAILURE;
  }
  return SUCCESS;

}

/* resize: returning 0 if the request cannot be satisfied.
 */
int test11() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  void *block = hl_alloc(heap, ARRAY_LEN);

  block = hl_resize(heap, block, HEAP_SIZE);

  if (block != NULL) {
      return FAILURE;
  }
  return SUCCESS;

}

/* resize: the contents of the block should be preserved.
 */
int test12() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  void *block = hl_alloc(heap, 10 * sizeof(int));
  int *ptr = (int *)block;

  int i = 1;
  while (i < 11) {
    *ptr = i;
    i++;
    ptr++;
  }

  block = hl_resize(heap, block, 15 * sizeof(int));
  ptr = (int *)block;

  i = 1;
  while (i < 11) {
    if (*ptr != i) {
        return FAILURE;
    }
    i++;
    ptr++;
  }

  return SUCCESS;

}

/* resize: the return value should be 8-byte aligned.
 */
int test13() {

  char array[HEAP_SIZE * 5];
  void *block;
  bool aligned_f = false;
  int i = 0;

  hl_init(&array, HEAP_SIZE);

  while (i < 8) {
    block = hl_alloc(&array, ARRAY_LEN + i);
    block = hl_resize(&array, block, ARRAY_LEN + i + 8);
    aligned_f = !((unsigned long)block & (ALIGNMENT - 1));
    if (aligned_f == false) {
        return FAILURE;
    }
    i++;
  }

  char *start = array + HEAP_SIZE + 3;
  hl_init(start, HEAP_SIZE);
  i = 0;

  while (i < 8) {
    block = hl_alloc(start, ARRAY_LEN + i);
    block = hl_resize(start, block, ARRAY_LEN + i + 8);
    aligned_f = !((unsigned long)block & (ALIGNMENT - 1));
    if (aligned_f == false) {
        return FAILURE;
    }
    i++;
  }

  return SUCCESS;

}

/* another version: released memory should be able to be allocated again.
 */
int test14() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  void *block, *last;

  while (1) {
    block = hl_alloc(heap, 5*ARRAY_LEN);
    if (block == NULL) {
      break;
    } else {
      last = block;
    }
  }
  hl_release(heap, last);
  block = hl_alloc(heap, 5*ARRAY_LEN);

  if (block == NULL) {
      return FAILURE;
  }
  return SUCCESS;

}

/* resize: returning 1 if the request can be satisfied.
 */
int test15() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  void *block = hl_alloc(heap, 2*ARRAY_LEN);

  block = hl_resize(heap, block, ARRAY_LEN);

  if (block == NULL) {
      return FAILURE;
  }
  return SUCCESS;

}

/* resize: when resized to a smaller size, the contents of the block should be preserved.
 */
int test16() {

  char heap[HEAP_SIZE];

  hl_init(heap, HEAP_SIZE);

  void *block = hl_alloc(heap, 10 * sizeof(int));
  int *ptr = (int *)block;

  int i = 1;
  while (i < 11) {
    *ptr = i;
    i++;
    ptr++;
  }

  block = hl_resize(heap, block, 5 * sizeof(int));
  ptr = (int *)block;

  i = 1;
  while (i < 6) {
    if (*ptr != i) {
        return FAILURE;
    }
    i++;
    ptr++;
  }

  return SUCCESS;

}
