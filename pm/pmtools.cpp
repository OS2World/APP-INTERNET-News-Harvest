
/***********************************************************************

pmtools.cpp,v 1.3 2002/07/19 23:48:14 root Exp

IO and PM support functions

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2002 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	04 Aug 00 SHL - Import
		19 Jul 02 SHL - Support CSet/2 build

***********************************************************************/



#define INCL_DOS
#define INCL_DOSERRORS /* how come this isn'd included by INCL_DOS ?! */
#define INCL_DOSMISC
#define INCL_DOSFILEMGR
#define INCL_PM

#include <os2.h>

#include <stdlib.h>
#include <stdio.h>
#include <new.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>

#include "pmtools.h"
#include "config.h"

extern Configuration *p_config;

/*
 * Returns the letter of the current disk
 *
 */
char getCurrentDisk(void)
{
    
    // DOES NOT WORK!
    // -- should now
    // fixme
    
    char *pszDirectory = new char[CCHMAXPATH];
    _getcwd(pszDirectory,CCHMAXPATH);
    
    
    char disk;
    disk = toupper(pszDirectory[0]);
    
    delete[] pszDirectory;
    
    return disk;
}


/*
 * Given a disk letter, return the amount of free space in Mbytes
 *
 */
unsigned long getFreeSpace(char disk)
{
    // use drive number 0 for current drive
    ULONG drive_number = (ULONG)(1+(toupper(disk)-'A'));  // drive A: is 1
    if (disk == '0') drive_number = 0;  // default disk
    FSALLOCATE fs = {0};
    APIRET rc = 0;
    ULONG total = 0;
    
    rc = DosQueryFSInfo(drive_number,FSIL_ALLOC,&fs,sizeof(FSALLOCATE));
    
    if (rc == NO_ERROR)
    {
        double bytes_per_sector = (double)fs.cbSector;
        double sectors_per_unit = (double)fs.cSectorUnit;
        double free_units       = (double)fs.cUnitAvail;
        double Mb_total;
        
        // Keep total at manageable level to ensure precision
        
        Mb_total = free_units / 1024.0;    // convert to Mb -> first divide
        
        Mb_total *= bytes_per_sector;
        
        Mb_total /= 1024.0;    // second divide
        
        Mb_total *= sectors_per_unit;
        
        total = (ULONG)Mb_total;
        
        
        // Byte total  - too big, wraps on sizes > 4.2Gb
        /*
        total = ULONG((ULONG)fs.cSectorUnit * 
                      (ULONG)fs.cbSector * 
                      (ULONG)fs.cUnitAvail);
        */
        
    }
    
    return total;

}





/*
 * Change a bitmaped-button's bitmap
 *
 * hwnd - dialog handle
 * ulId - Id of the button control
 * hbmp - handle of the new bitmap
 *
 */
void setButtonBmp(HWND hwnd, ULONG ulId, HBITMAP hbmp)
{
    WNDPARAMS wp;
    BTNCDATA  bcd;
    HBITMAP   hbmpold;
    
    if (hbmp != NULL)
    {
        wp.fsStatus  = WPM_CTLDATA;
        wp.cbCtlData = sizeof(BTNCDATA);
        wp.pCtlData  = &bcd;
        
        WinSendDlgItemMsg(hwnd,ulId,WM_QUERYWINDOWPARAMS,MPFROMP((PVOID)&wp),MPVOID);
        hbmpold = bcd.hImage;
        bcd.hImage = hbmp;
        WinSendDlgItemMsg(hwnd,ulId,WM_SETWINDOWPARAMS,MPFROMP((PVOID)&wp),MPVOID);
        GpiDeleteBitmap(hbmpold);
    }
}


/*
 * Given a filename & a hwnd, Load & create a Bitmap
 *
 */

extern char programDir[];

HBITMAP loadBitmap(HWND hwnd,char *filename)
{
    LONG     fSize;
    HBITMAP  hbm = NULL;
    void    *bPtr;
    FILE    *bmpFile;
    
    PBITMAPFILEHEADER2 pbfh2;

    char szPDrv[_MAX_PATH];
    char szPDir[_MAX_PATH];
    char szBDrv[_MAX_PATH];
    char szBDir[_MAX_PATH];
    char szBFName[_MAX_PATH];
    char szBExt[_MAX_PATH];
    char szBPath[_MAX_PATH];

#if __IBMCPP__ < 300
    _splitpath(programDir, szPDrv, szPDir, szBFName, szBExt);
#else
    _splitpath(programDir, szPDrv, szPDir, NULL, NULL);
#endif
    _splitpath(filename, szBDrv, szBDir, szBFName, szBExt);

    if (!*szBDrv)
        strcpy(szBDrv, szPDrv);
    if (!*szBDir)
        strcpy(szBDir, szPDir);

    _makepath(szBPath, szBDrv, szBDir, szBFName, szBExt);
    
    bmpFile = fopen (szBPath, "rb");

    if (bmpFile != NULL)
    {
        /* Check to see if it has the right format */
        CHAR ut[2];
        fread (ut, sizeof (CHAR), 2, bmpFile);
        if (ut[0] == 'B' && ut[1] == 'M')
        {
            /* Determine the size of the file */
            fseek (bmpFile, 0L, SEEK_END);
            fSize = ftell (bmpFile);
            
            /* allocate block for read */
            bPtr = new char[fSize];
            if (bPtr != NULL)
            {
                /* Move back to the start of the file ...*/
                fseek (bmpFile, 0L, SEEK_SET);
                /* ... and load the whole thing in one gulp! */
                fread (bPtr, sizeof (BYTE), fSize, bmpFile);
                
                fclose(bmpFile);
                
                /* point to the info header */
                pbfh2 = (PBITMAPFILEHEADER2)bPtr;
                
                /* create the bitmap */
                hbm = GpiCreateBitmap (WinGetPS(hwnd),&(pbfh2->bmp2),CBM_INIT,((BYTE*)bPtr+pbfh2->offBits),(PBITMAPINFO2)&(pbfh2->bmp2));
                
                /* free the block */
                delete[] bPtr;
            }
        }
    }
    
    return hbm;
}


/**
 * Center a window within its parent window.
 *
 * Entry:
 *    
 *    The handle of the window to center.
 *
 * Exit:
 *
 *    Nothing.
 */

void
pmtoolsCenterWindow( HWND hwnd )
/***********************/
{
   RECTL rclParent;
   RECTL rclWindow;

   WinQueryWindowRect( hwnd, &rclWindow );
   WinQueryWindowRect( WinQueryWindow( hwnd, QW_PARENT ), &rclParent );

   rclWindow.xLeft   = ( rclParent.xRight - rclWindow.xRight ) / 2;
   rclWindow.yBottom = ( rclParent.yTop   - rclWindow.yTop   ) / 2;

   WinSetWindowPos( hwnd, NULLHANDLE, rclWindow.xLeft, rclWindow.yBottom,
                    0, 0, SWP_MOVE );
}

/**
 * Create a help instance and associate it with a window.
 *
 * Entry:
 *
 *    hwndFrame      - The window handle.
 *    pszFileName    - The pathname of the help file.
 *                     If NULL or empty, will build a helpfile name
 *                     using the path and root file name of the executable.
 *    hmod           - The handle of the module containing the help table.
 *                     NULLHANDLE == current.
 *    usTableID      - The resource ID of the help table.
 *    pszWindowTitle - The tile for the help window (may be NULL of empty ).
 *
 * Exit:
 *
 *    The help window's handle on success.
 */
HWND CreateHelp( HWND    hwndFrame,
            PSZ     pszFileName,
            HMODULE hmod,
            USHORT  usTableID,
            PSZ     pszWindowTitle )
/**********************************/
{
   PPIB     ppib;
   PTIB     ptib;
   HELPINIT hi;
   PSZ      pszExt;
   CHAR     szName[CCHMAXPATH];
   HWND     hwndHelp;

   if (( pszFileName == NULL ) || ( *pszFileName ))
   {
      DosGetInfoBlocks( &ptib, &ppib );

      DosQueryModuleName( ppib->pib_hmte, sizeof( szName ), szName );

      pszExt = strrchr( szName, '.' );
      if ( pszExt )
      {
         strcpy( pszExt, ".hlp" );
      }
      else
      {
         strcat( szName, ".hlp" );
      }

      pszFileName = szName;
   }

   hi.cb                       = sizeof( HELPINIT );
   hi.ulReturnCode             = 0;
   hi.pszTutorialName          = NULL;
   hi.phtHelpTable             = ( PHELPTABLE ) MAKELONG( usTableID, 0xffff );
   hi.hmodHelpTableModule      = hmod;
   hi.hmodAccelActionBarModule = NULLHANDLE;
   hi.idAccelTable             = 0;
   hi.idActionBar              = 0;
   hi.pszHelpWindowTitle       = pszWindowTitle;
   hi.fShowPanelId             = CMIC_HIDE_PANEL_ID;
   hi.pszHelpLibraryName       = pszFileName;

   hwndHelp = WinCreateHelpInstance( WinQueryAnchorBlock( hwndFrame ), &hi );
   if ( hwndHelp != NULLHANDLE )
   {
      WinAssociateHelpInstance( hwndHelp, hwndFrame );
      return ( hwndHelp );
   }
   else
   {
      return ( NULLHANDLE );
   }
}

/**
 * Dispose of a help table instance.
 *
 * Entry:
 *
 *    hwndHelp - A help table handle from "CreateHelp()."
 *
 * Exit:
 *
 *    Nothing.
 */
void
DestroyHelp( HWND hwndHelp )
/**************************/
{
   if ( hwndHelp != NULLHANDLE )
   {
      WinAssociateHelpInstance( NULLHANDLE, hwndHelp );
      WinDestroyHelpInstance( hwndHelp );
   }
}

/**
 * Change the icon used on an icon-button control.
 *
 * Entry:
 *
 *    hwndButton - The button handle.
 *    hicon      - The icons handle.
 *
 * Exit:
 *
 *    Nothing.
 */
void SetButtonIcon(HWND hwndButton,HPOINTER hicon)
/**********************************/
{
   WNDPARAMS wp;
   BTNCDATA  bd;

   wp.fsStatus  = WPM_CTLDATA;
   wp.cbCtlData = sizeof( BTNCDATA );
   wp.pCtlData  = &bd;

   WinSendMsg( hwndButton, WM_QUERYWINDOWPARAMS, MPFROMP( &wp ), MPVOID );
   bd.hImage = hicon;
   WinSendMsg( hwndButton, WM_SETWINDOWPARAMS, MPFROMP( &wp ), MPVOID );
}

/**
 * Launch ( spawn ) another application.
 *
 * Entry:
 *
 *    hwnd           - The current window's handle.
 *    pszProgram     - The pathname of the program to launch.
 *    pszOptions     - Command-line options (if any) to pass to the spawned app.
 *    pszDirectory   - The path to launch the app from; i.e., this will be
 *                     the current directory at the time of launch.
 *
 * Exit:
 * 
 *    A handle to the launched app if successful.
 *
 *    NOTE:
 *
 *       The launching window ( hwnd ) will receive a WM_APPTERMINATENOTIFY
 *       message if still running when the spawned app terminates.
 *
 */
HWND
pmtoolsLaunchApplication( HWND    hwnd,
                   PSZ     pszProgram,
                   PSZ     pszOptions,
                   PSZ     pszDirectory )
/***************************************/
{
   PROGDETAILS pd = { 0 };

   pd.Length                        = sizeof( pd );
   pd.progt.progc                   = PROG_DEFAULT;
   pd.pszExecutable                 = pszProgram;
   pd.pszStartupDir                 = pszDirectory;
   pd.pszParameters                 = pszOptions;
   pd.swpInitial.hwndInsertBehind   = HWND_TOP;
   pd.swpInitial.fl                 = SWP_ACTIVATE | SWP_SHOW;
   pd.swpInitial.hwnd               = hwnd;

   return ( WinStartApp( hwnd, &pd, NULL, NULL, SAF_INSTALLEDCMDLINE ));
}

/* as above, but...               */
/* start in the current directory */
HWND pmtoolsLaunchApplication( HWND    hwnd,
                   PSZ     pszProgram,
                   PSZ     pszOptions)
/***************************************/
{
    PROGDETAILS pd = { 0 };
    UCHAR pszDirectory[1024];
    UCHAR pszDirectory2[1024];
    ULONG buf_size = sizeof(pszDirectory);
    
    *pszDirectory = '\0';
    ULONG rc = DosQueryCurrentDir(0,(PBYTE)pszDirectory,&buf_size);
    if (rc != 0)
    {
/*        switch(rc)
        {
            case ERROR_INVALID_DRIVE:
                log("DosQueryCurrentDir() - Invalid Drive");
                break;
            case ERROR_NOT_DOS_DISK:
                log("DosQueryCurrentDir() - Not a DOS Disk");
                break;
            case ERROR_DRIVE_LOCKED:
                log("DosQueryCurrentDir() - Drive Locked");
                break;
            case ERROR_BUFFER_OVERFLOW:
                log("DosQueryCurrentDir() - Buffer Overflow");
                break;
            default:   */
                log("Unknown error in DosQueryCurrentDir()");
/*        } */
    }
    
    strcpy((char*)pszDirectory2,"\\");
    strcat((char*)pszDirectory2,(const char *)pszDirectory);
    
   pd.Length                        = sizeof( pd );
   pd.progt.progc                   = PROG_DEFAULT;
   pd.pszExecutable                 = pszProgram;
   pd.pszStartupDir                 = (PSZ)pszDirectory2;
   pd.pszParameters                 = pszOptions;
   pd.swpInitial.hwndInsertBehind   = HWND_TOP;
   pd.swpInitial.fl                 = SWP_ACTIVATE | SWP_SHOW;
   pd.swpInitial.hwnd               = hwnd;
   
   
   if (p_config && p_config->getLogSpawn())
   {
       log("Launching program \"%s\"",pszProgram);
       log("options are \"%s\"",pszOptions);
       log("dir is      \"%s\"",pszDirectory2);
   }
   
   return ( WinStartApp( hwnd, &pd, NULL, NULL, SAF_INSTALLEDCMDLINE ));
}

/**
 * Prevent more than one instance of an application from running.
 *
 * Entry:
 *
 *    pszSemName  - A unique semaphore name to identify this application.
 *                  This is a shared semaphore, so the name MAY NOT be
 *                  NULL or empty. As with all OS/2 semaphores, the
 *                  name must begin with "\SEM32\."
 *
 *                  E.g.: "\\SEM32\\MYAPP"
 *
 *    fSwitch     - If TRUE, and another instance of the app is found
 *                  to be running, we'll use the task manager to
 *                  switch to the running version. Otherwise just
 *                  return.
 *
 * Exit:
 *
 *    A mutex semaphore handle if this is the only instance of
 *    the app running, otherwise, NULLHANDLE, indicating that
 *    another copy exists.
 */
HMTX
OneInstance( PSZ  pszSemName,
             BOOL fSwitch    )
/****************************/
{
   HMTX hmtx;

   if ( DosCreateMutexSem( pszSemName, &hmtx, DC_SEM_SHARED, TRUE ) == NO_ERROR )
   {
      return ( hmtx );
   }

   hmtx = NULLHANDLE;

   if ( DosOpenMutexSem( pszSemName, &hmtx ) == NO_ERROR )
   {
      if ( fSwitch )
      {
         PID     pid = 0;
         TID     tid = 0;
         ULONG   ulJunk;

         if ( DosQueryMutexSem( hmtx, &pid, &tid, &ulJunk ) == NO_ERROR )
         {
            HSWITCH hswitch;

            hswitch = WinQuerySwitchHandle( NULLHANDLE, pid );
            if ( hswitch != NULLHANDLE )
            {
               WinSwitchToProgram( hswitch );
            }
         }
      }

      DosCloseMutexSem( hmtx );
   }

   return ( NULLHANDLE );
}

/**
 * Load a menu.
 *
 * Entry:
 *
 *    hwnd     - The parent/owner of the menu. May be HWND_OBJECT.
 *    hmod     - The module containing the menu resource. NULLHANDLE == current.
 *    usResID  - The resource I.D.
 *
 * Exit:
 *
 *    A handle to the menu. If the parent/owner was NOT HWND_OBJECT,
 *    this routine will attempt to associate the loaded menu with
 *    the current window.
 */


HWND
LoadMenu( HWND    hwnd,
          HMODULE hmod,
          USHORT  usResID )
/*************************/
{
   HWND hmenu;

   hmenu = WinLoadMenu( hwnd, hmod, usResID );
   if (( hmenu != NULLHANDLE ) /* && ( hwnd != HWND_OBJ ) **KT */)
   {
      WinSendMsg( hwnd, WM_UPDATEFRAME, MPFROMLONG( FID_MENU ), MPVOID );
   }

   return ( hmenu );
}

/**
 * Load an icon and make it the current window's.
 *
 * Entry:
 *
 *    hwnd     - The application's frame window handle.
 *    hmod     - The module containing the resource - NULLHANDLE == current.
 *    usResID  - The resource I.D.
 *
 * Exit:
 *
 *    A handle to the icon. The routine will "attach" the icon to the window
 *    if it can.
 */
HPOINTER
LoadIcon( HWND    hwnd,
          HMODULE hmod,
          USHORT  usResID )
/*************************/
{
   HPOINTER hicon;

   hicon = WinLoadPointer( HWND_DESKTOP, hmod, usResID );
   if ( hicon != NULLHANDLE )
   {
      WinSendMsg( hwnd, WM_SETICON, MPFROMLONG( hicon ), MPVOID );
   }

   return ( hicon );
}

/**
 * Load an accelerator table and connect it to the window.
 *
 * Entry:
 *
 *    hwnd     - The application's frame window handle.
 *    hmod     - The module containing the resource - NULLHANDLE == current.
 *    usResID  - The resource I.D.
 *
 * Exit:
 *
 *    A handle to the accelerator table. The routine will "attach" the 
 *    table to the window if it can.
 */
HACCEL
LoadAccelerator( HWND    hwnd,
                 HMODULE hmod,
                 USHORT  usResID )
/********************************/
{
   HACCEL haccel;
   HAB    hab;

   hab    = WinQueryAnchorBlock( hwnd );
   haccel = WinLoadAccelTable( hab, hmod, usResID );
   if ( haccel != NULLHANDLE )
   {
      WinSetAccelTable( hab, haccel, hwnd );
   }

   return ( haccel );
}




char *pmtoolsFindExecutable(char *filename)
{
    char *rc;
    char *path = "PATH";
    
    if (*filename == '\0')
        return NULL;
    
    rc = pmtoolsFindFile(filename,path);
    if (rc == NULL &&
        !strstr(filename,".EXE") &&
        !strstr(filename,".exe") &&
        !strstr(filename,".CMD") &&
        !strstr(filename,".cmd") )
    {
        char *buffer = new char[5+strlen(filename)];
        char *eoln = buffer+strlen(filename)-1;
        strcpy(buffer,filename);
        
        // add ".exe" and find that
        strcat(buffer,".exe");
        rc = pmtoolsFindFile(buffer,path);
        if (rc == NULL)
        {
            strcat(eoln,".cmd");
            rc = pmtoolsFindFile(buffer,path);
        }
        
        delete[] buffer;
    }
    
    return rc;
}

char *pmtoolsFindFile(char *filename,char *pathname)
{
    char *rc = NULL;
    ULONG flags = SEARCH_IGNORENETERRS|SEARCH_ENVIRONMENT|SEARCH_CUR_DIRECTORY;
    static char full_path[1024];
    int  fp_len = sizeof(full_path);
    
    char *ptr = strchr(filename,'\\');
    if (ptr == NULL)
    {
        int error = DosSearchPath(flags,(PSZ)pathname,(PSZ)filename,(PBYTE)full_path,fp_len);

        if (error == 0)
        {
            // we found it 
//            log("Found %s as %s",filename,full_path);
            return full_path;
        }
        else
        {
            log("DosSearchPath() returned %d looking for %s on %s",error,filename,pathname);
        }
    }
    return NULL;
}

