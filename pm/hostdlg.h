#ifndef __HOSTDLG_H__
#define __HOSTDLG_H__

#include "newhost.h"

#define MAX_SERVER_NAME_LEN 256

extern MRESULT EXPENTRY HostDlgProc(HWND,ULONG,MPARAM,MPARAM);
extern void message(char *format, ...);
extern void exportServer(char *s);
extern void importServer(char *s);
extern void exportUser(char *s);
extern void importUser(char *s);
extern void exportPass(char *s);
extern void importPass(char *s);


#endif  // #ifndef __HOSTDLG_H__