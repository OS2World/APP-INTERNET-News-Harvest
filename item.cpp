/* item.cpp,v 1.2 2001/06/20 00:42:19 root Exp */

#include "item.h"

void printit(Item *f)
{
    f->print();
}

void Item::print() { printf("%lu %s\n",size,(filename?filename:"null")); }

// static int Item::compare(Item foo,Item bar)
int Item::compare(Item foo,Item bar)
{
    // printf("Comparing %d with %d returning %d\n",foo.w,bar.w,(foo.w<bar.w?-1:(foo.w>bar.w?1:0)) ); */
    int rc = stricmp(foo.filename,bar.filename);
    if (rc != 0)
        return rc;
    else
    {
        if (SMUDGE(foo.size)==SMUDGE(bar.size)) return 0;
        else if (foo.size < bar.size) return -1;
        else return 1;
    }
}


int Item::compare(Item *foo,Item *bar)
{
    int rc = stricmp(foo->filename,bar->filename);
    if (rc != 0)
        return rc;
    else
    {
        if (SMUDGE(foo->size)==SMUDGE(bar->size)) return 0;
        else if (foo->size < bar->size) return -1;
        else return 1;
    }
}

void Item::setSize(unsigned long s)
{
    size = s;
}



char *Item::getFilename()
{
    return filename;
}

unsigned long Item::getSize()
{
    return size;
}

char *Item::getTimeStamp()
{
    time_t t;
    struct tm *timeptr;
    static char stamp[100];
    
    t = time(NULL);
    timeptr = localtime(&t);
    
    strftime(stamp,sizeof(stamp)-1,"%d%b%Y",timeptr);
    if (stamp[0] == ' ')
        stamp[0] = '0';
    
    return stamp;
    
}

Item::Item(unsigned long s,char *f)
{
//    for (int i=0; i<80; i++)
//    {
//        buffer3[i]=buffer2[i]=buffer1[i]='!';
//    }
    
    size = s;
    if (f != NULL)
    {
        filename = new char[1+strlen(f)];
        if (filename == NULL)
        {
            log("FATAL - PANIC - failed to allocate %d bytes of memory",1+strlen(f));
            exit(2);
        }

        strcpy(filename,f);
    }
    else
    {
        filename = NULL;
    }
    
#ifdef DEBUG
#ifndef NOPRINT
    fprintf(stderr,"New Item[%p](unsigned long s=%lu char *f=\"%s\")\n",this,s,(f?f:"<null>"));
    fprintf(stderr,"\tfilename = \"%s\"\n",(filename?filename:"<null>"));
    fprintf(stderr,"\tsize     = \"%lu\"\n",size);
    fflush(stderr);
#endif
#endif // DEBUG
    
    
}

Item::~Item(void)
{
#ifdef DEBUG
#ifndef NOPRINT
    fprintf(stderr,"Delete Item[%p](void)\n",this);
    fprintf(stderr,"\tfilename = \"%s\"\n",(filename?filename:"<null>"));
    fprintf(stderr,"\tsize     = \"%lu\"\n",size);
    fflush(stderr);
#endif
#endif // DEBUG
    
    if (filename != NULL) delete[] filename;
}

void Item::validate()
{
/*
    int j;
    int error=0;

    if (this == NULL)
    {
        printf("Validate called on NULL item\n");
    }
    if (buffer1[0]!='!' || buffer2[0]!='!' || buffer3[0]!='!')
        for (int i=0; i<80; i++)
        {
            if (buffer1[i] != '!' || buffer2[i] != '!' ||buffer3[i] != '!')
            {
                printf("\n%c[31mBuffer1/2/3 overwrite!%c\n",27,7);
                printf("buffer1\n");
                for (j=0; j<70; j++)
                    printf("%c",buffer1[j]);
                printf("\n");
                printf("buffer2\n");
                for (j=0; j<70; j++)
                    printf("%c",buffer2[j]);
                printf("\n");
                printf("buffer3\n");
                for (j=0; j<70; j++)
                    printf("%c",buffer3[j]);
                printf("%c[0m\n",27);
                error++;
            }
            
            if (error> 0) exit(2);
        }
*/
}
