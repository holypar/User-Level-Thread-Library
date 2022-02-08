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
	uthread_t dependsOn;  
}; 

struct scheduler {
	int threadCounter; 
	queue_t readyQueue;  
	queue_t zombieQueue; 
	queue_t blockQueue; 
	struct thread* activeThread; 
};

struct scheduler scheduler;

/* Helper Functions */
static int findZombie(queue_t q, struct thread* thread, void *arg) {
	
	uthread_t argTid = (uthread_t)(long) arg;
	if (thread->tid == argTid) {
		return 1; 
	}
	return 0; 
}

static int findDependency(queue_t q, struct thread* thread, void *arg) {
	uthread_t argTid = (uthread_t)(long) arg;
	if (thread->dependsOn == argTid) {
		return 1; 
	}
	return 0; 
}

void freeThread(struct thread* threadPtr) {
	free(threadPtr->context); 
	uthread_ctx_destroy_stack(threadPtr->stackPointer);
	free(threadPtr);
}


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
	
	if (queue_length(scheduler.readyQueue) != 0) {
		/* Dequeue the thread from the ready queue as the next thread */
		struct thread* nextThread;
		 
		queue_dequeue(scheduler.readyQueue, &nextThread);
		nextThread->state = ACTIVE_STATE;

		/* Enqueue the current active thread to the back of the queue */
		scheduler.activeThread->state = READY_STATE; 
		queue_enqueue(scheduler.readyQueue, scheduler.activeThread); 
		
		
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
		
		/* Thread has finshed and needs become a zombie to be collected  */
		scheduler.activeThread->state = ZOMBIE_STATE;

		/* Set the return value into the thread and add to zombie queue*/
		scheduler.activeThread->returnValue = retval; 
		queue_enqueue(scheduler.zombieQueue, scheduler.activeThread); 

		/* We need to find what thread was waiting for this thread to complete */
		struct thread* blockedThread = NULL; 
		queue_iterate(scheduler.blockQueue, findDependency, (uthread_t*)scheduler.activeThread->tid, &blockedThread);
		
		if (blockedThread != NULL) {
			queue_delete(scheduler.blockQueue, blockedThread); 
			queue_enqueue(scheduler.readyQueue, blockedThread); 
		}

		struct thread* nextThread = NULL;
		queue_dequeue(scheduler.readyQueue, &nextThread);
		nextThread->state = ACTIVE_STATE;

		struct thread* activeThread = scheduler.activeThread; 
		scheduler.activeThread = nextThread; 

		uthread_ctx_switch(activeThread->context, nextThread->context); 
	}
	 
}





int uthread_join(uthread_t tid, int *retval)
{
	/* First check the zombie queue for a specific tid */
	struct thread* storage = NULL; 
	/* Go through the zombie queue and look for the tid, if it is there, it saves to storage */
	/* Otherwise, if it is not there, it does not save to storage */
	queue_iterate(scheduler.zombieQueue, findZombie, (uthread_t*)tid, &storage); 

	/* If there is something in the zombie queue, that means the thread finished */
	if (storage != NULL) {

		/* Set the return value, delete from zombiequeue, and free everything */
		*retval = storage->returnValue; 
		queue_delete(scheduler.zombieQueue, storage); 
		freeThread(storage); 
	} else {
		/* It is not in the zombie queue, we put the active thread into a blocking queue */
		scheduler.activeThread->state = BLOCK_STATE; 

		/* The active thread now depends on the tid given */
		scheduler.activeThread->dependsOn = tid; 

		/* Enqueues into the block */
		queue_enqueue(scheduler.blockQueue, scheduler.activeThread); 
		uthread_yield(); 
		
		/* The thread gets unblocked here */
		struct thread* newStorage = NULL; 

		/* Iterate through and find the thread that is finished */
		queue_iterate(scheduler.zombieQueue, findZombie, (uthread_t*)tid, &newStorage); 

		/* Set the return value, delete from zombiequeue, and free everything */
		*retval = newStorage->returnValue; 
		queue_delete(scheduler.zombieQueue, newStorage); 
		freeThread(newStorage);
	}
	
	return SUCCESS;
}
