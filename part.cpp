#include "part.h"

Part::~Part(void)
{
    if (buffer) delete[] buffer; 
    if (name) delete[] name; 
    if (filename) delete[] filename;
    if (string_bit) delete[] string_bit;
}

Part::Part(char *str,char *fname,int num,int o)
{
    init();
    
    if (str != NULL)
    {
        string_bit = new char[1+strlen(str)];
        strcpy(string_bit,str);
    }                      
    
    if (fname != NULL)
    {
        filename = new char[1+strlen(fname)];
        strcpy(filename,fname);
    }                      
    
    if (num >= 0)
        number = num;
        
    if (o >= 0)
        of = o;
}
   
char *Part::getName()
{
    return name;
}

char *Part::getFilename()
{
    if (filename == NULL)
        return "\0";
    return filename;
}               

char *Part::getIdString()
{
    if (string_bit == NULL)
        return "\0";
    return string_bit;
}

char *Part::getBuffer()
{
    return buffer;
}

int Part::getNumber()
{
    return number;
}

int Part::getTotal()
{
    return of;
}



int Part::read(FILE *in)
{
    // Looking for 2 lines:
    
    // 924281382 2 12 x924281231000
    // The Big L for FEDMEDIATE - TheBigL.zip (
    
    Part::clear();
    
    if (in == NULL || feof(in))
        return 0;
    
    char *buffer = new char[4096];
    char *ptr;
    
    fgets(buffer,4096,in);
    if (feof(in))
    {
        delete[] buffer;
        return 0;
    }
    
    ptr = strchr(buffer,'\r');
    if (!ptr) ptr = strchr(buffer,'\n');
    if (ptr) *ptr = '\0';
    
    if (sscanf(buffer,"%lu %d %d",&timestamp,&number,&of) != 3 ||
        (ptr = strchr(buffer,'x')) == NULL)
    {
        delete[] buffer;
        return 0;
    }
    
    filename = new char[1+strlen(ptr)];
    strcpy(filename,ptr+1);  // get the bit after the 'x'
    
    fgets(buffer,4096,in);
    if (feof(in))
    {
        delete[] buffer;
        return 0;
    }
    
    ptr = strchr(buffer,'\r');
    if (!ptr) ptr = strchr(buffer,'\n');
    if (ptr) *ptr = '\0';
    string_bit = strdup(buffer);
    
    sprintf(buffer,"read: part '%s' on time %lu, partt %d of %d",filename,timestamp,number,of);
    printf(buffer);
    sprintf(buffer,"read: signature is '%s'",string_bit);
    printf(buffer);
    
    delete[] buffer;
    return 1;
}




