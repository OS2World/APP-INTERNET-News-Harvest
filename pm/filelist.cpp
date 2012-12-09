#include "filelist.h"

FileList::~FileList()
{
    for (int i=0; i<count; i++)
    {
        if (list[i] != NULL)
            delete[] (list[i]);
    }
    
    if (base_path != NULL)
        delete[] base_path;
}


FileList::FileList(char *dir)
{
    if (dir)
    {
        // get the base path:
        // IDEA: Base path is always like "c:\tmp\*.xyz"
        // work back from eoln until we hit the first '\' (or '/')
        // that should be the full path.
        base_path = new char[1+strlen(dir)];
        strcpy(base_path,dir);
        char *ptr = base_path + strlen(base_path);
        while (ptr > base_path)
        {
            if (*ptr == '\\' || *ptr == '/')
                break;
            else
            {
                *ptr = '\0';
                ptr--;
            }
        }
        
        // now get the file list
        char *szDir = new char[4096];
        FILEFINDBUF3 ffbFile;
        HDIR hdir;
        int rc;
        ULONG dirlen = 1;
        
        count = 0;
        hdir = HDIR_CREATE;
        
        rc = DosFindFirst(dir,&hdir,0,&ffbFile,sizeof(FILEFINDBUF3),&dirlen,FIL_STANDARD);
        
        if (rc == 0) 
            do
            {
                if (*(ffbFile.achName) != '\0' && strcmp(ffbFile.achName,".") != 0 && strcmp(ffbFile.achName,"..") != 0)
                {
                    list[count] = new char[1+strlen(ffbFile.achName)];
                    strcpy(list[count],ffbFile.achName);
                    sizes[count] = ffbFile.cbFile;
                    count++;
                }
                
                dirlen = 1;
                rc = DosFindNext(hdir,&ffbFile,sizeof(FILEFINDBUF3),&dirlen);
                                           
            } while(rc == 0 && dirlen == 1);
        
        DosFindClose(hdir);
        delete[] szDir;
    }
}
