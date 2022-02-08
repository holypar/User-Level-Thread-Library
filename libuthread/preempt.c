#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"


struct sigaction sa;
sigset_t ss;
struct itimerval timerOld, timerNew;

void alarmHandler(void) {
	uthread_yield();
}

// setup alarm handler


/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define HZINMICRO 10000

void preempt_start(void)
{
	sigemptyset(&ss);
  	sigaddset(&ss, SIGVTALRM);
	/* Set up handler for alarm */
	sa.sa_handler = alarmHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGVTALRM, &sa, NULL);
	
	sigprocmask(SIG_BLOCK, &ss, NULL);
	
  	timerNew.it_interval.tv_sec = 0;
	timerNew.it_value.tv_sec = 0; 
	timerNew.it_interval.tv_usec = HZINMICRO;
  	timerNew.it_value.tv_usec = HZINMICRO;
	setitimer(ITIMER_VIRTUAL, &timerNew, &timerOld);
}

void preempt_stop(void)
{
	/* TODO */
}

void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &ss, NULL);
}

