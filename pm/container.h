/* container.h,v 1.2 2001/06/20 00:42:40 root Exp */

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#define INCL_WINWORKPLACE
#define INCL_WINPOINTERS
#define INCL_WININPUT
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS
#define INCL_WINMENUS
#define INCL_WINSTDCNR
#define INCL_WINDIALOGS
#define INCL_WINSYS
#define INCL_GPILCIDS
#define INCL_GPIBITMAPS
#define INCL_DOSMEMMGR
#define INCL_WINERRORS
#define INCL_SHLERRORS
#define INCL_GPIERRORS


#include <stdio.h>
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef UNIX
#include <new.h>
#endif

extern HAB hab;
void message(char *format, ...);

#include "array.h"
#include "icons.h"
#include "strutils.h"
#include "filelist.h"

#define KtoLINES(x) ((x*1024)/60*4/3)

#ifdef SLAINC // SHL 12 Jul 00
typedef enum { TYPE_SERVER=1, TYPE_GROUP, TYPE_FILE } ContainerElements;
#else
enum { TYPE_SERVER=1, TYPE_GROUP, TYPE_FILE } ContainerElements;
#endif

class UserData
{
public:
    UserData() { memset(this,0,sizeof(UserData)); }
    static int compare(UserData *ud1,UserData *ud2,PVOID ignored) { return strcmp((ud1?ud1->name:""),(ud2?ud2->name:"")); }
    
    void setIcons(HPOINTER c,HPOINTER o);
    void setMiniIcon(HPOINTER i) { core.hptrMiniIcon=i; }
    char *getName(void) { return name; }
    char *getUsername(void) { return username; }
    char *getPassword(void) { return password; }
    int  isServer() { return (element_type == TYPE_SERVER); }
    int  isGroup() { return (element_type == TYPE_GROUP); }
    int  isPlain() { return (element_type == TYPE_FILE); }
    void setType(int t) { element_type = t; }
    int  getType() { return element_type; }
    void setCull(long c) { cull = c; }
    long getCull(void) { return cull; }
    long getCullLines(void) { return KtoLINES(cull); }
    void setStart(long c) { start = c; }
    long getStart(void) { return start; }
    int getSkipped() { return skip == 1; }
    void setSkipped(int x) { skip = x; }
    int getCollapsed() { return (core.flRecordAttr & CRA_COLLAPSED); }
    void setCollapsed(int x) { 
        if (x) core.flRecordAttr |= CRA_COLLAPSED; 
        else 
        { 
            if (getCollapsed()) core.flRecordAttr &= (~CRA_COLLAPSED); 
        } 
    }
    
    void setName(char *s) { strncpy(name,(s?s:"null"),255); core.pszIcon=core.pszText=core.pszName=core.pszTree = name; core.flRecordAttr=CRA_EXPANDED|CRA_SELECTED; }
    void setUsername(char *s) { if (s) strncpy(username,s,32); }
    void setPassword(char *s) { if (s) strncpy(password,s,32); }

    
    UserData *getParent(void) { return parent; }
    void setParent(UserData *p) { parent = p; }
    void loadName(FILE *in) { fgets(name,255,in); trimCR(name); core.pszIcon=core.pszText=core.pszName=core.pszTree = name; core.flRecordAttr=CRA_EXPANDED|CRA_SELECTED; }
    
private:
    RECORDCORE   core;       // Control info for object
    TREEITEMDESC tree_icons; // Expanded & Collapsed icon handles
    char name[256];
    char username[32];
    char password[32];
    int  element_type;
    UserData *parent;
    long cull;
    long start;
    int collapsed,skip;
};

class Container
{
public:
    Container();
   ~Container();
    
    void Container::initialise(HWND dlg,ULONG iD);
    int insert(char *name,ULONG arg1,ULONG arg2);
//    int insert(char *name,char *parent_name,ULONG arg1,ULONG arg2);
    int insert(char *name,UserData *parent,ULONG arg1,ULONG arg2);
    int insert(char *name,char *userid,char *pass,ULONG arg1,ULONG arg2);
    UserData *allocateRecords(short num);
    UserData *findSelectedRecord(UserData *afterthis);
    void arrange(void);
    void arrange(UserData *it);
    void arrangeText(UserData *it);
    void remove(UserData *it);
    void removeAll(void);
    void setTextOnlyMode(void);
    int  insertFile(FILE *in,int num_items,int *so_far,int *stopping);
    int  insertFileList(FileList *fl,UserData *group);
    HPOINTER determineIcon(char *filename);
    void emphasis(UserData *it);
    void noEmphasis(UserData *it);
    void selected(UserData *it);
    void noSelected(UserData *it);
    void inUse(UserData *it);
    void noInUse(UserData *it);
    void moveUp(UserData *it);
    void moveDown(UserData *it);
    UserData *reInsert(UserData *ud,HPOINTER icon);
    void setItemFocus(UserData *it);
    void toggleSmallIcons(void);
    void getConfigList(FILE*);
    int  getNumServers(void);
    UserData *getLastInsert(void) { return last_insert; }
    void deleteChildren(UserData *group);
    void filter(char *cull_string);
    void unFilter(void);
    UserData *setIcon(UserData *ud,int icon_id);
    void toggleNoIcons(void);
    
    // Processing Services
    int getNextServer(UserData **current);
    int getNextGroup(UserData *server,UserData **current);
    
    void setDelete(int x) { no_delete = x; }
    void collapse(UserData *ud);
    
private:
    
    HWND             dialog;      // The owner window for the container control
    ULONG            id;          // The container control id
    HWND             cont;        // the control
//    array<UserData*> objects;     // things stored in the container
    int              num_objects; // number of objects
    int              mini_icons;  // display with small icons?
    int              no_icons;    // display with no icons?
    UserData*        last_insert; // last object inserted
    int              no_delete;   // delete the content of the container on destruct?
    
    HPOINTER icon_avi;
    HPOINTER icon_au;
    HPOINTER icon_bmp;
    HPOINTER icon_fli;
    HPOINTER icon_gif;
    HPOINTER icon_jpeg;
    HPOINTER icon_midi;
    HPOINTER icon_mp3;
    HPOINTER icon_mpeg;
    HPOINTER icon_pcx;
    HPOINTER icon_tga;
    HPOINTER icon_tiff;
    HPOINTER icon_wav;
    HPOINTER icon_html;
    HPOINTER icon_png;
    HPOINTER icon_dunno;
    HPOINTER icon_host_c;
    HPOINTER icon_host_o;
    HPOINTER icon_group_c;
    HPOINTER icon_group_o;
    HPOINTER icon_server;

    
};

#endif //#ifndef __CONTAINER_H__
