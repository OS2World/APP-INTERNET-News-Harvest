#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifndef UNIX
#include <new.h>
#endif

#include "array.h"
#include "log.h"

#define MIME_UNKNOWN 0
#define MIME_7BIT    1
#define MIME_8BIT    2
#define MIME_BASE64  3
#define MIME_BINARY  4
#define MIME_QUOTED  5
#define MIME_UU      6
#define MIME_BINHEX  7

#define MIME_EOLN "\r\n"
#define BASE_FILENAME  "NoFileName"

void cleanFilename(char *filename);

class Mime
{
public:
    Mime();
   ~Mime();
    
   int  preScan(char *block);
    
    int   getDataSize(int index) 
    { 
        //        return data_size[index]; 
        int rc = data_size[index];
        return rc;
        
    }
   char *getEncodingMethod(int index) { return encoding[index]; }
   char *getDataBlock(int index) { return data[index]; }
   char *getFilename(int index) { return filename[index]; }
   char *getContentName(int index) { return content[index]; }
   int   getContentType(int index) { return determineType(index); }
   int   getNumSections(void) { return num_parts; }
    
    
private:
    int  num_parts;
    int  num_boundary;
    array<char*> boundary;
    array<int>   data_size;
    array<char*> encoding;
    array<char*> data;
    array<char*> filename;
    array<char*> content;
    
    int  countBoundaries(char *block);
    void findBoundary(char *block);
    void deQuote(char *s);
    void scan(int index,char *block);
    int  determineType(int index);
    void B64Decode(int index);
    void UUDecode(int index);
    void BinHexDecode(int index);
    void cleanFilename(int index);
    int write(char *filename,int size,char *data);
    
    char *nextValidB64Code(char *p,int &code);
    char *nextValidUUCode(char *p,int &code);
    
    char *nextEoln(char *block);
    char *nextLine(char *block);
    char *nextSemi(char *block);
    char *nextBoundary(char *block);
    char *dup(char *s);
    int   B64Lookup(char ch);
    int   UULookup(char ch);
        
    
};
