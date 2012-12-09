#ifndef __MULTIPART_H__
#define __MULTIPART_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

#ifdef OS2
#include <direct.h>
#define MKDIR(x) mkdir(x)
#endif

#ifdef UNIX
#include <sys/types.h>
#include <sys/stat.h>   // for unix mkdir()
#define MKDIR(x) mkdir((x),S_IRWXU)
#endif

#ifdef WIN32
#include <direct.h>
#include "win32\stdafx.h"
#define MKDIR(x) _mkdir(x)
#endif


#include "array.h"
#include "newsserver.h"
#include "newsgroup.h"
#include "decoder.h"
#include "header.h"
#include "filestat.h"
#ifdef OS2
#include "filelist.h"
#endif
#include "part.h"


// number of days to hold part articles
//extern int NUM_DAYS;


//extern char *part_output_path;
//extern char *base_output_path;
//extern int   multi_part;
//extern unsigned long download_tally;
//extern unsigned long download_count;
//extern unsigned long download_group_count;
//extern unsigned long download_group_tally;


extern void log(char *format,...);
char *handlePartPosts(Header *head,char *body,char *part_output_path,unsigned long *download_count,unsigned long *download_group_count,unsigned long days);
//char *reAssembleParts(char *name);
char *reAssembleParts(char *id,char *part_output_path,unsigned long *download_count,unsigned long *download_group_count,unsigned long days);

void gainAccessToParts();
void releaseAccessToParts();


int partCull25PercentOldest(char *basepath);
char *buildIncomplete(char *basepath,char *tag);

#ifdef OS2
int partClean(char *basepath);
#endif


#endif //#define __MULTIPART_H__
 






































































