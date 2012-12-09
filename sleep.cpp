/* sleep.cpp,v 1.2 2001/06/20 00:42:20 root Exp */

#include "sleep.h"

int running;

//=== doSleep: Sleep in 100 mSec increments, unless the user requests halt ===

void doSleep(long millisec)
{
#ifdef OS2
    for (;millisec > 0 && running; millisec -= 100)
        DosSleep(millisec > 100 ? 100 : millisec);
#endif
}

void msecSleep(unsigned long msec)
{
#ifdef OS2
    DosSleep(msec);
#endif
}

void secSleep(unsigned long sec)
{
#ifdef OS2
    DosSleep(1000L*sec);
#endif
}

