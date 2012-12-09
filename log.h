#ifndef __xLOG_H__
#define __xLOG_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

#ifndef UNIX
#include <new.h>
#endif

 void log(char *format,...);
 void toggleLog(int x);
 void setLogfilePath(char *p);


#endif // #ifndef __xLOG_H__
