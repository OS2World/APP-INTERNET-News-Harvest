#ifndef __GROUPDLG_H__
#define __GROUPDLG_H__

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
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include "newgroup.h"
#include "groupdlg.h"

#ifndef UNIX
#include <new.h>
#endif


#include "strutils.h"

#define MAX_GROUP_NAME_LEN 4096

extern MRESULT EXPENTRY GroupDlgProc(HWND,ULONG,MPARAM,MPARAM);
extern void message(char *format, ...);
extern void exportGroupName(char *s);
extern void importGroupName(char *s);
extern void exportGroupNumber(long n);
extern long importGroupNumber(void);
extern void exportGroupCull(long n);
extern long importGroupCull(void);
extern int  makeDigitsOnly(char *);
extern void exportGroupUsername(char *s);
extern void exportGroupPassword(char *s);

#define ABS(x) (x<0?-x:x)

#endif  // #ifndef __GROUPDLG_H__
