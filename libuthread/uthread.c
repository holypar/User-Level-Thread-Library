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
	int returnValue; 
}; 

struct scheduler {
	int threadCounter; 
	queue_t readyQueue;  
	queue_t zombieQueue; 
	queue_t blockQueue; 
	struct thread* activeThread; 
};

struct scheduler scheduler;

int uthread_start(int preempt)
{
	/* Initialize the queue */
	queue_t readyQueue = queue_create();
	queue_t zombieQueue = queue_create(); 
	queue_t blockQueue = queue_create(); 

	scheduler.readyQueue = readyQueue;
	scheduler.zombieQueue = zombieQueue; 
	scheduler.blockQueue = blockQueue; 
	scheduler.threadCounter = 1; 

	/*Creation of main thread */
	struct thread* mainThread = malloc(sizeof(struct thread)); 
	mainThread->tid = scheduler.threadCounter - 1;
	mainThread->state = ACTIVE_STATE; 
	mainThread->returnValue = NULL; 
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

		/* Free the queues */
		queue_destroy(scheduler.readyQueue); 
		queue_destroy(scheduler.blockQueue);
		queue_destroy(scheduler.zombieQueue);
		
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
	newThread->returnValue = NULL; 
	
	/* Enqueues the thread to the ready queue */
	queue_enqueue(scheduler.readyQueue, newThread);  
	
	return newThread->tid;

}

void uthread_yield(void)
{
	
	if (scheduler.activeThread->state == ZOMBIE_STATE) 
		queue_enqueue(scheduler.zombieQueue, scheduler.activeThread); 

	if (queue_length(scheduler.readyQueue) != 0) {
		/* Dequeue the thread from the ready queue as the next thread */
		struct thread* nextThread;
		 
		queue_dequeue(scheduler.readyQueue, &nextThread);
		nextThread->state = ACTIVE_STATE;

		/* Enqueue the current active thread to the back of the queue */
		if (scheduler.activeThread->state != ZOMBIE_STATE) {
			scheduler.activeThread->state = READY_STATE; 
			queue_enqueue(scheduler.readyQueue, scheduler.activeThread); 
		} 
		
		/* Context switches the context */
		struct thread* activeThread = scheduler.activeThread; 
		scheduler.activeThread = nextThread; 

		uthread_ctx_switch(activeThread->context, nextThread->context); 
		 
	} 

}

uthread_t uthread_self(void)
{
	return scheduler.activeThread->tid;
}

void uthread_exit(int retval)
{

	if (scheduler.activeThread->state == ACTIVE_STATE) {
		
		scheduler.activeThread->state = ZOMBIE_STATE;

		scheduler.activeThread->returnValue = retval; 
		uthread_yield(); 
	}
	 
}


int uthread_join(uthread_t tid, int *retval)
{
	// main calls the threadjoin -> we see oh okay the tid is in readyqueue ;
	uthread_yield(); 
	while(1) 
	{

		//is the ready queue -> we block ourself we wait until our tid is in the zombie queue so we can delete it
		//or its zombie queue 

		// Checks if it is in the zombie queue 
		// If it is, break 
		uthread_yield(); 
	}
	// It is in the zombie queue 
	/*
		Iterate through queue and get the data
		Save the return value 
		Unblock the parent 
		Delete the node in the zombie queue 
		uthread_yield(); 
	*/
	
	return -1;
}
