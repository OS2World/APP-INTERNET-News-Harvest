#ifndef __LEDGENDDLG_H__
#define __LEDGENDDLG_H__

#define INCL_PM
#define INCL_WINSYS
#define INCL_WINFRAMEMGR          
#define INCL_WINMENUS             
#define INCL_WINDIALOGS
#define INCL_WINSTDFILE
#define INCL_DOSPROCESS
#define INCL_WINPOINTERS
#define INCL_WINWINDOWMGR
#define INCL_WININPUT
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS
#define INCL_WINSTDSPIN
#define INCL_WINSTDBOOK
#define INCL_WINMLE
#define INCL_WINMENUS
#define INCL_WINSTATICS
#define INCL_WINSTDCNR
#define INCL_WINSTDSLIDER
#define INCL_WINSTDVALSET


#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef UNIX
#include <new.h>
#endif


#include "ledgend.h"
#include "strutils.h"
#include "container.h"

extern MRESULT EXPENTRY LedgendDlgProc(HWND,ULONG,MPARAM,MPARAM);
extern void message(char *format, ...);

#endif  // #ifndef __LEDGENDDLG_H__
