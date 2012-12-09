
/***********************************************************************

nsdlg.cpp,v 1.6 2003/12/16 03:44:07 root Exp

Newsharvest startup and main dialog

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2001 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	21 Jun 01 SHL - Drop excess new/delete
		22 Jun 01 SHL - Drop obsolete window_ids[] logic
				Rework copyright messages
		19 Jul 02 SHL - Use correct ACCELTABLE id
				Drop excess accelerator setup
				Clean up Help interface
		19 Jul 02 SHL - Support CSet/2 build
		02 Dec 02 SHL - Add version to title

***********************************************************************/

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
#define INCL_WINSTDDRAG
#define INCL_WINSTDSLIDER
#define INCL_WINSTDVALSET
#define INCL_WINACCELERATORS
#define INCL_WINWORKPLACE

#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <direct.h>

#ifdef SOCKS
#include "libsock5.h"
#endif

#include "nsdlg.h"
#include "hostdlg.h"
#include "groupdlg.h"
#include "container.h"
#include "icons.h"
#include "config.h"
#include "download.h"
#include "mimeset.h"
#include "mimedlg.h"
#include "filelist.h"
#include "ledgenddlg.h"
#include "splash.h"
#include "splashdlg.h"
#include "pmtools.h"
#include "threads.h"
#include "help.h"
#include "multipart.h"
#include "Basic.hpp"
#include "Resource.h"
#include "unlink.h"
#include "version.h"

#undef INSERT_EACH_FILE

#define RANDOM(x) (rand()%(x))
#define RANDOMSEED (srand(time(NULL)))

static FILE *outfile = NULL;			// fixme to be gone or used

int can_break = 1;
int done_splash = 234;
int still_loading = 1;
LONG sys_horizontal_size = 0;

// this string will be set to just "groups" if we're registered and "Groups" if not
char *registered = "Groups.$$$";

/*
 * Function Definitions for user interface routines
 *
 */
MRESULT EXPENTRY MainDlgProc(HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY Dlg_Notebook(HWND,ULONG,MPARAM,MPARAM);
void _Optlink splashThread(void *argpack);
void _Optlink loadThread(void *argpack);
void _Optlink timeoutThread(void *arg);
void message(char *format, ...);
void logOptions(Configuration &config);

HAB       hab;                              // Anchor block handle
HMQ       hmq;                              // Message queue handle
Container tree;

Configuration config;
Configuration *p_config;
PSZ       szMainTitle = "News Harvest" VERSION;
DownloadArgs da;
MimeSet   mimes;
HWND      window;

#define START_STR "Go!"
#define STOP_STR  "Stop"

int stopping;
static TID thread_id;
static TID old_thread_id;

int looping;
int looping_times = -1;

// Enable/disabled by F12
int   tx_rx_log;
char *tx_rx_logname;

char programDir[_MAX_PATH];		// Location of executables and resources
char szLicensePath[_MAX_PATH];		// Location of license file

static char mimesPath[_MAX_PATH];

// what we use to putenv SOCKS var
//char socks_server[80];

int main(int argc,char **argv)
{
    QMSG  qmsg;
    BOOL success;
    
    // run without user intervention
    if (argc > 1)
    {
        log("WARNING: Command line arguments are obsolete!\nPlease use the \"User Interface\" options of the Configuration Panel");
/*
        for (int i=1; i<argc; i++)
        {
            if (stricmp(argv[i],"-auto")==0)
                unattended = 1;
            else if (strncmp(argv[i],"-loop",5)==0)
            {
                looping = atoi((argv[i])+5);
                if (looping <= 0)
                    looping = 1;
                if (strchr(argv[i],':'))
                {
                    looping_times = atoi(strchr(argv[i],':')+1);
                    if (looping_times == 0)
                        looping_times = -1;
                }
            }
        }
*/
    }
    
    // make sure only one copy is running
    HMTX sem_handle = OneInstance("\\SEM32\\NH_NSDLG",TRUE);
    if (sem_handle == NULLHANDLE)
        exit(0);
    
#ifdef SOCKS
//    SOCKSinit(argv[0]);
#endif
    
    RANDOMSEED;

    char *szDrv = new char[_MAX_DRIVE];
    char *szDir = new char[_MAX_DIR];

#if __IBMCPP__ < 300
    char *szName = new char[_MAX_PATH];
    char *szExt = new char[_MAX_PATH;
    _splitpath(argv[0], szDrv, szDir, szName, szExt);
#else
    _splitpath(argv[0], szDrv, szDir, NULL, NULL);
#endif

    _splitpath(argv[0], szDrv, szDir, NULL, NULL);

    _makepath(programDir, szDrv, szDir, "", "");
    _makepath(szLicensePath, "", programDir, "licence", "");

    delete[] szDrv;
    delete[] szDir;
    
    hab = WinInitialize( 0 );
    if(!hab)
    {
      DosBeep(60,100);
      exit(2);
    }
  
    hmq = WinCreateMsgQueue( hab, 0 );
    if(hmq != NULLHANDLE)
    {
        HWND hwnd_dlg;
        
        hwnd_dlg = WinLoadDlg(HWND_DESKTOP,NULLHANDLE,(PFNWP)MainDlgProc,NULLHANDLE,DLG_MAIN,NULL);
        if (hwnd_dlg != NULLHANDLE)
        {
            
            // set the program icon
            HPOINTER h_icon = WinLoadPointer(HWND_DESKTOP,NULLHANDLE,ICON_PROG);
            if (h_icon != NULLHANDLE)
            {
                WinSendMsg(hwnd_dlg,WM_SETICON,MPFROMLONG(h_icon),MPVOID);
            }
            
            WinProcessDlg(hwnd_dlg);
            WinDestroyWindow(hwnd_dlg);
        }
        else
        {
            DosBeep(60,100);
            unsigned long err = WinGetLastError(hab);
            message("Dialog Error 0x%lx",err);
        }
        WinTerminate(hab);
    }
    else
    {
        DosBeep(60,100);
        WinTerminate(hab);
    }
    
/*
    // show the dialog to do the work 
    success = WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop    
                         HWND_DESKTOP,       // Owned by desk top            
                         (PFNWP)MainDlgProc, // Addr. of procedure  
                         (HMODULE)0,         // Module handle                
                         DLG_MAIN,          // Dialog identifier in resource
                         NULL);              // Initialization data          
    
    if (success == DID_ERROR)
    {
        DosBeep(60,100);
        unsigned long err = WinGetLastError(hab);
        message("Dialog Error 0x%lx",err);
    }
*/
    return 0;

} // main

MRESULT EXPENTRY MainDlgProc(HWND hwndDlg,ULONG msg,MPARAM mp1,MPARAM mp2)
{
    static HPOINTER host_o,host_c,hp_wait,hp_orig,h_icon;
    HWND   cont;
    time_t the_time;
    int    i,j,x;
    static UserData *ud,*grp;
    static small_icons = 0;
    static FileList *dir;
    static int iconised = 0;
    static int original_width  = 0;
    static int original_length = 0;
    static RECTL previous = { 0L, 0L, 0L, 0L }; // xLeft, yBottom, xRight, yTop
    static RECTL current;
    SHORT old_h;
    SHORT old_v;
    SHORT new_h;
    SHORT new_v;
    int   h_delta;
    int   v_delta;
    static ThreadThings flt;
    char *bitmap_filename;
    char *psz;
    int no_delete;
    
    switch(msg)
    {
        case WM_INITDLG:
            // Set the title to include version number
            WinSetWindowText(hwndDlg,"News Harvest " VERSION);  // NOTE: C++ will concatenate 2 string "abc" "def" at compile

            // set the wait pointer
            hp_wait = WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
            hp_orig = WinQueryPointer(HWND_DESKTOP);
            WinSetPointer(HWND_DESKTOP,hp_wait);
            
            // Set a nice bitmap on the title
            bitmap_filename = "640.bmp";
            if (WinQueryWindowRect(WinWindowFromID(hwndDlg,ID_B_ABOUT),&current) == TRUE)
            {
                // message("xLeft = %d, yBottom = %d, xRight = %d, yTop = %d",current.xLeft,current.yBottom,current.xRight,current.yTop);
                if (current.xRight < 252)
                    bitmap_filename = "640.bmp";
                else if (current.xRight < 336)
                    bitmap_filename = "800.bmp";
                else 
                    bitmap_filename = "1024.bmp";
                
                // sys_horizontal_size = WinQuerySysValue(HWND_DESKTOP,SV_CXFULLSCREEN);
                // if (sys_horizontal_size < 800)
                //     bitmap_filename = "640.bmp";
                // else if (sys_horizontal_size < 1024)
                //     bitmap_filename = "800.bmp";
                // else if (sys_horizontal_size < 1280)
                //     bitmap_filename = "1024.bmp";
                // else if (sys_horizontal_size <= 1600)
                //     bitmap_filename = "1280.bmp";
            }
            setButtonBmp(hwndDlg,ID_B_ABOUT,loadBitmap(hwndDlg,bitmap_filename));
            
            // Restore or Center the window
//            if (WinRestoreWindowPos("NsDlg","Main",hwndDlg))
//            {
//                // WinRestoreWindowPos("NsDlg","Cont",WinWindowFromID(hwndDlg,ID_CONTAINER));
//            }
//            else
            {
                pmtoolsCenterWindow(hwndDlg);
            }

            // Work out the dimensions of the window
            SWP swp;
            WinQueryWindowPos(hwndDlg,(PSWP)&swp);
            original_width  = swp.cx;
            original_length = swp.cy;
            
            window = hwndDlg;
            
            // add us to the task list
            PID pid;
            SWCNTRL   swCntrl;
            WinQueryWindowProcess(hwndDlg,&pid,NULL);
            swCntrl.hwnd = hwndDlg;
            swCntrl.hwndIcon = NULL;
            swCntrl.hprog = (HPROGRAM) NULL;
            swCntrl.idProcess = pid;
            swCntrl.idSession = (LONG) NULL;
            swCntrl.uchVisibility = SWL_VISIBLE;
            swCntrl.fbJump = SWL_JUMPABLE;
            strncpy(swCntrl.szSwtitle,szMainTitle,MAXNAMEL);
            WinCreateSwitchEntry(hab,(PSWCNTRL)&swCntrl);
            
            host_c = WinLoadPointer(HWND_DESKTOP,0L,ICON_HOST_C);
            host_o = WinLoadPointer(HWND_DESKTOP,0L,ICON_HOST_O);
            
            cont = WinWindowFromID(hwndDlg,ID_CONTAINER);
            tree.initialise(hwndDlg,ID_CONTAINER);
            
            config.read("groups");
            
            // user interface options for looping
            looping = config.getUserLoopDelay();
            looping_times = -1;
            if (config.getUserLoopTimes() > 0)
            {
                if (looping == 0)
                    looping = 5;
                looping_times = config.getUserLoopTimes();
            }
            else
            {
                looping = 0;
            }
            
            p_config = &config;
            
            flt.tree   = &tree;
            flt.config = &config;
            flt.hwnd   = hwndDlg;
            
            // Show splash screen
            _beginthread(splashThread,NULL,32768,(void*)&flt);
            
            // Don't allow run until load is complete
            WinEnableControl(hwndDlg,ID_B_START,FALSE);

            // Put all previously extracted files in tree
            _beginthread(loadThread,NULL,32768,(void*)&flt);
            
	    _makepath(mimesPath,"",programDir,"mimes","");
            mimes.setFilename(mimesPath);
            mimes.load();
            
            log("");
            log("           News Harvest, %-s",VERSION);
            log("  Copyright 1997, 1998 KRT Pty. Limited");
#ifdef	    SLAINC
            log("     Copyright 2000, 2001 SLA, Inc.");
#endif
            log("");
            the_time = time(NULL);
            log("       %s",ctime(&the_time));
            srand(the_time);
            logOptions(config);
	    log(0);
            
            WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwndDlg,ID_CONTAINER),0);

            WinSetPointer(HWND_DESKTOP,hp_orig);
            
            // Start Help
            helpInit(&hab,&hwndDlg);
            
            /*
            small_icons = config.getSmallIcons();
            if (config.getSmallIcons())
                tree.toggleSmallIcons();
            */
            
            // small_icons = config.getUserSmallIcons();
            if (config.getUserSmallIcons() == 1)
                tree.toggleSmallIcons();
            
#if 0 // fixme to be gone?
            message("getenv(\"SOCKS_SERVER\") -> \"%s\"\n",(getenv("SOCKS_SERVER")?getenv("SOCKS_SERVER"):"null"));
            putenv("SOCKS_SERVER");
            message("getenv(\"SOCKS_SERVER\") -> \"%s\"\n",(getenv("SOCKS_SERVER")?getenv("SOCKS_SERVER"):"null"));
            putenv("SOCKS_SERVER=192.168.0.10");
            message("getenv(\"SOCKS_SERVER\") -> \"%s\"\n",(getenv("SOCKS_SERVER")?getenv("SOCKS_SERVER"):"null"));
            putenv("SOCKS_SERVER");
            message("getenv(\"SOCKS_SERVER\") -> \"%s\"\n",(getenv("SOCKS_SERVER")?getenv("SOCKS_SERVER"):"null"));
            
            // check out SOCKS stuff
            if (config.getSocksServerUse() == 0)
            {
                if (getenv("SOCKS_SERVER") != NULL)
                    putenv("SOCKS_SERVER");  // kill env variable
                if (getenv("SOCKS_SERVER") != NULL)
                    message("Unable to turn off SOCKS");
            }
            else
            {
                if (*(config.getSocksServer()) != '\0')
                {
                    if (putenv("SOCKS_SERVER") == -1) 
                        message("clear Failed");
                    sprintf(socks_server,"SOCKS_SERVER=%s",config.getSocksServer());
                    message("putenv(\"%s\")",socks_server);
                    if (putenv(socks_server) == -1) message("Failed");
                }
            }
#endif // fixme to be gone?

            break;
            
        case WM_MINMAXFRAME:
            iconised = !iconised;

            if (iconised == 1)
            {
                WinShowWindow(WinWindowFromID(hwndDlg,ID_CONTAINER),FALSE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_ARTICLE_BOX),FALSE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_ARTICLE_LINES),FALSE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_ARTICLE_SUBJECT),FALSE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_ARTICLE_PERCENT),FALSE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_B_START),FALSE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_B_CONFIG),FALSE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_B_HELP),FALSE);
            }
            else
            {
                WinShowWindow(WinWindowFromID(hwndDlg,ID_CONTAINER),TRUE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_ARTICLE_BOX),TRUE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_ARTICLE_LINES),TRUE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_ARTICLE_SUBJECT),TRUE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_ARTICLE_PERCENT),TRUE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_B_START),TRUE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_B_CONFIG),TRUE);
                WinShowWindow(WinWindowFromID(hwndDlg,ID_B_HELP),TRUE);
            }
            
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
            
        case WM_ADJUSTWINDOWPOS:
            if (!iconised && original_width > 0)
            {
                // has our window changed size/moved?
                WinQueryWindowRect(hwndDlg,&current);
                
                if (previous.xLeft == 0 && previous.yBottom == 0 &&
                    previous.xRight== 0 && previous.yTop == 0)
                {
		    // first time
                    memcpy(&previous,&current,sizeof(RECTL));
                    
                    // get the _ORIGINAL_ window stats
#if 0
                    for (x=0; x<NUM_WINDOWS; x++)
                        WinQueryWindowPos(WinWindowFromID(hwndDlg,window_ids[x]),(PSWP)&original_window_stats[x]);
                    
                    // now the container
                    WinQueryWindowPos(WinWindowFromID(hwndDlg,ID_CONTAINER),(PSWP)&original_container_stat);
#endif
                }
                else
                {
		    // calc change
                    old_h = previous.xRight - previous.xLeft;
                    old_v = previous.yTop - previous.yBottom;
                    new_h = current.xRight - current.xLeft;
                    new_v = current.yTop  - current.yBottom;
                    h_delta = (int)(new_h - old_h);
                    v_delta = (int)(new_v - old_v);
                    
                    if (v_delta != 0 || h_delta != 0)
                    {
                        // remember the old size
                        memcpy(&previous,&current,sizeof(RECTL));
                        
                        if (new_h >= original_width && new_h != 0)
                        {
                            // get pos & size of each window tp be moved
                            // then update their position
                            
                            // change size of container
                            SWP  cont_pos;
                            WinQueryWindowPos(WinWindowFromID(hwndDlg,ID_CONTAINER),(PSWP)&cont_pos);
                            cont_pos.cx += (long)h_delta;
                            cont_pos.cy += (long)v_delta;
                            WinSetMultWindowPos(hab,&cont_pos,1);
                            
                        }
                    }
                }
            }
            
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
            
        case WM_CLOSE:
            {
                // confirm program exit
                if (!config.getUserAutoStart())
                {
                    int rc = WinMessageBox(HWND_DESKTOP,hwndDlg,"Exit News Harvest ?","News Harvest " VERSION,0,MB_QUERY|MB_YESNO);
                    if (rc != MBID_YES)
                        break;
                }
                
                if (done_splash != DONE_SPLASH)
                    break;
                
                //tree.getConfigList(stdout);
                
                // don't save the container if it's not fully loaded
                if (!still_loading)
                    config.write("groups",&tree);
                
                tree.setDelete(0);  // turn off container clean up
                
                the_time = time(NULL);
                log("Window Closed %s",ctime(&the_time));
                log("\n\n_.-'~`-._.-'~`-._.-'~`-._.-'~`-._.-'~`-._.-'~`-._.-'~`-._.-'~`-._\n\n");
                
                // WinStoreWindowPos("NsDlg","Main",hwndDlg);
                // WinStoreWindowPos("NsDlg","Cont",WinWindowFromID(hwndDlg,ID_CONTAINER));
                
                killHelp();
                
                return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
            }

        case DL_SET_TITLE:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
	    WinSetWindowText(hwndDlg, psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller

	    break; // DL_SET_TITLE

        case DL_SHOW_SERVER:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_BOX), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GROUP:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_BOX), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_ARTICLE:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_ARTICLE_BOX), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SUBJECT:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_ARTICLE_SUBJECT), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_LINES:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_ARTICLE_LINES), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_PERCENT:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_ARTICLE_PERCENT), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SPEED:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_ARTICLE_KPS), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SEXTRACT:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_EXTRACT), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SREPOST:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_SKIP), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SCULL:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_CULL), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SFILTER:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_FILTER), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SEXTRACT_MB:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_EXTRACT_MB), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SREPOST_MB:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_SKIP_MB), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SCULL_MB:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_CULL_MB), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_SFILTER_MB:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_SERVER_FILTER_MB), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GEXTRACT:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_EXTRACT), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GREPOST:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_SKIP), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GCULL:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_CULL), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GFILTER:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_FILTER), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GEXTRACT_MB:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_EXTRACT_MB), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GREPOST_MB:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_SKIP_MB), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GCULL_MB:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_CULL_MB), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_SHOW_GFILTER_MB:
            psz = (char *)PVOIDFROMMP(mp1);
	    no_delete = SHORT1FROMMP(mp2);
            WinSetWindowText(WinWindowFromID(hwndDlg, ID_GROUP_FILTER_MB), psz);
	    if (!no_delete)
                delete[] psz;		// Delete for caller
	    break;

        case DL_TERM:
            thread_id = 0;
            running = 0;
            old_thread_id = 0;
            stopping = 0;
            WinSetWindowText(WinWindowFromID(hwndDlg,ID_B_START),(running?STOP_STR:START_STR));
            WinEnableControl(hwndDlg,ID_B_START,TRUE);
            // End automatically
            if (config.getUserAutoStart())
                WinPostMsg(hwndDlg,WM_CLOSE,0,0);
            break;            
            
        case DL_SAVESTATE:
            if (done_splash == DONE_SPLASH)
                config.write("groups",&tree);
            break;
            
        case DL_FINISHED:
            thread_id = 0;
            running = 0;
            old_thread_id = 0;
            stopping = 0;
            WinSetWindowText(WinWindowFromID(hwndDlg,ID_B_START),(running?STOP_STR:START_STR));
            WinEnableControl(hwndDlg,ID_B_START,TRUE);
            
            // save the config
            WinSendMsg(hwndDlg,DL_SAVESTATE,0,0);
            
            // End automatically
            if (config.getUserAutoStart())
                WinPostMsg(hwndDlg,WM_CLOSE,0,0);

            break;
        
        case WM_CONTROL:
            switch(SHORT2FROMMP(mp1))
            {
                case DM_RENDER:
                    message("Got DM_RENDER");
                    break;
                    
                case DM_RENDERFILE:
                    message("Got DM_RENDERFILE");
                    break;
                    
                case DM_FILERENDERED:
                    message("Got DM_FILERENDERED");
                    break;
                        
                case CN_HELP:
                    WinPostMsg(hwndDlg,WM_HELP,0,0);
                    break;
                    
                case CN_INITDRAG:
                {
                    PCNRDRAGINIT cdi = (PCNRDRAGINIT)mp2;
                    PRECORDCORE  rc  = (PRECORDCORE)cdi->pRecord;
                    
                    ud = (UserData *)rc;
                    
                    if (ud && ud->isPlain() && ud->getParent())
                    {
                        // message("Drag of %s",ud->getName());
                        
                        BOOL success;
                        PCSZ  file[1];
                        PCSZ  type[1];
                        PCSZ  target[1];
                        HPOINTER drag_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_DRAG);
                        char filename[CCHMAXPATH];
			// fixme to avoid overflow
                        
                        if (*(config.getBaseDir()))
                            sprintf(filename,"%s%s\\%s",config.getBaseDir(),ud->getParent()->getName(),ud->getName());
                        else
                            sprintf(filename,"%s\\%s",ud->getParent()->getName(),ud->getName());
                        
                        file[0]  = filename;
                        type[0]   = NULL;
                        target[0] = NULL;
                        
                        success = DrgDragFiles(hwndDlg,file,type,target,1,drag_icon,VK_ENDDRAG,FALSE,0L);
                        
                        if (!success)
                            DosBeep(100,100);
                        else
                        {
                            ud = tree.findSelectedRecord(NULL);
                            if (ud)
                            {
                                FILE *exists = fopen(filename,"r");
                                if (exists)
                                    fclose(exists);
                                else
                                {
                                    tree.remove(ud);
                                    tree.arrange();
                                }
                            }
                        }
                    }
                    
                    break;  // CN_INITDRAG
                } // CN_INITDRAG                    
                    
                case CN_CONTEXTMENU:
                {
                    POINTL point;
                    RECTL rclContainer;
                    HWND hwndContainer = WinWindowFromID(hwndDlg,ID_CONTAINER);
                    WinQueryPointerPos(HWND_DESKTOP, &point);
                    WinMapWindowPoints(HWND_DESKTOP,hwndContainer,&point,1);
                    WinQueryWindowRect(WinWindowFromID(hwndDlg,ID_CONTAINER),&rclContainer);
                    
                    // mouse is not inside container, map menu it to center
                    if (!WinPtInRect(hab,&rclContainer,&point))
                    {
                        point.x = (rclContainer.xRight - rclContainer.xLeft)/2;
                        point.y = (rclContainer.yTop - rclContainer.yBottom)/2;
                    }
                    
                    // clicked over record (if any) is sent in mp2
                    RECORDCORE *prc = (PRECORDCORE)mp2;
                    if (prc != NULL)
                    {
                        ud = (UserData *)prc;
                        tree.selected(ud);
                    }
                    else
                    {
                        ud = tree.findSelectedRecord(NULL);
                    }
                    
                    if (ud == NULL)
                    {
                        HWND hwndMenu = WinLoadMenu(hwndDlg, 0, ID_CONTAINER_BASE_MENU);
//                        if (small_icons)
//                            WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MINI_TOGGLE,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));

                        WinPopupMenu(hwndContainer, hwndDlg, hwndMenu,
                                     point.x, point.y,
                                     IDM_ADDSERVER, PU_NONE | PU_SELECTITEM | PU_HCONSTRAIN | PU_VCONSTRAIN |
                                     PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD);
                    }
                    else if (ud->isGroup())
                    {
                        HWND hwndMenu = WinLoadMenu(hwndDlg, 0, ID_CONTAINER_BASE_MENU_DEL);
//                        if (small_icons)
//                            WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MINI_TOGGLE,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
                        if (ud->getSkipped())
                        {
                            WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_SKIP,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
                        }
                        
                        // no GOTO if not running
                        WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_GOTO,TRUE),MPFROM2SHORT(MIA_DISABLED,(running?0:MIA_DISABLED)));
                        
                        WinPopupMenu(hwndContainer, hwndDlg, hwndMenu,
                                     point.x, point.y,
                                     IDM_ADDSERVER, PU_NONE | PU_SELECTITEM | PU_HCONSTRAIN | PU_VCONSTRAIN |
                                     PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD);
                    }
                    else if (ud->isServer())
                    {
                        HWND hwndMenu = WinLoadMenu(hwndDlg, 0, ID_CONTAINER_BASGRP_MENU);
//                        if (small_icons)
//                            WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MINI_TOGGLE,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
                        WinPopupMenu(hwndContainer, hwndDlg, hwndMenu,
                                     point.x, point.y,
                                     IDM_ADDSERVER, PU_NONE | PU_SELECTITEM | PU_HCONSTRAIN | PU_VCONSTRAIN |
                                     PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD);
                        
                    }
                    else if (ud->isPlain())
                    {
                        HWND hwndMenu = WinLoadMenu(hwndDlg, 0, ID_CONTAINER_PLAIN_MENU);
//                        if (small_icons)
//                            WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MINI_TOGGLE,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
                        WinPopupMenu(hwndContainer, hwndDlg, hwndMenu,
                                     point.x, point.y,
                                     IDM_DELETE, PU_NONE | PU_SELECTITEM | PU_HCONSTRAIN | PU_VCONSTRAIN |
                                     PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD);
                        
                    }
                    
                    // HERE tree.arrange();
                    
                    break;
                } // CN_CONTEXTMENU
                    
                case CN_ENTER:
                    
                    PNOTIFYRECORDENTER nre = (PNOTIFYRECORDENTER)mp2;
                    ud = (UserData*)nre->pRecord;
                    if (ud != NULL)
                    {
                        if (ud->isGroup())
                        {
                            
                            // show the dialog to do the work 
                            tree.emphasis(ud);  // be sure we are emphasized
                            exportGroupName(ud->getName());
                            exportGroupNumber(ud->getStart());
                            exportGroupCull(ud->getCull());
                            // no list available
                            exportGroupUsername(NULL);
                            exportGroupPassword(NULL);
        
                            WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop
                                       hwndDlg,            // Owned by main dialog
                                       (PFNWP)GroupDlgProc,// Addr. of procedure
                                       (HMODULE)0,         // Module handle                
                                       NEW_GROUP_DIALOG,   // Dialog identifier in resource
                                       NULL);              // Initialization data          
                            
                            char *group = new char[8192];
                            importGroupName(group);
                            ud->setCull(importGroupCull());
                            config.setLineCull(importGroupCull());
                            ud->setStart(importGroupNumber());
                            tree.noEmphasis(ud);  // be sure we are emphasized
                            if (*group != '\0' && strcmp(ud->getName(),group) != 0)
                            {
                                ud->setName(group);
                                tree.arrangeText(ud);
                            }
                            delete[] group;                            
                            
                        }
                        
                        else if (ud->isServer())
                        {
                            tree.emphasis(ud);  // be sure we are emphasized
                            exportServer(ud->getName());
                            exportUser(ud->getUsername());
                            exportPass(ud->getPassword());
                            WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop
                                       hwndDlg,            // Owned by main dialog
                                       (PFNWP)HostDlgProc, // Addr. of procedure
                                       (HMODULE)0,         // Module handle                
                                       ID_HOST_DIALOG,     // Dialog identifier in resource
                                       NULL);              // Initialization data          
                            char *server = new char[1024];
                            char *user   = new char[1024];
                            char *pass   = new char[1024];
                            importServer(server);
                            importUser(user);
                            importPass(pass);
                            
                            tree.noEmphasis(ud);  // be sure we are emphasized
                            if (strcmp(ud->getName(),server) != 0)
                            {
                                ud->setName(server);
                                tree.arrangeText(ud);
                            }
                            ud->setUsername(user);
                            ud->setPassword(pass);
                            
                            delete[] server;
                            delete[] user;
                            delete[] pass;
                            
                        }
                        else if (ud->isPlain())
                        {
			    // Spawn viewer
                            char *command = mimes.determineType(ud->getName());
                            if (!command || *command == '\0')
                            {
                                // Use OS/2 default
                                ud = tree.findSelectedRecord(NULL);
                                if (ud && ud->getParent())
                                {
                                    char *filename = new char[CCHMAXPATH];
                                    char *open_type = "OPEN=DEFAULT";
                                    UserData *parent = ud->getParent();
                                    
                                    memset(filename,0,CCHMAXPATH);
                                    if (*(config.getBaseDir()))
                                    {
                                        strcpy(filename,config.getBaseDir());
                                        strcat(filename,parent->getName());
                                    }
                                    else
                                    {
                                        strcpy(filename,parent->getName());
                                    }
                                    
                                    // is it fully qualified path ?
                                    char *curr_dir = new char[CCHMAXPATH];
                                    ULONG dir_size = CCHMAXPATH;
                                    if (!strchr(filename,':'))
                                    {
                                        DosQueryCurrentDir(0,curr_dir,&dir_size);
                                        
                                        // does it only need the drive letter?
                                        if (*filename == '\\')
                                        {
                                            curr_dir[2] = '\0';  // only keep "X:"
                                            strcat(curr_dir,filename);
                                        }
                                        // or the whole thing
                                        else
                                        {
                                            ULONG drive_num=0,drive_table=0;
                                            DosQueryCurrentDisk(&drive_num,&drive_table);
                                            char disk_letter[4] = "C:\\";
                                            
                                            *disk_letter = 'A' + drive_num -1;
                                            
                                            strcat(curr_dir,"\\");
                                            strcat(curr_dir,filename);
                                            strcpy(filename,disk_letter);
                                            strcat(filename,curr_dir);
                                            strcpy(curr_dir,filename);
                                        }
                                    }
                                    else
                                    {
                                        strcpy(curr_dir,filename);
                                    }
                                    
                                    strcat(curr_dir,"\\");
                                    strcat(curr_dir,ud->getName());
                                    // message("Open %s",curr_dir);
                                        
                                    HOBJECT hobj = (HOBJECT)WinQueryObject(curr_dir);
                                    
                                    if (hobj != NULLHANDLE)
                                        WinSetObjectData(hobj,open_type);
                                    else
                                        message("Unable to open object");
                                    
                                    delete[] curr_dir;
                                    delete[] filename;
                                }
                                
                            }
                            else
                            {
                                char *com_buf = new char[1024];
                                char *exe = new char[1024];
                                
                                strcpy(com_buf,command);
                                char *prog = strchr(com_buf,' ');
                                if (prog)
                                {
                                    *prog = '\0';
                                    char *full = pmtoolsFindExecutable(com_buf);
                                    if (full != NULL)
                                        strcpy(exe,full);
                                    else
                                        strcpy(exe,com_buf);
                                    *prog = ' ';
                                    prog++;
                                    strcat(exe," ");
                                }
                                
                                char *ptr = strstr(com_buf,"%1");
                                if (ptr)
                                {
                                    UserData *parent = ud->getParent();
                                    if (parent)
                                    {
                                        strcpy(ptr,"\0");
                                        
                                        if (strchr(ud->getName(),' ')) // space in filename
                                        {
                                            strcat(ptr,"\"");
                                        }
                                        
                                        // add base directory (if any)
                                        if (*(config.getBaseDir()))
                                            strcat(ptr,config.getBaseDir());
                                        
                                        // add newsgroup
                                        strcat(ptr,parent->getName());

                                        strcat(ptr,"\\");
                                        strcat(ptr,ud->getName());
                                        if (strchr(ud->getName(),' '))
                                            strcat(ptr,"\"");
                                        ptr = strstr(command,"%1");
                                        if (ptr)
                                        {
                                             strcat(com_buf,ptr+2);
                                            // message("Execute \"%s\"",com_buf);
                                            
                                            //system(com_buf);
                                            // dialog, .exe,  args+filename
                                            // log("Launching \"%s %s\"",exe,prog);
                                            
                                            // must system() .cmd executables
                                            ptr = stristr(exe,".cmd");
                                            if (ptr)
                                            {
                                                char *the_com = new char[4096];
                                                if (the_com)
                                                {
                                                    sprintf(the_com,"detach %s %s",exe,prog);
                                                    if (config.getLogSpawn())
                                                       log("Launching CMD \"%s\"",the_com+7);
                                                    system(the_com);
                                                    delete[] the_com;
                                                }
                                            }
                                            else
                                            {
                                                pmtoolsLaunchApplication(hwndDlg,exe,prog);
                                            }
                                        }
                                    }
                                }
                                delete[] com_buf;
                                delete[] exe;
                            }
                        }
                    }
                    break; // CN_ENTER
            } // switch

            break; // WM_CONTROL
            
        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1 ) )
            {
                case ID_B_ABOUT:
                    WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop
                               hwndDlg,            // Owned by main dialog
                               (PFNWP)LedgendDlgProc, // Addr. of procedure
                               (HMODULE)0,         // Module handle                
                               ID_LEDGENDS_DLG,    // Dialog identifier in resource
                               (PVOID)NULL);       // Initialization data

                    break;
                    
                case ID_B_HELP:
                    showHelpWindow(HELP_INTRO);
                    
                    break;
                    
                case ID_B_CONFIG:
                    {
                        int x = config.getUserSmallIcons();

			// Ensure Alt-f4 close does not get to main dialog system menu
                        HWND hwndSysMenu = WinWindowFromID(hwndDlg, FID_SYSMENU);
                     
                        WinEnableMenuItem(hwndSysMenu, SC_SYSMENU, FALSE);

                        WinDlgBox(HWND_DESKTOP,       // Place anywhere on desktop
                                  hwndDlg,            // Owned by main dialog
                                  (PFNWP)Dlg_Notebook, // Addr. of procedure
                                  (HMODULE)0,         // Module handle                
                                  DLG_NOTEBOOK,  // Dialog identifier in resource
                                  (PVOID)&config);    // Initialization data
                        
                        WinEnableMenuItem(hwndSysMenu, SC_SYSMENU, TRUE);

                        if (x != config.getUserSmallIcons())
                            tree.toggleSmallIcons();
                        
                        // user interface options for looping
                        looping = config.getUserLoopDelay();
                        looping_times = -1;
                        if (config.getUserLoopTimes() > 0)
                        {
                            if (looping == 0)
                                looping = 5;
                            looping_times = config.getUserLoopTimes();
                        }
                        else
                        {
                            looping = 0;
                        }
                    }
                    
                    logOptions(config);
                    
                    if (strcmp(registered,"groups") != 0)
                    {
                        // stop after extracting ~200 binaries, or ~10 MB
                        config.setMaxArticles(100+RANDOM(200));
                        config.setMaxDownload(5+RANDOM(10));
                    }

                    // save the config
                    WinPostMsg(hwndDlg,DL_SAVESTATE,0,0);
                    
                    break;
                    
                case ID_B_START:
                    // prepare struct for thread arguments
                    if (!running)
                    {
                        int is_ok = config.checkOutputStuff();
                        
                        if (is_ok == CONFIG_OK)
                        {
                            da.tree = &tree;
                            da.config = &config;
                            da.parent = hwndDlg;
                            da.switch_to = 0;
                            da.finish_up = 0;
                            thread_id = _beginthread(DownloadThread,NULL,65536,(void*)&da);
                            if (thread_id == -1)
                            {
                                message("Unable to start processing thread");
                                thread_id = 0;
                            }
                            else
                            {
                                running = 1;
                                WinSetWindowText(WinWindowFromID(hwndDlg,ID_B_START),(running?STOP_STR:START_STR));
                            }
                        }
                        else
                        {
                            switch(is_ok)
                            {
                                case CONFIG_BAD_OUTPUTPATH:
                                    log("Problem with Configured Output Path, does it exist? allow long filenames?");
                                    message("There is a problem with the Configured Output Path.\n Does it exist?\n Allow long filenames?");
                                    break;
                            }
                        }
                    }
                    else 
                    {
                        if (done_splash == DONE_SPLASH)
                        {
                            // start a thread to clobber it if it don't stop
                            _beginthread(timeoutThread,NULL,32768,NULL);
                            // tell it to stop
                            old_thread_id = thread_id;
                            thread_id = 0;
                            running = 0;
                            stopping = 1;
                            // Grey out re-start button
                            WinSetWindowText(WinWindowFromID(hwndDlg,ID_B_START),(running?STOP_STR:START_STR));
                            WinEnableControl(hwndDlg,ID_B_START,FALSE);
                        }
                    }
                    
                    break;
                    
                case ID_B_FINISH:
                    // Stop at end of current operation
                    if (running)
                        da.finish_up = 1;
                    
                    break; // ID_B_FINISH
                    
//                case IDM_MINI_TOGGLE:
//                    {
//                        tree.toggleSmallIcons();
//                        small_icons = !small_icons;
//                        config.setSmallIcons(small_icons);
//                    }
//                    break;
                    
                case IDM_SKIP:
                    ud = tree.findSelectedRecord(NULL);
                    if (ud)
                    {
                        if (ud->isGroup())
                        {
                            ud->setSkipped(!ud->getSkipped());
                            if (ud->getSkipped())
                                tree.setIcon(ud,ICON_GROUP_C);
                            else
                                tree.setIcon(ud,ICON_GROUP_O);
                        }
                    }
                    break;
                    
                case IDM_GOTO:
                    ud = tree.findSelectedRecord(NULL);
                    if (ud)
                    {
                        if (running && ud->isGroup())
                        {
                            da.switch_to = ud;
                        }
                    }
                    break;
                    
                case IDM_LOG:
                    
                    if (!tx_rx_log)
                    {
                        message("Transmission log enabled");
                        tx_rx_logname = "txrx_log";
                    }
                    else
                    {
                        message("Transmission log disabled");
                        tx_rx_logname = 0;
                    }
                    
                    tx_rx_log = !tx_rx_log;
                    
                    break;
                    
                case IDM_MIME_TYPES:
                    WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop
                               hwndDlg,            // Owned by main dialog
                               (PFNWP)MimeDlgProc, // Addr. of procedure
                               (HMODULE)0,         // Module handle                
                               MIME_DLG,  	   // Dialog identifier in resource
                               (PVOID)&mimes);     // Initialization data
                    break;
                    
                case IDM_DELETE:
                    ud = tree.findSelectedRecord(NULL);
                    if (ud)
                    {
                        if (ud->isPlain())
                        {
                            UserData *parent = ud->getParent();
                            if (parent)
                            {
                                char *filename = new char[CCHMAXPATH];
                                
                                if (*(config.getBaseDir()))
                                    sprintf(filename,"%s%s\\%s",config.getBaseDir(),ud->getParent()->getName(),ud->getName());
                                else
                                    sprintf(filename,"%s\\%s",ud->getParent()->getName(),ud->getName());

                                // message("Should unlink(\"%s\")",filename);
                                deleteFile(filename);
                                
                                delete[] filename;
                           }
                        }

                        tree.remove(ud);
                        tree.arrange();
                    }
                    break;
                    
                case IDM_CONTEXT_MENU:
                    
//                    message("Shift-F10");
                    WinSendMsg(hwndDlg,WM_CONTROL,MPFROM2SHORT(0,CN_CONTEXTMENU),0);
                    
                    break;
                    
                // OPEN the desktop folder for the group directory
                case IDM_OPEN:
                case IDM_OPEN_DEFAULT:
                case IDM_OPEN_ICON:
                case IDM_OPEN_TEXT:
                case IDM_OPEN_NAME:
                case IDM_OPEN_DETAIL:
                
                    ud = tree.findSelectedRecord(NULL);
                    if (ud)
                    {
                        char *filename = new char[CCHMAXPATH];
                        char *open_type = "OPEN=DEFAULT";
/*
                        switch( SHORT1FROMMP( mp1 ) )
                        {
                            case IDM_OPEN_ICON:   open_type = "OPEN=ICON"; break;
                            case IDM_OPEN_TEXT:   open_type = "OPEN=TEXT"; break;
                            case IDM_OPEN_NAME:   open_type = "OPEN=NAME"; break;
                            case IDM_OPEN_DETAIL: open_type = "OPEN=DETAIL"; break;
                        }
*/
                        
                        if (*(config.getBaseDir()))
                            sprintf(filename,"%s%s",config.getBaseDir(),ud->getName());
                        else
                            sprintf(filename,"%s",ud->getName());
                        
                        // is it fully qualified path ?
                        char *curr_dir = new char[CCHMAXPATH];
                        ULONG dir_size = CCHMAXPATH;
                        if (!strchr(filename,':'))
                        {
                            DosQueryCurrentDir(0,curr_dir,&dir_size);
                            
                            // does it only need the drive letter?
                            if (*filename == '\\')
                            {
                                curr_dir[2] = '\0';  // only keep "X:"
                                strcat(curr_dir,filename);
                            }
                            // or the whole thing
                            else
                            {
                                ULONG drive_num=0,drive_table=0;
                                DosQueryCurrentDisk(&drive_num,&drive_table);
                                char disk_letter[4] = "C:\\";
                                
                                *disk_letter = 'A' + drive_num -1;
                                
                                strcat(curr_dir,"\\");
                                strcat(curr_dir,filename);
                                strcpy(filename,disk_letter);
                                strcat(filename,curr_dir);
                                strcpy(curr_dir,filename);
                            }
                        }
                        else
                        {
                            strcpy(curr_dir,filename);
                        }
                        
//                        message("Open %s",curr_dir);
                        
                        HOBJECT hobj = (HOBJECT)WinQueryObject(curr_dir);
                        
                        if (hobj != NULLHANDLE)
                            WinSetObjectData(hobj,open_type);
                        else
                            message("Unable to open WPS folder");
                        
                        delete[] curr_dir;
                        delete[] filename;
                    }
                    
                    break;
                    
                case IDM_RESCAN:
                    
//                    message("Rescanning");
                    ud = tree.findSelectedRecord(NULL);
                    if (ud)
                    {
                        if (ud->isGroup())
                        {
                            
                            tree.deleteChildren(ud);
                            
                            char *dirdir = new char[CCHMAXPATH];
                            
                            if (*(config.getBaseDir()))
                                sprintf(dirdir,"%s%s\\*",config.getBaseDir(),ud->getName());
                            else
                                sprintf(dirdir,"%s\\*",ud->getName());                            
                            
			    // fixme to check leak
                            dir = new FileList(dirdir);
                            // insert into container
                            
#ifdef INSERT_EACH_FILE
                            for (int each=0; each < dir->getNumFiles(); each++)
                            {
                                //sprintf(buffer,"%s   (%1luk)",,dir->getFileSize(each)/1024);
                                tree.insert(dir->getFile(each),ud,8888L,8888L);
                            }
#else
                            tree.insertFileList(dir,ud);
#endif                            
                            
                            tree.arrange();
                            
                            delete[] dirdir;
                        }
                    }
                    break;                    
                    
                case IDM_UP:
                    ud = tree.findSelectedRecord(NULL);
//                    if (ud && !ud->isPlain())
                    if (ud)
                    {
                        tree.moveUp(ud);
                        tree.arrange();
                    }
                    break;
            
                case IDM_DOWN:
                    ud = tree.findSelectedRecord(NULL);
//                    if (ud && !ud->isPlain())
                    if (ud)
                    {
                        tree.moveDown(ud);
                        tree.arrange();
                    }
                    break;
                    
                case IDM_ADDSERVER:
                    {
                        // show the dialog to do the work
                        exportServer(NULL);
                        WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop
                                   hwndDlg,            // Owned by main dialog
                                   (PFNWP)HostDlgProc, // Addr. of procedure
                                   (HMODULE)0,         // Module handle                
                                   ID_HOST_DIALOG,     // Dialog identifier in resource
                                   NULL);              // Initialization data          
                        // did we get a new one Ok
                        char *serv = new char[4096];
                        importServer(serv);
                        if (*serv != '\0')
                            tree.insert(serv,config.getLineCull(),0);
                        delete[] serv;
                    }
                    break;

                case IDM_ADDGROUP:
                    // show the dialog to do the work 
                    exportGroupName(NULL);
                    exportGroupNumber(0);
                    ud = tree.findSelectedRecord(NULL);
                    if (ud != NULL && ud->getType() == TYPE_SERVER)
                    {
                        exportGroupCull(ud->getCull());
                        if (ud->getUsername() != '\0' &&
                            ud->getPassword() != '\0')
                        {
//                            message("Exportig username  [%s]",ud->getUsername());
//                            message("Exportig password  [%s]",ud->getPassword());
                            exportGroupUsername(ud->getUsername());
                            exportGroupPassword(ud->getPassword());
                        }
                        else
                        {
                            exportGroupUsername(NULL);
                            exportGroupPassword(NULL);
                        }
                    }

                    WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop
                               hwndDlg,            // Owned by main dialog
                               (PFNWP)GroupDlgProc,// Addr. of procedure
                               (HMODULE)0,         // Module handle                
                               NEW_GROUP_DIALOG,   // Dialog identifier in resource
                               ud->getName());     // Initialization data
                    
                    // did we get a new one Ok
                    char *group = new char[4096];
                    importGroupName(group);
                    if (*group != '\0')
                    {
                        if (strchr(group,' ') == NULL)
                        {
                            // single group
                            tree.insert(group,
                                        tree.findSelectedRecord(NULL),
                                        importGroupCull(),
                                        importGroupNumber());
                        }
                        else
                        {
                            // list of groups
                            char *cptr = strchr(group,' ');
                            char *group_list = group;
                            while (cptr)
                            {
                                *cptr = '\0';
                                cptr++;
                                
                                if (*group_list)
                                    tree.insert(group_list,tree.findSelectedRecord(NULL),importGroupCull(),importGroupNumber());
                                group_list = cptr;
                                cptr = strchr(group_list,' ');
                                
                            }
                        }
                    }
                    delete[] group;
                    break;
                    
            } // switch WM_COMMAND mp1

            break; // WM_COMMAND
        
        default:
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
    } // switch msg

    return (MRESULT)0;

} // MainDlgProc

void message(char *format, ...)
{
    char *buffer = new char[1024];
    va_list arg_ptr;
    
    memset(buffer,0,1024);
    va_start(arg_ptr,format);
    vsprintf(buffer,format,arg_ptr);
    va_end(arg_ptr);
    
    // no messages in auto mode
    if (!config.getUserAutoStart())                                                 // Version is a string
        WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,buffer,"News Harvest " VERSION,0,MB_OK|MB_ICONEXCLAMATION);
    
    if (outfile != NULL) {
        fputs(buffer,outfile);
        fputs("\n",outfile);
    }
    
    delete[] buffer;
    
}

void _Optlink splashThread(void *argpack)
{
    
    HAB hab;
    HMQ hmq;
    ThreadThings *flt = (ThreadThings*)argpack;
    
    hab = WinInitialize(0);
    if (hab) hmq = WinCreateMsgQueue(hab,0);
    
    char szPath[_MAX_PATH];
    FILE *rego = fopen(szLicensePath,"r");
    if (rego)
        fclose(rego);
    
    // show the dialog to do the work
    int success = WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop
               flt->hwnd,            // Owned by main dialog
               (PFNWP)SplashDlgProc, // Addr. of procedure
               (HMODULE)0,         // Module handle                
               (rego?SPLASH_DIALOG:SPLASH_DIALOG_UREG),     // Dialog identifier in resource
               (void *)flt);              // Initialization data
    
     if (success == DID_ERROR)
     {
         DosBeep(60,100);
         message("Welcome Wagon Derailed...");
         exit(2);
     }
    
    if (hmq) WinDestroyMsgQueue(hmq);
    if (hab) WinTerminate(hab);

    _endthread();

} // splashThread

void _Optlink loadThread(void *argpack)
{
    
    HAB hab;
    HMQ hmq;
    FileList *dir;
    static UserData *ud,*grp;
    ThreadThings *flt = (ThreadThings*)argpack;
    int i,j;
    
    hab = WinInitialize(0);
    if (hab) hmq = WinCreateMsgQueue(hab,0);    
    
    for (i=0; i<flt->config->getNumServers(); i++)
    {
        //log("Inserting server %s",flt->config->getServerName(i));
        
        // Insert server with a name & password
        if (flt->config->getServerUsername(i) != NULL && 
            flt->config->getServerPassword(i) != NULL)
        {
            flt->tree->insert(flt->config->getServerName(i),
                              flt->config->getServerUsername(i),
                              flt->config->getServerPassword(i),
                              flt->config->getLineCull(),0);
        }
        else
        {
            // insert server with NO user stuff
            flt->tree->insert(flt->config->getServerName(i),flt->config->getLineCull(),0);
        }

        ud = flt->tree->getLastInsert();
        
        //log("flt->config->getNumServerGroups(%d) == %d",i,flt->config->getNumServerGroups(i));

        for (j=0; j<flt->config->getNumServerGroups(i); j++)
        {
            //log("flt->config->getServerGroup(i==%d,j==%d)->getName() == %s",i,j,flt->config->getServerGroup(i,j)->getName());
            //log("flt->config->getServerName(i==%d) == %s",i,flt->config->getServerName(i));
            //log("flt->config->getServerGroup(i==%d,j==%d)->getLineCull() ==%d",i,j,flt->config->getServerGroup(i,j)->getLineCull());
            flt->tree->insert(flt->config->getServerGroup(i,j)->getName(),
                        //flt->config->getServerName(i),
                        ud,  // servers can have same name
                        flt->config->getServerGroup(i,j)->getLineCull(),
                        flt->config->getServerGroup(i,j)->getLastRecord());
            
            grp = flt->tree->getLastInsert();
            if (flt->config->getServerGroup(i,j)->getSkipped())
            {
                grp->setSkipped(1);
                flt->tree->setIcon(grp,ICON_GROUP_C);
            }
            if (flt->config->getServerGroup(i,j)->getCollapsed())
            {
                grp->setCollapsed(1);
                flt->tree->collapse(grp);
            }
            
            if (flt->config->getUserShowAllFiles())
            {
                // add dir content to container
                char *dirdir = new char[4096];
                
                if (*(flt->config->getBaseDir()))
                    sprintf(dirdir,"%s%s\\*",flt->config->getBaseDir(),flt->config->getServerGroup(i,j)->getName());
                else
                    sprintf(dirdir,"%s\\*",flt->config->getServerGroup(i,j)->getName());
                
                dir = new FileList(dirdir);
                // message("Read %d files from %s",dir->getNumFiles(),dirdir);
                // insert into container
                
#ifdef INSERT_EACH_FILE
                for (int each=0; each < dir->getNumFiles(); each++)
                {
                    //sprintf(buffer,"%s   (%1luk)",,dir->getFileSize(each)/1024);
                    flt->tree->insert(dir->getFile(each),grp,8888L,8888L);
                }
#else
                flt->tree->insertFileList(dir,grp);
#endif
                delete dir;
                delete[] dirdir;
            }
        }
    }
    
    flt->tree->arrange();
    
    still_loading = 0;
    WinEnableControl(flt->hwnd,ID_B_START,TRUE);
    
    // start up automatically
    if (config.getUserAutoStart())
        WinPostMsg(flt->hwnd,WM_COMMAND,MPFROM2SHORT(ID_B_START,0),0);
    
    if (hmq) WinDestroyMsgQueue(hmq);
    if (hab) WinTerminate(hab);

    _endthread();

} // loadThread

#define MAX_WAIT 20    //seconds

void _Optlink timeoutThread(void *arg)
{
    
    HAB hab;
    HMQ hmq;
    
    hab = WinInitialize(0);
    if (hab) hmq = WinCreateMsgQueue(hab,0);
    
    for (int i=0; i<MAX_WAIT*2; i++)  // wait for MAX_WAIT * 2 * 1/2 second
    {
        DosSleep(500);
        if (running != 0)
            break;
        if (thread_id != 0)
            break;
        if (old_thread_id == 0)
            break;
        if (stopping == 0)
            break;
    }
    if (i==60 && old_thread_id != 0)
    {
        // 30 seconds
        if (0 == DosKillThread(old_thread_id))
        {
            old_thread_id = 0;
            stopping = 0;
//            message("DEBUG: timeoutThread killed DL thread");
            WinPostMsg(window, DL_TERM, NULL, NULL );
        }
        else message("Error: Failed to terminate download thread");
    }
    
    if (hmq) WinDestroyMsgQueue(hmq);
    if (hab) WinTerminate(hab);

    _endthread();

} // timeoutThread

void logOptions(Configuration &config)
{
     log("Options:");
     log("    OutputDir=%s",config.getBaseDir());
     log("    PartsDir=%s",config.getPartsDir());
     log("    MaxNumParts=%d",config.getMultipartMaxParts());
     log("    MaxPartLines=%d",config.getMultipartMaxLines());
     log("    MultipartEnabled=%d",config.getMultipartUse());
//                    log("    NetscapeDir=%s",config.getNetscapeDir());
     log("    FilterFile=%s",config.getFilter());
     log("    FilterEnabled=%d",config.getFilterUse());
     log("    FastNameFilterEnabled=%d",config.getFastNameFilterUse());
     log("    LogFile=%s",config.getLog());
     log("    LoggingEnabled=%d",config.getLoggingUse());
     log("    LogSpawn=%d",config.getLogSpawn());
     log("    PostProcessCommand=%s",config.getProcessCommand());
     log("    PostProcessingEnabled=%d",config.getProcessUse());
     log("    MaxDownloadSize=%ld",config.getMaxDownload());
     log("    MaxNumArticles=%ld",config.getMaxArticles());
     log("    MaxPartAge=%ld",config.getMaxPartAge());
     log("    LineCullAmount=%ld",config.getLineCull());
//                    log("    SmallIcons=%d",config.getSmallIcons());
     log("    HeaderCacheSize=%d",config.getHeaderCacheSize());
     log("    TCPSettings=%s",config.getRegoTimeout());
     log("    PriorityClass=%d",config.getPriorityClass());
     log("    PriorityValue=%d",config.getPriorityValue());
     log("    BandwidthLimit=%-6.0lf",config.getBandwidthLimit());
     
}
