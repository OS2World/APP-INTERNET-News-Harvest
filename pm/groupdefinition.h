#ifndef __GROUPDEF_H__
#define __GROUPDEF_H__

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
#endif // #ifdef OS2PM

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#ifndef UNIX
#include <new.h>
#endif


#include "array.h"


class GroupDefinition
{
public:                 
    // data => alt.binaries.models.scale,0,10,Collapsed,Include

    GroupDefinition(char *data) 
    { 
        memset(this,0,sizeof(GroupDefinition)); 
        char *ptr = strchr(data,',');
        if (ptr)
        {
            *ptr='\0';
            strncpy(name,data,255);    // group name
            data = ptr+1;
            ptr = strchr(data,',');
            if (ptr)
            {
                *ptr='\0';
                last_record = atol(data);  // last record downloaded
                data = ptr+1;
                ptr = strchr(data,',');
                if (ptr)
                {
                    *ptr = '\0';
                    line_cull = atol(data);   // min line cull
                    data = ptr+1;
                    
                    ptr = strchr(data,',');
                    if (ptr)
                    {
                        *ptr = '\0';   // Collapsed/Expanded
                        if (*data == 'C' || *data == 'c')
                            collapsed = 1;
                        else
                            collapsed = 0;
                        
                        data = ptr+1;
                        if (*data == 'S' || *data == 's')
                            skip = 1;
                        else
                            skip = 0;

                    }
                }
            }
        }
    }
    void setName(char *n) { strncpy(name,(n?n:"(empty)"),255); }
    char *getName() { return name; }                        
    
    long getLastRecord() { return last_record; }
    void setLastRecord(long r) { last_record = r; }
    
    long getLineCull() { return line_cull; }
    void setLineCull(long l) { line_cull = l; }
    
    int getSkipped() { return skip == 1; }
    void setSkipped(int x) { skip = x; }
    
    int getCollapsed() { return collapsed == 1; }
    void setCollapsed(int x) { collapsed = x; }
    
private:
    char name[256];
    long last_record;
    long line_cull;
    int skip;
    int collapsed;

};


                                   

#endif //#define __GROUPDEF_H__



