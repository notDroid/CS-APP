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


#define SIZE_T_SIZE (sizeof(size_t))

// Basic Header and Footer Information Control
#define PACK(size, alloc) (size | alloc)
#define PUT(p, m) * (size_t *) p = m

#define HDPT(p) (((char *) p) - SIZE_T_SIZE)
#define GET_SIZE(p) ((*((size_t *) p)) & ~0x7)
#define GET_ALLOC(p) ((*((size_t *) p)) & 0x1)
#define SET_ALLOC(p, alloc) *((size_t *) p) = ((*((size_t *) p)) & ~0x1) | alloc
#define FTPT(p, size) (((char *) p) + size - 2 * SIZE_T_SIZE)
#define LFPT(p) (((char *) p) - 2 * SIZE_T_SIZE) 

#define NEXT_BLOCK(p, size) (((char *) p) + size)
#define PREV_BLOCK(p, prev_size) (((char *) p) - prev_size)

// Added functions
void coalesce(void *p);
void *realloc_coalesce(void *p, size_t size);
void place(void *p, size_t size);
void split(void *p, size_t new_size, size_t old_size);
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
    PUT(NEXT_BLOCK(p, 1 * SIZE_T_SIZE), PACK(2 * SIZE_T_SIZE, 1)); // Header of left boundary
    PUT(NEXT_BLOCK(p, 2 * SIZE_T_SIZE), PACK(2 * SIZE_T_SIZE, 1)); // Footer of left boundary
    PUT(NEXT_BLOCK(p, 3 * SIZE_T_SIZE), PACK(0, 1)); // Header of right boundary

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    void *p;
    int new_size = ALIGN(size + 2 * SIZE_T_SIZE); // Allocate header + block size + footer, then round up for alignment
    int meta_data;
    // printf("Allocating new memory, size: %d\n", new_size);

    // Search free implicit list
    p = search_free_list(new_size);
    if (p != NULL) {
        // printf("Found from free list: %p\n", p);
        place(p, new_size);
        return p;
    }

    // Get new memory
    p = mem_sbrk(new_size);
    if (p == (void *)-1) 
	    return NULL;

    // Set header + footer
    meta_data = PACK(new_size, 1);
    PUT(HDPT(p), meta_data);
    PUT(FTPT(p, new_size), meta_data);

    // Set 0 allocated header at the end
    PUT(HDPT(NEXT_BLOCK(p, new_size)), 1);
    // printf("New memory allocated: %p\n", p);
    return p;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    // printf("Freeing Memory: %p\n", ptr);
    // Mark deallocated
    SET_ALLOC(HDPT(ptr), 0);
    SET_ALLOC(FTPT(ptr, GET_SIZE(HDPT(ptr))), 0);

    // printf("Freed Memory\n");
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
    size_t copy_size;
    // Case 1, current block is enough
    if (ALIGN(size + 2 * SIZE_T_SIZE) <= GET_SIZE(HDPT(oldptr)))
        return oldptr;

    // Case 2, Neighbors have enough free memory
    newptr = realloc_coalesce(ptr, size);
    if (newptr != NULL) {
        return newptr;
    }

    // Case 3, call malloc
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    // Copy memory
    copy_size = GET_SIZE(HDPT(oldptr));
    if (size < copy_size)
        copy_size = size;
    memcpy(newptr, oldptr, copy_size);
    mm_free(oldptr);

    return newptr;
}

// Search for free blocks with block size >= required size
void *search_free_list(size_t size) {
    size_t block_size, alloc;
    char *p = (char *) mem_heap_lo();
    char *end = (char *) mem_heap_hi();

    p += 4 * SIZE_T_SIZE; // Start at the first real block
    // printf("Start point: %p, end point: %p\n", p, end);
    while (p < end) {
        block_size = GET_SIZE(HDPT(p));
        alloc = GET_ALLOC(HDPT(p));
        // printf("current location: %p, block size: %d, allocated: %d\n", p, block_size, alloc);

        // Return if end of list reached
        if (block_size == 0)
            return NULL;
        // Return if large enough free block
        if (!alloc && (block_size >= size))
            return (void *) p;
        p = NEXT_BLOCK(p, block_size);
    }

    return NULL;
}


// coalesce - combine free neighbors
void coalesce(void *p) {
    // printf("Coalesceing\n");
    size_t size = GET_SIZE(HDPT(p));
    size_t new_size = size;
    char *left = PREV_BLOCK(p, GET_SIZE(LFPT(p)));
    char *right = NEXT_BLOCK(p, size);
    int left_free = !GET_ALLOC(HDPT(left));
    int right_free = !GET_ALLOC(HDPT(right));
    
    if (left_free) {
        new_size += GET_SIZE(HDPT(left));
        p = PREV_BLOCK(p, GET_SIZE(LFPT(p)));
        // printf("Combining Left\n");
    }
    if (right_free) { 
        new_size += GET_SIZE(HDPT(right));
        // printf("Combining Right\n");
    }

    PUT(HDPT(p), PACK(new_size, 0));
    PUT(FTPT(p, new_size), PACK(new_size, 0));
    return;
}

// realloc coalesce - combine free neighbors
void *realloc_coalesce(void *p, size_t size) {
    void *old_p = p;
    size_t old_size = GET_SIZE(HDPT(p));
    size_t copy_size;
    size_t new_size = old_size;
    char *left_footer = LFPT(p);
    char *right_header = HDPT(NEXT_BLOCK(p, old_size));

    int left_size, right_size;
    int left_free = !GET_ALLOC(left_footer);
    int right_free = !GET_ALLOC(right_header);

    // Case 1: neither are free
    if (!left_free && !right_free) {
        return NULL;
    }

    // Case 2: both are free
    if (left_free && right_free) {
        left_size = GET_SIZE(left_footer);
        right_size = GET_SIZE(right_header);

        // Subcase 1: not enough combined space
        if (old_size + left_size + right_size < size) {
            return NULL;
        }
        // Subcase 2: we need both
        if (!(old_size + left_size >= size) && !(old_size + right_size >= size)) {
            new_size += left_size + right_size;
            p = PREV_BLOCK(p, left_size);
        }
        // Subcase 3: we only need one, choose least one
        else if (right_size <= left_size) { // right is smaller
            new_size += right_size;
        }
        else { // left is smaller
            new_size += left_size;
            p = PREV_BLOCK(p, left_size);
        }
    }
    // Case 3: only right is free
    else if (right_free) {
        right_size = GET_SIZE(right_header);
        // Subcase 1: not enough combined space
        if (old_size + right_size < size) {
            return NULL;
        }
        // Subcase 2: combine right
        new_size += right_size;
    }
    // Case 4: only left is free
    else {
        left_size = GET_SIZE(left_footer);
        // Subcase 1: not enough combined space
        if (old_size + left_size < size) {
            return NULL;
        }
        // Subcase 2: combine left
        new_size += left_size;
        p = PREV_BLOCK(p, left_size);
    }

    // Combine and allocate
    PUT(HDPT(p), PACK(new_size, 1));
    PUT(FTPT(p, new_size), PACK(new_size, 1));

    // If p changed (shifted to the left), we need to copy the memory
    if (p != old_p) {
        copy_size = old_size;
        if (size < copy_size)
            copy_size = size;
        memcpy(p, old_p, copy_size);
    }

    return (void *) p;
}


void place(void *p, size_t size) {
    // Split if size needed < size provided
    if (size < GET_SIZE(HDPT(p))) {
        split(p, size, GET_SIZE(HDPT(p)) - size);
        // printf("Split Free Block\n");
    }
    SET_ALLOC(HDPT(p), 1);
    SET_ALLOC(FTPT(p, size), 1);
    // SET_ALLOC(FTPT(p, GET_SIZE(HDPT(p))), 1);
}


void split(void *p, size_t left_size, size_t right_size) {
    char *next_block = NEXT_BLOCK(p, left_size);

    // Split into left and right
    PUT(HDPT(p), PACK(left_size, 0));
    PUT(FTPT(p, left_size), PACK(left_size, 0));

    PUT(HDPT(next_block), PACK(right_size, 0));
    PUT(FTPT(next_block, right_size), PACK(right_size, 0));
    return;
}