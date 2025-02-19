#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

/*
###### STRUCTS ######
*/

typedef struct {
    unsigned long address;
    int command;
} MemoryAccess;

typedef struct CacheLine {
    unsigned long tag_bits;
    struct CacheLine *prev;
    struct CacheLine *next;
    int index;
} CacheLine;

typedef struct {
    CacheLine *lines;
    int lru_index;
    int mru_index;
    int num_lines;
} CacheSet;

typedef struct {
    CacheSet *sets;
} Cache;


/*
###### FUNCTION LIST ######
*/

// Input parsing
void parse_command_line_args(int argc, char **argv);
int parse_line(MemoryAccess *mem_access);

// Cache update
void create_cache();
void update_cache(unsigned long address);

// Cache utility
int find_cache_line(CacheSet *set, unsigned long tag_bits);
void swap(CacheSet *set, int index);

/*
###### GLOBAL VARIABLES ######
*/

// simulator information
int hits = 0;
int misses = 0;
int evicts = 0;

// cache provided config
int num_set_bits;
int associativity;
int num_block_bits;
int verbose = 0;

// Cache
Cache cache;

// File
FILE *fp;

/*
###### CACHE SIMULATOR PROGRAM ######
*/
int main(int argc, char *argv[]) {
    MemoryAccess mem_access;

    // Parse CLI Args and Create Cache
    parse_command_line_args(argc, argv);
    create_cache();

    // Iterate through file
    while (parse_line(&mem_access) == 0) {
        if (verbose)
            printf("%c %lx, ", mem_access.command, mem_access.address);
        update_cache(mem_access.address);
        
        // Update again if M type (load then store requests)
        if (mem_access.command == 'M'){
            update_cache(mem_access.address);
        }
        if (verbose)
            printf("\n");
    }

    // Required print summary
    printSummary(hits, misses, evicts);
    return 0;
}

/*
###### PARSE INPUTS ######
*/

void parse_command_line_args(int argc, char **argv) {
    int c;
    while ((c = getopt(argc, argv, "vs:E:b:t:")) != -1) {
        switch (c) {
            case 's':
                num_set_bits = atoi(optarg);
                break;
            case 'E':
                associativity = atoi(optarg);
                break;
            case 'b':
                num_block_bits = atoi(optarg);
                break;
            case 't':
                fp = fopen(optarg, "r");
                break;
            case 'v':
                verbose = 1;
                break;
        }
    }
    return;
}


int parse_line(MemoryAccess *mem_access) {
    char buffer[64];
    char c;
    unsigned long x;
    int d;

    if (feof(fp)) {
        fclose(fp);
        return -1;
    }

    // Read input
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        return -1;
    }

    if (sscanf(buffer, " %c%*[ ,]%lx%*[ ,]%d", &c, &x, &d) != 3) {
        return -1;
    }
    // Try again if instruction memory request
    if (c == 'I') {
        return parse_line(mem_access);
    }

    mem_access->command = c;
    mem_access->address = x;
    return 0;
}


/*
###### CACHE SIMULATOR FUNCTIONS ######
*/

void create_cache() { 
    int i, j;
    int num_sets = (1<<num_set_bits);
    CacheSet *set;
    CacheLine *line;

    // Create array of sets
    cache.sets = (CacheSet *) malloc(num_sets * sizeof(CacheSet));

    // Allocate array of cache lines for every set
    for (i = 0; i < num_sets; i++) {
        // Create and initialize cache set and its cache lines
        set = &cache.sets[i];
        set->lines = (CacheLine *) malloc(associativity * sizeof(CacheLine));
        set->num_lines = 0;
        set->lru_index = 0;
        set->mru_index = 0;

        for (j = 0; j < associativity; j++) {
            line = &set->lines[j];
            line->index = j;
            line->prev = NULL;
            line->next = NULL;
        }
    }
    return;
}

void update_cache(unsigned long address) {
    int index;

    unsigned long set_bits = (address >> num_block_bits) & ((1<<num_set_bits) - 1);
    unsigned long tag_bits = address >> (num_block_bits + num_set_bits);
    CacheSet *set = &cache.sets[set_bits];

    index = find_cache_line(set, tag_bits);

    // Cache Miss
    if (index == -1) {
        misses++;
        if (verbose)
            printf(" miss");

        // If cache is full, evict
        if ((set->num_lines) == associativity) {
            evicts++;
            if (verbose)
                printf(" eviction");
            index = set->lru_index;
        }
        else {
            index = set->num_lines;
            set->num_lines++;
        }

        swap(set, index);
        set->lines[index].tag_bits = tag_bits;
        return;
    }

    // Cache Hit
    hits++;
    if (verbose)
        printf(" hit");
    swap(set, index);
}


int find_cache_line(CacheSet *set, unsigned long tag_bits) {
    int index;
    int num_lines = set->num_lines;
    
    for (index = 0; index < num_lines; index++) {
        if (set->lines[index].tag_bits == tag_bits) {
            return index;
        }
    }

    return -1;
}


// Updates linked list keeping track of the order of elements accessed
void swap(CacheSet *set, int index) {
    int mru_index = set->mru_index;
    // If its the most recently used element, skip
    if (index == mru_index) {
        return;
    }
    CacheLine *line = &set->lines[index];
    CacheLine *prev_mru = &set->lines[mru_index];
    CacheLine *next_lru;
    CacheLine *prev, *next;

    // If it was the least recently used element, we need to update the least recently used element
    if (index == set->lru_index) {
        next_lru = line->next;
        next_lru->prev = NULL;
        set->lru_index = next_lru->index;
    }
    // If its in the middle, connect the prev and next ones together
    else if ((line->prev!=NULL) && (line->next!=NULL)) {
        prev = line->prev;
        next = line->next;
        prev->next = next;
        next->prev = prev;
    }
    // If its not part of the existing list, add it to the end

    // Update most recently used
    prev_mru->next = line;
    set->mru_index = index;
    line->next = NULL;
    line->prev = prev_mru;
}