/* newhost.cpp,v 1.2 2001/06/20 00:42:42 root Exp */

#define INCL_PM
#define INCL_WINSYS
#define INCL_WINFRAMEMGR          
#define INCL_WINMENUS             
#define INCL_WINDIALOGS
#define INCL_WINSTDFILE
#define INCL_DOSPROCESS
#define INCL_WINPOINTERS
#define INCL_WINWINDOWMGR
#define INCL_WININPUT
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS
#define INCL_WINSTDSPIN
#define INCL_WINSTDBOOK
#define INCL_WINMLE
#define INCL_WINMENUS
#define INCL_WINSTATICS
#define INCL_WINSTDCNR
#define INCL_WINSTDSLIDER
#define INCL_WINSTDVALSET


#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>

#include <types.h>
//#include <unistd.h>
//#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#include "newhost.h"
#include "hostdlg.h"
#include "strutils.h"
#include "help.h"

char server_name[MAX_SERVER_NAME_LEN] = {"\0"};
char server_keep[MAX_SERVER_NAME_LEN] = {"\0"};
char server_user[32] = {"\0"};
char server_keep_user[32] = {"\0"};
char server_pass[32] = {"\0"};
char server_keep_pass[32] = {"\0"};

void exportServer(char *s)
{
    if (s != NULL)
        strncpy(server_name,s,MAX_SERVER_NAME_LEN);
    else
        memset(server_name,0,MAX_SERVER_NAME_LEN);
}
    
void importServer(char *s)
{
    if (s != NULL)
       strncpy(s,server_name,MAX_SERVER_NAME_LEN);
}

void exportUser(char *s)
{
    if (s != NULL)
        strncpy(server_user,s,32);
    else
        memset(server_user,0,32);
}
    
void importUser(char *s)
{
    if (s != NULL)
       strncpy(s,server_user,32);
}
void exportPass(char *s)
{
    if (s != NULL)
        strncpy(server_pass,s,32);
    else
        memset(server_pass,0,32);
}
    
void importPass(char *s)
{
    if (s != NULL)
       strncpy(s,server_pass,32);
}

#ifndef SLAINC // SHL 12 Jul 00
extern int  _Optlink _beginthread( void ( * _Optlink __thread )( void * ), void *, unsigned, void * );
extern void _Optlink _endthread( void );
#endif // SHL

void _Optlink LookupThread(void *arg)
{
    HAB hab;
    HMQ hmq;
    struct hostent *she;
    
    hab = WinInitialize(0);
    if (hab) hmq = WinCreateMsgQueue(hab,0);
    
    if (isdigit(*server_name))
    {
        // make an address from an ip string
        u_long address = inet_addr(server_name);
        if ((int)address == -1)
        {
            message("IP Address must be in a.b.c.d format");
        }
        else
        {
            she = gethostbyaddr((char*)&address,sizeof(address),AF_INET);
            if (she != NULL)
            {
                strcpy(server_name,she->h_name);
                WinPostMsg((HWND)arg, WM_USER, NULL, NULL );
            }
            else
                message("Lookup on \"%s\" failed",server_name);
        }
    }
    else if (isalpha(*server_name))
    {
        she = gethostbyname(server_name);
        if (she != NULL)
        {
            strcpy(server_name,she->h_name);
            WinPostMsg((HWND)arg, WM_USER, NULL, NULL );
        }
        else
            message("Lookup on \"%s\" failed",server_name);
    }
    else
        DosBeep(100,100);
    
    if (hmq) WinDestroyMsgQueue(hmq);
    if (hab) WinTerminate(hab);
    _endthread();
}



//                                                                         
//                                                                         
//                                                                           
//                                                                           
//                                                                           
MRESULT EXPENTRY HostDlgProc(HWND hwndDlg,ULONG msg,MPARAM mp1,MPARAM mp2)
{
    
    switch(msg)
    {
        
        case WM_INITDLG:
            strcpy(server_keep,server_name);
            strcpy(server_keep_user,server_user);
            strcpy(server_keep_pass,server_pass);
            WinSendMsg(WinWindowFromID(hwndDlg,ID_HOST_FIELD),EM_SETTEXTLIMIT,MPFROM2SHORT(MAX_SERVER_NAME_LEN-1,TRUE),MPFROM2SHORT(0,TRUE));
            WinSendMsg(WinWindowFromID(hwndDlg,ID_HOST_USER),EM_SETTEXTLIMIT,MPFROM2SHORT(32-1,TRUE),MPFROM2SHORT(0,TRUE));
            WinSendMsg(WinWindowFromID(hwndDlg,ID_HOST_PASS),EM_SETTEXTLIMIT,MPFROM2SHORT(32-1,TRUE),MPFROM2SHORT(0,TRUE));
            if (*server_name != '\0')
                WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOST_FIELD),server_name);
            if (*server_user != '\0')
                WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOST_USER),server_user);
            if (*server_pass != '\0')
                WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOST_PASS),server_pass);
            WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwndDlg,ID_HOST_FIELD),0);
            
            break;
                                                                                                                                                              
            
        case WM_CONTROL:
            switch(SHORT2FROMMP(mp1))
            {
                case EN_CHANGE:
                    if (SHORT1FROMMP(mp1) == ID_HOST_FIELD)
                    {
                        char *host_name = new char[4096];
                        WinQueryWindowText(WinWindowFromID(hwndDlg,ID_HOST_FIELD),CCHMAXPATH,host_name);
                        if (removeSpaces(host_name))
                            WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOST_FIELD),host_name);
                        delete[] host_name;
                    }
                    break;
            }
            break;
            
        case WM_USER:
            WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOST_FIELD),server_name);
            break;
            
        case WM_CLOSE:
            strcpy(server_name,server_keep);
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
            
        case WM_HELP:
            showHelpWindow(HELP_ADDHOST);
            break;

        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1 ) )
            {
                case ID_HOST_B_LOOKUP:
                {
                    memset(server_name,0,MAX_SERVER_NAME_LEN);
                    WinQueryWindowText(WinWindowFromID(hwndDlg,ID_HOST_FIELD),CCHMAXPATH,server_name);
                    _beginthread(LookupThread,NULL,32768,(void*)hwndDlg);
                    break;
                }
                case DID_OK:
                    memset(server_name,0,MAX_SERVER_NAME_LEN);
                    memset(server_user,0,32);
                    memset(server_pass,0,32);
                    WinQueryWindowText(WinWindowFromID(hwndDlg,ID_HOST_FIELD),CCHMAXPATH,server_name);
                    WinQueryWindowText(WinWindowFromID(hwndDlg,ID_HOST_USER),CCHMAXPATH,server_user);
                    WinQueryWindowText(WinWindowFromID(hwndDlg,ID_HOST_PASS),CCHMAXPATH,server_pass);
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) TRUE;
                    
                case DID_CANCEL:
                    strcpy(server_name,server_keep);
                    strcpy(server_user,server_keep_user);
                    strcpy(server_pass,server_keep_pass);
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) FALSE;
                    
            }
            break;
        
        default:
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
    }
    return (MRESULT) TRUE;
}



