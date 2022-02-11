#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>


/*  This tester checks whether our uthread library works as intended. It checks 
    it checks if every thread is having the correct states and are being sent to the queues needed. It features a thread creating a thread and calling join so the thread can be in its blocking state.

 */
int thread2(void) {
    printf("Thread 2 Finished\n");
    return 0;
}

int thread1(void) {
    uthread_t t2;
    t2 = uthread_create(thread2); 
    uthread_join(t2, NULL);
    printf("Thread 1 finished\n");
    return 0;
}

int main(void) {
    uthread_t t1;

    uthread_start(0);    
    t1 = uthread_create(thread1);
    uthread_join(t1, NULL);
    printf("This is main\n");
    uthread_stop(); 
    
    return 0;

}

