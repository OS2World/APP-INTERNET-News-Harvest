
#include "mimeset.h"

char *MimeSet::getName(int i)
{ 
    if (i<num_items)
        return name[i];
    else 
        return NULL;
}

char *MimeSet::getExtensions(int i)
{ 
    if (i<num_items)
        return extensions[i];
    else 
        return NULL;
}

char *MimeSet::getCommand(int i)
{ 
    char *x = NULL;
    
    if (i<num_items)
        x = command[i];
    
    return x;
}

char *MimeSet::getCommand(char *ext)
{
    if (ext)
    {
        makeLowerCase(ext);
        int rc = find(ext);
        
        if (rc == -1)
            return NULL;
        else
            return command[rc];
    }
    else
        return NULL;
}


void MimeSet::clear()
{
    for (int i=0; i<num_items; i++)
    {
        delete[] name[i];
        name[i] = NULL;
        delete[] extensions[i];
        extensions[i] = NULL;
        delete[] command[i];
        command[i] = NULL;
    }
    num_items=0;
}


int MimeSet::load()
{
    char *buffer = new char[4096];
    char *ptr1,*ptr2;
    
    if (filename)
    {
        FILE *in = fopen(filename,"r");
        if (in)
        {
            while (!feof(in))
            {
                fgets(buffer,1024,in);
                if (!feof(in) && strlen(buffer) > 3 && *buffer != '#')
                {
                    // looking for:
                    // JPEG Image,JPEG JPE JPG,Pmview.exe %
                    trimCR(buffer);
                    lrSpaceTrim(buffer);   // remove leading & training whitespace
//                    singleSpace(buffer);
                    char *ptr1 = strchr(buffer,' ');
                    if (ptr1)
                    {
                        *ptr1 = '\0';
                        name[num_items] = new char[1+strlen(buffer)];
                        strcpy(name[num_items],buffer);
                        ptr1++;
                        ptr2 = ptr1;
                        ptr1 = strchr(ptr2,' ');
                        if (ptr1)
                        {
                            *ptr1 = '\0';
                            extensions[num_items] = new char[1+strlen(ptr2)];
                            strcpy(extensions[num_items],ptr2);
                            makeUpperCase(extensions[num_items]);
                            
                            ptr1++;
                            command[num_items] = new char[1+strlen(ptr1)];
                            strcpy(command[num_items],ptr1);
                            
                            num_items++;
                        }
                    }
                }
            }
            fclose(in);
        }
    }
    
    delete[] buffer;
    
    if (num_items-1 > 0)
        return 1;
    
    return 0;
}

int MimeSet::reload()
{
    char buffer[1024];
    char *ptr1,*ptr2;
    
    
    clear();
    return load();
}



int MimeSet::save()
{
    if (filename)
    {
        FILE *out= fopen(filename,"w");
        if (out)
        {
            fprintf(out,"#\n# Abandon all hope, Ye who edit here\n#\n\n");
            int i;
            for (i=0; i<num_items; i++)
            {
                fprintf(out,"%s %s %s\n",name[i],extensions[i],command[i]);
            }
            fclose(out);
            return 1;
        }
    }
    return 0;
}




int MimeSet::find(char *ext)
{
    if (num_items == 0 || !ext)
        return -1;
    
    for (int i=0; i<num_items; i++)
    {
        if (strstr(extensions[i],ext))
            return i;
    }
    
    return -1;
}

int MimeSet::getIndexByName(char *nomen)
{
    if (num_items == 0 || !nomen)
        return -1;
    
    for (int i=0; i<num_items; i++)
    {
        if (strcmp(name[i],nomen))
            return i;
    }
    
    return -1;
}


char *MimeSet::determineType(char *filename)
{
    if (!filename) return NULL;
    
    int i;
    int def=-1;
    
    for (i=0; i<num_items; i++)
    {
        char buffer[256];
        char *item;
        
        strcpy(buffer,extensions[i]);
        item = strtok(buffer," ");
        while (item)
        {
            if (stricmp(name[i],"Default")==0)
               def = i;
            
            if (stristr(filename,item))
                return command[i];
            item = strtok(NULL," ");
        }
    }
    
    // default
    if (def != -1)
        return command[def];
    return NULL;
}

