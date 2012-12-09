#ifndef __MIMESET_H__
#define __MIMESET_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef UNIX
#include <new.h>
#endif


#include "array.h"
#include "strutils.h"


class MimeSet
{
public:
    MimeSet() { filename = NULL; num_items = 0; }
    
    void setFilename(char *f) { filename = f; }
    
    int  load();
    int  reload();
    int  save();
    void clear();
    void set(char *name,char *extensions,char *command);
    char *getCommand(char *extensions);
    
    int   getSetNum() { return num_items; }
    
    char *getName(int i);
    char *getExtensions(int i);
    char *getCommand(int i);
    
    void setName(int i,char *s)       { if (i < num_items){ lrSpaceTrim(s); if (strlen(name[i]) > strlen(s)) strcpy(name[i],s); else { delete[] name[i]; name[i]=new char[1+strlen(s)]; strcpy(name[i],s);}}}
    void setExtensions(int i,char *s) { if (i < num_items){ lrSpaceTrim(s); makeUpperCase(s); if (strlen(extensions[i]) > strlen(s)) strcpy(extensions[i],s); else { delete[] extensions[i]; extensions[i]=new char[1+strlen(s)]; strcpy(extensions[i],s);}}}
    void setCommand(int i,char *s)    { if (i < num_items){ lrSpaceTrim(s); if (strlen(command[i]) > strlen(s)) strcpy(command[i],s); else { delete[] command[i]; command[i]=new char[1+strlen(s)]; strcpy(command[i],s);}}}
    int  find(char *ext);
    int  getIndexByName(char *nomen);
    
    char *MimeSet::determineType(char *filename);
    
private:
    char *filename;
    int num_items;
    
    array<char *> name;
    array<char *> extensions;
    array<char *> command;
    
    
};


#endif //#ifndef __MIMESET_H__


