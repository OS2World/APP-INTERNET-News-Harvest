
/***********************************************************************

splash.cpp,v 1.3 2002/01/05 19:28:19 root Exp

Startup splash screen

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2001 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	21 Jun 01 SHL - Drop excess new/delete

***********************************************************************/

#include "splashdlg.h"

#define RANDOM(x) (rand()%(x))
#define RDIGIT()  (rand()%(10)+48)

// this string will be set to just "groups" if we're registered and "Groups" if not
extern char *registered;

static char rego_name[257] = {"(not registered) $jk435hjk34hjk54nlk6n549879832714kjhisdhufe78sry34indu8392dn3478d3478d2i3hs2i3hs732dhwqdnjn59382jshd78wey8232j\0" };
static char rego_key[257]  = {"(no key) $;34LK5J239WJEFLETJK54092LKJWEFMNDJRDIOPSDJRE IO2PJ RDXOWIEJIOJR 4I3OJR34OPJWKEJR LKEJRPQJW389234KLJ3H249823HSx\0" };

long getCheckSum (char *buf)
{
    long ret = 0L;
    int i;

    for (i=0; i<strlen(buf); i++)
    {
        ret += i * buf[i];
    }
    return (ret);
}

void _Optlink RegoThread(void *arg)
{
    HAB hab;
    HMQ hmq;
    ThreadThings *flt = (ThreadThings*)arg;
    
    hab = WinInitialize(0);
    if (hab) hmq = WinCreateMsgQueue(hab,0);
    
    int x = RANDOM(999);
    char buffer[256];
    char *ptr;
    long crc = getCheckSum(rego_name);
    done_splash = DONE_SPLASH;
    long y = (long)&buffer;
    double divider;
    
    
    sprintf(buffer,"%03d-%05lu-%05lu-au",x,crc,y);
    
    if (strncmp(buffer+3,rego_key+3,7) != 0)
    {
        strcpy(buffer,flt->config->getRegoTimeout());
        ptr = strchr(buffer,','); if (ptr) *ptr = '.';
        divider = atof(buffer);
        divider /= 0.69;
        crc = (long)divider;
        divider *= 0.69;
        
        // message("Has been run %d (%d) times",crc-16,crc);
        
        if (crc > 32)
	{
            for (x=0; x<(crc-16); x++)
                DosSleep(100);
	}
        
#	ifndef SLAINC // SHL 12 Jul 00
        strcpy(registered,"Groups"); // unregistered
#	else
        registered = "Groups"; // unregistered
#	endif
        
        if (crc < 16)
        {
            sprintf(buffer,"%d.%d",11,04);
        }
        else
        {
            sprintf(buffer,"%.2f",divider+0.69);
        }
        ptr = strchr(buffer,'.'); if (ptr) *ptr = ',';
        flt->config->setRegoTimeout(buffer);
        
        // stop after extracting ~200 binaries, or ~5 MB
        flt->config->setMaxArticles(100+RANDOM(200));
        flt->config->setMaxDownload(5+RANDOM(10));

        DosSleep(3000);
        DosBeep(100,200);
        WinSendMsg((HWND)flt->dialog, WM_USER, NULL, NULL );
    }
    else
    {
        DosSleep(1500);
        WinSendMsg((HWND)flt->dialog, WM_USER, NULL, NULL );
        WinPostMsg((HWND)flt->dialog, WM_CLOSE, NULL, NULL );
#	ifndef SLAINC // SHL 12 Jul 00
        strcpy(registered,"groups");  // registered
#	else
        registered = "groups";  // registered
#	endif
    }
        
    if (hmq) WinDestroyMsgQueue(hmq);
    if (hab) WinTerminate(hab);
    _endthread();
}


extern char szLicensePath[];


MRESULT EXPENTRY SplashDlgProc(HWND hwndDlg,ULONG msg,MPARAM mp1,MPARAM mp2)
{
    static int can_close = 81233;
    static long crc;
    time_t the_time;
    FILE *in;
    char *bitmap_filename;
    int   horiz_size;
    ThreadThings *flt;
    
    switch(msg)
    {
        
        case WM_INITDLG:
            
            flt = (ThreadThings*)mp2;
            flt->dialog = hwndDlg;
            
            // Set a nice bitmap on the title
            horiz_size = WinQuerySysValue(HWND_DESKTOP,SV_CXFULLSCREEN);
            bitmap_filename = "defaults.bmp";
            if (horiz_size < 800)
                bitmap_filename = "640s.bmp";
            else if (horiz_size < 1024)
                bitmap_filename = "800s.bmp";
            else if (horiz_size < 1280)
                bitmap_filename = "1024s.bmp";
            else if (horiz_size <= 1600)
                bitmap_filename = "1280s.bmp";
            setButtonBmp(hwndDlg,SPLASH_BUTTON,loadBitmap(hwndDlg,bitmap_filename));
            
            WinEnableControl(hwndDlg,SPLASH_DISMISS,FALSE);
            in = fopen(szLicensePath,"r");
            if (in)
            {
                char buffer[1024];
                while (!feof(in))
                {
                    fgets(buffer,sizeof(buffer),in);
                    char *ptr = strchr(buffer,'\r');
                    if (!ptr)
		        ptr = strchr(buffer,'\n');
                    if (ptr)
		        *ptr = '\0';
                    if (!feof(in))
                    {
                        if (*buffer == '\r' || *buffer == '\n' || *buffer == '#')
                        {
                            ;	// shit data
                        }
                        else if (strncmp(buffer,"RegoName",8) == 0)
                        {
                            strcpy(rego_name,buffer+10);
                        }
                        else if (strncmp(buffer,"RegoKey",7) == 0)
                        {
                            strcpy(rego_key,buffer+9);
                        }
                    }
                }
                fclose(in);
            }
            else
            {
                WinSetWindowText(WinWindowFromID(hwndDlg,SPLASH_MLE),
                                 "Thanks for evaluating News Harvest!\n\n"
                                 "News Harvest lets you extract binaries from all your "
                                 "favourite news groups while you work, while you sleep, "
                                 "or while you watch.\n\n"
                                 "If you like this product, please take the time to register it "
                                 "and support independant software development.  Note that "
                                 "non-registered versions display this dialog and incur a "
                                 "random (but large) MB and file download limit each session.\n\n"
                                 "Enjoy!");
            }
            
            if (strstr(rego_name,"(not ") == rego_name)
            {
                char *ptr;
                ptr = strchr(rego_name,'$');
                if (ptr) *ptr = '\0';
                ptr = strchr(rego_key,'$');
                if (ptr) *ptr = '\0';
            }
            
            WinSetWindowText(WinWindowFromID(hwndDlg,SPLASH_OWNER),rego_name);
            WinSetWindowText(WinWindowFromID(hwndDlg,SPLASH_KEY),rego_key);
            
            time(&the_time);
            srand(the_time);
            
            // be sure the "Dismiss" button is active
            WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwndDlg,SPLASH_DISMISS),0);
            
            _beginthread(RegoThread,NULL,78098,(void*)flt);
            
            break;
            
        case WM_USER:
            can_close = 78098;
            WinEnableControl(hwndDlg,SPLASH_DISMISS,TRUE);
            break;
            
        case WM_CLOSE:
            if (can_close == 78098)
                return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
            else
                break;

        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1 ) )
            {
                case SPLASH_DISMISS:
                    WinPostMsg(hwndDlg,WM_CLOSE,0,0);
                    break;
                    
                case SPLASH_BUTTON:
                    system("detach register.exe");
                    break;
                    
            }
            break;
        
        default:
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
    }
    return (MRESULT) TRUE;
}

// The end
