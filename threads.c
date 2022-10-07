#include "threads.h"

// currently scheduled thread 
static struct thread *current_thread = NULL;

// thread that was scheduled before the current one 
static struct thread *last_executed_thread = NULL; 

// counter var to generate unique increasing thread ids 
static int thread_id_counter = 0; 


// create a thread struct given a target function to execute and its arguemnts 
struct thread *thread_create(void (*f)(void*), void *arg) {

    // allocate thread struct memmory
    struct thread *t =  (struct thread*)malloc(sizeof(struct thread));      
    if (!t) {
        exit(-1); 
    }

    // allocate the threads stack 
    t->stack = (uint64_t*)malloc(STACK_SIZE*sizeof(uint64_t)); 
    if (!t) {
        exit(-1); 
    }

    t->id = thread_id_counter++; 
    t->f = f; 
    t->arg = arg; 
    t->bp = t->stack + STACK_SIZE - 1; // the "beginning of the stack" is the highest address
    t->sp = t->stack + STACK_SIZE - 1; 
    t->run_before = false;  
    t->next = NULL; 
}


// add a thread struct into the circular linked list containing all thread structs
void thread_add_runqueue(struct thread *t) {
    
    if (!current_thread) {
        // the list is currently empty 
        current_thread = t; 
        t->next = t; 
        return; 
    }

    // insert t  
    t->next = current_thread->next; 
    current_thread->next = t; 
}


// a thread calls this function to descheduel itself 
void thread_yield() {
    int val = setjmp(current_thread->context);
    if (!val) { 
        schedule(); 
        dispatch(); 
    }
    // when val is true we returned from a different context and want to continue
    // execution of this thread. When val is false we switch threads. 
}


// scheduele just sets the next thread in the linked list as the current 
void schedule() {
    last_executed_thread = current_thread; 
    current_thread = current_thread->next; 
}



void dispatch() {

    // save sp bp of current thread 
    if (last_executed_thread) {

        uint64_t current_sp;
        get_sp(current_sp);
        
        uint64_t current_bp; 
        get_bp(current_bp);

        last_executed_thread->sp = (uint64_t*)current_sp; 
        last_executed_thread->bp = (uint64_t*)current_bp; 
    }
    

    if (!current_thread->run_before) {
        // this thread has never run before
        
        // overwrite stack and base pointer
        // to make the thread use its own stack 
        set_sp(current_thread->sp);
        set_bp(current_thread->bp);

        current_thread->run_before = true; 

        // invoke the target function 
        (current_thread->f)(current_thread->arg);
        thread_exit();
    }
    else {
        // when the thread has been running earlier the
        // context switch will also restore the stack pointers
        longjmp(current_thread->context, 1); 
    }

}

// this removes and destructs a thread 
void thread_exit() {

    // find thread before current_thread
    struct thread *t = current_thread;
    while (t->next != current_thread) {
        t = t->next; 
    }

    // remove current_thread from list 
    t->next = current_thread->next; 

    // free memmory allocations 
    free(current_thread->stack); 
    free(current_thread); 
    
    if (current_thread == t->next) {
        // this was the last existing thread removed 
        current_thread = NULL; 
        last_executed_thread = NULL; 

        // exit point after the last thread finished execution 
        exit(0);
    }
    else {
        // dispatch the next thread in the list 
        current_thread = t->next;
        dispatch();
    }
}


void thread_start_threading() {
    schedule();
    dispatch(); 
}

int current_thread_id() {
    return current_thread->id; 
}

