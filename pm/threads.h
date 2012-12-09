/* threads.h,v 1.2 2001/06/20 00:42:43 root Exp */

#ifndef __THREADS_H__
#define __THREADS_H__

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
#define INCL_WINSTDDRAG
#define INCL_WINSTDSLIDER
#define INCL_WINSTDVALSET
#define INCL_WINACCELERATORS
#define INCL_WINWORKPLACE

#include <os2.h>
#include <process.h>
#include "container.h"
#include "config.h"

class ThreadThings
{
public:
    HWND hwnd;
    HWND dialog;
    Container *tree;
    Configuration *config;
};

#ifndef SLAINC // SHL 12 Jul 00
extern int  _Optlink _beginthread( void ( * _Optlink __thread )( void * ), void *, unsigned, void * );
extern void _Optlink _endthread( void );
#endif // SHL

#endif //__THREADS_H__