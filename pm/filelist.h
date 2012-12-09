#ifndef __FILELIST_H__
#define __FILELIST_H__

#define INCL_DOSFILEMGR
#define INCL_WIN
#include <os2.h>
#include <string.h>
#include <stdlib.h>

#ifndef UNIX
#include <new.h>
#endif


#include "array.h"

class FileList
{
public:
    FileList(char *dir);
   ~FileList();
    
    int getNumFiles() { return count; }
    char *getFile(int index) { if (index < 0 || index >= count) return NULL; else return list[index]; }
    unsigned long getFileSize(int index) { if (index < 0 || index >= count) return 0; else return sizes[index]; }
    char *getBasePath() { return base_path; }
    
    
private:
    array<char *> list;
    array<unsigned long> sizes;
    char *base_path;
    int count;
    
    void doFetch();
    
};



#endif // #ifndef __FILELIST_H__

