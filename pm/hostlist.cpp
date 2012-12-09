/* hostlist.cpp,v 1.2 2001/06/20 00:42:42 root Exp */

#include "hostlistdlg.h"

#ifndef SLAINC // SHL 12 Jul 00
extern int  _Optlink _beginthread( void ( * _Optlink __thread )( void * ), void *, unsigned, void * );
extern void _Optlink _endthread( void );
#endif // SHL


int so_far;
TID list_thread_id;

void _Optlink ListThread(void *arg)
{
    HAB hab;
    HMQ hmq;
    
    hab = WinInitialize(0);
    if (hab) hmq = WinCreateMsgQueue(hab,0);
    
    ListThreadArgs *lta = (ListThreadArgs*)arg;
    HWND dlg = lta->dialog;
    
/*
    char buffer[1024];
    int  i,j;
    FILE *in;
    
    j  = lta->num_items;
    in = lta->in;
    for (i=0; i<j; i++)
    {
        fgets(buffer,1024,in);
        trimCR(buffer);
        if(feof(in)) 
            break;
        lta->list->insert(buffer,NULL,7777L,7777L);
        if (lta->stopping)
            break;
    }
*/
    lta->list->insertFile(lta->in,lta->num_items,&so_far,&(lta->stopping));
    
    fclose(lta->in);
    
    if (!lta->stopping);
        WinPostMsg(dlg, WM_USER, NULL, NULL );
        
    if (hmq) WinDestroyMsgQueue(hmq);
    if (hab) WinTerminate(hab);
    list_thread_id = 0;
    _endthread();
}


//                                                                         
//                                                                         
//                                                                           
//                                                                           
//                                                                           
MRESULT EXPENTRY HostListDlgProc(HWND hwndDlg,ULONG msg,MPARAM mp1,MPARAM mp2)
{
    static array<char *> *server;
    HPOINTER  hptrOrig, hptrWait;
    char the_date[80];
    static int num_entries;
    static FILE *in;
    static Container *list;
    static ListThreadArgs lta;
    UserData *ud;
    static int timer_id=0;
    static int timer2_id=0;
    static int which_timer = 0;
    static NewsServer *ns;
    static char buffer[1024];
    static int num_items;
    static int final_items;
    static int loaded_backup = 0;
    
    switch(msg)
    {
        
        case WM_HELP:
            showHelpWindow(HELP_SELGROUP);
            break;
            
        case WM_INITDLG:
            timer_id = 0;
            timer2_id = 0;
            which_timer = 0;
            list_thread_id = 0;
            server = (array<char *> *)mp2;
            hptrWait = WinQuerySysPointer( HWND_DESKTOP, SPTR_WAIT, FALSE );
            hptrOrig = WinQueryPointer( HWND_DESKTOP );
            WinSetPointer( HWND_DESKTOP, hptrWait );
            
            WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_HOSTGROUP),(*server)[0]);
            WinSendMsg(WinWindowFromID(hwndDlg,ID_HOSTLIST_FILTERSTRING),EM_SETTEXTLIMIT,MPFROMSHORT(255),MPFROMSHORT(0));
            
            list = new Container();
            list->initialise(hwndDlg,ID_HOSTLIST_LIST);
            // list->toggleSmallIcons();
            list->setTextOnlyMode();
            
            num_entries = 0;
            in = fopen((*server)[0],"r");
            
            // if load failed, try to find an old backup file
            if (!in)
            {
                char *bakup = new char[4096];
                sprintf(bakup,"%s.old",(*server)[0]);
                in = fopen(bakup,"r");
                if (in) loaded_backup = 1;
                delete[] bakup;
            }
            if (in)
            {
                // shouldn't do a [REFRESH] while loading
                WinEnableControl(hwndDlg,ID_HOSTLIST_B_REFRESH,FALSE);
                
                fgets(the_date,80,in);
                trimCR(the_date);
                char *since = strchr(the_date,'/');
                if (since)
                {
                    *since = '\0';
                    since++;
                    if (!isdigit(*since))
                        since = NULL;
                }
                WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_REFRESH),the_date);
                
                fgets(buffer,1024,in);
                final_items = num_items = atoi(buffer);
                WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_NUMGROUPS),trimCR(buffer));
                WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_SOFAR),"0%");
                
                // prepare struct for thread arguments
                lta.list = list;
                lta.in = in;
                lta.num_items = num_items;
                lta.stopping = 0;
                
                list_thread_id = _beginthread(ListThread,NULL,32768,(void*)&lta);
//                list->insertFile(in,num_items);
//                fclose(in);
                timer2_id = WinStartTimer(hab,hwndDlg,TID_USERMAX-3,500);
                which_timer = 2;
            }
            
            ns = NULL;
            
            WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwndDlg,ID_HOSTLIST_LIST),0);

            WinSetPointer( HWND_DESKTOP, hptrOrig );
            
            break;
            
            
            
        case WM_CLOSE:
            lta.stopping = 1;
            if (ns != NULL)
            {
                ns->close();
                delete ns;
                ns = NULL;
            }
            WinStopTimer(hab,hwndDlg,timer_id);
            WinStopTimer(hab,hwndDlg,timer2_id);
            which_timer = 0;
            if (list_thread_id != 0)
                break;
            strcpy((*server)[0],"0");
            delete list;
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
            
        case WM_USER:
            list->arrange();
            WinEnableControl(hwndDlg,ID_HOSTLIST_B_REFRESH,TRUE);
            break;
            
        case WM_CONTROL:
            switch(SHORT2FROMMP(mp1))
            {
                        
                case CN_HELP:
                    WinSendMsg(hwndDlg,WM_HELP,0,0);
                    break;
                
                case CN_ENTER:
                {
                    PNOTIFYRECORDENTER nre = (PNOTIFYRECORDENTER)mp2;
                    ud = (UserData*)nre->pRecord;
                    if (ud != NULL)
                    {
                        //WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_SELECTED),ud->getName());
                        
                        HWND listbox = WinWindowFromID(hwndDlg,ID_HOSTLIST_SELECTED);
                        
                        // disable window update while inserting
                        WinEnableWindowUpdate(listbox,FALSE);
                        
                        int num_items = WinQueryLboxCount(listbox);
                        
                        // check we haven't already inserted the item
                        char *bbuffer = new char[4096];
                        for (int i=0; i<num_items; i++)
                        {
                            WinQueryLboxItemText(listbox,i,bbuffer,512);
                            if (strcmp(bbuffer,ud->getName()) == 0)
                                break;
                        }
                        delete[] bbuffer;
                        
                        // did we NOT find a match
                        if (i == num_items)
                            WinSendMsg(listbox,LM_INSERTITEM,MPFROMSHORT(LIT_SORTASCENDING),MPFROMP(ud->getName()));
                        
                        // enable it again
                        WinEnableWindowUpdate(listbox,TRUE);
                        
                    }
                    break;
                }
                    
                    
/*
                case EN_CHANGE:
                    if (SHORT1FROMMP(mp1) == ID_HOSTLIST_FILTERSTRING)
                    {
                        //WinSendMsg(WinWindowFromID(hwndDlg,ID_HOSTLIST_FILTER),BS_DEFAULT,MPFROMSHORT(0),MPFROMSHORT(0));
                        
                        WinSetWindowBits(WinWindowFromID(hwndDlg,ID_HOSTLIST_FILTER),
                                         QWL_STYLE,
                                         1,
                                         BS_DEFAULT);
                        
//                         char namestr[255];
//
//                        WinQueryWindowText(WinWindowFromID(hwndDlg,SHORT1FROMMP(mp1)),255,namestr);
//                        if (makeNewsGroupNameValidOnly(namestr))
//                            WinSetWindowText(WinWindowFromID(hwndDlg,SHORT1FROMMP(mp1)),namestr);
                        
                    }
                    break;
*/
            
            
            }
            break;
            
        case WM_TIMER:
            if (which_timer == 1)
            {
                num_entries = 0;
                // file only written on completeion of process
                in = fopen((*server)[0],"r");
                if (in)
                {
                    WinStopTimer(hab,hwndDlg,timer_id);
                    WinStopTimer(hab,hwndDlg,timer2_id);
                    timer_id = 0;
                    timer2_id = 0;
                    which_timer = 0;
                    
                    fgets(the_date,80,in);
                    char *since = strchr(the_date,'/');
                    if (since)
                    {
                        *since = '\0';
                        since++;
                        if (!isdigit(*since))
                            since = NULL;
                    }
                    trimCR(the_date);
                    WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_REFRESH),the_date);
                    
                    fgets(buffer,1024,in);
                    int num_items = atoi(buffer);
                    WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_NUMGROUPS),trimCR(buffer));
                    
                    // prepare struct for thread arguments
                    lta.list = list;
                    lta.in = in;
                    lta.num_items = num_items;
                    lta.stopping = 0;
                    
                    list->removeAll();
                    list->arrange();
                    
                    list_thread_id = _beginthread(ListThread,NULL,32768,(void*)&lta);
                    timer2_id = WinStartTimer(hab,hwndDlg,TID_USERMAX-3,500);
                    which_timer = 2;
                }
            }
            else if (which_timer == 2)  // fileInsert counter update
            {
                if (final_items > 0)
                {
                    int percent = 100*so_far/final_items;
                    char percent_buffer[15];
                    // DosBeep(100,100);
                    sprintf(percent_buffer,"(%d%%)",percent);
                    WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_SOFAR),percent_buffer);
                    if (percent > 99)
                    {
                        WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_SOFAR),"");
                        WinStopTimer(hab,hwndDlg,timer2_id);
                        which_timer = 0;
                        timer2_id = 0;
                        WinEnableControl(hwndDlg,ID_HOSTLIST_B_REFRESH,TRUE);
                    }
                }
                else
                {
                    WinSetWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_SOFAR),"");
                    WinStopTimer(hab,hwndDlg,timer2_id);
                    timer2_id = 0;
                    which_timer = 0;
                    WinEnableControl(hwndDlg,ID_HOSTLIST_B_REFRESH,TRUE);
                }
            }
            else
            {
                DosBeep(50,50);
                
            }
            
            
            break;
            
        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1 ) )
            {
                case ID_HOSTLIST_REMOVE:
                    {
                        HWND listbox = WinWindowFromID(hwndDlg,ID_HOSTLIST_SELECTED);
                        
                        // disable window update while inserting
                        WinEnableWindowUpdate(listbox,FALSE);
                        
                        long index;
                        do
                        {
                            index = WinQueryLboxSelectedItem(listbox);
                            if (index >= 0) 
                            {
                                WinDeleteLboxItem(listbox,index);
                            }
                        } while (index >= 0);
                        
                        // enable it again
                        WinEnableWindowUpdate(listbox,TRUE);
                    }
                    break;
                
                case ID_HOSTLIST_FILTER:
                    {
                        char *namestr = new char[4096];
                        if (namestr)
                        {
                            *namestr = '\0';
                            WinQueryWindowText(WinWindowFromID(hwndDlg,ID_HOSTLIST_FILTERSTRING),255,namestr);
                            if (*namestr == '\0')
                            {
                                list->unFilter();
                            }
                            else
                            {
                                UserData *ud = list->findSelectedRecord(NULL);
                                if (ud)
                                {
                                    list->noEmphasis(ud);
                                }
                                list->filter(namestr);
                                list->filter(namestr);
                                list->arrange();
                            }
                            
                            delete[] namestr;
                        }
                    }
                    break;
                
                case DID_OK:
                    lta.stopping = 1;
                    {
                        HWND listbox = WinWindowFromID(hwndDlg,ID_HOSTLIST_SELECTED);
                        
                        // disable window update while inserting
                        WinEnableWindowUpdate(listbox,FALSE);
                        
                        int num_items = WinQueryLboxCount(listbox);
                        sprintf((*server)[0],"%d",num_items);
                        
                        // check we haven't already inserted the item
                        for (int i=0; i<num_items; i++)
                        {
                            char *bbuffer = new char[4096];
                            WinQueryLboxItemText(listbox,i,bbuffer,512);
                            (*server)[i+1] = new char[1+strlen(bbuffer)];
                            strcpy((*server)[i+1],bbuffer);
                            delete[] bbuffer;
                        }
                        
                        // enable it again
                        WinEnableWindowUpdate(listbox,TRUE);
                    }
                    
                    if (ns != NULL)
                    {
                        ns->close();
                        delete ns;
                        ns=NULL;
                    }

                    
                    // all OK, close dialog, return
                    WinStopTimer(hab,hwndDlg,timer_id);
                    WinStopTimer(hab,hwndDlg,timer2_id);
                    which_timer = 0;
                    while(list_thread_id != 0)
                        DosSleep(100);
                    delete list;
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) TRUE;
                    
                case DID_CANCEL:
                    lta.stopping=1;
                    strcpy((*server)[0],"0");
                    if (ns != NULL)
                    {
                        ns->close();
                        delete ns;
                        ns=NULL;
                    }
                    WinStopTimer(hab,hwndDlg,timer_id);
                    WinStopTimer(hab,hwndDlg,timer2_id);
                    which_timer = 0;
                    while(list_thread_id != 0)
                        DosSleep(100);
                    delete list;
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) FALSE;
                    
                case ID_HOSTLIST_B_REFRESH:
                    {
                        FILE *tester = fopen("listdlg.exe","r");
                        if (tester) fclose(tester);
                        else
                        {
                            message("Failed to find LISTDLG.EXE");
                            break;
                        }
                    }
                    
                    WinEnableControl(hwndDlg,ID_HOSTLIST_B_REFRESH,FALSE);
                    if (timer_id == 0)
                    {
                        char *fiel = new char[4096];
                        char *ptr;
                        
                        // backup the current server list
                        if (!loaded_backup)
                        {
                            sprintf(fiel,"%s.old",(*server)[0]);
                            unlink(fiel);
                            sprintf(fiel,"copy %s %s.old",(*server)[0],(*server)[0]);
                            system(fiel);
//                            rename((*server)[0],fiel);
                        }
                        
                        // start collection of new one
                        sprintf(fiel,"detach listdlg.exe %s",(*server)[0]);
                        
//                        message("USername [%s]",(*server)[1]);
//                        message("password [%s]",(*server)[2]);
                        
                        // check if need username & password
                        if (*((*server)[1]) != '\0' && *((*server)[2]) != '\0')
                        {
                            strcat(fiel," ");
//                            ptr = strchr((*server)[1],' ');
//                            if (ptr)
//                            {
                                strcat(fiel,"\"");
                                strcat(fiel,(*server)[1]);
                                strcat(fiel,"\"");
//                            }
//                            else
//                            {
//                                strcat(fiel,(*server)[1]);
//                            }
                            strcat(fiel," ");
//                            ptr = strchr((*server)[2],' ');
//                            if (ptr)
//                            {
                                strcat(fiel,"\"");
                                strcat(fiel,(*server)[2]);
                                strcat(fiel,"\"");
 //                           }
 //                           else
 //                           {
 //                               strcat(fiel,(*server)[2]);
 //                           }
                        }
                            
                        
//                        message("Command[%s]",fiel);
                        system(fiel);
                        
                        timer_id = WinStartTimer(hab,hwndDlg,TID_USERMAX-2,5000);
                        which_timer = 1;
                        
                        delete[] fiel;

                    }
                    
                    break;
                    
            }
            break;
            
            
        default:
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
    }
    return (MRESULT) TRUE;
}



