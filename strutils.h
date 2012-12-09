/* strutils.h,v 1.2 2001/06/20 00:42:20 root Exp */

#ifndef __STRUTILS_H__
#define __STRUTILS_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

#ifndef UNIX
#include <new.h>
#endif


#include "unlink.h"

int checkDir(char *path);
int makeDigitsOnly(char *buffer);
int makeNumberOnly(char *buffer);
int makeNewsGroupNameValidOnly(char *buffer);
int removeSpaces(char *buffer);
int lrSpaceTrim(char *buffer);
int makeLowerCase(char *buffer);
int makeUpperCase(char *buffer);
char *newStrDup(char *s);
char *trimCR(char *buffer);
char *stristr(char *x,char *y);
#ifdef UNIX
int stricmp(const char *s1,const char *s2);  // unix has no stricmp
#endif

int singleSpace(char *s);

#endif //#ifndef __STRUTILS_H__

