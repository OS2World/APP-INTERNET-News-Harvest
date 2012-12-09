/* dlg_multi.cpp,v 1.2 2001/06/20 00:42:40 root Exp */

// Dialog procedure for multipart settings notebook page

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
#include "multi.h"
#include "config.h"
#include "strutils.h"

extern void message(char *format, ...);

extern Configuration *p_config;

//------------------------------------------------------------------------

MRESULT EXPENTRY Dlg_PartPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
     switch( msg )
     {
     case WM_INITDLG:
         {
             HWND path = WinWindowFromID(hwnd,PRT_PATH);
             HWND days = WinWindowFromID(hwnd,PRT_DAYS);
             HWND maxparts  = WinWindowFromID(hwnd,PRT_MAX);
             HWND maxlines  = WinWindowFromID(hwnd,PRT_MAX_LINES);

             WinSendMsg(path,EM_SETTEXTLIMIT,MPFROM2SHORT(1024,TRUE),MPFROM2SHORT(0,TRUE));
             WinSendMsg(days,EM_SETTEXTLIMIT,MPFROM2SHORT(4,TRUE),MPFROM2SHORT(0,TRUE));
             WinSendMsg(maxparts, EM_SETTEXTLIMIT,MPFROM2SHORT(4,TRUE),MPFROM2SHORT(0,TRUE));
             WinSendMsg(maxlines, EM_SETTEXTLIMIT,MPFROM2SHORT(6,TRUE),MPFROM2SHORT(0,TRUE));
             
             WinSetWindowText(path,p_config->getPartsDir());

             char *buffer = new char[1024];
             sprintf(buffer,"%d",p_config->getMaxPartAge());
             WinSetWindowText(days,buffer);
             sprintf(buffer,"%d",p_config->getMultipartMaxParts());
             WinSetWindowText(maxparts,buffer);
             sprintf(buffer,"%d",p_config->getMultipartMaxLines());
             WinSetWindowText(maxlines,buffer);
             delete[] buffer;
             
             if (p_config->getMultipartUse())
                 WinCheckButton(hwnd,PRT_USE,(unsigned short)TRUE);
             else
                 WinCheckButton(hwnd,PRT_USE,(unsigned short)FALSE);
             
             switch (p_config->getMultipartFullStop())
	     {
	     case 0:
                 WinCheckButton(hwnd,PRT_FULLSTOP,(unsigned short)FALSE);
                 WinCheckButton(hwnd,PRT_FULLDISABLE,(unsigned short)TRUE);
                 WinCheckButton(hwnd,PRT_FULLCLEAN,(unsigned short)FALSE);
                 break;
	     case 1:
                 WinCheckButton(hwnd,PRT_FULLSTOP,(unsigned short)TRUE);
                 WinCheckButton(hwnd,PRT_FULLDISABLE,(unsigned short)FALSE);
                 WinCheckButton(hwnd,PRT_FULLCLEAN,(unsigned short)FALSE);
                 break;
             default:
                 WinCheckButton(hwnd,PRT_FULLSTOP,(unsigned short)FALSE);
                 WinCheckButton(hwnd,PRT_FULLDISABLE,(unsigned short)FALSE);
                 WinCheckButton(hwnd,PRT_FULLCLEAN,(unsigned short)TRUE);
             } // switch
             
         } // WM_INITDLG
         break;

     case WM_CLOSE:
         {
             char *buffer = new char[1024];
             WinQueryWindowText(WinWindowFromID(hwnd,PRT_PATH),1023,buffer);
             lrSpaceTrim(buffer);
             p_config->setPartsDir(buffer);
             
             WinQueryWindowText(WinWindowFromID(hwnd,PRT_DAYS),1023,buffer);
             lrSpaceTrim(buffer);
             ULONG days = atol(buffer);
             p_config->setMaxPartAge(days);
             
             WinQueryWindowText(WinWindowFromID(hwnd,PRT_MAX),1023,buffer);
             lrSpaceTrim(buffer);
             int max = atoi(buffer);
             p_config->setMultipartMaxParts(max);
             
             WinQueryWindowText(WinWindowFromID(hwnd,PRT_MAX_LINES),1023,buffer);
             lrSpaceTrim(buffer);
             max = atoi(buffer);
             p_config->setMultipartMaxLines(max);
             
             WinEnableWindowUpdate(WinWindowFromID(hwnd,PRT_USE),FALSE);
             unsigned short previous = WinCheckButton(hwnd,PRT_USE,(unsigned short)TRUE);
             WinCheckButton(hwnd,PRT_USE,previous);
             WinEnableWindowUpdate(WinWindowFromID(hwnd,PRT_USE),TRUE);
             p_config->setMultipartUse((int)previous);
             
             
             // check the 3 radio buttons - returns 1,2,3  (not 0,1,2)
             int index = (int)WinSendMsg(WinWindowFromID(hwnd,PRT_FULLSTOP),BM_QUERYCHECKINDEX,0,0);
//             message("index == %d",index);
             if (index == -1)
                 index = 1; // failed - no button selected
             
             switch(index)
             {
                 case 1:
                     p_config->setMultipartFullStop(1);
		     break;
                 case 2:
                     p_config->setMultipartFullStop(0);
		     break;
                 case 3:
                     p_config->setMultipartFullStop(2);
		     break;
             }
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
                     case PRT_DAYS:
                         WinQueryWindowText(WinWindowFromID(hwnd,PRT_DAYS),1023,number_str);
                         if (makeDigitsOnly(number_str))
                             WinSetWindowText(WinWindowFromID(hwnd,PRT_DAYS),number_str);
                         break;
                     case PRT_MAX:
                         WinQueryWindowText(WinWindowFromID(hwnd,PRT_MAX),1023,number_str);
                         if (makeDigitsOnly(number_str))
                             WinSetWindowText(WinWindowFromID(hwnd,PRT_MAX),number_str);
                         break;
                 }
                 
                 delete[] number_str;
             }
         }
                 
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
                        HWND path = WinWindowFromID(hwnd,PRT_PATH);
                        HWND days = WinWindowFromID(hwnd,PRT_DAYS);
                        HWND max  = WinWindowFromID(hwnd,PRT_MAX);
                        WinSetWindowText(path,"C:\\NHOutput\\");
                        WinSetWindowText(days,"3");
                        WinSetWindowText(max,"100");
                        unsigned short previous = WinCheckButton(hwnd,PRT_USE,(unsigned short)TRUE);
                        
                        WinCheckButton(hwnd,PRT_FULLSTOP,(unsigned short)FALSE);
                        WinCheckButton(hwnd,PRT_FULLDISABLE,(unsigned short)FALSE);
                        WinCheckButton(hwnd,PRT_FULLCLEAN,(unsigned short)TRUE);
                   }

//                    InfoBox( "PB_DEFAULT in a notebook page" );
                    break;
                //----------------------------------
          }
          return 0;
     //----------------------------------
     case WM_VALIDATE:
         {
             WinEnableWindowUpdate(WinWindowFromID(hwnd,PRT_USE),FALSE);
             unsigned short previous = WinCheckButton(hwnd,PRT_USE,(unsigned short)TRUE);
             WinCheckButton(hwnd,PRT_USE,previous);
             WinEnableWindowUpdate(WinWindowFromID(hwnd,PRT_USE),TRUE);
             
             if (previous == TRUE)
             {
                 char *buffer = new char[1024];
                 
                 WinQueryWindowText(WinWindowFromID(hwnd,PRT_PATH),1023,buffer);
                 lrSpaceTrim(buffer);
                 if (*buffer != '\0');
                 {
//                     message("buffer == \"%s\"",buffer);
                     if (!checkDir(buffer))
                     {
                         message("Part path is not a valid directory");
                         WinSetWindowText(WinWindowFromID(hwnd,PRT_PATH),buffer);
                         WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwnd,PRT_PATH),0);
                         delete[] buffer;
                         return 0;
                     }
                     WinSetWindowText(WinWindowFromID(hwnd,PRT_PATH),buffer);
                 }
                 delete[] buffer;
             }
          }
                   
         return (MPARAM) 1; // !WinQueryButtonCheckstate( hwnd, CKB_VALIDATIONERROR );
     }//switch( msg )

     return WinDefDlgProc( hwnd, msg, mp1, mp2 );

}// Dlg_NbkpPage Dialog Procedure
