#include "groupdlg.h"
#include "hostlistdlg.h"
#include "help.h"

char group_name[MAX_GROUP_NAME_LEN+1] = {"\0"};
char g_username[32] = {"\0"};
char g_password[32] = {"\0"};
long group_number;
long group_cull;

void exportGroupName(char *s)
{
    if (s != NULL)
        strncpy(group_name,s,MAX_GROUP_NAME_LEN);
    else
        memset(group_name,0,MAX_GROUP_NAME_LEN);
}

void exportGroupUsername(char *s)
{
    if (s != NULL)
        strncpy(g_username,s,31);
    else
        memset(g_username,0,32);
}

void exportGroupPassword(char *s)
{
    if (s != NULL)
        strncpy(g_password,s,31);
    else
        memset(g_password,0,32);
}
    
void importGroupName(char *s)
{
    if (s != NULL)
       strncpy(s,group_name,MAX_GROUP_NAME_LEN);
}

void exportGroupNumber(long x)
{
    group_number = x;
}
    
long importGroupNumber(void)
{
    return group_number;
}

void exportGroupCull(long x)
{
    group_cull = x;
}
    
long importGroupCull(void)
{
    return group_cull;
}


//                                                                         
//                                                                         
//                                                                           
//                                                                           
//                                                                           
MRESULT EXPENTRY GroupDlgProc(HWND hwndDlg,ULONG msg,MPARAM mp1,MPARAM mp2)
{
    char number_str[20];
    static char *server;
    
    switch(msg)
    {
        
        case WM_INITDLG:
            // save variables
//            strcpy(group_keep,group_name);
//            group_number_keep = group_number;
//            group_cull_keep = group_cull;
            
            server = (char *)mp2;
            if (server == NULL)
                WinEnableControl(hwndDlg,NEW_GROUP_B_LIST,FALSE);
            
            // set maximal input string sizes for fields
            WinSendMsg(WinWindowFromID(hwndDlg,NEW_GROUP_ENTRY),EM_SETTEXTLIMIT,MPFROMSHORT(MAX_GROUP_NAME_LEN-1),MPFROMSHORT(0));
            WinSendMsg(WinWindowFromID(hwndDlg,NEW_GROUP_CULL),EM_SETTEXTLIMIT,MPFROM2SHORT(8,TRUE),MPFROM2SHORT(0,TRUE));
            WinSendMsg(WinWindowFromID(hwndDlg,NEW_GROUP_NEXT),EM_SETTEXTLIMIT,MPFROM2SHORT(8,TRUE),MPFROM2SHORT(0,TRUE));
            
            // load initial strings into entry fileds
            if (*group_name != '\0')
            {
                WinSetWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_ENTRY),group_name);
            }
            sprintf(number_str,"%ld",group_number);
            WinSetWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_NEXT),number_str);
            sprintf(number_str,"%ld",group_cull);
            WinSetWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_CULL),number_str);
            
            // be sure the group name field is active
            WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwndDlg,NEW_GROUP_ENTRY),0);
            break;
                                                                                                                                                              
            
        case WM_CONTROL:
            switch(SHORT2FROMMP(mp1))
            {
                case EN_CHANGE:
                    if (SHORT1FROMMP(mp1) == NEW_GROUP_CULL)
                    {
                        WinQueryWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_CULL),10,number_str);
                        if (makeDigitsOnly(number_str))
                            WinSetWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_CULL),number_str);
                        
                    }
                    else if (SHORT1FROMMP(mp1) == NEW_GROUP_NEXT)
                    {
                        WinQueryWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_NEXT),10,number_str);
                        if (makeNumberOnly(number_str))
                            WinSetWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_NEXT),number_str);
                        
                    }
                    break;
            }
            break;
            
        case WM_HELP:
            {
                    showHelpWindow(HELP_ADDGROUP);
            }
            
            break;
    
        case WM_CLOSE:
//            strcpy(group_name,group_keep);
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);

        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1 ) )
            {
                case NEW_GROUP_B_LIST:
                    {
                        array<char *> servers;
                        memset(group_name,0,MAX_GROUP_NAME_LEN);
                        // WinQueryWindowText(WinWindowFromID(hwndDlg,ID_HOST_FIELD),CCHMAXPATH,group_name);
                        
                        servers[0] = newStrDup(server);
                        servers[1] = newStrDup(g_username);
                        servers[2] = newStrDup(g_password);
                        
                        WinDlgBox( HWND_DESKTOP,       // Place anywhere on desktop
                                   hwndDlg,            // Owned by main dialog
                                   (PFNWP)HostListDlgProc,// Addr. of procedure
                                   (HMODULE)0,         // Module handle                
                                   ID_HOSTLIST_DIALOG,   // Dialog identifier in resource
                                   (PVOID)&servers);              // Initialization data
                        
                        int num_returned = atoi(servers[0]);
                        if (num_returned == 1 && *(servers[1]) != '\0')
                        {
                            WinSetWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_ENTRY),servers[1]);
                        }
                        else if (num_returned > 1)
                        {
                            *group_name = '\0';
                            int line_size = 0;
                            for (int i=1; i<=num_returned; i++)
                            {
                                line_size += 2+strlen(servers[i]);
                                if (line_size > MAX_GROUP_NAME_LEN)
                                {
                                    message("Too many groups added at once.\nAdding %d.",i);
                                    break;
                                }
                                
                                strcat(group_name,servers[i]);
                                strcat(group_name," ");
                            }
                            WinSetWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_ENTRY),group_name);
                        }
                        
                    }
                    break;
                    
                case DID_OK:
                    // string field
                    memset(group_name,0,MAX_GROUP_NAME_LEN);
                    WinQueryWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_ENTRY),MAX_GROUP_NAME_LEN-1,group_name);
                    // numeric fields
                    WinQueryWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_NEXT),10,number_str);
                    group_number = atol(number_str);
                    WinQueryWindowText(WinWindowFromID(hwndDlg,NEW_GROUP_CULL),10,number_str);
                    group_cull = atol(number_str);
                    
                    // all OK, close dialog, return
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) TRUE;
                    
                case DID_CANCEL:
//                    strcpy(group_name,group_keep);
//                    group_number = group_number_keep;
//                    group_cull = group_cull_keep;
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) FALSE;
                    
            }
            break;
        
        default:
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
    }
    return (MRESULT) TRUE;
}



