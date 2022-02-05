#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "queue.h"
#include "private.h"
#include "uthread.h"

#define READY_STATE 0
#define ACTIVE_STATE 1
#define BLOCK_STATE 2
#define ZOMBIE_STATE 3   

struct thread {
	uthread_t tid; 
	uthread_ctx_t* context;
	void* stackPointer; 
	int state; 
}; 


struct scheduler {
	int threadCounter; 
	queue_t readyQueue;  
	struct thread* activeThread; 
};

struct scheduler scheduler;

int uthread_start(int preempt)
{
	
	queue_t readyQueue = queue_create();
	
	scheduler.readyQueue = readyQueue;
	scheduler.threadCounter = 1; 
	/*Creation of main thread */
	struct thread* mainThread = malloc(sizeof(struct thread)); 
	mainThread->tid = scheduler.threadCounter - 1;
	scheduler.activeThread = mainThread;

	return -1;
}

int uthread_stop(void)
{	
	if(queue_length(scheduler.readyQueue) == 0 && scheduler.activeThread->tid == 0){

		/* Free the current thread */
		free(scheduler.activeThread->context); 
		uthread_ctx_destroy_stack(scheduler.activeThread->stackPointer); 
		free(scheduler.activeThread); 

		/* Free the ready queue */
		queue_destroy(scheduler.readyQueue); 
		
		return 0;
	}
	
	/* TODO */
	return -1;
}

int uthread_create(uthread_func_t func)
{
	/* Creates new thread */
	struct thread* newThread = malloc(sizeof(struct thread));   
	void* topOfStack = uthread_ctx_alloc_stack();
	newThread->stackPointer = topOfStack;
	scheduler.threadCounter = scheduler.threadCounter + 1; 
	newThread->tid = scheduler.threadCounter - 1; 

	uthread_ctx_t* context = malloc(sizeof(uthread_ctx_t));
	uthread_ctx_init(context, newThread->stackPointer, func);
	newThread->context = context; 
	newThread->state = READY_STATE; 

	/* Enqueues the thread to the ready queue */
	queue_enqueue(scheduler.readyQueue, newThread);  
	
	return newThread->tid;

	/* TODO Handle Errors */
}

void uthread_yield(void)
{
	
	/* If the ready queue is not empty, push the current thread to the back of the queue and resume the next ready thread */
	if (queue_length(scheduler.readyQueue) != 0) {
		void* nextThread;
		queue_dequeue(scheduler.readyQueue, (void**)&nextThread);
		queue_enqueue(scheduler.readyQueue, scheduler.activeThread); 
		uthread_ctx_switch(scheduler.activeThread, nextThread); 
		scheduler.activeThread = nextThread; 
	} 

	/* TODO */
}

uthread_t uthread_self(void)
{
	return scheduler.activeThread->tid;
}

void uthread_exit(int retval)
{
	/* TODO */
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO */
	return -1;
}

