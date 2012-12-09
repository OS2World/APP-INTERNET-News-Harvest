/* help.h,v 1.2 2001/06/20 00:42:41 root Exp */

/*
 * Module for providing the online-help
 *
 */

#ifndef __HELP_H__
#define __HELP_H__

#define INCL_WIN
#include <os2.h>

// Start up the help
int helpInit(HAB *hab,HWND *hwnd);

// Shut down the help
void killHelp(void);

// bring up the help window at the desired ID position
void showHelpWindow(LONG topic_id);

#define HELP_INTRO     1500
#define HELP_MAINPANEL 2000
#define HELP_ADDHOST   3000

#define HELP_ADDGROUP  4000
#define HELP_ADDGROUPNAME  4010
#define HELP_ADDGROUPCULL  4020
#define HELP_ADDGROUPNEXT  4030

#define HELP_SELGROUP        4500
#define HELP_SELGROUPSEL     4510
#define HELP_SELGROUPFILTER  4520
#define HELP_SELGROUPREFRESH 4530

#define HELP_CONFIG    5000
#define HELP_MIMES     6000

#define HELP_FILE  "nh.hlp"
#define HELP_TITLE "News Harvest Help"
#define HELP_TABLE 1000

#endif
