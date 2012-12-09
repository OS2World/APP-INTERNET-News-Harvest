
/***********************************************************************

help.cpp,v 1.4 2002/01/05 22:52:05 root Exp

Help Support

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2001 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	22 Jun 01 SHL - Drop excess delete[]

***********************************************************************/

#include <stdlib.h>

#include "help.h"

static HWND help_window;
static int helpOK;
static char *pszHelpPath;

extern char programDir[];

// initialize the help panel for the main conversion panel
// Return 1 if OK
int helpInit(HAB *hab,HWND *hwnd)
{

    HELPINIT hi;
 
    hi.cb = sizeof(HELPINIT);
    hi.ulReturnCode = 0;
 
    hi.pszTutorialName = (PSZ)NULL;
    hi.phtHelpTable = (HELPTABLE*)MAKELONG(HELP_TABLE,0xFFFF);
    hi.hmodHelpTableModule = 0;
    hi.hmodAccelActionBarModule = 0;
    hi.idAccelTable = 0;
    hi.idActionBar = 0;
 
    hi.pszHelpWindowTitle = HELP_TITLE;
    hi.fShowPanelId = CMIC_HIDE_PANEL_ID;

    pszHelpPath = new char[_MAX_PATH];
    _makepath(pszHelpPath, "", programDir, HELP_FILE, "");
    hi.pszHelpLibraryName = pszHelpPath;
 
    help_window = WinCreateHelpInstance(*hab,&hi);
 
    // did it work?
    if (!help_window ||
        hi.ulReturnCode ||
        !WinAssociateHelpInstance(help_window,*hwnd))
    {
	delete[] pszHelpPath;
	pszHelpPath = 0;
        return 0;
    }
 
    helpOK = 1;	 
    return 1;			// Say OK

} // helpInit

// close down the help facility
void killHelp(void)
{

    if (helpOK)
    {
        WinDestroyHelpInstance(help_window);
	delete[] pszHelpPath;
	pszHelpPath = 0;
	helpOK = 0;
    }
}

extern void message(char *format, ...);

// open help window with help on a particular topic
void showHelpWindow(LONG topic_number)
{
    if (helpOK)
        WinSendMsg(help_window,HM_DISPLAY_HELP,MPFROMLONG(MAKELONG(topic_number,NULL)),MPFROMSHORT(HM_RESOURCEID));
    else
        message("Help file not found!");
}
