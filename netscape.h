#ifndef __NETSCAPE_H__
#define __NETSCAPE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

extern void updateNetscapeNews(char *netscape_path,char *server,char *group,unsigned long article);
extern void log(char *format,...);

#endif  //#ifndef __NETSCAPE_H__

