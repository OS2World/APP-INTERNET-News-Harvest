/* header.h,v 1.2 2001/06/20 00:42:19 root Exp */

#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef UNIX
#include <new.h>
#endif

#include "array.h"
#include "obj_string.h"

#define MAX_NUM_PARTS 200


extern void log(char *format,...);

class Header
{
public:
    Header(char *block);
    Header(char *block,int article);
   ~Header(void);
    int numLines(void);
    int containsString(char *);
    int containsStringNoCase(char *s);
    int filter(array<String>&, int num_filters);
    int isMultiPart();
    int partNumber();
    int partTotal();
    int getArticleNum();
    char *partName();
    char *partString();
    char *getContent();
    char *getSubject();
    char *getSender();
    
private:
    void init();
    int decodeSubject(char *subjectLine);
    
    char *block;
    char *lower_block;
    int   lines;
    unsigned int   str_len;
    int article;
    
    char *subject;
    char *lower_subject;
    char *sender;
    char *lower_sender;

    char *part_line;
    char *part_name;
    int   part_num;
    int   part_total;
    
};

#endif // #ifndef __HEADER_H__
