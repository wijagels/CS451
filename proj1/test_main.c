/*
 * Binghamton CS 451/551 Project "Memory manager".
 * You do not need to turn in this file.
 */

#include "interposition.h"
#include "memory_manager.h"

int main(int argc, char* argv[]) {
    // Your code
    unsigned char bitmap[] = {0xF7, 0xFF};
    bitmap_print_bitmap(bitmap, sizeof(bitmap));
    printf("%d\n", bitmap_find_first_bit(bitmap, sizeof(bitmap), 0));
    bitmap_set_bit(bitmap, sizeof(bitmap), 3);
    bitmap_print_bitmap(bitmap, sizeof(bitmap));
    bitmap_clear_bit(bitmap, sizeof(bitmap), 3);
    bitmap_print_bitmap(bitmap, sizeof(bitmap));
    printf("%d\n", bitmap_bit_is_set(bitmap, sizeof(bitmap), 3));

    mem_mngr_init();

    char* arr = malloc(10);
    arr = "helloworld";
    printf("%s\n", arr);

    // test your code here.

    mem_mngr_leave();

    return 0;
}
