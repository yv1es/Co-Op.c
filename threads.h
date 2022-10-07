#ifndef THREADS_H_
#define THREADS_H_
 
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdbool.h>



/*
*   This small module supports cooperative threading. 
*   This means a running thread needs to call thead_yield() to unscheduel itself.
*   
*   Use thread_create and supply a function which the thread should execute and its arguments to create a thread struct. 
*   Add the thread structs to the runqueue with thread_add_runqueue. Then call thread_start_threading. 
*
*   One can find an example in main.c 
*/


// stack size per thread (in 8 byte words) 
#define STACK_SIZE 512  // 4kB  

// macros to access %rpb and %rsp registers 
#define set_sp(r) asm("movq %0, %%rsp;" : : "r"((r)))
#define set_bp(r) asm("movq %0, %%rbp;" : : "r"((r)))

#define get_sp(r) asm( "mov %%rsp, %0" : "=rm" ((r)));
#define get_bp(r) asm( "mov %%rbp, %0" : "=rm" ((r)));


// struct that contains all information about a thread
struct thread {
    int id; 
    void (*f)(void *);
    void *arg; 
    uint64_t *stack;  // every thead has its own stack 
    uint64_t *bp;
    uint64_t *sp; 
    jmp_buf context; 
    bool run_before; 
    struct thread *next; 
};


struct thread *thread_create(void (*f)(void *), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);
int current_thread_id();

#endif // THREADS_H_
