/* Class_Notebook.cpp,v 1.2 2001/06/20 00:42:40 root Exp */

// Class_Notebook.cpp - generic PM notebook support class

#define INCL_DOS
#define INCL_PM
#include <os2.h>

#include "Basic.hpp"
#include "Class_Notebook.hpp"

//------------------------------------------------------------------------

Class_Notebook::Class_Notebook( HWND hwnd, ULONG ulNotebookDialogResource )
{
     // Constructor

     hwndNotebook = WinWindowFromID( hwnd,  ulNotebookDialogResource );
}

//------------------------------------------------------------------------

void Class_Notebook::AddMajor( ULONG ulNotebookPageResource,
                               char *szTabText,
                               PFNWP fnwpNotebookDialog,
                               char *szStatusText )
{
     // Add a major page

     AddTab( BKA_MAJOR,
             ulNotebookPageResource,
             szTabText,
             fnwpNotebookDialog,
             szStatusText );
}

//------------------------------------------------------------------------

void Class_Notebook::AddMinor( ULONG ulNotebookPageResource,
                               char *szTabText,
                               PFNWP fnwpNotebookDialog,
                               char *szStatusText )
{
     // Add a minor page

     AddTab( BKA_MINOR,
             ulNotebookPageResource,
             szTabText,
             fnwpNotebookDialog,
             szStatusText );
}

//------------------------------------------------------------------------

MRESULT Class_Notebook::SendMessage( ULONG ulPageID,
                                  ULONG msg,
                                  MPARAM mp1,
                                  MPARAM mp2 )
{
     // Send a message to the window procedure of a notebook page

     MRESULT mReturn;
     if( ulPageID )
     {
          // Send the message to a specific notebook page

          HWND hwndPage = (HWND) WinSendMsg( hwndNotebook,
                                             BKM_QUERYPAGEWINDOWHWND,
                                             (MPARAM) ulPageID,
                                             0 );
          mReturn = WinSendMsg( hwndPage, msg, mp1, mp2 );
     }
     else
     {
          // Send the message to all notebook pages

          ulPageID = (ULONG) WinSendMsg( hwndNotebook,
                                         BKM_QUERYPAGEID,
                                         0,
                                         MPFROM2SHORT( BKA_FIRST, 0 ) );
          while( ulPageID )
          {
               SendMessage( ulPageID, msg, mp1, mp2 );
               ulPageID = (ULONG) WinSendMsg( hwndNotebook,
                                              BKM_QUERYPAGEID,
                                              MPFROMP( ulPageID ),
                                              MPFROM2SHORT( BKA_NEXT, 0 ) );
          }
     }

     return mReturn;
}

//------------------------------------------------------------------------

BOOL Class_Notebook::PostMessage( ULONG ulPageID,
                                  ULONG msg,
                                  MPARAM mp1,
                                  MPARAM mp2 )
{
     // Post a message to the window procedure of a notebook page

     BOOL bReturn;

     if( ulPageID )
     {
          // Post the message to a specific notebook page

          HWND hwndPage = (HWND) WinSendMsg( hwndNotebook,
                                             BKM_QUERYPAGEWINDOWHWND,
                                             (MPARAM) ulPageID,
                                             0 );
          bReturn = WinPostMsg( hwndPage, msg, mp1, mp2 );
     }
     else
     {
          // Post the message to all notebook pages

          ulPageID = (ULONG) WinSendMsg( hwndNotebook,
                                         BKM_QUERYPAGEID,
                                         0,
                                         MPFROM2SHORT( BKA_FIRST, 0 ) );
          while( ulPageID )
          {
               PostMessage( ulPageID, msg, mp1, mp2 );
               ulPageID = (ULONG) WinSendMsg( hwndNotebook,
                                              BKM_QUERYPAGEID,
                                              MPFROMP( ulPageID ),
                                              MPFROM2SHORT( BKA_NEXT, 0 ) );
          }
     }

     return bReturn;
}

//------------------------------------------------------------------------

void Class_Notebook::AddTab( USHORT usType, ULONG ulNotebookPageResource, char *szTabText, PFNWP fnwpNotebookDialog, char *szStatusText )
{
     // Add a notebook tab

     ULONG ulPageID;
     HWND hwndPage;
     PDLGTEMPLATE ppb;

     ulPageID = (ULONG) WinSendMsg( hwndNotebook,
                                    BKM_INSERTPAGE,
                                    0L,
                                    MPFROM2SHORT( (BKA_STATUSTEXTON | BKA_AUTOPAGESIZE | usType ),
                                                  BKA_LAST ) );
     WinSendMsg( hwndNotebook,
                 BKM_SETTABTEXT,
                 MPFROMLONG( ulPageID ),
                 MPFROMP( szTabText ) );
     WinSendMsg( hwndNotebook,
                 BKM_SETSTATUSLINETEXT,
                 MPFROMLONG( ulPageID ),
                 MPFROMP( szStatusText ) );
     DosGetResource( (HMODULE) NULL,
                     RT_DIALOG,
                     ulNotebookPageResource,
                     (PPVOID) &ppb);
     hwndPage = WinCreateDlg( HWND_DESKTOP,
                              (HWND) NULL,
                              fnwpNotebookDialog,
                              ppb,
                              NULL );
     DosFreeResource( (PVOID) ppb );
     WinSendMsg( hwndNotebook,
                 BKM_SETPAGEWINDOWHWND,
                 MPFROMLONG( ulPageID ),
                 MPFROMHWND( hwndPage ) );
}

USHORT Class_Notebook::getPageNo(void)
{
    USHORT uPageNo = 0;

    ULONG ulCurPageId = (ULONG)WinSendMsg(hwndNotebook,
		                        BKM_QUERYPAGEID,
			                MPFROMLONG(0),
			                MPFROM2SHORT(BKA_TOP, BKA_MAJOR));

    if (ulCurPageId != 0 && ulCurPageId != BOOKERR_INVALID_PARAMETERS)
    {

        ULONG ulPageId = (ULONG)WinSendMsg(hwndNotebook,
		                         BKM_QUERYPAGEID,
			                 MPFROMLONG(0),
			                 MPFROM2SHORT(BKA_FIRST, BKA_MAJOR));

        while (ulPageId != 0 && ulPageId != BOOKERR_INVALID_PARAMETERS)
        {
            uPageNo++;
	    if (ulPageId == ulCurPageId)
	       break;
            ulPageId = (ULONG)WinSendMsg(hwndNotebook,
	                                 BKM_QUERYPAGEID,
			                 MPFROMLONG(ulPageId),
			                 MPFROM2SHORT(BKA_NEXT, BKA_MAJOR));
        } // while
    }

    return uPageNo;
}

void Class_Notebook::gotoPageNo(USHORT uPageNo)
{
    USHORT uCurPageNo = 0;

    ULONG ulPageId = (ULONG)WinSendMsg(hwndNotebook,
		                       BKM_QUERYPAGEID,
			               MPFROMLONG(0),
			               MPFROM2SHORT(BKA_FIRST, BKA_MAJOR));

    while (ulPageId != 0 && ulPageId != BOOKERR_INVALID_PARAMETERS)
    {
	uCurPageNo++;
	if (uCurPageNo == uPageNo)
	{
	    WinPostMsg(hwndNotebook, BKM_TURNTOPAGE, MPFROMLONG(ulPageId), 0);
	    break;
	}
	ulPageId = (ULONG)WinSendMsg(hwndNotebook,
	                             BKM_QUERYPAGEID,
			             MPFROMLONG(ulPageId),
			             MPFROM2SHORT(BKA_NEXT, BKA_MAJOR));
    } // while
}

