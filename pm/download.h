
/***********************************************************************

download.h,v 1.4 2002/01/05 22:52:33 root Exp

download class

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2001 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	30 Nov 01 SHL - Comments.  Move ExtractPack

***********************************************************************/


#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__

#define INCL_PM
#define INCL_WINSYS
#define INCL_WINFRAMEMGR          
#define INCL_WINMENUS             
#define INCL_WINDIALOGS
#define INCL_WINSTDFILE
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
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
#define INCL_WINACCELERATORS

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <direct.h>


#ifndef UNIX
#include <new.h>
#endif


#include "container.h"
#include "config.h"
#include "nsdlg.h"
#include "decoder.h"
#include "newsserver.h"
#include "newsgroup.h"
#include "header.h"
#include "filestat.h"
#include "multipart.h"
#include "strutils.h"
#include "netscape.h"
#include "filelist.h"
#include "pmtools.h"
#include "obj_string.h"
#include "unlink.h"

#define DL_FINISHED		WM_USER+1   // finished ok
#define DL_NOSPACE		WM_USER+2   // no space left on outout device
#define DL_SOCKET		WM_USER+3   // some sort of net error
#define DL_TERM			WM_USER+4   // user terminated

#define DL_NEWFILE		WM_USER+10  // a new file downloaded
#define DL_SAVESTATE		WM_USER+11  // should save the config file

#define DL_SET_TITLE		WM_USER+12  // Set window title

#define DL_SHOW_SERVER		WM_USER+13
#define DL_SHOW_GROUP		WM_USER+14
#define DL_SHOW_ARTICLE		WM_USER+15
#define DL_SHOW_SUBJECT		WM_USER+16
#define DL_SHOW_LINES		WM_USER+17
#define DL_SHOW_PERCENT		WM_USER+18
#define DL_SHOW_SPEED		WM_USER+19

#define DL_SHOW_SEXTRACT	WM_USER+20
#define DL_SHOW_SREPOST		WM_USER+21
#define DL_SHOW_SCULL		WM_USER+22
#define DL_SHOW_SFILTER		WM_USER+23
#define DL_SHOW_SEXTRACT_MB	WM_USER+24
#define DL_SHOW_SREPOST_MB	WM_USER+25
#define DL_SHOW_SCULL_MB	WM_USER+26
#define DL_SHOW_SFILTER_MB	WM_USER+27

#define DL_SHOW_GEXTRACT	WM_USER+28
#define DL_SHOW_GREPOST		WM_USER+29
#define DL_SHOW_GCULL		WM_USER+30
#define DL_SHOW_GFILTER		WM_USER+31
#define DL_SHOW_GEXTRACT_MB	WM_USER+32
#define DL_SHOW_GREPOST_MB	WM_USER+33
#define DL_SHOW_GCULL_MB	WM_USER+34
#define DL_SHOW_GFILTER_MB	WM_USER+35

// command line arguments
extern int looping;
extern int looping_times;

extern void message(char *format, ...);


#ifndef SLAINC // SHL 12 Jul 00
// def. for standard thread funcs
extern int  _Optlink _beginthread( void ( * _Optlink __thread )( void * ), void *, unsigned, void * );
extern void _Optlink _endthread( void );
#endif // SHL

// universal user message
void message(char *format, ...);

// class to pass arguments between MainDlgProc and Download thread
// thread can have only one parameter, so we pass a pointer to one of these.
class DownloadArgs
{
public:
    HWND parent;
    Container *tree;
    Configuration *config;
    UserData *switch_to;	// Set by MainDlg to requests newsgroup switch
    int finish_up;		// Set by MainDlg to clean stop on current operation
};

// our main man
extern void _Optlink DownloadThread(void *arg);
extern void writeLastGroup(char *server,char *group);

#endif //#ifndef __DOWNLOAD_H__
