/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// Basic Header and Footer Information Control
#define PACK(size, alloc) (size | alloc)
#define PUT(p, m) * (size_t *) p = m

#define HDPT(p) (((char *) p) - SIZE_T_SIZE)
#define GET_SIZE(p) ((*((size_t *) p)) & ~0x7)
#define GET_ALLOC(p) ((*((size_t *) p)) & 0x1)
#define SET_ALLOC(p, alloc) *((size_t *) p) = ((*((size_t *) p)) & ~0x1) | alloc
#define FTPT(p, size) (((char *) p) + size - 2 * SIZE_T_SIZE)

#define NEXT_BLOCK(p, size) (((char *) p) + size)
#define PREV_BLOCK(p, size) (((char *) p) - size)

// Added functions
void coalesce(void *p);
void *search_free_list(size_t size);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // Initialize boundaries
    void *p = mem_sbrk(4 * SIZE_T_SIZE);
    if (p == (void *)-1)
	    return -1;

    // First word ignored for double word alignment
    PUT(NEXT_BLOCK(p, 1), 8 | 1); // Header of left boundary
    PUT(NEXT_BLOCK(p, 2), 8 | 1); // Footer of left boundary
    PUT(NEXT_BLOCK(p, 3), 1); // Header of right boundary

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    void *p;
    int newsize = ALIGN(size + 2 * SIZE_T_SIZE); // Allocate header + block size + footer, then round up for alignment
    int meta_data = PACK(newsize, 1);

    // Search free implicit list
    p = search_free_list(newsize);
    if (p != NULL) {
        SET_ALLOC(HDPT(p), 1);
        SET_ALLOC(FTPT(p, GET_SIZE(p)), 1);
        return p;
    }

    // Get new memory
    p = mem_sbrk(newsize);
    if (p == (void *)-1)
	    return NULL;

    // Set header + footer
    PUT(HDPT(p), meta_data);
    PUT(FTPT(p, newsize), meta_data);

    // Set 0 allocated header at the end
    PUT(HDPT(NEXT_BLOCK(p, newsize)), 1);
    return p;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    // Mark deallocated
    SET_ALLOC(HDPT(ptr), 0);
    SET_ALLOC(FTPT(ptr, GET_SIZE(ptr)), 0);

    // Combine neighbors
    coalesce(ptr);
    return;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

// Search for free blocks with block size >= required size
void *search_free_list(size_t size) {
    size_t block_size, alloc;
    char *p = (char *) mem_heap_lo();
    char *end = (char *) mem_heap_hi();

    p += 16; // Start at the first real block
    while (p < end) {
        block_size = GET_SIZE(HDPT(p));
        alloc = GET_ALLOC(HDPT(p));

        // Return if end of list reached
        if (block_size == 0)
            return NULL;
        // Return if large enough free block
        if (!alloc && (block_size >= size))
            return p;
        p = NEXT_BLOCK(p, block_size);
    }

    return NULL;
}


// coalesce - combine free neighbors
void coalesce(void *p) {
    return;
}
