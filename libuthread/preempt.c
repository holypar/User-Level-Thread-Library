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

struct sigaction signalAction, signalActionOld;
sigset_t signalSets;
struct itimerval timerOld, timerNew;

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
	
	sigprocmask(SIG_BLOCK, &signalSets, NULL);
	
  	timerNew.it_interval.tv_sec = 0;
	timerNew.it_value.tv_sec = 0; 
	timerNew.it_interval.tv_usec = HZINMICRO;
  	timerNew.it_value.tv_usec = HZINMICRO;
	setitimer(ITIMER_VIRTUAL, &timerNew, &timerOld);
}

void preempt_stop(void)
{
	sigaction(SIGVTALRM, &signalActionOld, NULL); 
	setitimer(ITIMER_VIRTUAL, &timerOld, NULL);
}

void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &signalSets, NULL);
}

void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &signalSets, NULL);
}

