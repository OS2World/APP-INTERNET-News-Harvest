
/***********************************************************************

Dlg_Priority.cpp,v 1.3 2002/01/05 19:28:15 root Exp

Dialog procedure for priority settings notebook page

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2001 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	21 Jun 01 SHL - Drop excess new/delete
		12 Jul 01 SHL - Change bytes/sec to spin button

***********************************************************************/

#define INCL_WIN
#define INCL_PM
#define INCL_WINBUTTONS
#define INCL_WINWINDOWMGR
#define INCL_CIRCULARSLIDER

#include <stdlib.h>
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "Basic.hpp"
#include "Resource.h"
#include "priority.h"
#include "config.h"
#include "strutils.h"

extern Configuration *p_config;
extern void message(char *format,...);

//------------------------------------------------------------------------

MRESULT EXPENTRY Dlg_PriorityPage(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int priority;

    switch (msg)
    {
    case WM_INITDLG:
	{
	    HWND hwndSPB = WinWindowFromID(hwnd, SPB_PRI_BANDWIDTH);

	    WinSendMsg(hwndSPB,
		       SPBM_SETLIMITS,
		       MPFROMLONG(999999L),
		       0);

	    WinSendMsg(hwndSPB,
		       SPBM_SETMASTER,
		       NULLHANDLE,
		       0);

	    WinSendMsg(hwndSPB,
		       SPBM_SETTEXTLIMIT,
		       MPFROMSHORT(6),
		       0);

	    ULONG ul = (ULONG) p_config -> getBandwidthLimit();

	    WinSendMsg(hwndSPB,
		       SPBM_SETCURRENTVALUE,
		       MPFROMLONG(ul),
		       0);

	    HWND hwndCSLD = WinWindowFromID(hwnd, CSLD_PRI_LEVEL);

	    WinEnableWindow(hwndCSLD, FALSE);

	    /*
	     * The slider control cannot be completely set from the dialog
	     * template so some aspects must be set here.  We will set the
	     * volume range to 0-100, increment to 1-10, and the initial
	     * volume level to 75.
	     */
	    WinSendMsg(hwndCSLD,
		       CSM_SETRANGE,
		       MPFROMLONG(0),
		       MPFROMLONG(62L));

	    WinSendMsg(hwndCSLD,
		       CSS_NONUMBER,
		       MPFROMLONG(0),
		       MPFROMLONG(0));

	    WinSendMsg(hwndCSLD,
		       CSM_SETINCREMENT,
		       (MPARAM) 4L,
		       (MPARAM) 0L);

	    WinSendMsg(hwndCSLD,
		       CSM_SETVALUE,
		       (MPARAM) (LONG) (p_config -> getPriorityValue() + 31),
		       (MPARAM) NULL);

	    priority = p_config -> getPriorityValue() + 31;

	    WinEnableWindow(hwndCSLD, TRUE);

	    if (p_config -> getPriorityClass() == PRIORITY_NORMAL)
	    {
		WinCheckButton(hwnd, PB_PRI_NORMAL, (unsigned short) TRUE);
		WinCheckButton(hwnd, PB_PRI_IDLE, (unsigned short) FALSE);
	    }
	    else
	    {
		// PRIORITY_IDLE
		WinCheckButton(hwnd, PB_PRI_NORMAL, (unsigned short) FALSE);
		WinCheckButton(hwnd, PB_PRI_IDLE, (unsigned short) TRUE);
	    }
	}
	break;

    case WM_CLOSE:
	{
	    p_config -> setPriorityValue(priority - 31);

	    USHORT previous = WinCheckButton(hwnd, PB_PRI_NORMAL, (USHORT) TRUE);

	    p_config -> setPriorityClass((previous == TRUE ? PRIORITY_NORMAL : PRIORITY_IDLE));

	    ULONG ul;

	    WinSendMsg(WinWindowFromID(hwnd, SPB_PRI_BANDWIDTH),
		       SPBM_QUERYVALUE,
		       MPFROMP(&ul),
		       MPFROM2SHORT(0, 0));
	    p_config -> setBandwidthLimit((double) ul);
	}

	return 0;			// 06 Mar 01 SHL

    case WM_CONTROL:
	{
	    USHORT usId = SHORT1FROMMP(mp1);
	    USHORT usNotifyCode = SHORT2FROMMP(mp1);

	    switch (usId)
	    {
	    case SPB_PRI_BANDWIDTH:
		{
		    HWND hwndSPB = WinWindowFromID(hwnd, SPB_PRI_BANDWIDTH);
		    LONG l;

		    WinSendMsg(hwndSPB,
			       SPBM_QUERYVALUE,
			       MPFROMP(&l),
			       MPFROM2SHORT(0, 0));
		    switch (usNotifyCode)
		    {
		    case SPBN_UPARROW:
			{
			    if (l > 0 && l < 4)
				l++;
			    else
				l += l / 4;
			    if (l <= 0)
				l = 512;
			    WinSendMsg(hwndSPB,
				       SPBM_SETCURRENTVALUE,
				       MPFROMLONG(l),
				       0);
			    break;
			}
		    case SPBN_DOWNARROW:
			{
			    if (l > 0 && l < 4)
				l--;
			    else
				l -= l / 4;
			    if (l <= 512)
				l = 0;
			    WinSendMsg(hwndSPB,
				       SPBM_SETCURRENTVALUE,
				       MPFROMLONG(l),
				       0);
			    break;
			}
		    case SPBN_ENDSPIN:
			break;
		    case SPBN_CHANGE:
			break;
		    }			// usNotifyCode

		    break;
		}

	    case CSLD_PRI_LEVEL:
		switch (usNotifyCode)
		{
		case CSN_CHANGED:
		case CSN_TRACKING:
		    {
			priority = SHORT1FROMMP(mp2);
			char buffer[128];

			sprintf(buffer, "Relative Priority (%d)", priority - 31);
			WinSetWindowText(WinWindowFromID(hwnd, GB_PRI_PRIORITY), buffer);
			break;
		    }
		}
		break;
	    }				// switch usId

	    break;			// WM_CONTROL

	}

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
	{
	    // case PB_UNDO:
	    //     InfoBox( "PB_UNDO in a notebook page" );
	    //     break;

	case PB_DEFAULT:
	    // The default pushbutton has been pressed
	    // The entries should be reset to their default values

	    // InfoBox( "PB_DEFAULT in a notebook page" );

	    WinSendMsg(WinWindowFromID(hwnd, CSLD_PRI_LEVEL), CSM_SETVALUE, (MPARAM) 31, (MPARAM) NULL);
	    WinSetWindowText(WinWindowFromID(hwnd, GB_PRI_PRIORITY), "Relative Priority (0)");
	    WinCheckButton(hwnd, PB_PRI_NORMAL, (unsigned short) TRUE);
	    WinCheckButton(hwnd, PB_PRI_IDLE, (unsigned short) FALSE);
	    WinSetWindowText(WinWindowFromID(hwnd, SPB_PRI_BANDWIDTH), "0");

	    break;

	}
	return 0;			// WM_COMMAND

    case WM_VALIDATE:
	return (MPARAM) 1;		// !WinQueryButtonCheckstate( hwnd, CKB_VALIDATIONERROR );

    }					//switch( msg )

    return WinDefDlgProc(hwnd, msg, mp1, mp2);

}					// Dlg_NbkpPage Dialog Procedure
