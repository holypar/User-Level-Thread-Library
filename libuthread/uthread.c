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
	queue_t readyQueue; 
	queue_t blockQueue; 
	queue_t zombieQueue; 
	int activeThreadId;
};

int uthread_start(int preempt)
{
	
	queue_t readyQueue = queue_create();
	queue_t blockQueue = queue_create();
	queue_t zombieQueue = queue_create();
	struct scheduler* scheduler = malloc(sizeof( struct scheduler)); 
	
	scheduler->readyQueue = readyQueue;
	scheduler->blockQueue = blockQueue;
	scheduler->zombieQueue = zombieQueue;
	
	/*Creation of main thread */
	struct thread* mainThread = malloc(sizeof(struct thread)); 
	mainThread->tid = 0;
	scheduler->activeThreadId = mainThread->tid;

	/* TODO */
	return -1;
}

int uthread_stop(void)
{
	
	/* TODO */
	return -1;
}

int uthread_create(uthread_func_t func)
{
	struct thread* newThread = malloc(sizeof(struct thread));   
	void* topOfStack = uthread_ctx_alloc_stack();
	newThread->stackPointer = topOfStack;
	// if there are no zombie process then we can create a new thread only. https://www.geeksforgeeks.org/zombie-processes-prevention/
	/* once that if statement is checked you can now create a new thread and make its its threadid = activeThreadId++
	initilize context 
	return the id.
	*/
	struct scheduler* scheduler = malloc(sizeof( struct scheduler));     //i think i need to make this struct global cause im pretty sure the scheduler is empty here again cause its fresh memory.

	if (queue_length(scheduler->zombieQueue == 0)){
		newThread->tid = scheduler->activeThreadId++;
		uthread_ctx_t* context = malloc(sizeof(uthread_ctx_t));
		uthread_ctx_init(context,newThread->stackPointer,func);	
		queue_enqueue(scheduler->readyQueue,newThread);  //adds the new created thread to the ready queue
		return newThread->tid;

	}


	return -1;
}

void uthread_yield(void)
{
	/* TODO */
}

uthread_t uthread_self(void)
{
	/* TODO */
	return -1;
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

