/*
 * Binghamton CS 451/551 Project "Memory manager".
 * This file needs to be turned in.	
 */


#include "memory_manager.h"

static STRU_MEM_LIST * mem_pool = NULL;

/*
 * Print out the current status of the memory manager.
 * Reading this function may help you understand how the memory manager organizes the memory.
 * Do not change the implementation of this function. It will be used to help the grading.
 */
void mem_mngr_print_snapshot(void)
{
    STRU_MEM_LIST * mem_list = NULL;

    printf("============== Memory snapshot ===============\n");

    mem_list = mem_pool; // Get the first memory list
    while(NULL != mem_list)
    {
        STRU_MEM_BATCH * mem_batch = mem_list->first_batch; // Get the first mem batch from the list 

        printf("mem_list %p slot_size %d batch_count %d free_slot_bitmap %p\n", 
                   mem_list, mem_list->slot_size, mem_list->batch_count, mem_list->free_slots_bitmap);
        bitmap_print_bitmap(mem_list->free_slots_bitmap, mem_list->bitmap_size);

        while (NULL != mem_batch)
        {
            printf("\t mem_batch %p batch_mem %p\n", mem_batch, mem_batch->batch_mem);
            mem_batch = mem_batch->next_batch; // get next mem batch
        }

        mem_list = mem_list->next_list;
    }

    printf("==============================================\n");
}

/*
 * Initialize the memory manager.
 * You may add your code related to initialization here if there is any.
 */
void mem_mngr_init(void)
{
    mem_pool = NULL;
}

/*
 * Clean up the memory manager (e.g., release all the memory allocated)
 */
void mem_mngr_leave(void)
{
	/* Add your code here */
}

/*
 * Allocate a chunk of memory 	
 */
void * mem_mngr_alloc(size_t size)
{
	/* Add you code here */
}

/*
 * Free a chunk of memory pointed by ptr
 */
void mem_mngr_free(void * ptr)
{
	/* Add your code here */
}