#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>


int thread2(void) {
    printf("Hello World\n"); 
    printf("Thread2 Finished\n");
    return 0;
}

int thread1(void) {
    uthread_create(thread2); 
    while (1) {
        
    }
    printf("Thread1 Finished\n");
    return 0;
}

int main(void) {
    uthread_t tid;

    uthread_start(1); 
    tid = uthread_create(thread1); 
    uthread_join(tid, NULL);
    return 0; 
}

