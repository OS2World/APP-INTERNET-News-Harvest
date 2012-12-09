#ifndef __SPLASH_H__
#define __SPLASH_H__

#define INCL_PM
#define INCL_WINSYS
#define INCL_WINDIALOGS
#define INCL_DOSPROCESS
#define INCL_WINPOINTERS
#define INCL_WINSTATICS
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef UNIX
#include <new.h>
#endif


#include "pmtools.h"
#include "splash.h"  // diaslog ids
#include "threads.h"

extern MRESULT EXPENTRY SplashDlgProc(HWND,ULONG,MPARAM,MPARAM);
extern void message(char *format,...);
extern int done_splash;


#endif
