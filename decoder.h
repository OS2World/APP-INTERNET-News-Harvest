
/***********************************************************************

decoder.h,v 1.3 2003/12/16 03:49:59 root Exp

Decoder definitions

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2003 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	xx Xxx 02 KRT - added yEnc support

***********************************************************************/

#ifndef __DECODER_H__
#define __DECODER_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#ifdef UNIX
#include <unistd.h>
#endif

#ifndef UNIX
#include <new.h>
#endif

#include "filestat.h"
#include "array.h"
#include "item.h"
#include "binarytree.h"
#include "header.h"
#include "mime.h"
#include "charqueue.h"
#include "unlink.h"
#include "log.h"
#include "strutils.h"

// Encoding Types
// enum { UU, XX, BASE64, YENC, UNKNOWN } EncodingTypes;
#define UU      0
#define XX      1
#define BASE64	2
#define YENC    3
#define UNKNOWN	4

// Encoding Errors
// enum { OK, UNKNOWN_TYPE, NO_FILENAME, MALFORMED, EMPTY } EncodingErrors;
#define OK		0
#define UNKNOWN_TYPE	1
#define NO_FILENAME	2
#define MALFORMED	3
#define EMPTY		4

// Winged File Types
// enum { DUNNO, JPEG, GIF, MIDI, ZIP } WingedFileTypes;
#define DUNNO   0
#define JPEG    1
#define GIF	    2
#define MIDI    3
#define ZIP     4

// Actions
// enum { NONE, DECODED, SKIPPED } Actions;
#define NONE 	0
#define DECODED 1
#define SKIPPED	2


class Decoder
{
public:
    Decoder();
    Decoder(char *out_path);
   ~Decoder();
    
    int decode(char *block);
    void  setOutputPath(char *);
    void  setOutputPostProcessor(char *);
    
    int getItemsProcessed() { return items_processed; }
    FileStat *getFileStat(int i) { return &(files[i]); }
    void validateHistory();
    
    void saveTextAttachments(int toggle) { save_text = toggle; }
    int checkNameSeenBefore(char *fileName);
    
private:
    unsigned long UUXXDecode(FILE *out,char **block,int encoding_type);
    int   UUXXSkip(char **block);
    char  UULookup(char x)
    {
        char rc = 0;
        if (x >= '!' && x <= '_') rc = (char)(x-32);
        return rc;
    }

    char  XXLookup(char x);
    unsigned long B64Decode(FILE *out,char **block);
    int   B64Skip(char **block);
    int   B64WingIt(char *group,char **block);
    int   isValidB64(char x);
    char  B64Lookup(char x);
	unsigned long YEncDecode(FILE *out,char **block);
    char *getEncodingName(int encoding_type);
    char *nextLine(char *ptr);
    char *extractLine(char **strblock);
    char *getDecodeFilename(char *string_block,int encoding_type);
    int   determineType(char *string_block);
    int   anyIn(char *in_this,char *of_these);
    void  loadDecodes(void);
    void  addToDecodes(Item *i);
    char *checkMangle(char *path);
    int   checkFullDisk(void);
    int   deleteIfSeenBefore(Item *itm,char *filenamepath);
    char *findBoundary(char *string_block);
    char *findExtension(char *path);
    char *buildFullPath(char *filename,char *output_path);
    int   write(char *filename,int size,char *block);
	char *strrstr(char *haystack,char *needle);


   
    char *output_path;
    char *exit_exe;
    array<FileStat> files;
    int items_processed;
    BinaryTree<Item> decodes;
    array<Item *>history;
    int hist_size;
    int save_text;
    

};


#endif //#ifndef __DECODER_H__
