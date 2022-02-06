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
#define SUCCESS 0
#define ERROR -1 

struct thread {
	uthread_t tid; 
	uthread_ctx_t* context;
	void* stackPointer; 
	int state; 
}; 

struct scheduler {
	int threadCounter; 
	queue_t readyQueue;  
	queue_t zombieQueue; 
	struct thread* activeThread; 
};

struct scheduler scheduler;

int uthread_start(int preempt)
{
	/* Initialize the queue */
	queue_t readyQueue = queue_create();
	queue_t zombieQueue = queue_create(); 

	scheduler.readyQueue = readyQueue;
	scheduler.zombieQueue = zombieQueue; 
	scheduler.threadCounter = 1; 

	/*Creation of main thread */
	struct thread* mainThread = malloc(sizeof(struct thread)); 
	mainThread->tid = scheduler.threadCounter - 1;
	scheduler.activeThread = mainThread;

	return SUCCESS;
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
		
		return SUCCESS;
	}
	
	return ERROR;
}

int uthread_create(uthread_func_t func)
{
	/* Creates new thread struct */
	struct thread* newThread = malloc(sizeof(struct thread));
	if (newThread == NULL)
		return ERROR;    
	
	/* Creates stack pointer */
	void* topOfStack = uthread_ctx_alloc_stack();
	if (topOfStack == NULL)
		return ERROR; 
	newThread->stackPointer = topOfStack;
	scheduler.threadCounter = scheduler.threadCounter + 1; 
	newThread->tid = scheduler.threadCounter - 1; 

	/* Creates context */
	uthread_ctx_t* context = malloc(sizeof(uthread_ctx_t));
	if (context == NULL)
		return ERROR;
	
	/* Initializes contexts and stores it */
	uthread_ctx_init(context, newThread->stackPointer, func);
	newThread->context = context; 
	newThread->state = READY_STATE; 

	/* Enqueues the thread to the ready queue */
	queue_enqueue(scheduler.readyQueue, newThread);  
	
	return newThread->tid;

}

void uthread_yield(void)
{
	
	/* If the ready queue is not empty, push the current thread to the back of the queue and resume the next ready thread */
	if (queue_length(scheduler.readyQueue) != 0) {
		/* Dequeue the thread from the ready queue as the next thread */
		struct thread* nextThread;
		queue_dequeue(scheduler.readyQueue, &nextThread);

		/* Enqueue the current active thread to the back of the queue */
		queue_enqueue(scheduler.readyQueue, scheduler.activeThread); 

		/* Context switches the context */
		uthread_ctx_switch(scheduler.activeThread->context, nextThread->context); 
		scheduler.activeThread = nextThread; 
	} 

}

uthread_t uthread_self(void)
{
	return scheduler.activeThread->tid;
}

void uthread_exit(int retval)
{
	scheduler.activeThread->state = ZOMBIE_STATE; 
	//enque the active thread into the zombiequeue
	//save return value somehow 
	
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO */
	// Understand how to get the return value 
	// Decide whether to use a zombie queue or a zombie state 
 	while(1)
	{
		if (scheduler.activeThread->state == ZOMBIE_STATE) 
			break; 
		
	}
	
	return -1;
}

