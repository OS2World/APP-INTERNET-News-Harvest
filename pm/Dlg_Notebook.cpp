
/***********************************************************************

Dlg_Notebook.cpp,v 1.4 2002/07/19 23:44:43 root Exp

Configuration Notebook dialog procedure

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2001 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	13 Jul 01 SHL - Reorder
		19 Jul 02 SHL - Clean up Help interface

***********************************************************************/

#define INCL_DOS
#define INCL_WIN
#define INCL_PM

#include <string.h>
#include <stdlib.h>
#include <os2.h>

#include "Basic.hpp"
#include "Resource.h"
#include "Class_Notebook.hpp"
#include "pmtools.h"
#include "help.h"
extern void message(char *format, ...);

//------------------------------------------------------------------------
// Global variables

struct wdDlg_Notebook {
     Class_Notebook *Notebook;// Notebook
     ULONG ulPageId;// ID of currently displayed window
};

wdDlg_Notebook *wd;

static USHORT uLastPageNo;

//------------------------------------------------------------------------
//Function prototypes

MRESULT EXPENTRY Dlg_PriorityPage ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY Dlg_RegoPage ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY Dlg_PartPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY Dlg_OutputPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY Dlg_LogPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY Dlg_ProcessFilter( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY Dlg_PostPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY Dlg_UserPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY Dlg_SocksPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);

//------------------------------------------------------------------------

MRESULT EXPENTRY Dlg_Notebook( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
     wd = (wdDlg_Notebook *) WinQueryWindowPtr( hwnd, 0 );

     switch( msg )
     {
     //----------------------------------
     case WM_INITDLG:
     {
	  wd = new wdDlg_Notebook;
	  if( !wd )
	  {
	       message( "Error creating a window word in WM_INITDLG of Dlg_Notebook" );
	       WinDestroyWindow( hwnd );
	  }
	  if( !WinSetWindowPtr( hwnd, 0, wd ) )
	  {
	       message( "Error setting window word 0  in WM_INITDLG of Dlg_Notebook" );
	       WinDestroyWindow( hwnd );
	  }

	 pmtoolsCenterWindow(hwnd);

	  // There currently is no notebook page selected
	  // This initialization is required so that a validation will not
	  // be performed on the first notebook page while it is being
	  // created
	  wd->ulPageId = 0;

	  // Create an instance of the notebook
	  wd->Notebook = new Class_Notebook( hwnd, NBK_NOTEBOOK );

	  // Add major notebook tabs
	  wd->Notebook->AddMajor( NBKP_PRIORITY,
				  "Priority",
				  Dlg_PriorityPage,
				  "" );

	  wd->Notebook->AddMajor( NBKP_FILTERS,
				  "Filtering",
				  Dlg_ProcessFilter,
				  "" );

	  wd->Notebook->AddMajor( NBKP_NOTEBOOK8,
				  "User Interface",
				  Dlg_UserPage,
				  "" );

	  wd->Notebook->AddMajor( NBKP_NOTEBOOK5,
				  "Logging",
				  Dlg_LogPage,
				  "" );

	  wd->Notebook->AddMajor( NBKP_MULTIPART,
				  "Multipart",
				  Dlg_PartPage,
				  "" );

	  wd->Notebook->AddMajor( NBKP_NOTEBOOK7,
				  "Post Processing",
				  Dlg_PostPage,
				  "" );

	  wd->Notebook->AddMajor( NBKP_NOTEBOOK4,
				  "Output",
				  Dlg_OutputPage,
				  "" );

	  wd->Notebook->AddMajor( NBKP_NOTEBOOK9,
				  "Socks v5",
				  Dlg_SocksPage,
				  "" );

	  wd->Notebook->AddMajor( NBKP_NOTEBOOK1,
				  "Registration",
				  Dlg_RegoPage,
				  "" );

	  // Turn to last known page

	  wd->Notebook->gotoPageNo(uLastPageNo);

	  // Give the focus to the notebook dialog, so that the titlebar
	  // is of the appropriate color i.e. that of a selected window

	  WinSetFocus( HWND_DESKTOP, WinQueryWindow( hwnd, QW_PARENT ) );
     }

     break;

     case WM_CLOSE:
	 {
	      // Validate the current notebook page
	      if( !wd->Notebook->SendMessage( wd->ulPageId, WM_VALIDATE, 0, 0 ) )
	      {
		   // Validation error on the current notebook page
		   DosBeep( 1440, 100 );// Warn the user that something is going on
		   return 0;
	      }

	      uLastPageNo = wd->Notebook->getPageNo();

	      // Send a WM_CLOSE to all the notebook pages
	      wd->Notebook->SendMessage( 0, WM_CLOSE, 0, 0 );

	      // Release dynamic memory
	      delete wd->Notebook;
	      delete wd;

	      // Close the notebook dialog (main dialog window)
	      // WinPostMsg( hwnd, WM_QUIT, 0, 0 );
	 }
	 break;

     case WM_COMMAND:
	  switch( SHORT1FROMMP( mp1 ) )
	  {
		case DID_CANCEL:
		     // Prevent ESC from closing the dialog
		     return 0;
		//----------------------------------
	       case PB_UNDO:
		    // Send a message to the currently displayed notebook page
		    // The message is simply that the PB_UNDO pushbutton has
		    // been pressed
		    wd->Notebook->SendMessage( wd->ulPageId, WM_COMMAND, MPARAM( PB_UNDO ), 0 );
		    break;
		//----------------------------------
	       case PB_DEFAULT:
		    // Post a message to the currently displayed notebook page
		    // The message is simply that the PB_DEFAULT pushbutton has
		    // been pressed
		    wd->Notebook->PostMessage( wd->ulPageId, WM_COMMAND, MPARAM( PB_DEFAULT ), 0 );
		    break;
	  }
	  break;
     //----------------------------------
     case WM_CONTROL:
	 switch( SHORT1FROMMP( mp1 ) )
	 {
	 case NBK_NOTEBOOK:
	     switch( SHORT2FROMMP( mp1 ) )
	     {
	     case BKN_PAGESELECTED:
		 // Keep track of the ID of the currently displayed notebook page
		 wd->ulPageId = ( (PPAGESELECTNOTIFY) mp2 )->ulPageIdNew;
		 break;

	     case BKN_HELP:
		 // The notebook control received a WM_HELP message
                 WinPostMsg(hwnd,WM_HELP,0,0);
		 break;

	     case BKN_PAGESELECTEDPENDING:
		  // The current page is about to be changed
		  // This would be an appropriate time to perform validations
		  // of the controls on that page

		  // Do not perform validation when creating the notebook control
		  // i.e. when going to the initial notebook page
		  if( wd->ulPageId && !wd->Notebook->SendMessage( wd->ulPageId, WM_VALIDATE, 0, 0 ) )
		  {
		       // Validation error on the current notebook page
		       // Prevent the change of page
		       ( (PPAGESELECTNOTIFY) mp2 )->ulPageIdNew = 0;
			DosBeep( 1440, 100 );// Warn the user that something is going on
		  }
	     }
	     break;
	 }
	 break;

     case WM_VERIFYSHUTDOWN:
	  // It's ok to close the notebook
	  return MPARAM( TRUE );

     }//switch( msg )

     return WinDefDlgProc( hwnd, msg, mp1, mp2 );

}// Dlg_Notebook Dialog Procedure

