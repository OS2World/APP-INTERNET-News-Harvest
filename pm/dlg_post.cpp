/* dlg_post.cpp,v 1.2 2001/06/20 00:42:41 root Exp */

// Dialog procedure for post process settings notebook page

#define INCL_WIN
#define INCL_PM

#include <stdlib.h>
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "Basic.hpp"
#include "Resource.h"
#include "post.h"
#include "config.h"
#include "strutils.h"

extern void message(char *format, ...);
extern Configuration *p_config;

//------------------------------------------------------------------------

MRESULT EXPENTRY Dlg_PostPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
     switch( msg )
     {
     case WM_INITDLG:
         {
             WinSendMsg(WinWindowFromID(hwnd,POST_COMM),EM_SETTEXTLIMIT,MPFROM2SHORT(1024,TRUE),MPFROM2SHORT(0,TRUE));
             
             WinSetWindowText(WinWindowFromID(hwnd,POST_COMM),p_config->getProcessCommand());
             WinCheckButton(hwnd,POST_USE,(unsigned short)p_config->getProcessUse());
         }
         break;

     case WM_CLOSE:
         {
             char *buffer = new char[1024];
             WinQueryWindowText(WinWindowFromID(hwnd,POST_COMM),1023,buffer);
             lrSpaceTrim(buffer);
             p_config->setProcessCommand(buffer);
             delete[] buffer;
             
             unsigned short previous = WinCheckButton(hwnd,POST_USE,(unsigned short)TRUE);
             p_config->setProcessUse((int)previous);
         }

         return 0;	// 06 Mar 01 SHL

     case WM_COMMAND:
          switch( SHORT1FROMMP( mp1 ) )
          {
               case PB_UNDO:
                    // The undo pushbutton has been pressed
                    // The entries should be reset to their last saved values
                    break;
                    
               case PB_DEFAULT:
                     WinCheckButton(hwnd,POST_USE,(unsigned short)FALSE);
                    break;
          }
          return 0;

     case WM_VALIDATE:
         {
             // Validate the entries
             // get status of check box
             WinEnableWindowUpdate(WinWindowFromID(hwnd,POST_USE),FALSE);
             unsigned short previous = WinCheckButton(hwnd,POST_USE,(unsigned short)TRUE);
             WinCheckButton(hwnd,POST_USE,previous);
             WinEnableWindowUpdate(WinWindowFromID(hwnd,POST_USE),TRUE);
             
             // post process command don't matter if not enabled
             if (previous)
             {
                 char *buffer = new char[1024];
                 WinQueryWindowText(WinWindowFromID(hwnd,POST_COMM),1023,buffer);
                 lrSpaceTrim(buffer);
                 FILE *in = fopen(buffer,"r");
                 if (in)
                   fclose(in);
                 else
                 {
                     // message("Failed to find program \"%s\", giving the full path is more efficient",buffer);
                 }
                 delete[] buffer;
             }
         }
         return (MPARAM) 1; // !WinQueryButtonCheckstate( hwnd, CKB_VALIDATIONERROR );

     }//switch( msg )

     return WinDefDlgProc( hwnd, msg, mp1, mp2 );

}// Dlg_NbkpPage Dialog Procedure
