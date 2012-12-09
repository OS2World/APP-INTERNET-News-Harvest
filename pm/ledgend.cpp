/* ledgend.cpp,v 1.2 2001/06/20 00:42:42 root Exp */

#include "ledgenddlg.h"

char *ledgends[] = {
                                 "Anthony Winner",
                                 "Brian Arrighetti",
                                 "Chris Cureau",
                                 "Christopher McRae",
                                 "David Calafrancesco",
                                 "Eric Bentley",
                                 "Gilbert D",
                                 "Stephen Walsh",
                                 "J Paquin",
                                 "James A Jones",
                                 "John Anthony D'Errico",
                                 "John Beal",
                                 "John Bridges",
                                 "Justin Ong",
                                 "Larry Nottingham",
                                 "Larry Bristol",
                                 "Mac Warp",
                                 "Nick Cato",
                                 "Paul Weiner",
                                 "Ron Cooper",
                                 "Rob Hughes",
                                 "Richard S. Harris",
                                 "Sader",
                                 "Robert C. Santore",
                                 "Trine Curtis",
                                 "Van",
                                 "Steven H. Levine"
                               };

#define NUM_LEDGENDS (sizeof(ledgends)/sizeof(char *))

MRESULT EXPENTRY LedgendDlgProc(HWND hwndDlg,ULONG msg,MPARAM mp1,MPARAM mp2)
{
    static Container *list = NULL;
    int i;
    

    switch(msg)
    {
        
        case WM_INITDLG:
            list = new Container();
            list->initialise(hwndDlg,ID_LEDGENDS_CONT);
            // list->setTextOnlyMode();
            
            // insert all the names into the container
            for (i=0; i<NUM_LEDGENDS; i++)
            {
                list->insert(ledgends[i],NULL,8888L,8888L);
                list->setIcon(list->getLastInsert(),ICON_SUGGEST);
            }
            
            // be sure the Dismiss button field is active
            WinFocusChange(HWND_DESKTOP,WinWindowFromID(hwndDlg,ID_LEDGENDS_DIS),0);
            break;
            
        case WM_CLOSE:
            if (list != NULL)
                delete list;
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);

        case WM_COMMAND:
            switch( SHORT1FROMMP( mp1 ) )
            {
                case ID_LEDGENDS_DIS:
                    // close dialog, return
                   if (list != NULL)
                       delete list;
                    WinDismissDlg( hwndDlg, TRUE);
                    return (MRESULT) TRUE;
            }
            break;
        
        default:
            return WinDefDlgProc(hwndDlg,msg,mp1,mp2);
    }
    return (MRESULT) TRUE;
}



