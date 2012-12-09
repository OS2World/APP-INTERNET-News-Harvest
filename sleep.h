/* sleep.h,v 1.2 2001/06/20 00:42:20 root Exp */

#ifndef __SLEPP_H__
#define __SLEPP_H__

#ifdef OS2
#define INCL_DOSPROCESS
#include <os2.h>
#endif

extern int running;		// Used by doSleep and others

// sleep functions

// sleep for <msec> milliseconds if running
void doSleep(long millisec);

// sleep for <msec> milliseconds
void msecSleep(unsigned long msec);

// sleep for <sec> seconds
void secSleep(unsigned long sec);



#endif // #ifndef __SLEPP_H__