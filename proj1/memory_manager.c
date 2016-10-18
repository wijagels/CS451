/*
 * Binghamton CS 451/551 Project "Memory manager".
 * This file needs to be turned in.
 */

#include "memory_manager.h"

static STRU_MEM_LIST* mem_pool = NULL;

static void free_pool(STRU_MEM_LIST*);
static void free_batch(STRU_MEM_BATCH*);
static STRU_MEM_LIST* mem_list_ctor(int);
static STRU_MEM_BATCH* mem_batch_ctor(int);

/*
 * Print out the current status of the memory manager.
 * Reading this function may help you understand how the memory manager
 * organizes the memory.
 * Do not change the implementation of this function. It will be used to help
 * the grading.
 */
void mem_mngr_print_snapshot(void) {
    STRU_MEM_LIST* mem_list = NULL;

    printf("============== Memory snapshot ===============\n");

    mem_list = mem_pool;  // Get the first memory list
    while (NULL != mem_list) {
        STRU_MEM_BATCH* mem_batch =
            mem_list->first_batch;  // Get the first mem batch from the list

        printf("mem_list %p slot_size %d batch_count %d free_slot_bitmap %p\n",
               mem_list, mem_list->slot_size, mem_list->batch_count,
               mem_list->free_slots_bitmap);
        bitmap_print_bitmap(mem_list->free_slots_bitmap, mem_list->bitmap_size);

        while (NULL != mem_batch) {
            printf("\t mem_batch %p batch_mem %p\n", mem_batch,
                   mem_batch->batch_mem);
            mem_batch = mem_batch->next_batch;  // get next mem batch
        }

        mem_list = mem_list->next_list;
    }

    printf("==============================================\n");
}

/*
 * Initialize the memory manager.
 * You may add your code related to initialization here if there is any.
 */
void mem_mngr_init(void) { mem_pool = NULL; }

/*
 * Clean up the memory manager (e.g., release all the memory allocated)
 */
void mem_mngr_leave(void) { free_pool(mem_pool); }

/*
 * Recursively destroy a memory list
 */
static void free_pool(STRU_MEM_LIST* pool) {
    if (!pool) return;
    free_batch(pool->first_batch);
    free_pool(pool->next_list);
    free(pool->free_slots_bitmap);
    free(pool);
}

/*
 * Recursively destroy a list of memory batches
 */
static void free_batch(STRU_MEM_BATCH* batch) {
    if (!batch) return;
    free_batch(batch->next_batch);
    free(batch->batch_mem);
    free(batch);
}

/*
 * Allocate and initialize a memory pool with a bitmap and single batch
 */
static STRU_MEM_LIST* mem_list_ctor(int slot_size) {
    STRU_MEM_LIST* list = malloc(sizeof(STRU_MEM_LIST));
    list->slot_size = slot_size;
    list->batch_count = 1;
    list->next_list = NULL;
    list->first_batch = mem_batch_ctor(slot_size);
    list->free_slots_bitmap = calloc(1, sizeof(unsigned char) * BITMAP_MULT);
    list->bitmap_size = 1;
    return list;
}

/*
 * Allocate and initialize a memory batch
 */
static STRU_MEM_BATCH* mem_batch_ctor(int slot_size) {
    STRU_MEM_BATCH* batch = malloc(sizeof(STRU_MEM_BATCH));
    batch->batch_mem = calloc(MEM_BATCH_SLOT_COUNT, slot_size);
    batch->next_batch = NULL;
    return batch;
}

/*
 * Allocate a chunk of memory
 */
void* mem_mngr_alloc(size_t size) {
    STRU_MEM_LIST* last = NULL;
    STRU_MEM_LIST* pool = mem_pool;
    size = SLOT_ALIGNED_SIZE(size); /* We don't care about the original size */
    while (pool) {
        /* Seek until we encounter an acceptable slot size */
        if ((int)size == pool->slot_size) {
            break;
        }
        last = pool;
        pool = pool->next_list;
    }
    if (!mem_pool) {
        /* No head */
        mem_pool = mem_list_ctor(size);
        pool = mem_pool;
    } else if (!pool) {
        /* Need to append a new pool */
        last->next_list = mem_list_ctor(size);
        pool = last->next_list;
    }
    int idx =
        bitmap_find_first_bit(pool->free_slots_bitmap, pool->bitmap_size, 0);
    if (idx == BITMAP_OP_NOT_FOUND) {
        ++pool->batch_count;
        unsigned char* new_bitmap =
            calloc(pool->batch_count, sizeof(unsigned char) * BITMAP_MULT);
        /* Grow the bitmap starting from index 0 */
        memcpy(new_bitmap + (MEM_BATCH_SLOT_COUNT / BIT_PER_BYTE),
               pool->free_slots_bitmap, pool->batch_count - 1);
        free(pool->free_slots_bitmap);
        pool->free_slots_bitmap = new_bitmap;
        pool->bitmap_size += BITMAP_MULT;
        /* Push to the front because I'm lazy */
        STRU_MEM_BATCH* new_batch = mem_batch_ctor(size);
        new_batch->next_batch = pool->first_batch;
        pool->first_batch = new_batch;
        idx = bitmap_find_first_bit(pool->free_slots_bitmap, pool->bitmap_size,
                                    0);
    }
    STRU_MEM_BATCH* batch = pool->first_batch;
    for (int i = 0; i < idx / MEM_BATCH_SLOT_COUNT; i++) {
        batch = batch->next_batch;
    }
    bitmap_set_bit(pool->free_slots_bitmap, pool->bitmap_size, idx);
    return batch->batch_mem + (size * (idx % MEM_BATCH_SLOT_COUNT));
}

/*
 * Free a chunk of memory pointed by ptr
 */
void mem_mngr_free(void* ptr) { /* Add your code here */
    STRU_MEM_LIST* pool = mem_pool;
    while (pool) {
        STRU_MEM_BATCH* batch = pool->first_batch;
        size_t index = 0;
        while (batch) {
            size_t slot_sz = pool->slot_size;
            void* upper = batch->batch_mem + MEM_BATCH_SLOT_COUNT * slot_sz;
            if (ptr >= batch->batch_mem && ptr < upper) {
                index += (ptr - batch->batch_mem) / slot_sz;
                fprintf(stderr, "%d, %d\n", index,
                        bitmap_bit_is_set(pool->free_slots_bitmap,
                                          pool->bitmap_size, index));
                if (1 != bitmap_bit_is_set(pool->free_slots_bitmap,
                                           pool->bitmap_size, index)) {
                    fprintf(stderr,
                            "Double free of %p detected, fix your code\n", ptr);
                    exit(1);
                }
                bitmap_clear_bit(pool->free_slots_bitmap, pool->bitmap_size,
                                 index);
                return;
            }
            index += MEM_BATCH_SLOT_COUNT;
            batch = batch->next_batch;
        }
        pool = pool->next_list;
    }
    fprintf(stderr, "Attempt to free memory that was not allocated at %p\n",
            ptr);
}
