#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define HZINMICRO 10000

/* Global variables for signals and timers */
struct sigaction signalAction, signalActionOld;
sigset_t signalSets;
struct itimerval timerOld, timerNew;

/* Handler to yield everytime there is a signal */
void alarmHandler(int signum) {
	(void) signum; 
	uthread_yield();
}

void preempt_start(void)
{
	sigemptyset(&signalSets);
  	sigaddset(&signalSets, SIGVTALRM);
	/* Set up handler for alarm */
	signalAction.sa_handler = alarmHandler;
	sigemptyset(&signalAction.sa_mask);
	signalAction.sa_flags = 0;
	sigaction(SIGVTALRM, &signalAction, &signalActionOld);
	
	preempt_disable(); 
	
	/* Setting up the timer */
  	timerNew.it_interval.tv_sec = 0;
	timerNew.it_value.tv_sec = 0; 
	timerNew.it_interval.tv_usec = HZINMICRO;
  	timerNew.it_value.tv_usec = HZINMICRO;
	setitimer(ITIMER_VIRTUAL, &timerNew, &timerOld);
}

void preempt_stop(void)
{
	/* Restoring the sigaction signals and timer */
	sigaction(SIGVTALRM, &signalActionOld, NULL); 
	setitimer(ITIMER_VIRTUAL, &timerOld, NULL);
}

void preempt_enable(void)
{
	/* Unblocking the SIGVTARLM signal */
	sigprocmask(SIG_UNBLOCK, &signalSets, NULL);
}

void preempt_disable(void)
{
	/* Blocking the SIGVTARLM signal */
	sigprocmask(SIG_BLOCK, &signalSets, NULL);
}

