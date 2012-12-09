/* config.h,v 1.2 2001/06/20 00:42:40 root Exp */

// config.h - configuration file support

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef OS2PM
#define INCL_WINPOINTERS
#define INCL_WININPUT
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS
#define INCL_WINMENUS
#define INCL_WINSTDCNR
#define INCL_WINDIALOGS
#define INCL_WINSYS
#define INCL_GPILCIDS
#define INCL_GPIBITMAPS
#define INCL_DOSMEMMGR
#define INCL_WINERRORS
#define INCL_SHLERRORS
#define INCL_GPIERRORS

#include <os2.h>

#endif // OS2PM

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

#include "unlink.h"
#include "log.h"

#ifndef UNIX
#include <new.h>
#endif



#ifndef OS2
#define ULONG unsigned long;
#endif

#ifndef MAXPATH
#define MAXPATH 2048
#endif

#define CONFIG_OK             0
#define CONFIG_BAD_OUTPUTPATH 1

#include "array.h"
#include "serverdefinition.h"
#include "groupdefinition.h"
#include "strutils.h"
#ifdef OS2PM
#include "container.h"
#endif

#define PRIORITY_NORMAL 1
#define PRIORITY_IDLE   2

extern int getNextNonBlankLine(FILE *in,char *buffer);
#ifdef OS2PM
extern HAB hab;
#endif
void message(char *format, ...);

class ServerDefinition;
class GroupDefinition;

class Configuration
{   
public:
    Configuration() 
    { 
        memset(this,0,sizeof(Configuration)); 

	// fixme to not zero already zeroed
        line_cull=5;
        max_download_size=0;
	max_num_articles=0; 
        num_servers=0; 
        header_cache=16; 
        
        priority_class = PRIORITY_NORMAL;
        priority_value = 0;
        
        logging_use = 0;
        log_spawn = 0;
        post_process_use = 0;
        filter_use = 0;
        fast_name_filter_use = 0;
        bytes_per_sec_bandwidth_limit = 0.0;
        
        max_part_age=3;
        multipart_use = 0;
        multipart_full_stop = 2;
        multipart_max_parts=30;
        multipart_max_lines=40000;

        user_autostart = 0;
        user_new_files = 0;
        user_small_icons = 0;
        user_loop_times = 0;
        user_loop_delay = 1800;
        
        socks_use = 0;
        memset(socks_server,0,sizeof(socks_server));
        char *s = getenv("SOCKS_SERVER");
        if (s)
        {
            strcpy(socks_server,s);
            socks_use = 1;
        }
    }
    
    int read(char *filename);
    FILE *writeHeader(char *filename);
#ifdef OS2PM
    int write(char *filename,Container *c);
#endif
    int write(char *filename);
    
    long getNumServers() { return num_servers; }
    long getNumServerGroups(int i) { if (i < num_servers) return servers[i]->getNumGroups(); else return 0; }
    
    ServerDefinition *getServer(int i) { if (i < num_servers) return servers[i]; else return NULL; }
    char *getServerName(int i)     { if (i < num_servers) return servers[i]->getName(); else return NULL; }
    char *getServerUsername(int i) { if (i < num_servers) return servers[i]->getUsername(); else return NULL; }
    char *getServerPassword(int i) { if (i < num_servers) return servers[i]->getPassword(); else return NULL; }
    
    GroupDefinition  *getServerGroup(int s,int g) { if (s < num_servers) return servers[s]->getGroup(g); else return NULL; }
    char *getServerGroupName(int s,int g) { if (s < num_servers) return servers[s]->getGroup(g)->getName(); else return NULL; }
    ULONG getServerGroupCull(int s,int g) { if (s < num_servers) return servers[s]->getGroup(g)->getLineCull(); else return 0; }
    
    ULONG getLineCull(void) { return line_cull; }
    void  setLineCull(ULONG x) { line_cull = x; }
    ULONG getMaxPartAge(void) { return max_part_age; }
    void  setMaxPartAge(ULONG x) { max_part_age = x; }
    ULONG getMaxDownload(void) { return max_download_size; }
    void  setMaxDownload(ULONG x) { max_download_size = x; }
    ULONG getMaxArticles(void) { return max_num_articles; }
    void  setMaxArticles(ULONG x) { max_num_articles = x; }
    
    char *getBaseDir(void) { return base_output_dir; }
    void  setBaseDir(char *s) { strcpy(base_output_dir,(s?s:"")); }
    char *getPartsDir(void) { return base_parts_dir; }
    void  setPartsDir(char *s) { strcpy(base_parts_dir,(s?s:"")); }
    int   getMultipartUse(void) { return multipart_use; }
    void  setMultipartUse(int x) { multipart_use = x; }
    int   getMultipartMaxParts(void) { return multipart_max_parts; }
    void  setMultipartMaxParts(int x) { multipart_max_parts = x; }
    int   getMultipartMaxLines(void) { return multipart_max_lines; }
    void  setMultipartMaxLines(int x) { multipart_max_lines = x; }
    int   getMultipartFullStop(void) { return multipart_full_stop; }
    void  setMultipartFullStop(int x) { multipart_full_stop = x; }
    char *getNetscapeDir(void) { return base_netscape_dir; }
    void  setNetscapeDir(char *s) { strcpy(base_netscape_dir,(s?s:"")); }
    char *getLog(void) { return log_file; }
    void  setLog(char *s) { strcpy(log_file,(s?s:"")); }
    int   getLoggingUse(void) { return logging_use; }
    void  setLoggingUse(int x) { logging_use = x; }
    int   getLogSpawn(void) { return log_spawn; }
    void  setLogSpawn(int x) { log_spawn = x; }
    char *getFilter(void) { return filter_file; }
    void  setFilter(char *s) { strcpy(filter_file,(s?s:"")); }
    int   getFilterUse(void) { return filter_use; }
    int   getFastNameFilterUse(void) { return fast_name_filter_use; }
    void  setFilterUse(int x) { filter_use = x; }
    void  setFastNameFilterUse(int x) { fast_name_filter_use = x; }
    char *getProcessCommand(void) { return process_command; }
    void  setProcessCommand(char *s) { strcpy(process_command,(s?s:"")); }
    int   getProcessUse(void) { return post_process_use; }    
    void  setProcessUse(int x) { post_process_use = x; }
    char *getRegoTimeout(void) { return rego_timeout; }
    void  setRegoTimeout(char *s) { strcpy(rego_timeout,(s?s:"")); }
    
    void  setSocksServer(char *s) { strcpy(socks_server,(s?s:"")); }
    char *getSocksServer(void) { return socks_server; }
    int   getSocksServerUse(void) { return socks_use; }
    void  setSocksServerUse(int x) { socks_use = x; }
    
    
//    int  getSmallIcons(void) { return small_icons; }
//    void setSmallIcons(int logical) { small_icons = logical; }
    int  getHeaderCacheSize(void) { return header_cache; }
    void setHeaderCacheSize(int val) { header_cache = val; if (val <0) val = 0; }
    
    int getPriorityClass(void) { return priority_class; }    
    void setPriorityClass(int x) { priority_class = x; }
    int getPriorityValue(void) { return priority_value; }
    void setPriorityValue(int x) { priority_value = x; }
    
    int checkOutputStuff(void);
    
    
    int  getUserAutoStart(void) { return user_autostart; }
    void setUserAutoStart(int x) { user_autostart = x; }
    int  getUserShowAllFiles(void) { return user_new_files; }
    void setUserShowAllFiles(int x) { user_new_files = x; }
    int  getUserSmallIcons(void) { return user_small_icons; }
    void setUserSmallIcons(int x) { user_small_icons = x; }
    int  getUserLoopTimes(void) { return user_loop_times; }
    void setUserLoopTimes(int x) { user_loop_times = x; }
    int  getUserLoopDelay(void) { return user_loop_delay; }
    void setUserLoopDelay(int x) { user_loop_delay = x; }
    
    double getBandwidthLimit(void) { return bytes_per_sec_bandwidth_limit; }
    void   setBandwidthLimit(double x) { bytes_per_sec_bandwidth_limit = x; }
    
    time_t getTimeStamp(void) { return timestamp; }
    
private:                                
    char base_output_dir[MAXPATH];   
    char base_parts_dir[MAXPATH];    
    char base_netscape_dir[MAXPATH];
    char log_file[MAXPATH];        
    char filter_file[MAXPATH];     
    char process_command[MAXPATH]; 
    char rego_timeout[MAXPATH];
    long line_cull;          
    long max_part_age;       
    long max_download_size;  
    long max_num_articles;     
    long num_servers;
    double bytes_per_sec_bandwidth_limit;
//    int small_icons;
    int header_cache;
    
    int priority_class;
    int priority_value;

    int multipart_max_parts;		// per binary
    int multipart_max_lines;		// per part
    int multipart_use;
    int multipart_full_stop;

    int logging_use;
    int log_spawn;

    int post_process_use;

    int filter_use;
    int fast_name_filter_use;
    
    int user_autostart;
    int user_new_files;
    int user_small_icons;
    int user_loop_times;
    int user_loop_delay;
    
    time_t timestamp;
    
    array<ServerDefinition *> servers;
    
    char socks_server[80];
    int  socks_use;
         
    
};


#endif //#define __CONFIG_H__
