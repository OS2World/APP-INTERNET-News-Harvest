#ifndef __HOSTLISTDLG_H__
#define __HOSTLISTDLG_H__

#define INCL_PM
#define INCL_WINSYS
#define INCL_WINFRAMEMGR          
#define INCL_WINMENUS             
#define INCL_WINDIALOGS
#define INCL_WINSTDFILE
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
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <process.h>

#ifndef UNIX
#include <new.h>
#endif



#include "hostlist.h"
#include "strutils.h"
#include "container.h"
#include "newsserver.h"
#include "newsgroup.h"
#include "header.h"
#include "help.h"

class ListThreadArgs
{
public:
    FILE *in;
    int   num_items;
    Container *list;
    HWND  dialog;
    int   stopping;
};


extern MRESULT EXPENTRY HostListDlgProc(HWND,ULONG,MPARAM,MPARAM);

#endif  // #ifndef __HOSTLISTDLG_H__
