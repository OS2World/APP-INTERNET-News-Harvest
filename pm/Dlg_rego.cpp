/* Dlg_rego.cpp,v 1.2 2001/06/20 00:42:40 root Exp */

// Dialog procedure for registration settings notebook page

#define INCL_WIN
#define INCL_PM

#include <stdlib.h>
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "Basic.hpp"
#include "Resource.h"
#include "rego.h"

extern void message(char *format, ...);
extern char szLicensePath[];

//------------------------------------------------------------------------


MRESULT EXPENTRY Dlg_RegoPage ( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
     switch( msg )
     {
     case WM_INITDLG:
         {
             HWND name = WinWindowFromID(hwnd,REG_NAME);
             HWND code = WinWindowFromID(hwnd,REG_CODE);
             WinSendMsg(name,EM_SETTEXTLIMIT,MPFROM2SHORT(255,TRUE),MPFROM2SHORT(0,TRUE));
             WinSendMsg(code,EM_SETTEXTLIMIT,MPFROM2SHORT(255,TRUE),MPFROM2SHORT(0,TRUE));
             
             FILE *in = fopen(szLicensePath,"r");
             if (in)
             {
                 char *buffer = new char[4096];
                 while (!feof(in))
                 {
                     fgets(buffer,4096,in);
                     char *ptr = strchr(buffer,'\r');
                     if (!ptr) ptr = strchr(buffer,'\n');
                     if (ptr) *ptr = '\0';
                     if (!feof(in))
                     {
                         if (*buffer == '\r' || *buffer == '\n' || *buffer == '#')
                         {
                             // shit data
                         }
                         else if (strncmp(buffer,"RegoName",8) == 0)
                         {
                             WinSetWindowText(name,buffer+10);
                         }
                         else if (strncmp(buffer,"RegoKey",7) == 0)
                         {
                             WinSetWindowText(code,buffer+9);
                         }
                     }
                 }
                 delete[] buffer;
                 fclose(in);
             }
         }
         
         
         break;
     //----------------------------------
     case WM_CLOSE:
          // Release dynamic allocations
	  // InfoBox( "Closing a notebook page" );

	  return 0;	// 06 Mar 01 SHL

     case WM_COMMAND:
          // switch( SHORT1FROMMP( mp1 ) )
          // {
          // }
          return 0;

     case WM_VALIDATE:
          // Validate the entries

          // InfoBox( "Validating a notebook page" );
          // Returns from WinQueryButtonCheckstate are:
          //      0.  Checkbox is clear:  no validation error
          //      1.  Checkbox is checked:  validation error
          // WM_VALIDATE will return TRUE if there is no validation error
          // and FALSE if there is a validation error
          return (MPARAM) 1; // !WinQueryButtonCheckstate( hwnd, CKB_VALIDATIONERROR );
     }//switch( msg )

     return WinDefDlgProc( hwnd, msg, mp1, mp2 );

}// Dlg_NbkpPage Dialog Procedure
