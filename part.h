#ifndef __PART_H__
#define __PART_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef UNIX
#include <new.h>
#endif

class Part
{
public:
    Part() { init(); }
    Part(char *str,char *filename,int num,int o);
   ~Part(); 
   
   char *getName();
   char *getIdString();
   char *getBuffer();
   char *getFilename();
   int   getNumber();
   int   getTotal();
   time_t getTimestamp() { return timestamp; }
   
   int read(FILE *in);

   
private:         
    void init() { number=of=0; buffer=name=filename=string_bit=NULL; }
    void clear() 
    { 
        if (buffer) delete[] buffer; 
        if (name) delete[] name; 
        if (filename) delete[] filename;
        if (string_bit) delete[] string_bit;
        Part::init(); 
    }
    
    int number,of;
    char *buffer;
    char *name;
    char *filename;
    char *string_bit;
    time_t timestamp;
};      

typedef  Part* PartPtr;

#endif

