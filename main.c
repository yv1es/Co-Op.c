#include <stdio.h>
#include "threads.h"


/* 
*  These are some example functions to test the threads. 
*/

int factorial(int n) {
    if (n < 1) {
        return 1; 
    }
    else {
        return n * factorial(n-1); 
    }
}

// prints all factorials up to an including n's factorial 
void print_factorials(int *n) {
    for (int i = 0; i <= *n; i++) {
        printf("[Thread %d] Factorial %d! = %d\n", current_thread_id(), i, factorial(i)); 


        // descheduel itself 
        if (i % 5 == 0) {
            thread_yield(); 
        }
    }

    printf("[Thread %d] Exited!\n", current_thread_id()); 
}

// prints the fibonacci numbers 
void print_fibonacci(int *n){    
    int a = 0; 
    int b = 1; 
    int c; 

    for (int i = 0; i < *n; i++) {
        printf("[Thread %d] Fibonaci Nr. %d! = %d\n", current_thread_id(), i, a); 
        c = a + b; 
        a = b; 
        b = c; 

        // descheduel itself 
        thread_yield(); 
    }

    printf("[Thread %d] Exited!\n", current_thread_id()); 
} 

void print_even(int *n) {
    for (int i = 0; i < *n; i+=2) {
        printf("[Thread %d] Even numbers %d\n", current_thread_id(), i); 

        // descheduel itself 
        if (i > *n/2) {
            thread_yield(); 
        }
    }

    printf("[Thread %d] Exited!\n", current_thread_id()); 
}




int main() {

    int arg0 = 10; 
    int arg1 = 15;
    int arg2 = 30; 


    // initialize threads 
    struct thread *t0 = thread_create((void (*)(void*))&print_factorials, &arg0);
    struct thread *t1 = thread_create((void (*)(void*))&print_fibonacci, &arg1);
    struct thread *t2 = thread_create((void (*)(void*))&print_even, &arg2);

    thread_add_runqueue(t0);
    thread_add_runqueue(t1);
    thread_add_runqueue(t2);

    thread_start_threading();
}