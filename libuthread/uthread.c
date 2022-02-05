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
	uthread_ctx_t* context = malloc(sizeof(uthread_ctx_t));
	mainThread->stackPointer = uthread_ctx_alloc_stack();
	uthread_ctx_init(context, mainThread->stackPointer, main()); 
	mainThread->context = context; 
	mainThread->state = READY_STATE;
	queue_enqueue(scheduler->readyQueue, mainThread);

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
	/* TODO */
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

