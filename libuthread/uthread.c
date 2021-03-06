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
#define FOUND 1
#define NOTFOUND 0

/* Struct for the thread */
struct thread {
	uthread_t tid; 
	uthread_ctx_t* context;
	void* stackPointer; 
	int state; 
	int returnValue;
	uthread_t dependsOn;  
}; 

/* Struct for the scheduler */
struct scheduler {
	int preempt; 
	int threadCounter; 
	queue_t readyQueue;  
	queue_t zombieQueue; 
	queue_t blockQueue; 
	struct thread* activeThread; 
};

/* Defining a global scheduler shared across the threads */
struct scheduler scheduler;

/* Helper Functions */

/* Function to queue iterate through a zombie queue and find a specific tid */
static int findZombie(queue_t q, void* thread, void *arg) {

	struct thread* newThread = (struct thread*)thread; 
	uthread_t argTid = (uthread_t)(long) arg;
	(void)q; 

	/* 
	If we find the tid matching the a thread in the zombie queue, 
	we return back to queue iterate and let the function do the rest 
	*/
	if (newThread->tid == argTid) {
		return FOUND; 
	}
	return NOTFOUND; 
}

/* Function to queue iterate through a blocked queue and find a specific tid */
static int findDependency(queue_t q, void* thread, void *arg) {

	struct thread* newThread = (struct thread*)thread; 
	uthread_t argTid = (uthread_t)(long) arg;
	(void)q; 

	/* 
	If we find the dependency matching the a thread in the blocked queue, 
	we return back to queue iterate and let the function do the rest 
	*/
	if (newThread->dependsOn == argTid) {
		return FOUND; 
	}
	return NOTFOUND; 
}

/* Function to free a thread */
void freeThread(struct thread* threadPtr) {
	free(threadPtr->context); 
	uthread_ctx_destroy_stack(threadPtr->stackPointer);
	free(threadPtr);
}

/* Function to free a thread */
void initializeQueues(void) {

	/* Initialize the queue */
	queue_t readyQueue = queue_create();
	queue_t zombieQueue = queue_create(); 
	queue_t blockQueue = queue_create(); 

	/* Sets the queues in the scheduler */
	scheduler.readyQueue = readyQueue;
	scheduler.zombieQueue = zombieQueue; 
	scheduler.blockQueue = blockQueue; 
	scheduler.threadCounter = 1; 

}

void createMainThread(void) {
	/*Creation of main thread and set default values */
	struct thread* mainThread = malloc(sizeof(struct thread)); 
	mainThread->context = malloc(sizeof(uthread_ctx_t));
	mainThread->stackPointer = uthread_ctx_alloc_stack();
	mainThread->tid = scheduler.threadCounter - 1;
	mainThread->state = ACTIVE_STATE; 
	scheduler.activeThread = mainThread;
}

void blockActiveThread(uthread_t dependsOn) {
	/* It is not in the zombie queue, we put the active thread into a blocking queue */
	scheduler.activeThread->state = BLOCK_STATE; 

	/* The active thread now depends on the tid given */
	scheduler.activeThread->dependsOn = dependsOn; 

	/* Enqueues into the block */
	queue_enqueue(scheduler.blockQueue, scheduler.activeThread); 
}

void scheduleNextThread(void) {
	/* Schedules the next thread to be run */
	struct thread* nextThread = NULL;
	queue_dequeue(scheduler.readyQueue, (void**)&nextThread);
	nextThread->state = ACTIVE_STATE;	

	struct thread* activeThread = scheduler.activeThread; 
	scheduler.activeThread = nextThread; 
	
	/* Enable preemption before context switching */
	if (scheduler.preempt)
		preempt_enable();

	uthread_ctx_switch(activeThread->context, nextThread->context); 

}

void freeActiveThread(void) {
	/* Free the current thread */
	free(scheduler.activeThread->context); 
	uthread_ctx_destroy_stack(scheduler.activeThread->stackPointer); 
	free(scheduler.activeThread); 
}

void freeQueues(void) {
	/* Free the queues */
	queue_destroy(scheduler.readyQueue); 
	queue_destroy(scheduler.blockQueue);
		
	/* 
	If there is something in the zombie queue, 
	we free all of the nodes first 
	*/
	struct thread* someThread; 
	while (queue_destroy(scheduler.zombieQueue)) {
		queue_dequeue(scheduler.zombieQueue, (void**)&someThread);
		freeThread(someThread); 
	}
		
}

int uthread_start(int preempt)
{
	scheduler.preempt = 0; 
	if (preempt) {
		scheduler.preempt = preempt; 
		preempt_start();
		preempt_disable();
	} 
	
	/* See helper functions */
	initializeQueues(); 
	
	createMainThread(); 

	if (scheduler.preempt) 
		preempt_enable(); 

	return SUCCESS;
}

int uthread_stop(void)
{	
	if (scheduler.preempt) 
		preempt_disable(); 

	if(queue_length(scheduler.readyQueue) == 0 
	&& scheduler.activeThread->tid == 0) {

		/* See helper functions */
		freeActiveThread(); 

		freeQueues(); 
		
		if (scheduler.preempt)
			preempt_stop(); 

		return SUCCESS;
	}
	
	return ERROR;
}

int uthread_create(uthread_func_t func)
{
	
	if (scheduler.preempt)
		preempt_disable(); 
	
	/* Creates new thread struct */
	struct thread* newThread = malloc(sizeof(struct thread));
	if (newThread == NULL)
		return ERROR;    
	
	/* Creates stack pointer */
	void* topOfStack = uthread_ctx_alloc_stack();
	if (topOfStack == NULL)
		return ERROR; 
	
	/* Creates context */
	uthread_ctx_t* context = malloc(sizeof(uthread_ctx_t));
	if (context == NULL)
		return ERROR;

	/* Set the default values */
	newThread->stackPointer = topOfStack;
	scheduler.threadCounter = scheduler.threadCounter + 1; 
	newThread->tid = scheduler.threadCounter - 1; 

	/* Initializes contexts and stores it */
	uthread_ctx_init(context, newThread->stackPointer, func);
	newThread->context = context; 
	newThread->state = READY_STATE; 
	
	/* Enqueues the thread to the ready queue */
	queue_enqueue(scheduler.readyQueue, newThread);  
	
	if (scheduler.preempt)
		preempt_enable(); 

	return newThread->tid;

}

void uthread_yield(void)
{
	/* Enqueues the thread to the ready queue */
	if (queue_length(scheduler.readyQueue) != 0) {
		
		if (scheduler.preempt)
			preempt_disable();

		struct thread* nextThread;
		/* Dequeue the thread from the ready queue as the next thread */
		queue_dequeue(scheduler.readyQueue, (void**)&nextThread);
		nextThread->state = ACTIVE_STATE;

		/* Enqueue the current active thread to the back of the queue */
		scheduler.activeThread->state = READY_STATE; 
		queue_enqueue(scheduler.readyQueue, scheduler.activeThread); 
		
		/* Save the active thread and set it before context switching */
		struct thread* activeThread = scheduler.activeThread; 
		scheduler.activeThread = nextThread; 

		/* Before context switching, we can turn preemption back on */
		if (scheduler.preempt)
			preempt_enable();
		
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
		
		if (scheduler.preempt)
			preempt_disable();
		
		/* Thread has finshed and needs become a zombie to be collected  */
		scheduler.activeThread->state = ZOMBIE_STATE;

		/* Set the return value into the thread and add to zombie queue*/
		scheduler.activeThread->returnValue = retval; 
		queue_enqueue(scheduler.zombieQueue, scheduler.activeThread); 

		/* We need to find what thread was waiting for this thread to complete */
		struct thread* blockedThread = NULL; 
		queue_iterate(scheduler.blockQueue, findDependency, 
			(void*)((long) scheduler.activeThread->tid), 
			(void**)&blockedThread);
		
		/* If a thread was waiting on us, we send it back to the ready queue */
		if (blockedThread != NULL) {
			queue_delete(scheduler.blockQueue, blockedThread); 
			queue_enqueue(scheduler.readyQueue, blockedThread); 
		}

		scheduleNextThread(); 
	}
}

int uthread_join(uthread_t tid, int *retval)
{
	if (scheduler.preempt)
		preempt_disable();
	/* First check the zombie queue for a specific tid */
	struct thread* storage = NULL; 
	/* Go through the zombie queue and look for the tid, if it is there, it saves to storage */
	/* Otherwise, if it is not there, it does not save to storage */
	queue_iterate(scheduler.zombieQueue, findZombie, (void*)((long) tid), (void**)&storage); 

	/* If there is something in the zombie queue, that means the thread finished */
	if (storage != NULL) {

		/* Set the return value, delete from zombiequeue, and free everything */
		*retval = storage->returnValue; 
		queue_delete(scheduler.zombieQueue, storage); 
		freeThread(storage); 
	} else {
		
		blockActiveThread(tid); 

		scheduleNextThread(); 
			
		if (scheduler.preempt)
			preempt_disable();	

		/* The thread gets unblocked here */
		struct thread* newStorage = NULL; 

		/* Iterate through and find the thread that is finished */
		queue_iterate(scheduler.zombieQueue, findZombie, (void*)((long) tid), (void**)&newStorage); 
		
		/* Set the return value, delete from zombiequeue, and free everything */
		if (retval != NULL) 
			*retval = newStorage->returnValue; 
		queue_delete(scheduler.zombieQueue, newStorage); 
		freeThread(newStorage);

		if (scheduler.preempt)
			preempt_enable();
	}
	
	return SUCCESS;
}
