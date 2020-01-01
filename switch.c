#include <stdio.h>
#include <stdlib.h>

/* Stack buffer variables */
int *p_actual_buffer;
int *p_buffer;
int buffer_len;

/* Stack transfer variables */
int *p_stack_start;
int *p_stack_end;
int *old_rsp;

/**
 * @brief Swaps the stack of the previous function with the stack stored in the
 *        global buffer
 */
__attribute__((naked)) void swap_stack() {

    /* Get the value of base of stack of previous function */
    asm ("mov %%rbp, %0": "=r" (p_stack_start));

    /* Initialize the pointer to the global buffer */
    p_buffer = p_actual_buffer;

    /* Copy the stack contents of other function to the current stack */
    while (buffer_len--) {
        *p_stack_start-- = *p_buffer++;
    }

    /* Update the value of stack pointer */
    asm ("mov %0, %%rsp": : "r" (old_rsp));

    /* Return */
    asm ("ret");
}

/**
 * @brief Stores the stack of the previous function in the global buffer
 */
__attribute__((naked)) void store_stack() {

    /* Get the value of base of stack of previous function */
    asm ("mov %%rbp, %0": "=r" (p_stack_start));
    /* Get the value of top of stack of previous function */
    asm ("mov %%rsp, %0": "=r" (p_stack_end));
    asm ("mov %%rsp, %0": "=r" (old_rsp));

    /* Initialize the pointer to the global buffer */
    p_buffer = p_actual_buffer;

    /* Get the buffer length */
    buffer_len = (int)(p_stack_start - p_stack_end) + 1;

    /* Copy the contents of the caller function in the global stack buffer */
    while (p_stack_start >= p_stack_end) {
        *p_buffer++ = *p_stack_start--;
    }

    /* Return */
    asm ("ret");
}

/**
 * @brief Dummy function which stores its stack in the global buffer
 */
void func1() {

    /* Store the stack of this function at the given moment */
    store_stack();

    /* Debug print */
    printf("Inside function 1\n");

}

/**
 * @brief Dummy function which swaps its stack with the global buffer stack
 */
void func2() {

    /* Swap the stack of the current function with that in the global buffer */
    swap_stack();

    /* Debug print */
    printf("Inside function 2\n");
}

int main() {

    /* Allocate the stack buffer */
    p_actual_buffer = (int *)malloc(sizeof(int) * 512);

    /* Call function 1 */
    func1();

    /* Call function 2 */
    func2();

    return 0;
}

/**
 * The program must print the debug printf of function 1 twice.
 */
