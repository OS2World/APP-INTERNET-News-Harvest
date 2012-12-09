#ifndef __PMTools_H
#define __PMTools_H

#include <os2def.h>
#include "strutils.h"  // for log


   extern char     getCurrentDisk(void);
   extern unsigned long getFreeSpace(char disk);
   extern void     setButtonBmp(HWND hwnd, ULONG ulId, HBITMAP hbmp);
   extern HBITMAP  loadBitmap(HWND hwnd,char *bmpFile);
   extern void     pmtoolsCenterWindow( HWND hwnd );
   extern HWND     CreateHelp( HWND hwndFrame, PSZ pszFileName, HMODULE hmod, USHORT usTableID, PSZ pszWindowTitle );
   extern void     DestroyHelp( HWND hwndHelp );
   extern void     SetButtonIcon( HWND hwndButton, HPOINTER hicon );
   extern HWND     pmtoolsLaunchApplication( HWND hwnd, PSZ pszProgram, PSZ pszOptions, PSZ pszDirectory );
   extern HWND     pmtoolsLaunchApplication( HWND hwnd, PSZ pszProgram, PSZ pszOptions);
   extern HMTX     OneInstance( PSZ pszSemName, BOOL fSwitch );
   extern HWND     LoadMenu( HWND hwnd,HMODULE hmod, USHORT usResID );
   extern HPOINTER LoadIcon( HWND hwnd, HMODULE hmod, USHORT usResID );
   extern HACCEL   LoadAccelerator( HWND hwnd, HMODULE hmod, USHORT usResID );
   extern char    *pmtoolsFindExecutable(char *filename);
   extern char    *pmtoolsFindFile(char *filename,char *path);



#endif /* __PMTools_H */

