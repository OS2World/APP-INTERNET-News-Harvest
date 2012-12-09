/* config.cpp,v 1.2 2001/06/20 00:42:40 root Exp */

// config.cpp - configuration file support

#include "config.h"
#include "log.h"

int getNextNonBlankLine(FILE *in,char *buffer)
{
    char *ptr,line[1024];
        
    *buffer = '\0';
        
    while (!feof(in))
    {        
        *line = '#';
        
        // get first non-comment 
        while (!feof(in) && (*line == '#' || *line == '\n'))
            fgets(line,1024,in);
             
        // now have either EOF, or non '#...' line
        if (feof(in)) return 0;                    
                   
        ptr = line+strlen(line)-1;
        while (isspace(*ptr))
        {
            *ptr = '\0';
            ptr--;
            if (ptr <= line) 
                break;
        }      
             
        ptr = line;
        while (*ptr && isspace(*ptr)) ptr++;
        
        if (*ptr != '\0')  // got a non-blank line
        {          
            strcpy(buffer,ptr);
            return 1;
        }
    }                         
    
    return 0;
}
    
    

int Configuration::read(char *filename)
{                                             
    char *buffer = new char[4096];
    char *ptr;
    int   i,j;                      
    long actual_num_groups;
    FILE *in = fopen(filename,"r");
    
    if (in)
    {
        getNextNonBlankLine(in,buffer);      // Consume "Config"
        
        // read the config items ->  <something>=<something>
        while (1)
        {
            // drop out clauses
            if (!getNextNonBlankLine(in,buffer)) break;
            if (*buffer=='\0' || strncmp(buffer,"Server",6) == 0) break;
            
            ptr = strchr(buffer,'=');
            if (!ptr) break;
            *ptr = '\0'; 
            ptr++;
            
            // check length
            if (strlen(ptr) > 512) 
                *(ptr+512) = '\0';
            
            if (strcmp(buffer,"OutputDir") == 0)               strcpy(base_output_dir,ptr);
            else if (strcmp(buffer,"PartsDir") == 0)           strcpy(base_parts_dir,ptr);
            else if (strcmp(buffer,"NetscapeDir") == 0)        strcpy(base_netscape_dir,ptr);
            else if (strcmp(buffer,"LogFile") == 0)          { strcpy(log_file,ptr); ::setLogfilePath(ptr); }
            else if (strcmp(buffer,"FilterFile") == 0)         strcpy(filter_file,ptr);
            else if (strcmp(buffer,"FilterEnabled") == 0)      filter_use = atoi(ptr);
            else if (strcmp(buffer,"FastNameFilterEnabled") == 0)      fast_name_filter_use = atoi(ptr);
            else if (strcmp(buffer,"PostProcessCommand") == 0) strcpy(process_command,ptr);
            else if (strcmp(buffer,"TCPSettings") == 0)        strcpy(rego_timeout,ptr);
            else if (strcmp(buffer,"SocksServer") == 0)        strcpy(socks_server,ptr);
            else if (strcmp(buffer,"UserInterface") == 0)      
            {
                // should be "0,1,0,9999,9999"
                user_autostart = -48 + (int)(*ptr);
                ptr += 2;
                user_new_files = -48 + (int)(*ptr);
                ptr += 2;
                user_small_icons = -48 + (int)(*ptr);
                ptr += 2;
                user_loop_times = atoi(ptr);
                ptr = strchr(ptr,',');
                if (ptr)
                {
                    ptr++;
                    user_loop_delay = atoi(ptr);
                }
            }
            
            else if (strcmp(buffer,"MaxPartAge") == 0) max_part_age = atol(ptr);
            else if (strcmp(buffer,"LineCullAmount") == 0) line_cull = atol(ptr);
            else if (strcmp(buffer,"MaxDownloadSize") == 0) max_download_size = atol(ptr);
            else if (strcmp(buffer,"MaxNumArticles") == 0) max_num_articles = atol(ptr);
            else if (strcmp(buffer,"NumServers") == 0) num_servers = atoi(ptr);
            else if (strcmp(buffer,"SmallIcons") == 0) ; // small_icons = atoi(ptr);
            else if (strcmp(buffer,"HeaderCacheSize") == 0) header_cache = atoi(ptr);
            else if (strcmp(buffer,"PriorityClass") == 0) priority_class = atoi(ptr);
            else if (strcmp(buffer,"PriorityValue") == 0) priority_value = atoi(ptr);
            else if (strcmp(buffer,"BandwidthLimit") == 0) bytes_per_sec_bandwidth_limit = (double)atof(ptr);
            else if (strcmp(buffer,"LoggingEnabled") == 0) { logging_use = atoi(ptr); toggleLog(logging_use); }
            else if (strcmp(buffer,"LogSpawn") == 0) { log_spawn = atoi(ptr); }
            else if (strcmp(buffer,"MultipartEnabled") == 0) multipart_use = atoi(ptr);
            else if (strcmp(buffer,"MultipartFullStop") == 0) multipart_full_stop = atoi(ptr);
            else if (strcmp(buffer,"PostProcessingEnabled") == 0) post_process_use = atoi(ptr);
            else if (strcmp(buffer,"SocksEnabled") == 0) socks_use = atoi(ptr);
            else if (strcmp(buffer,"MaxNumParts") == 0) multipart_max_parts = atoi(ptr);
            else if (strcmp(buffer,"MaxPartLines") == 0) multipart_max_lines = atoi(ptr);
            else if (strcmp(buffer,"TimeStamp") == 0) timestamp = atol(ptr);
            else
                break;   // read crap
        }
        
        if (*base_output_dir   && base_output_dir[strlen(base_output_dir)-1] != '\\')     strcat(base_output_dir,"\\");
        if (*base_parts_dir    && base_parts_dir[strlen(base_parts_dir)-1] != '\\')       strcat(base_parts_dir,"\\");
        if (*base_netscape_dir && base_netscape_dir[strlen(base_netscape_dir)-1] != '\\') strcat(base_netscape_dir,"\\");
        
        for (i=0; i<num_servers; i++)
        {                                        
            if (i>0)
                getNextNonBlankLine(in,buffer);      // Consume "Server"
            
            if (strcmp(buffer,"Server") != 0)
            {
                num_servers = i;
                break;
            }
            
            servers[i] = new ServerDefinition();
            
            for (j=0; j<2; j++)
            {
                if (!getNextNonBlankLine(in,buffer)) break;   
                ptr = strchr(buffer,'=');
                if (!ptr) break;
                *ptr = '\0'; 
                ptr++; 
            
                if (strcmp(buffer,"Name") == 0) servers[i]->setName(ptr);
                else if (strcmp(buffer,"NumGroups") == 0) { actual_num_groups = atol(ptr); }
                else
                    break;    // error
            } 
            
            // read each group for this server
            for (j=0; j<actual_num_groups; j++)
            {
                if (!getNextNonBlankLine(in,buffer)) break;   
                ptr = strchr(buffer,'=');
                if (!ptr) break; 
                servers[i]->addGroupDefinition(new GroupDefinition(ptr+1));
            }
            
            servers[i]->setNumGroups(actual_num_groups);
        }
        fclose(in);
    }
    
    delete[] buffer;
    
    return 0;
}    

FILE *Configuration::writeHeader(char *filename)
{
    FILE *out;
    
    if (filename == NULL)
        out = stdout;
    else
        out = fopen(filename,"w");
    
    fprintf(out,"#\n# Abandon all hope, Ye who edit here\n#\n\n");
    
    time(&timestamp);
    fprintf(out,"Config\n");
    fprintf(out,"    TimeStamp=%lu\n",(unsigned long)timestamp);  // assume time() returns long int - naughty naughty!
    fprintf(out,"    OutputDir=%s\n",base_output_dir);
    fprintf(out,"    PartsDir=%s\n",base_parts_dir);
    fprintf(out,"    MaxNumParts=%d\n",multipart_max_parts);
    fprintf(out,"    MaxPartLines=%d\n",multipart_max_lines);
    fprintf(out,"    MultipartEnabled=%d\n",multipart_use);
    fprintf(out,"    MultipartFullStop=%d\n",multipart_full_stop);
    fprintf(out,"    NetscapeDir=%s\n",base_netscape_dir);
    fprintf(out,"    SocksServer=%s\n",socks_server);
    fprintf(out,"    SocksEnabled=%d\n",socks_use);
    fprintf(out,"    FilterFile=%s\n",filter_file);    
    fprintf(out,"    FilterEnabled=%d\n",filter_use);
    fprintf(out,"    FastNameFilterEnabled=%d\n",fast_name_filter_use);
    fprintf(out,"    LogFile=%s\n",log_file);
    fprintf(out,"    LogSpawn=%d\n",log_spawn);
    fprintf(out,"    LoggingEnabled=%d\n",logging_use);
    fprintf(out,"    PostProcessCommand=%s\n",process_command);
    fprintf(out,"    PostProcessingEnabled=%d\n",post_process_use);
    fprintf(out,"    BandwidthLimit=%-6.0lf\n",bytes_per_sec_bandwidth_limit);
    fprintf(out,"    MaxDownloadSize=%ld\n",max_download_size);
    
//    fprintf(out,"    MaxNumArticles=%ld\n",max_num_articles);
    fprintf(out,"    MaxNumArticles=0\n");
    
    fprintf(out,"    MaxPartAge=%ld\n",max_part_age);
    fprintf(out,"    LineCullAmount=%ld\n",line_cull);
//    fprintf(out,"    SmallIcons=%d\n",small_icons);
    fprintf(out,"    HeaderCacheSize=%d\n",header_cache);
    fprintf(out,"    TCPSettings=%s\n",rego_timeout);
    fprintf(out,"    PriorityClass=%d\n",priority_class);
    fprintf(out,"    PriorityValue=%d\n",priority_value);
    fprintf(out,"    UserInterface=%d,%d,%d,%d,%d\n",user_autostart,user_new_files,user_small_icons,user_loop_times,user_loop_delay);
    return out;
}

#ifdef OS2PM
int Configuration::write(char *filename,Container *c)
{
    FILE *out = writeHeader(filename);
    int   i,j;
    
    fprintf(out,"    NumServers=%ld\n",(long)c->getNumServers());
    
    c->getConfigList(out);
            
    if (out != stdout)
        fclose(out);
    
    return 1;
}
#endif

// check everything is mostly OK for start:
//    - output dir exists
//    - enough space for output
//    - etc.
// return code on failure.
int Configuration::checkOutputStuff(void)
{
    FILE *out;
    int bad = 0;
    
    // if base output path check it's writeable, and long filename
    char *buffer = new char[4096];
    sprintf(buffer,"%snews_harvest_test.abc",base_output_dir);
    out = fopen(buffer,"wb");
    if (out)
    {
        fclose(out);
        deleteFile(buffer);
    }
    else
    {
        bad = 1;
    }
    delete[] buffer;
    
    if (bad)
        return CONFIG_BAD_OUTPUTPATH;
    
    // must be ok if got here
    return CONFIG_OK;
}

int Configuration::write(char *filename)
{
    FILE *out = writeHeader(filename);
    int   i,j;
    
    fprintf(out,"    NumServers=%ld\n",num_servers);
    
    for (i=0; i<num_servers; i++)
    {
        fprintf(out,"Server\n");
        if (servers[i]->getUsername() && servers[i]->getPassword())
        {
            fprintf(out,"    Name=%s,%s,%s\n",servers[i]->getName(),
                                              servers[i]->getUsername(),
                                              servers[i]->getPassword());
        }
        else
        {
            fprintf(out,"    Name=%s\n",servers[i]->getName());
        }
        fprintf(out,"    NumGroups=%ld\n",servers[i]->getNumGroups());
        for (j=0; j<servers[i]->getNumGroups(); j++)
        {
            GroupDefinition *gd;
            gd = servers[i]->getGroup(j);
            fprintf(out,"    Group=%s,%ld,%ldk,%s,%s\n",
                    gd->getName(),
                    gd->getLastRecord(),
                    gd->getLineCull(),
                    (gd->getCollapsed()?"Collapsed":"Expanded"),
                    (gd->getSkipped()?"Skip":"Include")
                   );
        }

        fprintf(out,"\n");
    }
            
    if (out != stdout)
        fclose(out);
    
    return 1;
}


