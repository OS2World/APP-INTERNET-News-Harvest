
/***********************************************************************

dlg_processfilter.cpp,v 1.4 2002/01/05 22:51:19 root Exp

Settings Notebook Filter page window procedure

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2001 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	12 Jul 01 SHL - Click moves listbox item to entry box

************************************************************************/

// Dialog procedure for filter settings notebook page

#define INCL_WIN
#define INCL_PM
#define INCL_WINLISTBOXES
#define INCL_WINBUTTONS
#define INCL_WINWINDOWMGR

#include <stdlib.h>
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "Basic.hpp"
#include "Resource.h"
#include "filter.h"
#include "config.h"
#include "strutils.h"

extern Configuration *p_config;

//------------------------------------------------------------------------

MRESULT EXPENTRY Dlg_ProcessFilter(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
	{
	    WinSendMsg(WinWindowFromID(hwnd, FIL_FILENAME), EM_SETTEXTLIMIT, MPFROM2SHORT(255, TRUE), MPFROM2SHORT(0, TRUE));
	    WinSendMsg(WinWindowFromID(hwnd, FIL_ENTRY), EM_SETTEXTLIMIT, MPFROM2SHORT(1024, TRUE), MPFROM2SHORT(0, TRUE));

	    WinSetWindowText(WinWindowFromID(hwnd, FIL_FILENAME), p_config -> getFilter());
	    WinCheckButton(hwnd, FIL_USE, (unsigned short) p_config -> getFilterUse());
	    WinCheckButton(hwnd, FIL_FAST_NAME_USE, (unsigned short) p_config -> getFastNameFilterUse());
	}
	break;

    case WM_CONTROL:
	{
	    USHORT usId = SHORT1FROMMP(mp1);
	    USHORT usNotifyCode = SHORT2FROMMP(mp1);

	    switch (usId)
	    {
	    case FIL_FILENAME:
		{
		    switch (usNotifyCode)
		    {
		    case EN_CHANGE:
			{
			    char filename[_MAX_PATH+1];

			    WinQueryWindowText(WinWindowFromID(hwnd, FIL_FILENAME), _MAX_PATH, filename);
			    lrSpaceTrim(filename);
			    FILE *in = fopen(filename, "r");

			    if (in)
			    {
				// Populate listbox
				long some_left = 1;
				HWND hwndLB = WinWindowFromID(hwnd, FIL_LIST);

				WinEnableWindowUpdate(hwndLB, FALSE);

				// clean out list
				while (some_left)
				{
				    some_left = (LONG) WinSendMsg(hwndLB, LM_DELETEITEM, MPFROMLONG(0), NULL);
				}

				// re-fill it
				char buffer[4096];

				while (!feof(in))
				{
				    fgets(buffer, 4096, in);
				    if (!feof(in));
				    {
					char *ptr = strchr(buffer, '\n');

					if (ptr)
					    *ptr = '\0';
					lrSpaceTrim(buffer);
					if (strlen(buffer) > 1)
					{
					    WinSendMsg(hwndLB,
						       LM_INSERTITEM,
						       MPFROMSHORT(LIT_SORTASCENDING),
						       MPFROMP(buffer));
					}
				    }
				} // while

				fclose(in);

				WinEnableWindowUpdate(hwndLB, TRUE);
			    }
			}
		    }
		} // FIL_FILENAME
		break;

	    case FIL_LIST:
		switch (usNotifyCode)
		{
		case LN_SELECT:
		    {
			HWND hwndLB = WinWindowFromID(hwnd, FIL_LIST);
			SHORT sSelected = (SHORT)WinQueryLboxSelectedItem(hwndLB);

			if (sSelected != LIT_NONE)
			{
			    char line[1024];
			    LONG l = WinQueryLboxItemText(hwndLB, sSelected, line, 1023);
			    if (l > 0)
			    {
			        WinSetWindowText(WinWindowFromID(hwnd, FIL_ENTRY),
					         line);
			    }
			}
		    }
		}
		break;
	    } // switch usId
	    break;
	} // WM_CONTROL

    case WM_CLOSE:
	{
	    // get status of check box
	    unsigned short previous = WinCheckButton(hwnd, FIL_USE, (unsigned short) TRUE);

	    p_config -> setFilterUse(previous);

	    previous = WinCheckButton(hwnd, FIL_FAST_NAME_USE, (unsigned short) TRUE);
	    p_config -> setFastNameFilterUse(previous);

	    char *buffer = new char[1024];

	    WinQueryWindowText(WinWindowFromID(hwnd, FIL_FILENAME), 1023, buffer);
	    lrSpaceTrim(buffer);
	    p_config -> setFilter(buffer);
	    delete[]buffer;
	}

	return 0;			// 06 Mar 01 SHL

    case WM_COMMAND:
	switch (SHORT1FROMMP(mp1))
	{
	    // case PB_UNDO:
	    //     // The undo pushbutton has been pressed
	    //     // The entries should be reset to their last saved values
	    //
	    //     InfoBox( "PB_UNDO in a notebook page" );
	    //     break;

	case PB_DEFAULT:
	    // The default pushbutton has been pressed
	    // The entries should be reset to their default values

	    WinSetWindowText(WinWindowFromID(hwnd, FIL_FILENAME), "filters");
	    {
		long some_left = 1;
		HWND hwndLB = WinWindowFromID(hwnd, FIL_LIST);

		WinEnableWindowUpdate(hwndLB, FALSE);

		while (some_left)
		{
		    some_left = (LONG) WinSendMsg(hwndLB, LM_DELETEITEM, MPFROMLONG(0), NULL);
		}

		WinSendMsg(hwndLB, LM_INSERTITEM, MPFROMSHORT(LIT_SORTDESCENDING), MPFROMP("$$$"));
		WinSendMsg(hwndLB, LM_INSERTITEM, MPFROMSHORT(LIT_SORTDESCENDING), MPFROMP("MAKE MONEY"));
		WinSendMsg(hwndLB, LM_INSERTITEM, MPFROMSHORT(LIT_SORTDESCENDING), MPFROMP("thumbnail"));

		WinEnableWindowUpdate(hwndLB, TRUE);
		WinCheckButton(hwnd, FIL_USE, (unsigned short) TRUE);
		WinCheckButton(hwnd, FIL_FAST_NAME_USE, (unsigned short) FALSE);
	    }

	    break;			// PB_DEFAULT

	case FIL_DEL:
	    {
		HWND hwndLB = WinWindowFromID(hwnd, FIL_LIST);

		WinEnableWindowUpdate(hwndLB, FALSE);
		LONG index;

		do
		{
		    index = (LONG) WinSendMsg(hwndLB, LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), NULL);
		    if (index != -1)
		    {
			WinSendMsg(hwndLB, LM_DELETEITEM, MPFROMLONG(index), NULL);
		    }
		}
		while (index != -1);
		WinEnableWindowUpdate(hwndLB, TRUE);
	    }
	    break;

	case FIL_ADD:
	    {
		HWND hwndLB = WinWindowFromID(hwnd, FIL_LIST);

		WinEnableWindowUpdate(hwndLB, FALSE);
		char *buffer = new char[4096];

		WinQueryWindowText(WinWindowFromID(hwnd, FIL_ENTRY), 4096, buffer);
		lrSpaceTrim(buffer);
		WinSendMsg(hwndLB, LM_INSERTITEM, MPFROMSHORT(LIT_SORTASCENDING), MPFROMP(buffer));
		delete[]buffer;
		WinEnableWindowUpdate(hwndLB, TRUE);
	    }
	    break;
	} // switch WM_COMMAND mp1

	return 0;
	// end WM_COMMAND

    case WM_VALIDATE:
	{
	    // get status of check box
	    WinEnableWindowUpdate(WinWindowFromID(hwnd, FIL_USE), FALSE);
	    unsigned short previous = WinCheckButton(hwnd, FIL_USE, (unsigned short) TRUE);

	    WinCheckButton(hwnd, FIL_USE, previous);
	    WinEnableWindowUpdate(WinWindowFromID(hwnd, FIL_USE), TRUE);

	    // only check filename if 'use' is enabled
	    if (previous == TRUE)
	    {
		char *buffer = new char[1024];

		WinQueryWindowText(WinWindowFromID(hwnd, FIL_FILENAME), 1023, buffer);
		lrSpaceTrim(buffer);
		if (*buffer != '\0')
		{
		    FILE *out = fopen(buffer, "w");

		    if (!out)
		    {
			// MessageBox("Unable to open \"%s\" for writing",buffer);
		    }
		    else
		    {
			char *line = new char[1024];
			int items = (int) WinQueryLboxCount(WinWindowFromID(hwnd, FIL_LIST));

			// for each entry in list-box write to file
			for (int i = 0; i < items; i++)
			{
			    *line = '\0';
			    WinQueryLboxItemText(WinWindowFromID(hwnd, FIL_LIST), (SHORT) i, line, 1023);
			    fprintf(out, "%s\n", line);
			}
			fprintf(out, "\n");
			delete[]line;
			fclose(out);
		    }
		}
		delete[]buffer;
	    }				// if using filters

	}

	return (MPARAM) 1;		// !WinQueryButtonCheckstate( hwnd, CKB_VALIDATIONERROR );

	// end WM_VALIDATE

    } // switch msg

    return WinDefDlgProc(hwnd, msg, mp1, mp2);

} // Dlg_ProcessFilter

// The end
