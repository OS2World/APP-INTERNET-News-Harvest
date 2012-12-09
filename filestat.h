#ifndef __FILESTAT_H__
#define __FILESTAT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef UNIX
#include <new.h>
#endif

#include "log.h"

class FileStat
{
public:
    FileStat(void);
   ~FileStat(void);
    int getError(void);
    unsigned long getSize(void);
    char *getExtension(void);
    char *getName(void);
    char *getFullPath(void);
    int   getEncodingMethod(void);
    int   getAction(void);
    
    void setError(int);
    void setSize(unsigned long);
    void setExtension(char *);
    void setName(char *);
    void setFullPath(char *);
    void setEncodingMethod(int);
    void setAction(int);
    
    int hasError(void);
    int hasEncodingMethod(void);
    int hasExtension(void);
    int hasName(void);
    int hasFullPath(void);
    int hasAction(void);
    
    void clear(void);
    void print(void);
    
    int getPartNumber(void);
    int getPartTotal(void);
    int hasPartNum(void);
    int hasPartTotal(void);
    void setPart(int p,int o);
    
    
    FileStat &operator= (FileStat &f)
    {
        error_code = f.error_code;
        size = f.size;
        encoding = f.encoding;
        action = f.action;
        setName(f.name);
        setExtension(f.extension);
        setFullPath(f.full_path);
        part = f.part;  of = f.of;
        return *this;
    }
    
private:
    int error_code;
    unsigned long size;
    char *extension;
    char *name;
    char *full_path;
    int   encoding;
    int   action;
    int   part,of;
    
    void init(void);
};

#endif //#ifndef __FILESTAT_H__
