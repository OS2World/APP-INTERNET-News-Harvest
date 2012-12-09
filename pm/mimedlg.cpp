#include "mimedlg.h"


//                                                                         
//                                                                         
//                                                                           
//                                                                           
//                                                                           
MRESULT EXPENTRY MimeDlgProc(HWND hwndDlg,ULONG msg,MPARAM mp1,MPARAM mp2)
{
    HPOINTER  hptrOrig, hptrWait;
    static int num_entries;
    static int entry;
    static FILE *in;
    static Container *list;
    static MimeSet *mimes;
    UserData *ud;
    char buffer[1024];
    int i;
    
    switch(msg)
    {
        
        case WM_INITDLG:
            hptrWait = WinQuerySysPointer( HWND_DESKTOP, SPTR_WAIT, FALSE );
            hptrOrig = WinQueryPointer( HWND_DESKTOP );
            WinSetPointer( HWND_DESKTOP, hptrWait );
            
            WinSendMsg(WinWindowFromID(hwndDlg,MIME_NAME),EM_SETTEXTLIMIT,MPFROMSHORT(128),MPFROMSHORT(0));
            WinSendMsg(WinWindowFromID(hwndDlg,MIME_EXT), EM_SETTEXTLIMIT,MPFROMSHORT(128),MPFROMSHORT(0));
            WinSendMsg(WinWindowFromID(hwndDlg,MIME_EXE), EM_SETTEXTLIMIT,MPFROMSHORT(128),MPFROMSHORT(0));
            
            mimes = (MimeSet*)mp2;
            list = new Container();
            list->initialise(hwndDlg,MIME_CONT);
            
            num_entries = mimes->getSetNum();
            for (i=0; i<num_entries; i++)
            {
                sprintf(buffer,"%s: %s",mimes->getName(i),mimes->getExtensions(i));
//                message("%d/%d %s is %s",i,num_entries,mimes->getName(i),mimes->getExtensions(i));
                
                list->insert(buffer,(UserData*)NULL,8888L,8888L);
            }
            
            WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwndDlg,MIME_CONT),0);
            
            WinSetWindowText(WinWindowFromID(hwndDlg,MIME_NAME),mimes->getName(mimes->getSetNum()-1));
            WinSetWindowText(WinWindowFromID(hwndDlg,MIME_EXT),mimes->getExtensions(mimes->getSetNum()-1));
            WinSetWindowText(WinWindowFromID(hwndDlg,MIME_EXE),mimes->getCommand(mimes->getSetNum()-1));
            list->setItemFocus(list->getLastInsert());
            
            WinSetPointer( HWND_DESKTOP, hptrOrig );
            
            entry = -1;
            break;
            
        case WM_HELP:
            showHelpWindow(HELP_MIMES);
            break;
            
            
        case WM_CLOSE:
            mimes->reload();
            delete list;
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
            
        case WM_USER:
            list->arrange();
            break;
            
        case WM_CONTROL:
            switch(SHORT2FROMMP(mp1))
            {
                
                case CN_HELP:
                    WinSendMsg(hwndDlg,WM_HELP,0,0);
                    break;
                    
                // sent every time emphasis of an object is changed, so if
                // change from A to B, get deselect on A, and select on B
                case CN_EMPHASIS:
                {
                    PNOTIFYRECORDEMPHASIS nre = (PNOTIFYRECORDEMPHASIS)mp2;
                    ud = (UserData*)nre->pRecord;
                    
                    if (ud != NULL)
                    {
                        char *buffer = new char[4096];
                        strcpy(buffer,ud->getName());
                        char *ptr = buffer+strlen(buffer)-1;
                        while (ptr>buffer && *ptr!='.') ptr--;
                        if (*ptr == '.')
                        {
                            int index = mimes->find(ptr);
                            
                            if (index > -1)
                            {
                                entry = index;
                                WinSetWindowText(WinWindowFromID(hwndDlg,MIME_NAME),mimes->getName(index));
                                WinSetWindowText(WinWindowFromID(hwndDlg,MIME_EXT),mimes->getExtensions(index));
                                WinSetWindowText(WinWindowFromID(hwndDlg,MIME_EXE),mimes->getCommand(index));
                            }
                        }
                        delete[] buffer;
                    }
                }
                break;
                    
                case EN_CHANGE:
                {
                    if (SHORT1FROMMP(mp1) == MIME_EXE)
                    {
                        ud = list->findSelectedRecord(NULL);
                        static int lastindex = 0;
                        if (ud && entry != -1)
                        {
                            if (lastindex == entry)
                            {
                                char *buffer = new char[4096];
                                WinQueryWindowText(WinWindowFromID(hwndDlg,MIME_EXE),129,buffer);
                                mimes->setCommand(entry,buffer);
                                delete[] buffer;
                            }
                            lastindex = entry;
                        }
                    }
                }
                break;
                    
            }
            break;
            
        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1 ) )
            {
                case IDM_DELETE:
//                    message("delete");
                    break;
                    
                case DID_OK:
                    // all OK, close dialog, return
                    if (entry != -1)
                    {
                        WinQueryWindowText(WinWindowFromID(hwndDlg,MIME_EXE),129,buffer);
                        mimes->setCommand(entry,buffer);
                    }
                    mimes->save();
                    delete list;
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) TRUE;
                    
                case DID_CANCEL:
                    mimes->reload();
                    delete list;
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) FALSE;
            }
            break;
            
            
        default:
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
    }
    return (MRESULT) TRUE;
}



