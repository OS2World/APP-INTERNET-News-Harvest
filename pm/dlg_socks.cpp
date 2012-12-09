/* dlg_socks.cpp,v 1.2 2001/06/20 00:42:41 root Exp */

// Dialog procedure for socks settings notebook page

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
#include "dlg_socks.h"
#include "config.h"
#include "strutils.h"

extern void message(char *format, ...);

extern Configuration *p_config;
//------------------------------------------------------------------------


// use this for putenv() so _must_ be static
//char socks_server_var[80] = { '\0' };

MRESULT EXPENTRY Dlg_SocksPage (HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
     switch( msg )
     {
     case WM_INITDLG:
         {
             HWND path = WinWindowFromID(hwnd,SOCKS_ADDRESS);
             WinSendMsg(path,EM_SETTEXTLIMIT,MPFROM2SHORT(79,TRUE),MPFROM2SHORT(0,TRUE));
             
             // if they haven't defined a server, try to get the default one
//             if (*(p_config->getSocksServer()) == '\0')
//             {
                 if (getenv("SOCKS_SERVER") != NULL)
                     WinSetWindowText(path,getenv("SOCKS_SERVER"));
//             }
             else
             {
                 WinSetWindowText(path,"Environment variable SOCKS_SERVER is not set");
             }
//             WinCheckButton(hwnd,SOCKS_USE,(unsigned short)p_config->getSocksServerUse());
         }
         
         
         break;
     //----------------------------------
     case WM_CLOSE:
         {
             /*
             char *buffer = new char[1024];
             WinQueryWindowText(WinWindowFromID(hwnd,SOCKS_ADDRESS),1023,buffer);
             lrSpaceTrim(buffer);
             p_config->setSocksServer(buffer);
             delete[] buffer;
             
             unsigned short previous = WinCheckButton(hwnd,SOCKS_USE,(unsigned short)TRUE);
             p_config->setSocksServerUse(previous);
             */
         }

	 return 0;	// 06 Mar 01 SHL
         
     case WM_COMMAND:
          /*
          switch( SHORT1FROMMP( mp1 ) )
          {
               case PB_DEFAULT:
                   {
                       WinSetWindowText(WinWindowFromID(hwnd,SOCKS_ADDRESS),"");
                       WinCheckButton(hwnd,SOCKS_USE,(unsigned short)TRUE);
                   }
                   break;
          }
          */
          return 0;

     case WM_VALIDATE:
         {
/*
             // get status of check box
             WinEnableWindowUpdate(WinWindowFromID(hwnd,SOCKS_USE),FALSE);
             unsigned short previous = WinCheckButton(hwnd,SOCKS_USE,(unsigned short)TRUE);
             WinCheckButton(hwnd,SOCKS_USE,previous);
             WinEnableWindowUpdate(WinWindowFromID(hwnd,SOCKS_USE),TRUE);
             
             // get address
             char *buffer=new char[1024];
             WinQueryWindowText(WinWindowFromID(hwnd,SOCKS_ADDRESS),1023,buffer);
             lrSpaceTrim(buffer);
             
             // If they say to use socks, & server is not empty
             if (previous && *buffer != '\0')
             {
                 sprintf(socks_server_var,"SOCKS_SERVER=%s",buffer);
                 putenv(socks_server_var);
             }
             else if (!previous)
             {
                 // DONT USE SERVER
                 strcpy(socks_server_var,"SOCKS_SERVER");
                 putenv(socks_server_var);
             }
             
             delete[] buffer;
*/
         }
         return (MPARAM) 1; 
     }

     return WinDefDlgProc( hwnd, msg, mp1, mp2 );

}
