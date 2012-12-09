#include "filestat.h"
#include "decoder.h"

FileStat::FileStat(void)
{
    FileStat::init();
    
#ifdef DEBUG
    fprintf(stderr,"New FileStat[%p](void)\n",this);
    fprintf(stderr,"\textension = \"%s\"\n",(extension?extension:"<null>"));
    fprintf(stderr,"\tname      = \"%s\"\n",(name?name:"<null>"));
    fprintf(stderr,"\tfull_path = \"%s\"\n",(full_path?full_path:"<null>"));
    fflush(stderr);
#endif // DEBUG
}

FileStat::~FileStat(void)
{
    
#ifdef DEBUG
    fprintf(stderr,"Delete FileStat[%p](void)\n",this);
    fprintf(stderr,"\textension = \"%s\"\n",(extension?extension:"<null>"));
    fprintf(stderr,"\tname      = \"%s\"\n",(name?name:"<null>"));
    fprintf(stderr,"\tfull_path = \"%s\"\n",(full_path?full_path:"<null>"));
    fflush(stderr);
#endif // DEBUG

    
    if (extension != NULL)
        delete[] extension;
    if (name != NULL)
        delete[] name;
    if (full_path != NULL)
        delete[] full_path;
}

int FileStat::getError(void)
{
    return error_code;
}

int FileStat::hasError(void)
{
    return (error_code != OK);
}

void FileStat::setError(int e)
{
    error_code = e;
}

int FileStat::getPartNumber(void)
{
    return part;
}
int FileStat::getPartTotal(void)
{
    return of;
}
int FileStat::hasPartNum(void)
{
    return (part > 0);
}
int FileStat::hasPartTotal(void)
{
    return (of > 0);
}

void FileStat::setPart(int p,int o)
{
    part = p;  of = o;
}

int FileStat::getEncodingMethod(void)
{
    return encoding;
}

int FileStat::hasEncodingMethod(void)
{
    return encoding != UNKNOWN;
}

void FileStat::setEncodingMethod(int e)
{
    encoding = e;
}

int FileStat::getAction(void)
{
    return action;
}

int FileStat::hasAction(void)
{
    return action != NONE;
}

void FileStat::setAction(int a)
{
    action = a;
}

unsigned long FileStat::getSize(void)
{
    return size;
}

void FileStat::setSize(unsigned long s)
{
    size = s;
}

char *FileStat::getExtension(void)
{
    if (extension == NULL)
        return "";
    else
        return extension;
}

int FileStat::hasExtension(void)
{
    return (extension != NULL);
}

void FileStat::setExtension(char *ex)
{
    if (extension != NULL)
        delete[] extension;
    extension = NULL;
    if (ex != NULL)
    {
        extension = new char[128+strlen(ex)];
        if (extension == NULL)
        {
            log("FATAL - PANIC - Failed to allocate %d bytes of memory (%s:%d)",1+strlen(ex),__FILE__,__LINE__);
            exit(2);
        }

        strcpy(extension,ex);
        
        // make lower case
        char *ptr = extension;
        while (*ptr != '\0')
        {
            if (*ptr >= 'A' && *ptr <= 'Z')
                *ptr += 'a' - 'A';
            ptr++;
        }
    }
}

char *FileStat::getName(void)
{
    
    if (name == NULL)
        return "";
    else
        return name;
}

int FileStat::hasName(void)
{
    return (name != NULL);
}

void FileStat::setName(char *nam)
{
    if (name != NULL)
        delete[] name;
    name = NULL;
    if (nam != NULL)
    {
        name = new char[1+strlen(nam)];
        if (name == NULL)
        {
            log("FATAL - PANIC - Failed to allocate %d bytes of memory (%s:%d)",1+strlen(nam),__FILE__,__LINE__);
            exit(2);
        }

        strcpy(name,nam);
    }
}

int FileStat::hasFullPath(void)
{
    return (full_path != NULL);
}

char *FileStat::getFullPath(void)
{
    if (full_path == NULL)
        return "";
    else
        return full_path;
}

void FileStat::setFullPath(char *fp)
{
    if (full_path != NULL)
        delete[] full_path;
    full_path = NULL;
    if (fp != NULL)
    {
        full_path = new char[1+strlen(fp)];
        if (full_path == NULL)
        {
            log("FATAL - PANIC - Failed to allocate %d bytes of memory (%s:%d)",1+strlen(fp),__FILE__,__LINE__);
            exit(2);
        }

        strcpy(full_path,fp);
    }
}

void FileStat::clear(void)
{
    error_code = OK;
    size = 0;
    encoding = UNKNOWN;
    action = NONE;
    if (extension != NULL)
        delete[] extension;
    if (name != NULL)
        delete[] name;
    if (full_path != NULL)
        delete[] full_path;
    extension = NULL;
    name = NULL;
    full_path = NULL;
    part=of=0;
}

void FileStat::print(void)
{
    printf("\n");
    printf("Name:       \"%s\"\n",getName());
    printf("Extension:  \"%s\"\n",getExtension());
    printf("Full Path:  \"%s\"\n",getFullPath());
    printf("Error Code: %d\n",error_code);
    printf("Size:       %lu\n",size);
    printf("Encoding:   %d\n",encoding);
    printf("Action:     %d\n",action);
    printf("Part:       %d/%d\n",part,of);
}

// ----- PRIVATE PARTS --------------------------------


void FileStat::init(void)
{
    error_code = OK;
    size = 0;
    part=of=0;
    encoding = UNKNOWN;
    action = NONE;
    extension = NULL;
    name = NULL;
    full_path = NULL;
}
