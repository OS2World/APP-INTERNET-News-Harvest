
/***********************************************************************

newsgroup.h,v 1.2 2007/04/26 03:07:48 root Exp

Newgroup class

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2005 Steven Levine and Associates, Inc.
All rights reserved.

12 Oct 05 SHL Drop unused.  Move includes to newsgroup.cpp

***********************************************************************/

#ifndef __NEWSGROUP_H__
#define __NEWSGROUP_H__

class NewsGroup
{
public:
    NewsGroup(char *server,char *name,unsigned long num,unsigned long first,unsigned long last);
   ~NewsGroup();
    
    char *getName();
    char *getServer();

    unsigned long firstArticle();
    unsigned long lastArticle();
    unsigned long numArticles();	// unused, keep for future

private:
    char *name;
    char *server;
    unsigned long first_article;
    unsigned long last_article;
    unsigned long num_articles;		// unused, keep for future
    
};

#endif // #ifndef __NEWSGROUP_H__
