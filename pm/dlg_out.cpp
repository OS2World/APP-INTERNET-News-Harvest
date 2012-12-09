/* dlg_out.cpp,v 1.2 2001/06/20 00:42:41 root Exp */

// Dialog procedure for output settings notebook page

#define INCL_WIN
#define INCL_PM

#include <stdlib.h>
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "Basic.hpp"
#include "Resource.h"
#include "out.h"
#include "config.h"
#include "strutils.h"

extern void message(char *format, ...);
extern Configuration *p_config;
extern char szLicensePath[];

int rego_file_found = 0;
static int done_already = 1;

//------------------------------------------------------------------------


MRESULT EXPENTRY Dlg_OutputPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
     switch( msg )
     {
     case WM_INITDLG:
         {
             HWND path = WinWindowFromID(hwnd,OUT_PATH);
             HWND max  = WinWindowFromID(hwnd,OUT_MB);
             WinSendMsg(path,EM_SETTEXTLIMIT,MPFROM2SHORT(1024,TRUE),MPFROM2SHORT(0,TRUE));
             WinSendMsg(max,EM_SETTEXTLIMIT,MPFROM2SHORT(6,TRUE),MPFROM2SHORT(0,TRUE));
             
             WinSetWindowText(path,p_config->getBaseDir());
             char *num_str = new char[1024];
             sprintf(num_str,"%d",p_config->getMaxDownload());
             WinSetWindowText(max,num_str);
             delete[] num_str;
         }
         break;

     case WM_CLOSE:
         {
             char *buffer = new char[1024];
             WinQueryWindowText(WinWindowFromID(hwnd,OUT_PATH),1023,buffer);
             lrSpaceTrim(buffer);
             p_config->setBaseDir(buffer);
             
             WinQueryWindowText(WinWindowFromID(hwnd,OUT_MB),1023,buffer);
             ULONG value = atol(buffer);
             p_config->setMaxDownload(value);
             
             delete[] buffer;
         }

	 return 0;	// 06 Mar 01 SHL

     case WM_CONTROL:
         switch(SHORT2FROMMP(mp1))
         {
         case EN_CHANGE:
             {
                 switch (SHORT1FROMMP(mp1))
                 {
                     case OUT_MB:
                     {
                         if (done_already)
                         {
                             done_already = 0;
                             FILE *rego = fopen(szLicensePath,"r");
                             if (rego)
                                 fclose(rego);
                             else
                                 message("Unregistered News Harvest will finish after a random amount < 15 MB");
                         }
                         rego_file_found = 1;
                         char *number_str = new char[1024];
                         WinQueryWindowText(WinWindowFromID(hwnd,OUT_MB),1023,number_str);
                         if (makeDigitsOnly(number_str))
                             WinSetWindowText(WinWindowFromID(hwnd,OUT_MB),number_str);
                         delete[] number_str;
                     }
                 }
             }
         }
         break;
         
     //----------------------------------
     case WM_COMMAND:
          switch( SHORT1FROMMP( mp1 ) )
          {
               case PB_UNDO:
                    // The undo pushbutton has been pressed
                    // The entries should be reset to their last saved values
                    break;
                //----------------------------------
               case PB_DEFAULT:
                    // The default pushbutton has been pressed
                   // The entries should be reset to their default values
                   {
                        HWND path = WinWindowFromID(hwnd,OUT_PATH);
                        HWND max  = WinWindowFromID(hwnd,OUT_MB);
                        WinSetWindowText(path,"C:\\NHOutput\\");
                        WinSetWindowText(max,"0");
                   }

                    break;
                //----------------------------------
          }
          return 0;
     //----------------------------------
     case WM_VALIDATE:
          // Validate the entries
         {
             char *buffer = new char[1024];
             
             WinQueryWindowText(WinWindowFromID(hwnd,OUT_PATH),1023,buffer);
             lrSpaceTrim(buffer);
             if (*buffer != '\0');
             {
                 if (!checkDir(buffer))
                 {
                     message("Output path is not a vailid directory");
                     WinSetWindowText(WinWindowFromID(hwnd,OUT_PATH),buffer);
                     WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwnd,OUT_PATH),0);
                     delete[] buffer;
                     return 0;
                 }
                 WinSetWindowText(WinWindowFromID(hwnd,OUT_PATH),buffer);
             }
             delete[] buffer;
          }
          return (MPARAM) 1; 
     }

     return WinDefDlgProc( hwnd, msg, mp1, mp2 );

}// Dlg_NbkpPage Dialog Procedure
