#include "strutils.h"

// check the path given in <path> is writable
int checkDir(char *path)
{
    if (!*path) return 0;

    int len = strlen(path);
    if (path[len-1] != '\\')
        strcat(path,"\\");

    char *filename = new char[strlen(path)+128];
    sprintf(filename,"%s_NewsHarvest_$$$",path);
    FILE *out=fopen(filename,"w");
    if (out)
    {
        fclose(out);
        deleteFile(filename);
        delete[] filename;
        return 1;
    }
    else
    {
        delete[] filename;
        return 0;
    }
}



char *trimCR(char *buffer)
{
    char *ptr = strchr(buffer,'\r');
    if (ptr) *ptr = '\0';
    else
    {
        ptr = strchr(buffer,'\n');
        if (ptr) *ptr = '\0';
    }

    return buffer;
}

// strdup
char *newStrDup(char *s)
{
    char *buffer = new char[1+strlen(s)];
    strcpy(buffer,s);

    return buffer;
}

// given a string, remove anything other than 0-9 from it.
// If any characters were removed return 1, else return 0
int makeDigitsOnly(char *buffer)
{
    // anythng to do ?
    if (buffer == NULL || *buffer=='\0')
        return 0;


    int   length = 1+strlen(buffer);
    int   rc = 0;
    char *ptr = buffer;
    char *tmp = new char[length];
    char *cur = tmp;

    *tmp = '\0';
    while (*ptr)
    {
        if (*ptr >= '0' && *ptr <= '9')
        {
            *cur = *ptr;
            cur++;
            *cur='\0';
        }
        else
        {
            rc = 1;  // we filted out a non-digit
        }
        ptr++;
    }

    if (rc)
    {
        strcpy(buffer,tmp);
    }

    delete[] tmp;

    return rc;
}

// given a string, remove anything other than [-]0-9 from it.
// If any characters were removed return 1, else return 0
int makeNumberOnly(char *buffer)
{
    // anythng to do ?
    if (buffer == NULL || *buffer=='\0')
        return 0;


    int   length = 1+strlen(buffer);
    int   rc = 0;
    char *ptr = buffer;
    char *tmp = new char[length];
    char *cur = tmp;

    *tmp = '\0';
    while (*ptr)
    {
        if ((*ptr >= '0' && *ptr <= '9') || *ptr == '-')
        {
            // minus is ok if it's the 1st char
            if (*ptr == '-')
            {
                if (cur == tmp)   // if we aint found anything valid yet
                {
                    *cur = *ptr;
                    cur++;
                    *cur='\0';
                }
                else  // only accept a minus as first char in number
                {
                    rc = 1;
                }
            }
            else
            {
                *cur = *ptr;
                cur++;
                *cur='\0';
            }
        }
        else
        {
            rc = 1;  // we filted out a non-digit
        }
        ptr++;
    }

    if (rc)
    {
        strcpy(buffer,tmp);
    }

    delete[] tmp;

    return rc;
}

// given a string, lower case all letters, remove anything except
// [A-z].[0-9]
// If any characters were removed return 1, else return 0
int makeNewsGroupNameValidOnly(char *buffer)
{
    // anythng to do ?
    if (buffer == NULL || *buffer=='\0')
        return 0;


    int   length = 1+strlen(buffer);
    int   rc = 0;
    char *ptr = buffer;
    char *tmp = new char[length];
    char *cur = tmp;

    *tmp = '\0';
    while (*ptr)
    {
        if ((*ptr >= 'A' && *ptr <= 'Z') ||
            (*ptr >= 'a' && *ptr <= 'z') ||
            (*ptr >= '0' && *ptr <= '9') ||
            (*ptr == '.'))
        {
            if (*ptr >= 'A' && *ptr <= 'Z')
            {
                *ptr = tolower(*ptr);
                rc = 1;
            }
            *cur = *ptr;
            cur++;
            *cur='\0';
        }
        else
        {
            rc = 1;  // we filted out a non-digit
        }
        ptr++;
    }

    if (rc)
    {
        strcpy(buffer,tmp);
    }

    delete[] tmp;

    return rc;
}

/*
 * Given a char* string, remove all spaces inside.
 * just spaces, NOT _whitespace_
 *
 */
int removeSpaces(char *buffer)
{
    // anythng to do ?
    if (buffer == NULL || *buffer=='\0')
        return 0;


    int   length = 1+strlen(buffer);
    int   rc = 0;
    char *ptr = buffer;
    char *tmp = new char[length];
    char *cur = tmp;

    *tmp = '\0';
    while (*ptr)
    {
        if (*ptr != ' ')
        {
            *cur = *ptr;
            cur++;
            *cur='\0';
        }
        else
        {
            rc = 1;  // we filted out a space
        }
        ptr++;
    }

    if (rc)
    {
        strcpy(buffer,tmp);
    }

    delete[] tmp;

    return rc;
}


int makeLowerCase(char *buffer)
{
    char *ptr = buffer;
    int rc = 0;

    while (*ptr)
    {
        if (isupper(*ptr))
        {
            rc = 1;
            *ptr = tolower(*ptr);
        }
        ptr++;
    }
    return rc;
}

int makeUpperCase(char *buffer)
{
    char *ptr = buffer;
    int rc = 0;

    while (*ptr)
    {
        if (islower(*ptr))
        {
            rc = 1;
            *ptr = toupper(*ptr);
        }
        ptr++;
    }

    return rc;
}


// given a string, remove any left or right blocks of whitespace
// return 1 if changes were made, 0 otherwise
int lrSpaceTrim(char *buffer)
{
    // anythng to do ?
    if (buffer == NULL || *buffer=='\0')
        return 0;

    int   length = 1+strlen(buffer);
    int   rc = 0;
    char *tmp = new char[length];
    char *cur;
    char *ptr = tmp;

    strcpy(tmp,buffer);

    // skip any leading whitespace
    while (*ptr && isspace(*ptr)) { ptr++; rc = 1; }
    cur = ptr;

    // skip any trailing whitespace
    ptr = tmp+length-2;          // point to last char before \0
    while (ptr > cur && isspace(*ptr)) { *ptr='\0'; ptr--; rc = 1; }

    if (ptr == cur && isspace(*ptr))
    {
        *ptr = '\0';
        rc = 1;
    }

    // were any changes made ?
    if (rc)
    {
        strcpy(buffer,cur);
    }

    delete[] tmp;

    return rc;
}

#ifdef UNIX

/*
 * Case independant strcmp()
 *
 */
int stricmp(const char *x,const char *y)
{
    char *lowerx,*lowery;
    int rc = 0;

    lowerx = new char[1+strlen(x)];
    strcpy(lowerx,x);
    makeLowerCase(lowerx);
    lowery = new char[1+strlen(y)];
    strcpy(lowery,y);
    makeLowerCase(lowery);

    rc = strcmp(lowerx,lowery);

    delete[] lowerx;
    delete[] lowery;


    return rc;
}

#endif

/*
 * Case independant strstr()
 */
char *stristr(char *x,char *y)
{
    char *lowerx,*lowery;
    int delta = 0;

    lowerx = new char[1+strlen(x)];
    strcpy(lowerx,x);
    makeLowerCase(lowerx);
    lowery = new char[1+strlen(y)];
    strcpy(lowery,y);
    makeLowerCase(lowery);

    char *ptr = strstr(lowerx,lowery);
    if (ptr) delta = (int)(ptr-lowerx);

    delete[] lowerx;
    delete[] lowery;

    if (delta)
        return x+delta;
    else
        return NULL;
}

int singleSpace(char *s)
{
    int rc = 0;
    if (s)
    {
        int len = 1+strlen(s);
        char lastch=0;
        char *out = new char[len];
        memset(out,0,len);
        char *ptr1,*ptr2;
        ptr1 = s; ptr2 = out;
        while (*ptr1)
        {
            if (*ptr1 == ' ')
            {
                if (lastch == ' ')  // double space
                {
                    ptr1++;
                    rc++;
                }
                else
                {
                    *ptr2 = *ptr1;
                    ptr2++; ptr1++;
                }
            }
            else
            {
                *ptr2 = *ptr1;
                ptr2++;
                ptr1++;
            }
            lastch = *ptr1;
        }
        strcpy(s,out);
        delete[] out;
    }


    return rc;

}
