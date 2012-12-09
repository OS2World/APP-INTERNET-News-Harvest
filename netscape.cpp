#include "netscape.h"

void updateNetscapeNews(char *netscape_path,char *server,char *group,unsigned long article)
{
    char *buffer = new char[4096];
    char *filename = new char[4096];
    
    if (server != NULL && *netscape_path != '\0')
    {
        log("Updating Netscape News...");
#ifndef NOPRINT
        printf("Updating Netscape News...\r");
#endif
        
        char *ptr;
        
        // build netscape .rc filename from server name
        // => netscape_path + 1st word of server name + ".rc"
        strcpy(filename,netscape_path);
#ifdef WINX
        strcat(filename,"news/");
#else
        strcat(filename,"news\\");
#endif
        ptr = strchr(server,'.');  // server may be 'news' or 'news.xxx.org'
        if (ptr)
        {
            *ptr = '\0';
            strcat(filename,server);
            *ptr = '.';
        }
        else
        {
            strcat(filename,server);
        }
        strcat(filename,".rc");
        
        // Ok: we have path to .rc file, now open & update it.
        
        int found = 0;
        FILE *in = fopen(filename,"r");
        if (in)
        {
            FILE *out = tmpfile();
            if (out)
            {
                // Read thru entries looking for group name
                while (!feof(in))
                {
                    fgets(buffer,4096,in);
                    if (!feof(in))
                    {
                        if (!found)
                        {
                            ptr = strchr(buffer,':');
                            if (ptr) *ptr='\0';
                            // is this the group to update
                            if (strcmp(group,buffer) != 0)
                            {
                                if (ptr) *ptr = ':';
                                fputs(buffer,out);
                            }
                            else
                            {
                                fprintf(out,"%s: 1-%lu\n",group,article);
                                found = 1;
                            }
                        }
                        else  // group line found & processed already
                        {
                            fputs(buffer,out);
                        }
                    }
                }
                // It's not a Netscape subscribed group, subscribe it.
//                if (!found)
//                {
//                    fprintf(out,"%s 1-%lu\n",group,article);
//                }
                
                // DONE!  But the update file is not written to the right place!
                // copy it back from our temporary file
                // NOTE:  in & out are being swapped!
                fclose(in);
                rewind(out);
                in = out;
                out = fopen(filename,"w");
                if (!out)
                {
#ifndef NOPRINT
                    printf("Failed to update Netscape News groups file\n");
#endif
                    log("Failed to update Netscape News groups file");
                }
                else
                {
                    while(!feof(in))
                    {
                        fgets(buffer,4096,in);
                        if (!feof(in))
                            fputs(buffer,out);
                    }
                }
#ifndef NOPRINT                
                printf("Netscape News groups file updated.\n");
#endif
                log("Netscape News groups file updated.");
                
                fclose(out);  
            }
            else
            {
#ifndef NOPRINT                
                printf("Failed creating temporary file for Netscape News update\n");
#endif
                log("Failed creating temporary file for Netscape News update");
            }
            
            fclose(in);
        }
        else
        {
#ifndef NOPRINT            
            printf("Can't find Netscape News groups file!\n\"%s\"\n",filename);
#endif
            log("Can't find Netscape News groups file!\n\"%s\"",filename);
//            *netscape_path = '\0';
        }
    }
    
    delete[] filename;
    delete[] buffer;
}

