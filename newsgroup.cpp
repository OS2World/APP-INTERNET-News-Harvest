
/***********************************************************************

newsgroup.cpp,v 1.4 2007/04/26 03:04:58 root Exp

Newsgroup class

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2002, 2005 Steven Levine and Associates, Inc.
All rights reserved.

06 Jan 02 SHL - Drop unused
12 Oct 05 SHL - Drop more unused

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef UNIX
#include <unistd.h>
#else
#include <new.h>
#endif

#include "log.h"
#include "unlink.h"

#include "newsgroup.h"

NewsGroup::NewsGroup(char *_server,char *_name,unsigned long num,unsigned long first,unsigned long last)
{
    name = 0;
    server = 0;

    int namelen = (_name?strlen(_name):0);
    int servlen = (_server?strlen(_server):0);

    if (servlen > 3 && namelen > 4 ) // at least "news:alt."
    {
	server = new char[1+servlen];
	if (!server)
	{
	    log("FATAL - PANIC - Failed to allocate %d bytes of memory (%s:%d)",1+servlen,__FILE__,__LINE__);
	    exit(2);
	}
	strcpy(server,_server);
	char *ptr = server;
	while (*ptr)
	{
	    *ptr = tolower(*ptr);
	    ptr++;
	}
	ptr = server + -1 + servlen;
	while (ptr >= server && *ptr == ' ')
	{
	    *ptr = '\0';
	    ptr--;
	}

	name = new char[1+namelen];
	if (!name)
	{
	    log("FATAL - PANIC - Failed to allocate %d bytes of memory (%s:%d)",1+namelen,__FILE__,__LINE__);
	    exit(2);
	}
	strcpy(name,_name);
	ptr = name;
	while (*ptr)
	{
	    *ptr = tolower(*ptr);
	    ptr++;
	}
	ptr = name + -1 + namelen;
	while (ptr >= name && *ptr == ' ')
	{
	    *ptr = '\0';
	    ptr--;
	}
    }

    first_article = first;
    last_article = last;
    num_articles = num;
}

NewsGroup::~NewsGroup()
{
    delete[] server;
    delete[] name;
}

char *NewsGroup::getServer()
{
    return server ? server : "";
}

char *NewsGroup::getName()
{
    return name ? name : "";
}

unsigned long NewsGroup::firstArticle()
{
    return first_article;
}

unsigned long NewsGroup::lastArticle()
{
    return last_article;
}

unsigned long NewsGroup::numArticles()
{
    return num_articles;
}
