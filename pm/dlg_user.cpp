/* dlg_user.cpp,v 1.2 2001/06/20 00:42:41 root Exp */

// Dialog procedure for user settings notebook page

#define INCL_WIN
#define INCL_PM
#define INCL_WINBUTTONS
#define INCL_WINWINDOWMGR

#include <stdlib.h>
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "Basic.hpp"
#include "Resource.h"
#include "user.h"
#include "config.h"
#include "strutils.h"
#include "obj_string.h"

extern Configuration *p_config;
//------------------------------------------------------------------------



MRESULT EXPENTRY Dlg_UserPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
     switch( msg )
     {
     case WM_INITDLG:
         {
             WinSendMsg(WinWindowFromID(hwnd,USER_LOOP),EM_SETTEXTLIMIT,MPFROM2SHORT(4,TRUE),MPFROM2SHORT(0,TRUE));
             WinSendMsg(WinWindowFromID(hwnd,USER_WAIT),EM_SETTEXTLIMIT,MPFROM2SHORT(4,TRUE),MPFROM2SHORT(0,TRUE));
             String number;
             number = p_config->getUserLoopTimes();
             WinSetWindowText(WinWindowFromID(hwnd,USER_LOOP),number.asCharStar());
             number = (int)p_config->getUserLoopDelay();
             WinSetWindowText(WinWindowFromID(hwnd,USER_WAIT),number.asCharStar());
             
             if (p_config->getUserAutoStart())
                 WinCheckButton(hwnd,USER_AUTO,(unsigned short)TRUE);
             else
                 WinCheckButton(hwnd,USER_AUTO,(unsigned short)FALSE);
             
             if (p_config->getUserShowAllFiles())
             {
                 WinCheckButton(hwnd,USER_SMALLTREE,(unsigned short)FALSE);
                 WinCheckButton(hwnd,USER_BIGTREE,(unsigned short)TRUE);
             }
             else
             {
                 WinCheckButton(hwnd,USER_SMALLTREE,(unsigned short)TRUE);
                 WinCheckButton(hwnd,USER_BIGTREE,(unsigned short)FALSE);
             }
             
             if (p_config->getUserSmallIcons())
                 WinCheckButton(hwnd,USER_ICONS,(unsigned short)TRUE);
             else
                 WinCheckButton(hwnd,USER_ICONS,(unsigned short)FALSE);
         }
         
         break;
     //----------------------------------
     case WM_CLOSE:
         {
             unsigned short previous = WinCheckButton(hwnd,USER_AUTO,(unsigned short)TRUE);
             p_config->setUserAutoStart(previous);
             previous = WinCheckButton(hwnd,USER_BIGTREE,(unsigned short)TRUE);
             p_config->setUserShowAllFiles(previous);
             previous = WinCheckButton(hwnd,USER_ICONS,(unsigned short)TRUE);
             p_config->setUserSmallIcons(previous);
             
             char *buffer = new char[1024];
             WinQueryWindowText(WinWindowFromID(hwnd,USER_LOOP),1023,buffer);
             p_config->setUserLoopTimes(atoi(buffer));
             WinQueryWindowText(WinWindowFromID(hwnd,USER_WAIT),1023,buffer);
             p_config->setUserLoopDelay(atoi(buffer));
             delete[] buffer;
         }

	 return 0;	// 06 Mar 01 SHL
         
     case WM_CONTROL:
         switch(SHORT2FROMMP(mp1))
         {
             case EN_CHANGE:
             {
                 char *number_str = new char[1024];
                 switch (SHORT1FROMMP(mp1))
                 {
                     case USER_LOOP:
                         WinQueryWindowText(WinWindowFromID(hwnd,USER_LOOP),1023,number_str);
                         if (makeDigitsOnly(number_str))
                             WinSetWindowText(WinWindowFromID(hwnd,USER_LOOP),number_str);
                         break;
                     case USER_WAIT:
                         WinQueryWindowText(WinWindowFromID(hwnd,USER_WAIT),1023,number_str);
                         if (makeDigitsOnly(number_str))
                             WinSetWindowText(WinWindowFromID(hwnd,USER_WAIT),number_str);
                         break;
                 }
                 
                 delete[] number_str;
             }
         }
                 
         
     //----------------------------------
     case WM_COMMAND:
          switch( SHORT1FROMMP( mp1 ) )
          {
               case PB_DEFAULT:
                    // The default pushbutton has been pressed
                   // The entries should be reset to their default values
                   {
                       WinCheckButton(hwnd,USER_AUTO,(unsigned short)FALSE);
                       WinCheckButton(hwnd,USER_SMALLTREE,(unsigned short)FALSE);
                       WinCheckButton(hwnd,USER_BIGTREE,(unsigned short)TRUE);
                       WinCheckButton(hwnd,USER_ICONS,(unsigned short)TRUE);
                       WinSetWindowText(WinWindowFromID(hwnd,USER_LOOP),"0");
                       WinSetWindowText(WinWindowFromID(hwnd,USER_WAIT),"0");
                   }
                    break;
                //----------------------------------
          }
          return 0;
     //----------------------------------
     case WM_VALIDATE:
         {
/*             // get status of check box
             WinEnableWindowUpdate(WinWindowFromID(hwnd,USER_AUTO),FALSE);
             unsigned short previous = WinCheckButton(hwnd,USER_AUTO,(unsigned short)TRUE);
             WinCheckButton(hwnd,USER_AUTO,previous);
             WinEnableWindowUpdate(WinWindowFromID(hwnd,USER_AUTO),TRUE);

             // check log filename is available & writeable if being used
             char *buffer=new char[1024];
             WinQueryWindowText(WinWindowFromID(hwnd,LOG_PATH),1023,buffer);
             lrSpaceTrim(buffer);
             delete[] buffer;
*/         }
         
          return (MPARAM) 1; // !WinQueryButtonCheckstate( hwnd, CKB_VALIDATIONERROR );
     }//switch( msg )

     return WinDefDlgProc( hwnd, msg, mp1, mp2 );

}// Dlg_NbkpPage Dialog Procedure
