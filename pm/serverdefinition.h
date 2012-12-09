#ifndef __SERVERDEF_H__
#define __SERVERDEF_H__

#ifdef OS2PM
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
#include <os2.h>
#endif //#ifdef OS2PM

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#ifndef UNIX
#include <new.h>
#endif


#include "../array.h"
#include "groupdefinition.h"


#ifdef OS2PM
extern HAB hab;
#endif //#ifdef OS2PM

void message(char *format, ...);

class ServerDefinition
{ 
public:
    ServerDefinition() { memset(name,0,sizeof(name)); num_groups=0;}
    void setName(char *n);
    char *getName() { return name; }
    char *getUsername() { return username; }
    char *getPassword() { return password; }
    
    void setNumGroups(unsigned long g) { num_groups = g; }
    unsigned long getNumGroups(void) { return num_groups; }
    
    GroupDefinition *getGroup(int g) { if (g < num_groups) return groups[g]; else return NULL; }
    void addGroupDefinition(GroupDefinition *gd) { groups[num_groups] = gd; num_groups++; }
    
private:             
    array<GroupDefinition *>groups;
    int num_groups;
    char name[256];
    char username[32];
    char password[32];
};                  




#endif //#define __SERVERDEF_H__



