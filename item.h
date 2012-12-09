#ifndef __ITEM_H__
#define __ITEM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>

#ifndef UNIX
#include <new.h>
#endif

#include "log.h"
#include "strutils.h"

#define SMUDGE(x) ((x/1024)*1024)

class Item
{
public:
    Item(unsigned long s,char *f);
   ~Item(void);
    void print();
   
    void validate();
   
    static int compare(Item foo,Item bar);
    static int compare(Item *foo,Item *bar);
   
    char *getFilename();
    unsigned long getSize();
    char *getTimeStamp();
   
    void setSize(unsigned long s);
    
//private:
    unsigned long size;
    char *filename;
};


void printit(Item *);


#endif
