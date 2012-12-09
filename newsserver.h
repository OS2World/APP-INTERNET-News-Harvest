
/***********************************************************************

newsserver.h,v 1.4 2004/12/05 01:01:03 root Exp

Newsserver class

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2003 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	04 Jul 01 SHL Pass Configuration to getBody
		15 Dec 03 SHL Rename BUFFER_SIZE to NS_BUFFER_SIZE

***********************************************************************/

#ifndef __NEWSSERVER_H__
#define __NEWSSERVER_H__

#ifndef OS2
//#define OS2
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifndef UNIX
#include <new.h>
#endif

#ifdef SOCKS
#include "libsock5.h"
#endif

#include "charqueue.h"
#include "newsgroup.h"
#include "array.h"
#include "xhdrpacket.h"
#include "bincat.h"
#include "log.h"
#include "config.h"
#include "sleep.h"

#ifndef BSD_SELECT
#define BSD_SELECT
#endif

#ifndef WIN32
/*
#include <types.h>
//#include <unistd.h>
//#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
*/
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#endif

#define NS_BUFFER_SIZE 8192

#define OPEN_OK   1
#define OPEN_FAIL 0
#define OPEN_WAIT 2

extern int is_sleeping;

class NewsServer
{
public:
    NewsServer(char *address,int port);
    NewsServer(char *address);
    NewsServer();
   ~NewsServer();
   
    void setSocksServer(char *s);
   
    int   open();
    int   close();
    int   closed();
    NewsGroup *setGroup(char *groupname);
    char *getBody(unsigned long article_number,int target_lines,int (* callback)(const float, const int, const int), Configuration* pCfg);
    char *getHead(unsigned long article_number);
    char *getLongHead(unsigned long article_number);
    char *getXhdrHead(unsigned long article);
    char *getSignOnBanner(void) { return server_signon_message; }
    
    unsigned long getCPS();
    void resetCPS();
    int getGroupList(array<char *> &empty_list);
    int getGroupList(array<char *> &empty_list,int *count,char *since_date);
    int getArticleLines(array<XhdrPacket> &,unsigned int low,unsigned int high);
    int getArticleSubjects(array<XhdrPacket> &list,unsigned int low,unsigned int high);
    
    void setDebugFile(char *filename);
    
    int  usingXhdr(void);
    void setXhdrCacheSize(int x) { if (x < 0) xhdr_cache_size = 0; else xhdr_cache_size = x; }
    void setXhdr(int x) { no_xhdr = x; }
    
    int login(char *username,char *password);
    int reLogin(void);
    char *attempt(char *command);
    
    void setBandwidthLimit(double bytes_per_sec);
    
#ifdef WIN32
    NewsServer(char *address,int *status);
    int initWinsock();
    int removeWinSock();
    // gavin: i added this. see code
    void set(char *addr, int p, int* status);
#endif
    
private:
    void set(char *address,int port);
    void imposeBandwidthLimit();
    int tx(char *buffer,unsigned long length);
    int tx(char *buffer);
    int rx();
    char *rxLine();
    char *lineSansRx();
    
    char *debug_file;
    
    int   sock;
    int   port;
    char *address;
    char *group;
    char *end_of_return;
    char  buffer[NS_BUFFER_SIZE+1];
    CharQueue incoming;

    unsigned long cps_chars_read;		// per article
    time_t cps_start_time;			// per article

    
    struct hostent *host;
    struct in_addr in_address;
    struct sockaddr_in server;
    unsigned int timeout;
    
    int no_xhdr;
    
    int xhdr_cache_size;
    long xhdr_cached_from;
    long xhdr_cached_upto;
    array<XhdrPacket> xhdr_lines_cache;
    array<XhdrPacket> xhdr_subject_cache;
//    array<XhdrPacket> xhdr_type_cache;
//    array<XhdrPacket> xhdr_encoding_cache;
//    array<XhdrPacket> xhdr_disposition_cache;
    
    
    char last_username[80];
    char last_password[80];
    char server_signon_message[512];
    
    time_t bandwidth_start_time;
    unsigned long bandwidth_chars_read_hist;
    unsigned long bandwidth_chars_read;
    double bandwidth_limit;
    
    char socks_server[81];
    
};

#endif //#ifndef __NEWSSERVER_H__
