/* dlg_log.cpp,v 1.2 2001/06/20 00:42:40 root Exp */

// Dialog procedure for log settings notebook page

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
#include "dlg_log.h"
#include "log.h"
#include "config.h"
#include "strutils.h"

extern void message(char *format, ...);

extern Configuration *p_config;

//------------------------------------------------------------------------



MRESULT EXPENTRY Dlg_LogPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
     switch( msg )
     {
     case WM_INITDLG:
         {
             HWND path = WinWindowFromID(hwnd,LOG_PATH);
             WinSendMsg(path,EM_SETTEXTLIMIT,MPFROM2SHORT(1024,TRUE),MPFROM2SHORT(0,TRUE));
             WinSetWindowText(path,p_config->getLog());
             WinCheckButton(hwnd,LOG_USE,(unsigned short)p_config->getLoggingUse());
             WinCheckButton(hwnd,LOG_SPAWN,(unsigned short)p_config->getLogSpawn());
         }
         
         break;

     case WM_CLOSE:
         {
             char *buffer = new char[CCHMAXPATH];
             WinQueryWindowText(WinWindowFromID(hwnd,LOG_PATH),CCHMAXPATH,buffer);
             lrSpaceTrim(buffer);
             p_config->setLog(buffer);
             delete[] buffer;
             
             unsigned short checked = WinCheckButton(hwnd,LOG_USE,(unsigned short)TRUE);
             p_config->setLoggingUse(checked);
             checked = WinCheckButton(hwnd,LOG_SPAWN,(unsigned short)TRUE);
             p_config->setLogSpawn(checked);
         }

	 return 0;	// 06 Mar 01 SHL

     case WM_COMMAND:
          switch( SHORT1FROMMP( mp1 ) )
          {
               case PB_DEFAULT:
                   {
                       HWND path = WinWindowFromID(hwnd,LOG_PATH);
                       WinSetWindowText(path,"C:\\NHOutput\\Logfile.txt");
                       WinCheckButton(hwnd,LOG_USE,(unsigned short)TRUE);
                       WinCheckButton(hwnd,LOG_SPAWN,(unsigned short)FALSE);
                   }
                   break;
          }
          return 0;
     //----------------------------------
     case WM_VALIDATE:
         {
             // get status of check box
             WinEnableWindowUpdate(WinWindowFromID(hwnd,LOG_USE),FALSE);
             unsigned short checked = WinCheckButton(hwnd,LOG_USE,(unsigned short)TRUE);
             WinCheckButton(hwnd,LOG_USE,checked);
             WinEnableWindowUpdate(WinWindowFromID(hwnd,LOG_USE),TRUE);
             
             // check log filename is available & writeable if being used
             char *filename=new char[CCHMAXPATH];
             WinQueryWindowText(WinWindowFromID(hwnd,LOG_PATH),CCHMAXPATH,filename);
             lrSpaceTrim(filename);
             if (checked && *filename != '\0')
             {
                 int in_use = p_config->getLoggingUse();
		 toggleLog(0);		// Avoid name collisions
                 FILE *in=fopen(filename,"a");
                 if (!in)
                 {
		     toggleLog(in_use);	// Restore
                     message("Can not append to requested log file (%d)",errno);
                     delete[] filename;
                     return 0;
                 }
                 fclose(in);
             }
         
             toggleLog(checked);
             setLogfilePath(filename);
             
             delete[] filename;
         }
         return (MPARAM) 1; 
     }

     return WinDefDlgProc( hwnd, msg, mp1, mp2 );

}
