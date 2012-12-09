#include "container.h"

Container::Container(void)
{
    num_objects = 0;
    cont = NULL;
    mini_icons = 0;
    no_icons = 0;
    last_insert = NULL;
    no_delete = 1;
    
    icon_jpeg = WinLoadPointer(HWND_DESKTOP,0,ICON_JPEG);
    icon_avi  = WinLoadPointer(HWND_DESKTOP,0,ICON_AVI);
    icon_au   = WinLoadPointer(HWND_DESKTOP,0,ICON_AU);
    icon_bmp  = WinLoadPointer(HWND_DESKTOP,0,ICON_BMP);
    icon_fli  = WinLoadPointer(HWND_DESKTOP,0,ICON_FLI);
    icon_gif  = WinLoadPointer(HWND_DESKTOP,0,ICON_GIF);
    icon_midi = WinLoadPointer(HWND_DESKTOP,0,ICON_MIDI);
    icon_mp3  = WinLoadPointer(HWND_DESKTOP,0,ICON_MP3);
    icon_mpeg = WinLoadPointer(HWND_DESKTOP,0,ICON_MPG);
    icon_pcx  = WinLoadPointer(HWND_DESKTOP,0,ICON_PCX);
    icon_tga  = WinLoadPointer(HWND_DESKTOP,0,ICON_TGA);
    icon_tiff = WinLoadPointer(HWND_DESKTOP,0,ICON_TIF);
    icon_wav  = WinLoadPointer(HWND_DESKTOP,0,ICON_WAV);
    icon_html = WinLoadPointer(HWND_DESKTOP,0,ICON_HTML);
    icon_png  = WinLoadPointer(HWND_DESKTOP,0,ICON_PNG);
    
    icon_dunno= WinLoadPointer(HWND_DESKTOP,0,ICON_DUNNO);
    
    icon_host_o  = WinLoadPointer(HWND_DESKTOP,0,ICON_HOST_O);
    icon_host_c  = WinLoadPointer(HWND_DESKTOP,0,ICON_HOST_C);
    icon_group_o = WinLoadPointer(HWND_DESKTOP,0,ICON_GROUP_O);
    icon_group_c = WinLoadPointer(HWND_DESKTOP,0,ICON_GROUP_C);
    icon_server  = WinLoadPointer(HWND_DESKTOP,0,ICON_SERVER);
}

Container::~Container(void)
{
    // remove *all* records from the container with no arrange
    //    WinSendMsg(cont,CM_REMOVERECORD,MPFROMP((PRECORDCORE)CMA_FIRST),MPFROM2SHORT(0,0));
    if (no_delete)
        WinSendMsg(cont,CM_REMOVERECORD,MPFROMP((PRECORDCORE)CMA_FIRST),MPFROM2SHORT(0,CMA_FREE));
    
    WinDestroyPointer(icon_jpeg); 
    WinDestroyPointer(icon_avi); 
    WinDestroyPointer(icon_au); 
    WinDestroyPointer(icon_bmp); 
    WinDestroyPointer(icon_fli); 
    WinDestroyPointer(icon_gif); 
    WinDestroyPointer(icon_midi); 
    WinDestroyPointer(icon_mp3); 
    WinDestroyPointer(icon_mpeg); 
    WinDestroyPointer(icon_pcx); 
    WinDestroyPointer(icon_tga); 
    WinDestroyPointer(icon_tiff); 
    WinDestroyPointer(icon_wav); 
    WinDestroyPointer(icon_html); 
    WinDestroyPointer(icon_png); 
    WinDestroyPointer(icon_dunno); 
    WinDestroyPointer(icon_host_o);
    WinDestroyPointer(icon_host_c);
    WinDestroyPointer(icon_group_o);
    WinDestroyPointer(icon_group_c);
    WinDestroyPointer(icon_server);
}

void Container::initialise(HWND dlg,ULONG iD)
{
    
    dialog = dlg;
    id     = iD;
    
    // tell the control it's a tree-with-icons style
    CNRINFO cnrinfo;
    memset(&cnrinfo,0,sizeof(CNRINFO));
    cnrinfo.cb = sizeof(CNRINFO);
    cnrinfo.flWindowAttr = CV_ICON | CV_TREE| CA_TREELINE;
    cnrinfo.cxTreeLine = 1;
    
        
    
    cont = WinWindowFromID(dlg,iD);
    
    WinSendMsg(cont,CM_SETCNRINFO,MPFROMP(&cnrinfo),MPFROMLONG(CMA_SLBITMAPORICON|CMA_SLTREEBITMAPORICON|CMA_FLWINDOWATTR));
    
}

UserData *Container::setIcon(UserData *ud,int icon_id)
{
    HPOINTER ico;

    ico = WinLoadPointer(HWND_DESKTOP,0L,icon_id);
            
    // change the icon - should work, but don't
    ud->setIcons(ico,ico);
    WinSendMsg(cont,CM_INVALIDATERECORD,MPFROMP(&ud),MPFROM2SHORT(1,CMA_TEXTCHANGED|CMA_REPOSITION));
    return ud;
    
}

void UserData::setIcons(HPOINTER c,HPOINTER o)
{ 
    core.hptrIcon=c;
    core.hptrMiniIcon=c; // *
    tree_icons.hptrCollapsed=c;
    tree_icons.hptrExpanded=o; 
    core.pTreeItemDesc=&tree_icons; 
}



int Container::insert(char *name,ULONG arg1,ULONG arg2)
{
    return insert(name,(UserData*)NULL,arg1,arg2);
}

void Container::emphasis(UserData *it)
{
    WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)it),MPFROM2SHORT(TRUE,CRA_SELECTED|CRA_INUSE));
}

void Container::noEmphasis(UserData *it)
{
    WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)it),MPFROM2SHORT(FALSE,CRA_SELECTED|CRA_INUSE));
}

void Container::selected(UserData *it)
{
    WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)it),MPFROM2SHORT(TRUE,CRA_SELECTED));
}

void Container::noSelected(UserData *it)
{
    WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)it),MPFROM2SHORT(FALSE,CRA_SELECTED));
}

void Container::inUse(UserData *it)
{
    WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)it),MPFROM2SHORT(TRUE,CRA_INUSE));
}

void Container::noInUse(UserData *it)
{
    WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)it),MPFROM2SHORT(FALSE,CRA_INUSE));
}


/*
int Container::insert(char *name,char *parent,ULONG arg1,ULONG arg2)
{
    if (parent == NULL)
        return insert(name,(UserData*)NULL,arg1,arg2);
    else
    {
        // find our parent
        for (int i=0; i<num_objects; i++)
        {
            if (strncmp(parent,objects[i]->getName(),255)==0)
                return insert(name,objects[i],arg1,arg2);
        }
    }
}
*/

HPOINTER Container::determineIcon(char *name)
{
    HPOINTER the_icon;
    if (*name)
    {
        if     (stristr(name,".avi"))  the_icon = icon_avi;
        else if(stristr(name,".au"))   the_icon = icon_au;
        else if(stristr(name,".bmp"))  the_icon = icon_bmp;
        else if(stristr(name,".fli"))  the_icon = icon_fli;
        else if(stristr(name,".flc"))  the_icon = icon_fli;
        else if(stristr(name,".gif"))  the_icon = icon_gif;
        else if(stristr(name,".jpg"))  the_icon = icon_jpeg;
        else if(stristr(name,".jpe"))  the_icon = icon_jpeg;
        else if(stristr(name,".jpeg")) the_icon = icon_jpeg;
        
        else if(stristr(name,".mid"))  the_icon = icon_midi;
        else if(stristr(name,".midi")) the_icon = icon_midi;
        else if(stristr(name,".mp3"))  the_icon = icon_mp3;
        else if(stristr(name,".mpg"))  the_icon = icon_mpeg;
        else if(stristr(name,".mpe"))  the_icon = icon_mpeg;
        else if(stristr(name,".mpeg")) the_icon = icon_mpeg;
        else if(stristr(name,".pcx"))  the_icon = icon_pcx;
        else if(stristr(name,".tga"))  the_icon = icon_tga;
        else if(stristr(name,".tif"))  the_icon = icon_tiff;
        else if(stristr(name,".tiff")) the_icon = icon_tiff;
        else if(stristr(name,".wav"))  the_icon = icon_wav;
        else if(stristr(name,".htm"))  the_icon = icon_html;
        else if(stristr(name,".html")) the_icon = icon_html;
        else if(stristr(name,".png"))  the_icon = icon_png;
        else the_icon = icon_dunno;
        
//        if     (stristr(name,".avi"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_AVI);
//        else if(stristr(name,".au"))   the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_AU);
//        else if(stristr(name,".bmp"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_BMP);
//        else if(stristr(name,".fli"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_FLI);
//        else if(stristr(name,".flc"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_FLI);
//        else if(stristr(name,".gif"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_GIF);
//        else if(stristr(name,".jpg"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_JPEG);
//        else if(stristr(name,".jpe"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_JPEG);
//        else if(stristr(name,".jpeg")) the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_JPEG);
//        else if(stristr(name,".mid"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_MIDI);
//        else if(stristr(name,".midi")) the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_MIDI);
//        else if(stristr(name,".mp3"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_MP3);
//        else if(stristr(name,".mpg"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_MPG);
//        else if(stristr(name,".mpeg")) the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_MPG);
//        else if(stristr(name,".pcx"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_PCX);
//        else if(stristr(name,".tga"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_TGA);
//        else if(stristr(name,".tif"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_TIF);
//        else if(stristr(name,".tiff")) the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_TIF);
//        else if(stristr(name,".wav"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_WAV);
//        else if(stristr(name,".htm"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_HTML);
//        else if(stristr(name,".html")) the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_HTML);
//        else if(stristr(name,".png"))  the_icon = WinLoadPointer(HWND_DESKTOP,0,ICON_PNG);
    }
    
    return the_icon;
    
}

// Insert a Server Object
int Container::insert(char *name,char *user,char *pass,ULONG arg1,ULONG arg2)
{
    UserData *ud;
    UserData *parent = NULL;
    
    ud = allocateRecords(1);
    if (ud)
    {
        last_insert = ud;
        
        num_objects++;
        
        // set all the stuff in our record
        ud->setName(name);
        ud->setUsername(user);
        ud->setPassword(pass);
        ud->setType(TYPE_SERVER);
        ud->setIcons(icon_server,icon_server);
        ud->setCull(arg1);
        ud->setStart(arg2);
        
        // Build up insertion information
        RECORDINSERT ri;   
        memset(&ri,0,sizeof(RECORDINSERT));
        ri.cb = sizeof(RECORDINSERT);
        ri.zOrder = CMA_END;                    // add on top of siblings
        if (!parent || !parent->getCollapsed())
            ri.fInvalidateRecord = TRUE;            // re-arrange after insert
        ri.pRecordParent = (PRECORDCORE)parent; // set tree hierarchy
        ri.cRecordsInsert = 1;                  // number of records to insert
        ri.pRecordOrder = (RECORDCORE*)CMA_END; // add to end of list

        MRESULT rc = WinSendMsg(cont,CM_INSERTRECORD,MPFROMP((PRECORDCORE)ud),MPFROMP(&ri));
        if (rc == 0)
            message("CM_INSERTRECORD returned %d",rc);
        
        return 1;
    }
    else
    {
        DosBeep(100,100);
        return 0;
    }
}




int Container::insert(char *name,UserData *parent,ULONG arg1,ULONG arg2)
{
    UserData *ud;
    
    ud = allocateRecords(1);
    if (ud)
    {
        last_insert = ud;
        
//        objects[num_objects] = ud;  // keep a pointer to the block
        num_objects++;
        
        // set all the stuff in our record
        ud->setName(name);
        if (arg1 == arg2 && arg1 == 7777L)
        {
            ud->setType(TYPE_GROUP);
            ud->setIcons(icon_group_o,icon_group_c);
        }
        else if (arg1 == arg2 && arg1 == 8888L)
        {
            ud->setType(TYPE_FILE);
            ud->setParent(parent);
            HPOINTER the_icon;
            if (*name)
            {
                the_icon = determineIcon(name);
//
//                if (the_icon != icon_dunno)
//                {
                    ud->setIcons(the_icon,the_icon);
//                }
//                else
//                {
//                    // TRY TO LOAD THE Shared WPS ICON
//                    the_icon = WinLoadFileIcon(name,FALSE);
//                    if (the_icon != NULL)
//                    {
//                        ud->setIcons(the_icon,the_icon);
//                        WinSendMsg(cont,CM_INVALIDATERECORD,MPFROMP(&ud),MPFROM2SHORT(1,CMA_TEXTCHANGED|CMA_REPOSITION));
//                    }
//                }
            }
            
        }
        else if (parent == NULL)
        {
            ud->setType(TYPE_SERVER);
            ud->setIcons(icon_server,icon_server);
        }
        else
        {
            ud->setType(TYPE_GROUP);
            ud->setParent(parent);
            ud->setIcons(icon_group_o,icon_group_c);
        }
        
        ud->setCull(arg1);
        ud->setStart(arg2);
        
        // Build up insertion information
        RECORDINSERT ri;   
        memset(&ri,0,sizeof(RECORDINSERT));
        ri.cb = sizeof(RECORDINSERT);
        ri.zOrder = CMA_END;                    // add on top of siblings
        if (!parent || !parent->getCollapsed())
            ri.fInvalidateRecord = TRUE;            // re-arrange after insert
        ri.pRecordParent = (PRECORDCORE)parent; // set tree hierarchy
        ri.cRecordsInsert = 1;                  // number of records to insert
        ri.pRecordOrder = (RECORDCORE*)CMA_END; // add to end of list

//        MRESULT rc = WinSendDlgItemMsg(dialog,id,CM_INSERTRECORD,MPFROMP((PRECORDCORE)ud),MPFROMP(&ri));
        MRESULT rc = WinSendMsg(cont,CM_INSERTRECORD,MPFROMP((PRECORDCORE)ud),MPFROMP(&ri));
        if (rc == 0)
            message("CM_INSERTRECORD returned %d",rc);
        
        return 1;
    }
    else
    {
        DosBeep(100,100);
        return 0;
    }
}


void Container::remove(UserData *it)
{
    // Remove from the container && free
    // If the object is a tree-root, the enitre branch is deleted too
/*
    PRECORDCORE prc = (PRECORDCORE)it;
    
    if (prc->hptrIcon)
        WinDestroyPointer(prc->hptrIcon);
    if (prc->hptrMiniIcon)
        WinDestroyPointer(prc->hptrMiniIcon);
*/
    WinSendMsg(cont,CM_REMOVERECORD,MPFROMP(&it),MPFROM2SHORT(1,CMA_FREE));
    
}

void Container::removeAll(void)
{
    UserData *first;
    
    first = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(NULL),MPFROM2SHORT(CMA_FIRST,CMA_ITEMORDER));
    while(first)
    {
        remove(first);
        first = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(NULL),MPFROM2SHORT(CMA_FIRST,CMA_ITEMORDER));
    }
}


void Container::arrange(void)
{
    UserData *ud = findSelectedRecord(NULL);
    WinSendMsg(cont,CM_INVALIDATERECORD,MPFROMP(NULL),MPFROM2SHORT(0,0));
    if (ud != NULL)
        WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)ud),MPFROM2SHORT(TRUE,CRA_SELECTED));
}

void Container::arrangeText(UserData *it)
{
    UserData *ud = findSelectedRecord(NULL);
    WinSendMsg(cont,CM_INVALIDATERECORD,MPFROMP(&it),MPFROM2SHORT(1,CMA_TEXTCHANGED));
    if (ud != NULL)
        WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)ud),MPFROM2SHORT(TRUE,CRA_SELECTED));
}

void Container::arrange(UserData *it)
{
    UserData *ud = findSelectedRecord(NULL);
    WinSendMsg(cont,CM_INVALIDATERECORD,MPFROMP(&it),MPFROM2SHORT(1,CMA_REPOSITION));
    if (ud != NULL)
        WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)ud),MPFROM2SHORT(TRUE,CRA_SELECTED));
}


void Container::setItemFocus(UserData *it)
{
    UserData *ud = findSelectedRecord(NULL);
    if (ud != NULL)
        WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)ud),MPFROM2SHORT(FALSE,CRA_SELECTED));
    if (it != NULL)
        WinSendMsg(cont,CM_SETRECORDEMPHASIS,MPFROMP((RECORDCORE*)it),MPFROM2SHORT(TRUE,CRA_SELECTED));
}



UserData *Container::allocateRecords(short num)
{
    UserData *ud;
    PVOID     pv;
    LONG      additional_space = (LONG)(sizeof(UserData)-sizeof(RECORDCORE));
    
    pv = WinSendMsg(cont,CM_ALLOCRECORD,MPFROMLONG(additional_space),MPFROMSHORT((short)num));
    if (pv == NULL)
    {
        switch(WinGetLastError(hab))
        {
             case PMERR_INSUFFICIENT_MEMORY:
                 message("PMERR_INSUFFICIENT_MEMORY");
                 break;
             case PMERR_INVALID_PARAMETERS:
                 message("PMERR_INVALID_PARAMETERS");
                 break;
            // default:
                 // message("Unknown error in Container Allocation");
        }
        ud = NULL;
    }
    else
    {
        ud = (UserData*)PVOIDFROMMR(pv);
    }
                                
    return ud;
}

UserData *Container::findSelectedRecord(UserData *after_this)
{
    if (after_this == NULL)
        after_this = (UserData*)(PRECORDCORE)CMA_FIRST;
    
    PVOID pv;
    UserData *ud;
    
    pv = WinSendMsg(cont,CM_QUERYRECORDEMPHASIS,MPFROMP((PRECORDCORE)after_this),MPFROMLONG(CRA_SELECTED));
    ud = (UserData*)PVOIDFROMMR(pv);
    
    return ud;
}


void Container::setTextOnlyMode(void)
{
    
    CNRINFO ccinfo;
    memset (&ccinfo, 0, sizeof ccinfo);
    ccinfo.cb = sizeof(CNRINFO);
    ccinfo.flWindowAttr = CV_TEXT;
//    WinSendMsg (WinWindowFromID(dialog,id),CM_SETCNRINFO, &ccinfo,MPFROMLONG(CMA_FLWINDOWATTR));
    WinSendMsg (cont,CM_SETCNRINFO, &ccinfo,MPFROMLONG(CMA_FLWINDOWATTR));
//    WinInvalidateRect (WinWindowFromID(dialog,id), 0, 0);
    WinInvalidateRect (cont, 0, 0);
    
}

int Container::insertFileList(FileList *fl,UserData *parent)
{
    UserData *ud;
    UserData *first;
    int i,num_items;
    HPOINTER the_icon;
    
    num_items = fl->getNumFiles();
    
    // Build up insertion information
    RECORDINSERT ri;   
    memset(&ri,0,sizeof(RECORDINSERT));
    ri.cb = sizeof(RECORDINSERT);
    ri.zOrder = CMA_END;                    // add on top of siblings
//    ri.fInvalidateRecord = TRUE;            // re-arrange after insert
    ri.pRecordParent = (PRECORDCORE)parent; // set tree hierarchy
    ri.pRecordOrder = (RECORDCORE*)CMA_END; // add to end of list
    
    
    if (num_items > 0)
    {
        ri.cRecordsInsert = num_items;          // number of records to insert
        first = allocateRecords(num_items);
        ud = first;
        if (ud)
        {
            for (i=0; i<num_items; i++)
            {
                num_objects++;
                ud->setName(fl->getFile(i));
                the_icon = determineIcon(fl->getFile(i));
                ud->setIcons(the_icon,the_icon);
                ud->setType(TYPE_FILE);
                ud->setParent(parent);
// NEW STUFF
/*
                    char *kkk = new char[4096];
                    strcpy(kkk,fl->getBasePath());
                    strcat(kkk,fl->getFile(i));
                    
                    the_icon = WinLoadFileIcon(kkk,FALSE);
                    if (the_icon != NULL)
                    {
                        ud->setIcons(the_icon,the_icon);
                        WinSendMsg(cont,CM_INVALIDATERECORD,MPFROMP(&ud),MPFROM2SHORT(1,CMA_TEXTCHANGED|CMA_REPOSITION));
                    }
                    delete[] kkk;
*/
// ENDE NEW STUFF
                    
                PRECORDCORE pr = (PRECORDCORE)ud;
                ud = (UserData*)pr->preccNextRecord;
            }
            
            MRESULT rc = WinSendMsg(cont,CM_INSERTRECORD,MPFROMP((PRECORDCORE)first),MPFROMP(&ri));
            return 1;
        }
    }
    return 0;
}




int Container::insertFile(FILE *in,int num_items,int *so_far,int *stopping)
{
    UserData *ud;
    UserData *first;
    char buffer[1024];
    int i;
    int STEP = 50;
    PRECORDCORE pr;
    
    // monitored by external thread
    *so_far = 0;
    
    // Build up insertion information
    RECORDINSERT ri;   
    memset(&ri,0,sizeof(RECORDINSERT));
    ri.cb = sizeof(RECORDINSERT);
    ri.zOrder = CMA_END;                    // add on top of siblings
    ri.fInvalidateRecord = TRUE;            // re-arrange after insert
    ri.pRecordParent = (PRECORDCORE)NULL;   // set tree hierarchy
    ri.pRecordOrder = (RECORDCORE*)CMA_END; // add to end of list
    
    while (!(*stopping) && num_items > STEP)
    {
        first = allocateRecords(STEP);
        ud = first;
        if (ud)
        {
            for (i=0; i<STEP; i++)
            {
//                objects[num_objects] = ud;  // keep a pointer to the block
                num_objects++;
//                fgets(buffer,1024,in);
//                trimCR(buffer);
//                ud->setName(buffer);
                ud->loadName(in);
//                ud->setType(TYPE_FILE);
//                ud->setIcons(WinLoadPointer(HWND_DESKTOP,0,ICON_GROUP_O),
//                             WinLoadPointer(HWND_DESKTOP,0,ICON_GROUP_C));
                pr = (PRECORDCORE)ud;
                ud = (UserData*)pr->preccNextRecord;
            }
            
            ri.cRecordsInsert = STEP;          // number of records to insert
//            MRESULT rc = WinSendDlgItemMsg(dialog,id,CM_INSERTRECORD,MPFROMP((PRECORDCORE)first),MPFROMP(&ri));
            MRESULT rc = WinSendMsg(cont,CM_INSERTRECORD,MPFROMP((PRECORDCORE)first),MPFROMP(&ri));
            if (rc == 0)
                message("CM_INSERTRECORD returned %d",rc);
        }
        num_items -= STEP;
        *so_far += STEP;
        if (STEP < 1000)
            STEP += 50;
        
//        arrange();
    }
    
    if (!(*stopping) && num_items > 0)
    {
        ri.cRecordsInsert = num_items;          // number of records to insert
        first = allocateRecords(num_items);
        ud = first;
        if (ud)
        {
            for (i=0; i<num_items; i++)
            {
//                objects[num_objects] = ud;  // keep a pointer to the block
                num_objects++;
//                fgets(buffer,1024,in);
//                trimCR(buffer);
//                ud->setName(buffer);
                ud->loadName(in);
//                ud->setType(TYPE_FILE);
//                ud->setIcons(WinLoadPointer(HWND_DESKTOP,0,ICON_GROUP_O),
//                             WinLoadPointer(HWND_DESKTOP,0,ICON_GROUP_C));
                pr = (PRECORDCORE)ud;
                ud = (UserData*)pr->preccNextRecord;
                
            }
            *so_far += num_items;
            
//            MRESULT rc = WinSendDlgItemMsg(dialog,id,CM_INSERTRECORD,MPFROMP((PRECORDCORE)first),MPFROMP(&ri));
            MRESULT rc = WinSendMsg(cont,CM_INSERTRECORD,MPFROMP((PRECORDCORE)first),MPFROMP(&ri));
            if (rc == 0)
                message("CM_INSERTRECORD returned %d",rc);
            
            return 1;
        }
    }
    
//    arrange();
    return 0;
}

UserData *Container::reInsert(UserData *ud,HPOINTER icon)
{
    PRECORDCORE parent;
    
    parent = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(ud),MPFROM2SHORT(CMA_PREV,CMA_ITEMORDER));
    
    if (parent == NULL)
        parent = (PRECORDCORE)CMA_FIRST;
    
    // make a backup of our object
    UserData keep;
    memcpy(&keep,ud,sizeof(UserData));

    // remove AND delete (takes out any kids too!)
    WinSendMsg(cont,CM_REMOVERECORD,MPFROMP(&ud),MPFROM2SHORT(1,CMA_FREE));
    
    ud = allocateRecords(1);
    ud->setName(keep.getName());
    ud->setSkipped(keep.getSkipped());
    ud->setCollapsed(keep.getCollapsed());
    ud->setCull(keep.getCull());
    ud->setStart(keep.getStart());
    ud->setType(keep.getType());
    ud->setParent(keep.getParent());
    ud->setIcons(icon,icon);
    
    RECORDINSERT ri;   
    memset(&ri,0,sizeof(RECORDINSERT));
    ri.cb = sizeof(RECORDINSERT);
    if (!keep.getParent() || !keep.getParent()->getCollapsed())
        ri.fInvalidateRecord = TRUE;
    ri.pRecordParent = (PRECORDCORE)ud->getParent();
    ri.pRecordOrder = (RECORDCORE*)parent;
    ri.cRecordsInsert = 1;                  // number of records to insert
    
    // re-insert
    WinSendMsg(cont,CM_INSERTRECORD,MPFROMP((PRECORDCORE)ud),MPFROMP(&ri));
    setItemFocus(ud);
    
    return ud;
}

    
// given an item, move it up in the container
void Container::moveUp(UserData *ud)
{
    PRECORDCORE parent,parents_parent;
    array<PRECORDCORE> children;
    int num_children=0;
    
    parent = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(ud),MPFROM2SHORT(CMA_PREV,CMA_ITEMORDER));
    if (parent)
    {
        // if there is any children associated with this record we must save them
        children[0] = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(ud),MPFROM2SHORT(CMA_FIRSTCHILD,CMA_ITEMORDER));
        if (children[0] != NULL)
        {
            num_children = 1;
            PRECORDCORE next;
            do
            {
                next = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(children[num_children-1]),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
                if (next != NULL)
                {
                    children[num_children] = next;
                    num_children++;
                }
                
            } while (next != NULL);
        }
        
        parents_parent = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(parent),MPFROM2SHORT(CMA_PREV,CMA_ITEMORDER));
    
        if (parents_parent == NULL)
            parents_parent = (PRECORDCORE)CMA_FIRST;
    
    
        // remove but don't delete
        WinSendMsg(cont,CM_REMOVERECORD,MPFROMP(&ud),MPFROM2SHORT(1,0));
        
        RECORDINSERT ri;   
        memset(&ri,0,sizeof(RECORDINSERT));
        ri.cb = sizeof(RECORDINSERT);
        if (!ud->getParent() || !ud->getParent()->getCollapsed())
            ri.fInvalidateRecord = FALSE; // TRUE;
        ri.pRecordParent = (PRECORDCORE)ud->getParent();
        ri.pRecordOrder = (RECORDCORE*)parents_parent;
        ri.cRecordsInsert = 1;                  // number of records to insert
        
        // re-insert
        WinSendMsg(cont,CM_INSERTRECORD,MPFROMP((PRECORDCORE)ud),MPFROMP(&ri));
        if (num_children > 0)
        {
            ri.pRecordParent = (PRECORDCORE)ud;
            ri.pRecordOrder = (RECORDCORE*)CMA_END;
            for (int i=0; i<num_children; i++)
            {
                WinSendMsg(cont,CM_INSERTRECORD,MPFROMP(children[i]),MPFROMP(&ri));
            }
        }
        setItemFocus(ud);
    }
}



// given an item, move down up in the container
void Container::moveDown(UserData *ud)
{
    PRECORDCORE child;
    array<PRECORDCORE> children;
    int num_children=0;
    
    // if there is any children associated with this record we must save them
    children[0] = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(ud),MPFROM2SHORT(CMA_FIRSTCHILD,CMA_ITEMORDER));
    if (children[0] != NULL)
    {
        num_children = 1;
        PRECORDCORE next;
        do
        {
            next = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(children[num_children-1]),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
            if (next != NULL)
            {
                children[num_children] = next;
                num_children++;
            }
            
        } while (next != NULL);
    }
    
    // remove the main record
    child = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(ud),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
    if (child == NULL)
        child = (PRECORDCORE)CMA_END;

    // remove but don't delete
    WinSendMsg(cont,CM_REMOVERECORD,MPFROMP(&ud),MPFROM2SHORT(1,0));
    
    RECORDINSERT ri;   
    memset(&ri,0,sizeof(RECORDINSERT));
    ri.cb = sizeof(RECORDINSERT);
    if (!ud->getParent() || !ud->getParent()->getCollapsed())
        ri.fInvalidateRecord = TRUE;
    ri.pRecordParent = (PRECORDCORE)ud->getParent();
    ri.pRecordOrder = (RECORDCORE*)child;
    ri.cRecordsInsert = 1;                  // number of records to insert
    
    // re-insert
    WinSendMsg(cont,CM_INSERTRECORD,MPFROMP((PRECORDCORE)ud),MPFROMP(&ri));
    
    if (num_children > 0)
    {
        ri.pRecordParent = (PRECORDCORE)ud;
        ri.pRecordOrder = (RECORDCORE*)CMA_END;
        for (int i=0; i<num_children; i++)
        {
            WinSendMsg(cont,CM_INSERTRECORD,MPFROMP(children[i]),MPFROMP(&ri));
        }
    }

    
    setItemFocus(ud);
}

void Container::toggleNoIcons(void)
{
    
    /*
    memset(&cnrinfo,0,sizeof(CNRINFO));
    cnrinfo.cb = sizeof(CNRINFO);
    cnrinfo.flWindowAttr = CV_ICON | CV_TREE| CA_TREELINE;
    cnrinfo.cxTreeLine = 1;
    
    
    
    cont = WinWindowFromID(dlg,iD);
    
    WinSendMsg(cont,CM_SETCNRINFO,MPFROMP(&cnrinfo),MPFROMLONG(CMA_SLBITMAPORICON|CMA_SLTREEBITMAPORICON|CMA_FLWINDOWATTR));
    */
    
    CNRINFO cnrinfo;

    WinSendMsg(cont,CM_QUERYCNRINFO,MPFROMP(&cnrinfo),MPFROMSHORT(sizeof(CNRINFO)));
    
    if (!no_icons)
        cnrinfo.flWindowAttr |= CV_ICON;
    else
        cnrinfo.flWindowAttr &= (~CV_ICON);
        
    no_icons = !mini_icons;
               
    WinSendMsg(cont,CM_SETCNRINFO,MPFROMP(&cnrinfo),MPFROMLONG(CMA_SLBITMAPORICON|CMA_FLWINDOWATTR|CMA_SLTREEBITMAPORICON));
}



void Container::toggleSmallIcons(void)
{
    
    CNRINFO cnrinfo;

    WinSendMsg(cont,CM_QUERYCNRINFO,MPFROMP(&cnrinfo),MPFROMSHORT(sizeof(CNRINFO)));
    
    if (!mini_icons)
        cnrinfo.flWindowAttr |= CV_MINI;
    else
        cnrinfo.flWindowAttr &= (~CV_MINI);
        
    mini_icons = !mini_icons;
               
    WinSendMsg(cont,CM_SETCNRINFO,MPFROMP(&cnrinfo),MPFROMLONG(CMA_FLWINDOWATTR|CMA_SLBITMAPORICON));
}

int Container::getNumServers(void)
{
    int tally= 0;
    
//    for (int i=0; i<num_objects; i++)
//    {
//        if (objects[i]->isServer())
//            tally++;
//    }
//    return tally;
    
    
    UserData *parent;
    UserData *current;
    
    // find the first server
    parent = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(NULL),MPFROM2SHORT(CMA_FIRST,CMA_ITEMORDER));
    if (parent == NULL)
        return 0;
    else
        tally = 1;
    
    // find all the other servers
    current = parent;
    do
    {
        current = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(current),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
        if (current != NULL)
        {
            tally++;
        }
    } while(current != NULL);

    return tally;
    
}

void Container::getConfigList(FILE *out)
{
    UserData *parent,*child;
    int num_children;
    char buffer[1024];
    
    parent = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(NULL),MPFROM2SHORT(CMA_FIRST,CMA_ITEMORDER));
    while (parent != NULL)
    {
        
        fprintf(out,"\nServer\n");
        
        if (*(parent->getUsername())!='\0' && *(parent->getPassword())!= '\0')
        {
            fprintf(out,"    Name=%s,%s,%s\n",parent->getName(),
                                              parent->getUsername(),
                                              parent->getPassword());
        }
        else
        {
            fprintf(out,"    Name=%s\n",parent->getName());
        }
        
        
        
        // coutn the number of groups
        num_children = 0;
        child = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(parent),MPFROM2SHORT(CMA_FIRSTCHILD,CMA_ITEMORDER));
        while (child != NULL)
        {
            num_children++;
            child = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(child),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
        }
        
        fprintf(out,"    NumGroups=%d\n",num_children);
        
        child = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(parent),MPFROM2SHORT(CMA_FIRSTCHILD,CMA_ITEMORDER));
        while (child != NULL)
        {
            
            fprintf(out,"    Group=%s,%ld,%ldk,%s,%s\n",
                    child->getName(),
                    child->getStart(),
                    child->getCull(),
                    (child->getCollapsed()?"Collapsed":"Expanded"),
                    (child->getSkipped()?"Skip":"Include")
                  );
            child = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(child),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
        }
        
        // get the next parent
        parent = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(parent),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));

    }
    
}

void Container::collapse(UserData *ud)
{
    WinSendMsg(cont,CM_COLLAPSETREE,MPFROMP(ud),NULL);
}

int Container::getNextServer(UserData **current)
{
    int rc = 0;
    UserData *parent;
    
    if (*current != NULL)  // get the NEXT
    {
        *current = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(*current),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
        if (*current)
            rc = 1;
    }
    else // get the FIRST
    {
        parent = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(NULL),MPFROM2SHORT(CMA_FIRST,CMA_ITEMORDER));
        if (parent != NULL)
        {
            *current = (UserData*)parent;
            rc = 1;
        }
    }

    return rc;
}

int Container::getNextGroup(UserData *parent,UserData **current)
{
    int rc = 0;
    UserData *child;
    
    if (parent != NULL)
    {
        if (*current == NULL) // get the FIRST
        {
            child = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(parent),MPFROM2SHORT(CMA_FIRSTCHILD,CMA_ITEMORDER));
            if (child)
            {
                *current = child;
                rc = 1;
            }
        }
        else  // get the NEXT
        {
            *current = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(*current),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
            if (*current)
                rc = 1;
        }
    }

    return rc;
}

void Container::deleteChildren(UserData *group)
{
    UserData *ud;
    
    do
    {
        ud = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(group),MPFROM2SHORT(CMA_FIRSTCHILD,CMA_ITEMORDER));
        if (ud != NULL)
        {
            remove(ud);
        }
        
    } while (ud != NULL);
}

// if container content-object name does not container the cull string
// then cull it.
void Container::unFilter(void)
{
    RECORDCORE *current;
    RECORDCORE *first = NULL;
    
    // find the first item (top-level)
    
    current = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(NULL),MPFROM2SHORT(CMA_FIRST,CMA_ITEMORDER));
    
    if (num_objects > 0)
        do
        {
            if (current != NULL)
            {
                
                if (current->flRecordAttr & CRA_FILTERED)
                     current->flRecordAttr &= (~CRA_FILTERED);
                
                // if is selected, must un-select it
                if (current->flRecordAttr & CRA_SELECTED)
                {
                    current->flRecordAttr &= (~CRA_SELECTED);
                }
                
                // select the first record
                if (first == NULL)
                {
                    first = current;
                    ((PRECORDCORE)current)->flRecordAttr |= CRA_SELECTED;
                }
                
                // find all the other servers
                current = (PRECORDCORE)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(current),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
                
            }
            
            
        } while(current != NULL);
    
    arrange();

}



// if container content-object name does not container the cull string
// then cull it.
void Container::filter(char *cull_string)
{
    UserData *current;
    UserData *first = NULL;
    
    // find the first item (top-level)
    current = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(NULL),MPFROM2SHORT(CMA_FIRST,CMA_ITEMORDER));
    
    if (num_objects > 0)
        do
        {
            if (current != NULL)
            {
                if (!(((PRECORDCORE)current)->flRecordAttr & CRA_FILTERED))
                {
                    if (strstr(current->getName(),cull_string) == NULL)
                    {
                        ((PRECORDCORE)current)->flRecordAttr |= CRA_FILTERED;
                    }
                    else if (first == NULL)
                    {
                        first = current;
                        emphasis(current);
                        // message("Setting emphasis on %s",current->getName());
                    }
                }
                // find all the other servers
                current = (UserData*)WinSendMsg(cont,CM_QUERYRECORD,MPFROMP(current),MPFROM2SHORT(CMA_NEXT,CMA_ITEMORDER));
            }
            
        } while(current != NULL);
    
}

