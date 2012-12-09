#include "mime.h"



Mime::Mime()
{
    num_parts = 0;
    num_boundary = 0;
}

Mime::~Mime()
{
    int i;

    for (i=0; i<num_parts; i++)
    {
        if (encoding[i]) { delete[] encoding[i]; encoding[i]=NULL; }
        if (filename[i]) { delete[] filename[i]; filename[i]=NULL; }
        if (content[i])  { delete[] content[i];  content[i]=NULL;  }
        if (data[i])     { delete[] data[i];     data[i]=NULL;     }
    }

    for (i=0; i<num_boundary; i++)
    {
        if (boundary[i]) { delete[] boundary[i]; boundary[i]=NULL; }
    }
}

int Mime::countBoundaries(char *block)
{
    char *ptr;
    int tally = 0;

    if (num_boundary > 0)
    {
        ptr = nextBoundary(block);
        while (ptr != NULL)
        {
            tally++;
            ptr = nextBoundary(ptr+1);
        }
    }

    return tally;

}

void Mime::findBoundary(char *block)
{
    char *ptr,*eoln;

    // First thing: is there a boundary tag boundary="-----------sefsfejkdhsf"
    if (block == NULL || *block == '\0')
        ptr = NULL;
    else
        ptr = strstr(block,"boundary=");

    if (ptr)
    {
        while (ptr)
        {
            ptr += strlen("boundary=");
            if (*ptr == '\"')
            {
                ptr++;
                eoln = strchr(ptr+1,'\"');
            }
            else
                eoln = nextEoln(ptr);

            if (eoln)
            {
                char keep = *eoln;
                *eoln = '\0';
                boundary[num_boundary] = new char[strlen(ptr)+3];
                strcpy(boundary[num_boundary],"--");
                strcat(boundary[num_boundary],ptr);
                num_boundary++;
                *eoln = keep;
            }
            ptr = strstr(ptr,"boundary=");
        }
    }
    else  // sometimes no boundary definiton, but look like "-----------<SOMETHING>"
    {   // KT - can't search UU for "--" it can be part of encoding text
        if (block == NULL || *block == '\0' || strstr(block,"uuencode") != NULL)
            ptr = NULL;
        else
            ptr = strstr(block,"--");
/*
 - 22/11/98: Modidifed to handle multiple boundaries from multipart-posts
             when several complete articles concatenated together.
             relies on end-of-boundary that is boundary with '--' appended

        if (ptr)
        {
            eoln = nextEoln(ptr);

            if (eoln)
            {
                char keep = *eoln;
                *eoln = '\0';
                boundary[0] = dup(ptr);
                *eoln = keep;
                num_boundary = 1;

            }
        }
*/
        char *end_boundary = new char[4096];
        while (ptr)
        {
            eoln = nextEoln(ptr);

            if (eoln)
            {
                char keep = *eoln;
                *eoln = '\0';

                boundary[num_boundary] = dup(ptr);
                // make string for end-of-section
                strcpy(end_boundary,ptr);
                strcat(end_boundary,"--");

                *eoln = keep;
                num_boundary++;
            }

            ptr = strstr(ptr,end_boundary);
            if (ptr)
            {
                ptr += strlen(end_boundary);
                ptr = strstr(ptr,"--");
            }

        }
        delete[] end_boundary;
    }
//    for (i=0; i<num_boundary; i++)
//        printf("Boundary[%d] = \"%s\"\n",i,boundary[i]);

}

void Mime::deQuote(char *s)
{
    if (*s == '\"')
    {
        char *x = dup(s);
        strcpy(x,s+1);
        char *ptr = strchr(x,'\"');
        if (ptr) *ptr = '\0';
        strcpy(s,x);
        delete[] x;
    }
}

int Mime::write(char *filename,int size,char *block)
{
    int rc = 0;

//    printf("Write(%s,%d,%p)\n",filename,size,block);

#ifdef UNIX
    FILE *out = fopen(filename,"w");
#else
    FILE *out = fopen(filename,"wb");
#endif
    if (out)
    {
        rc = fwrite(block,1,size,out);
        fclose(out);
    }
    return rc;
}


int Mime::preScan(char *block)
{
    int   num_boundaries;
    int   i;
    char *section,*next;
    char  ch;

    findBoundary(block);
    num_boundaries = countBoundaries(block);

    if (num_boundaries == 0)
    {
        num_parts = 1;
        scan(0,block);
        cleanFilename(0);
//        printf("   Filename is noe \"%s\"\n",filename[0]);
//        printf("   DataSize = %d\n",data_size[0]);

//        if (data_size[0] > 2)
//            write(filename[0],data_size[0],data[0]);
    }
    else
    {
        num_parts = num_boundaries;
        section = nextBoundary(block);
        for (i=0; i<num_boundaries; i++)
        {
            // cut out block
            if (section)
                next = nextBoundary(section+1);
            else
                next = NULL;

            if (next)
            {
                ch = *next;
                *next = '\0';
            }

            if (section)
            {
                scan(i,section);
            }
            cleanFilename(i);

//            printf("   DataSize = %d\n",data_size[i]);

//            if (data_size[i] > 2)
//                write(filename[i],data_size[i],data[i]);

            if (next)
            {
                *next = ch;
            }
            section = next;
        }
    }

    return num_boundaries;
}

void Mime::scan(int index,char *block)
{
    char *ptr,*eoln,*otr;

    encoding[index] = NULL;
    filename[index] = NULL;
    content[index]  = NULL;
    data[index]     = NULL;
    data_size[index]= 0;

    // two dashes after boundary indicates EOF
/*
    if (num_boundary > 0)
    {
        ptr = block + strlen(boundary);
        if (*ptr)
        {
            if (*(ptr+1) == '-')
                if (*(ptr+2) == '-')
                    return;  // '--' after boundary is ende
        }
    }
*/
//    printf("Section %d\n",index);

    // Be sure Content tokens are all on the same line
    //    Content-Type: application/octet-stream;
    //    name="s_alp302.zip"
    // Should be
    // Content-Type: application/octet-stream; name="s_alp302.zip"


    ptr = strstr(block,"Content-");
    while (ptr)
    {
        eoln = nextEoln(ptr);
        otr = strstr(ptr,";\r\n");
        while (otr && eoln && otr+1 == eoln)   // next token should be on this line
        {
            *(otr+1)=' ';
            *(otr+2)=' ';
            eoln = nextEoln(eoln);
            otr = strstr(otr,";\r\n");
        }
        if (!eoln || !otr)
            break;
        ptr = strstr(eoln,"Content-");
    }




    // Content-Transfer-Encoding: 7bit
    ptr = strstr(block,"Content-Transfer-Encoding: ");
    if (ptr)
    {
        ptr += strlen("Content-Transfer-Encoding: ");
        eoln = nextEoln(ptr);
        if (eoln)
        {
            char keep = *eoln;
            *eoln = '\0';
            encoding[index] = dup(ptr);
            *eoln = keep;

//            printf("   encoding = \"%s\"\n",encoding[index]);
        }
    }

    // Content-Type: text/html; charset=us-asci; name="sdjnsdfjn.sadf"
    ptr = strstr(block,"Content-Type: ");
    if (ptr)
    {
        ptr += strlen("Content-Type: ");
        eoln = nextEoln(ptr);
        if (eoln)
        {
            char keep = *eoln;
            *eoln = '\0';
            content[index] = dup(ptr);
            otr = nextSemi(content[index]);
            if (otr)
                *otr = '\0';
            deQuote(content[index]);

//            printf("   content = \"%s\"\n",content[index]);

            ptr = strstr(ptr,"name=");
            if (ptr)
            {
    // add extra space to account for future filename mangles
    filename[index] = new char[64+strlen(ptr+strlen("name="))];
    strcpy(filename[index],ptr+strlen("name="));
                //filename[index] = dup(ptr+strlen("name="));
                deQuote(filename[index]);
//                printf("   filename = \"%s\"\n",filename[index]);
            }

            *eoln = keep;
        }
    }

    // Content-Disposition: inline; filename="sdjnsdfjn.sadf"
    ptr = strstr(block,"Content-Disposition: ");
    if (ptr)
    {
        ptr += strlen("Content-Disposition: ");
        eoln = nextEoln(ptr);
        if (eoln)
        {
            char keep = *eoln;
            *eoln = '\0';
            char *fptr = strstr(ptr,"filename=");
            if (fptr)
            {
                fptr += strlen("filename=");
                if (filename[index])
                    delete[] filename[index];

    // allow extra space in filename for future mangles
    filename[index] = new char[strlen(fptr)+64];
    strcpy(filename[index],fptr);
                //filename[index] = dup(fptr);

                deQuote(filename[index]);
//                printf("   filename = \"%s\"\n",filename[index]);
            }
            *eoln = keep;
        }
    }

    // Find start of article
    // Idea:  seems to be after any boundary, and 1 blank after Content* lines
    ptr = block;
    if (num_boundary > 0)
        ptr = nextLine(block);
    data[index] = dup(ptr);
    if (ptr)
    {
        // Should be a blank line after header but before encoded data
        // Zero in on the header
        ptr = strstr(ptr,"Content-");
        // Then find next blank line
        if (ptr)
            ptr = strstr(ptr,MIME_EOLN MIME_EOLN);

        if (ptr)
        {
            // skip the blank lines
            while (ptr && strncmp(ptr,MIME_EOLN,strlen(MIME_EOLN)) == 0)
                ptr = nextLine(ptr);

            if (ptr)
            {
                delete[] data[index];
                data[index] = dup(ptr);
            }
        }
    }

    // If we have enough to produce output decode it, else free it.
    if (encoding[index] != NULL)
    {
//        for (int i=0; i<60; i++)
//        {
//            if (data[index][i] == '\0') break;
//            printf("%c",data[index][i]);
//        }
//        printf("\n");

        // convert to output format, decoding if necessary
        if (determineType(index) == MIME_BASE64)
        {
            B64Decode(index);
        }
        else if (determineType(index) == MIME_UU)
        {
            UUDecode(index);
        }
        else if (determineType(index) == MIME_BINHEX)
        {
            BinHexDecode(index);
        }
        else
        {
            data_size[index] = strlen(data[index]);
        }
    }
    else
    {
        delete[] data[index];
        data[index] = NULL;
        data_size[index] = 0;
    }

}



char *Mime::nextEoln(char *block)
{
    if (!block) return NULL;
    return strstr(block,MIME_EOLN);
}

char *Mime::nextLine(char *block)
{
    if (!block) return NULL;
    char *p = nextEoln(block);
    if (p) return p+strlen(MIME_EOLN);
    else return NULL;
}

char *Mime::nextSemi(char *block)
{
    if (!block) return NULL;
    return strchr(block,';');
}

char *Mime::nextBoundary(char *block)
{
    if (!block) return NULL;
    if (num_boundary == 0) return NULL;

    int i;

    array<char *> ptr;

    // find all next boundaries
    for (i=0; i<num_boundary; i++)
    {
        if (strlen(boundary[i]) > strlen(block))
            ptr[i] = NULL;
        else
            ptr[i] = strstr(block,boundary[i]);
    }

    // want first one that's not null
    // i.e.: first boundary in the string, so lowest pointer
    char *eob = block+strlen(block);
    char *rc = eob;  // make biggest
    for (i=0; i<num_boundary; i++)
    {
        if (ptr[i] != NULL)
        {
            if (ptr[i] < rc)
                rc = ptr[i];
            ptr[i] = NULL;
        }
    }

    if (rc != eob)
        return rc;
    else
        return NULL;
}

char *Mime::dup(char *s)
{
    char *x = NULL;
    if (s)
    {
        x = new char[1+strlen(s)];
        strcpy(x,s);
    }
    return x;
}

int Mime::determineType(int index)
{
    if (encoding[index] == NULL || encoding[index][0]=='\0')
    {
        return MIME_UNKNOWN;
    }
    else if (strstr(encoding[index],"binhex") != NULL)
        return MIME_BINHEX;
    else if (strstr(encoding[index],"uu") != NULL)
        return MIME_UU;
    else if (strstr(encoding[index],"7") != NULL)
        return MIME_7BIT;
    else if (strstr(encoding[index],"64") != NULL)
        return MIME_BASE64;
    else if (strstr(encoding[index],"8") != NULL)
        return MIME_8BIT;
    else if (strstr(encoding[index],"quot") || strstr(encoding[index],"QUOT"))
        return MIME_QUOTED;  // quoted printable
    else if (strstr(encoding[index],"bin") || strstr(encoding[index],"BIN"))
        return MIME_BINARY;
    else
        return MIME_UNKNOWN;

}

void cleanFilename(char *filename)
{
    if (filename != NULL && filename[0] != '\0')
    {
        char *copy = new char[128+strlen(filename)];
        *copy = '\0';
        char *ptr = filename;
        while (*ptr && isspace(*ptr)) ptr++;

        // remove any quotes, so "xyz.ttf" doan become _xyz.ttf_
        char *cursor = copy;
        while (*ptr != '\0')
        {
            if (*ptr != '\"')
            {
                *cursor = *ptr;
                cursor++;
                *cursor = '\0';
            }
            ptr++;
        }
        //strcpy(copy,ptr);

        // remove trailing spaces
        if (*copy)
        {
            ptr = copy+strlen(copy)-1;
            while (ptr > copy)
            {
                if (isspace(*ptr)) *ptr='\0';
                else break;
                ptr--;
            }
        }

        // common to get filenames C:\sfsf\sadfasdf\xyz.jpg
        // make these to be like xyz.jpg
        if (*copy)
        {
            ptr = copy+strlen(copy)-1;
            while (ptr > copy)
            {
                // last '//' but not if it gives us empty string
                if (*ptr == '\\' && *(ptr+1) != '\0')
                {
                    char *post_slash = new char[1+strlen(ptr+1)];
                    strcpy(post_slash,ptr+1);
                    strcpy(copy,post_slash);
                    delete[] post_slash;
                    break;
                }
                ptr--;
            }
        }

        // remove illegal filename characters
        ptr = copy;
        while (*ptr)
        {
            if (*ptr == '\"' || *ptr == 47  || *ptr == ':'  || *ptr == '&' ||
                *ptr == ';'  || *ptr == '/' || *ptr == '\\' || *ptr == '<' ||
                *ptr == '>'  || *ptr == '%' || *ptr == ','  || *ptr == '*' ||
                *ptr == '='  || *ptr == '|' || *ptr == '?'  || *ptr == '~')
                *ptr = '_';
            ptr++;
        }

        strcpy(filename,copy);
        delete[] copy;
    }

}

void Mime::cleanFilename(int index)
{
    // make sure filename is valid
    if (filename[index] != NULL && filename[index][0] != '\0')
    {
        ::cleanFilename(filename[index]);
    }
    else
    {
        // htere is no filename, create one
        if (filename[index] != NULL)
        {
            delete[] filename[index];
            filename[index] = NULL;
        }

        filename[index] = new char[128+strlen(BASE_FILENAME)];
        strcpy(filename[index],BASE_FILENAME);
    }

    // if there is no file type extension, try to make one
    if (strchr(filename[index],'.') == NULL)
    {
        // try to determine some credible extension
        if (content[index] != NULL && strchr(content[index],'/'))
        {
            char *ptr = strchr(content[index],'/');
            if (ptr)
            {
                ptr++;
                if (strcmp(ptr,"plain") == 0)
                    strcat(filename[index],".txt");
                else
                {
                    strcat(filename[index],".");
                    strcat(filename[index],ptr);
                }
            }
            else
                strcat(filename[index],".unk");
        }
        else
        {
            int ftype = determineType(index);
            if (ftype == MIME_BINARY || ftype == MIME_8BIT)
                strcat(filename[index],".bin");
            else if (ftype == MIME_7BIT || ftype == MIME_QUOTED)
            {
                if (data[index] && data[index][0])
                {
                    if (data[index][0]=='<' && data[index][1]=='H')
                        strcat(filename[index],".html");
                    else
                        strcat(filename[index],".txt");
                }
                else
                {
                    strcat(filename[index],".txt");
                }
            }
            else if (ftype == MIME_BASE64)
            {

                // 3 valid chars, short circuit checked
                if (data[index][0] && data[index][1] && data[index][2])
                {
                    if (strncmp(data[index],"/9j",3) == 0)
                        strcat(filename[index],".jpg");

                    else if (strncmp(data[index],"SUk",3) == 0)
                        strcat(filename[index],".tiff");

                    else if (strncmp(data[index],"R0l",3) == 0)
                        strcat(filename[index],".gif");

                    else if (strncmp(data[index],"PGh",3) == 0)
                        strcat(filename[index],".html");

                    else if (strncmp(data[index],"Qk0",3) == 0)
                        strcat(filename[index],".bmp");

                    else if (strncmp(data[index],"UEs",3) == 0)
                        strcat(filename[index],".zip");
                }
            }
            else
            {
                strcat(filename[index],".unk");
            }
        }
    }
}

int Mime::B64Lookup(char ch)
{
    int x = (int)ch;

    if (x >= 'A' && x <= 'Z') return x-65;
    if (x >= 'a' && x <= 'z') return x-71;
    if (x >= '0' && x <= '9') return x+4;
    if (x == '+') return 62;
    if (x == '/') return 63;

    return -1;
}

char *Mime::nextValidB64Code(char *ptr,int &code)
{
    while(1)
    {
        if (*ptr == '\0' || *ptr == '=')
        {
            code = 0;
            break;
        }
        code = B64Lookup(*ptr);
        if (code != -1)
            break;

        ptr++;
    }

    return ptr;
}

/*
static char binhex_decode[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, -1, -1,
    13, 14, 15, 16, 17, 18, 19, -1, 20, 21, -1, -1, -1, -1, -1, -1,
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, -1,
    37, 38, 39, 40, 41, 42, 43, -1, 44, 45, 46, 47, -1, -1, -1, -1,
    48, 49, 50, 51, 52, 53, 54, -1, 55, 56, 57, 58, 59, 60, -1, -1,
    61, 62, 63, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
*/

void Mime::BinHexDecode(int index)
{
    // BinHex4 has a header
    char  binhex_filename_length;
    char  binhex_filename[64];
    long  binhex_data_length;
//u    char  binhex_version;
//u    long  binhex_type;
//u    long  binhex_creator;
//u    short binhex_flags;
//u    long  binhex_resource_length;
//u    short binhex_crc1;
//u    char* binhex_data_fork;
//    short binhex_crc2;
//    char* binhex_resource_fork;
//    short binhex_crc3;


    char *ptr;

    if (data[index] == NULL)
        return;

    ptr = data[index];
    int datalen = strlen(ptr);
    char *ende = ptr+datalen;

    char *output = new char[datalen+128];

    // find the length of the filename block
    if (ptr < ende)
    {
        binhex_filename_length = *ptr;
        ptr++;
    }
    // extract the filename block
    if (ptr < ende)
    {
        memset(binhex_filename,0,sizeof(binhex_filename));
        memcpy(binhex_filename,ptr,(int)(binhex_filename_length));
        ptr += (int)binhex_filename_length;
//        if (filename[index] != NULL)
//            delete[] filename[index];
//        filename[index] = new char[128 + (int)binhex_filename_length];
//        strcpy(filename[index],binhex_filename);
    }
    // extract the version
    ptr++;
    // extract the type
    ptr += sizeof(long);
    // extract the creator
    ptr += sizeof(long);
    // extract the flags
    ptr += sizeof(short);
    // extract the length of the data fork
    if (ptr < ende)
    {
        memcpy(&binhex_data_length,ptr,sizeof(long));
        ptr += sizeof(long);
    }
    // extract the length of resource fork
    ptr += sizeof(long);
    // extract the CRC
    ptr += sizeof(short);

    // now decode the data block
    if (ptr < ende)
    {
        strcpy(output,data[index]);
    }

    delete[] data[index];
    data[index] = NULL;

    // was anything decoded ?
    if (datalen > 0)
    {
        data[index] = new char[128+datalen];
        data_size[index] = datalen; // (0 to bytes-1)
        ptr = data[index];

        memcpy(ptr,output,datalen);
    }

    delete[] output;

}




void Mime::B64Decode(int index)
{
    char *ptr,*eostr;
    int   code,octet1,octet2,octet3;
    int   bytes = 0;
//    array<char> output;

    if (data[index] == NULL)
        return;

    ptr = data[index];
    int datalen = strlen(ptr);
    eostr = ptr + datalen;

    // idea! -> output size cant possibly be bigger than the input
    // is should always be 75% times input size.  So allocate that
    // amount of memory.

    char *output = new char[datalen];  // actually allocate 25% too much... cest la vie

    while (*ptr && *ptr != '=')
    {
        octet1 = octet2 = octet3 = 0;

        ptr = nextValidB64Code(ptr,code);
        octet1 = (code & 0x3f) << 2;

        if (ptr >= eostr || *ptr == '\0') break;
        ptr++;
        if (ptr >= eostr || *ptr == '\0') break;

        ptr = nextValidB64Code(ptr,code);
        octet1 += (code & 0x30) >> 4;
        octet2  = (code & 0x0f) << 4;

        if (ptr >= eostr || *ptr == '\0') break;
        ptr++;
        if (ptr >= eostr || *ptr == '\0') break;

        ptr = nextValidB64Code(ptr,code);
        octet2 += (code & 0x3c) >> 2;
        octet3  = (code & 0x03) << 6;

        if (ptr >= eostr || *ptr == '\0') break;
        ptr++;
        if (ptr >= eostr || *ptr == '\0') break;

        ptr = nextValidB64Code(ptr,code);
        octet3 += (code & 0x3f);

        // have 3 bytes in octet1, 2 & 3.
        output[bytes] = octet1;
        bytes++;
        output[bytes] = octet2;
        bytes++;
        output[bytes] = octet3;
        bytes++;

        if (ptr >= eostr || *ptr == '\0') break;
        ptr++;
    }

    delete[] data[index];
    data[index] = NULL;

    // was anything decoded ?
    if (bytes > 0)
    {
        data[index] = new char[bytes];
        data_size[index] = bytes; // (0 to bytes-1)
        ptr = data[index];
//        for (int i=0; i<bytes; i++, ptr++)
//        {
//            *ptr = output[i];
//        }

        memcpy(ptr,output,bytes);
    }

    delete[] output;

}


int Mime::UULookup(char x)
{
    if (x == '`' || x == ' ') return 0;
    if (x >= '!' && x <= '_') return x-32;

    return -1;
}

char *Mime::nextValidUUCode(char *ptr,int &code)
{
    if (*ptr == '\0')
    {
        code = 0;
    }
    else
    {
        code = UULookup(*ptr);
    }

    return ptr;
}

#ifndef DEC
#define DEC(c) (((c) - ' ') & 077)
#endif


void Mime::UUDecode(int index)
{
    char *ptr,*eoln,keep;
//    int   i,code,octet1,octet2,octet3;
    int   bytes = 0;

    // array<char> output;

    if (data[index] == NULL || !data[index][0])
        return;

    ptr = data[index];

    // idea! -> output size cant possibly be bigger than the input
    // is should always be 75% times input size.  So allocate that
    // amount of memory.

    long datalen = strlen(ptr);
    char *output = new char[datalen];  // actually allocate 25% too much... cest la vie

    // begin 644 fikename.ext
    ptr = strstr(data[index],"begin ");
    if (!ptr)
        return;

    log("   MIME-UUDecoding...");

    ptr = nextEoln(ptr);
    if (!ptr)
        return;

    ptr+=strlen(MIME_EOLN);

    // should be now pointing at start of encoded data




    char *buf;
    char *bp;
    int n, expected, actual;


    while (1)
    {
        /* for each input line */
        do
        {
           eoln = nextEoln(ptr);
           if (eoln)
           {
               keep = *eoln;
               *eoln = '\0';
           }
           else
           {
               n=0;
               break;
           }

            n=0;
           if (*ptr == '\0')
               break;

           buf = ptr;
           n = DEC(buf[0]);

           /* Calculate expected # of chars and pad if necessary */
           expected = ((n+2)/3)<<2;

           actual = strlen(buf)-1;

            //   for (i = strlen(buf)-1; i <= expected; i++) buf[i] = ' ';

            if (actual != expected)
            {
                // bad data, but try to wing it.
                // me suspects that some uuencoder dont encode the last line
                // properly if data is not an even 3 bytes, so pad with zeros
                if (!strstr(buf,"CUT"))  // common to get lines ---CUT-HERE----
                {
                    if (actual < expected)
                    {
                        if (expected - actual == 1)
                        {
             //               log("PATCHING.");
                            strcat(buf," ");
                            break;
                        }
                        else if (expected - actual == 2)
                        {
               //             log("PATCHING..");
                            strcat(buf,"  ");
                            break;
                        }
                        else if (actual - expected < 3)
                        {
                            // too many bytes, but not a huge difference, use anyway
                            log("   Too much data, %d octets, should be %d, using anyway",actual,expected);
                            log("   Data: %s",buf);
                            break;
                        }
                    }
                }
                // Illegal line length, skip it
                log("Corrupt binary data, %d octets, should be %d.",actual,expected);
                log("Data: %s",buf);
            }

        } while (expected != actual);

        if ((n <= 0) || (buf[0] == '\n'))
           break;

        bp = &buf[1];
        char *p;
        while (n > 0)
        {
            p = bp;
            {
                int c1, c2, c3;

                c1 = DEC(*p) << 2 | DEC(p[1]) >> 4;
                c2 = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
                c3 = DEC(p[2]) << 6 | DEC(p[3]);
                if (n >= 1)
                {
                    output[bytes] = (char)c1;
                    bytes++;
                }
                if (n >= 2)
                {
                    output[bytes] = (char)c2;
                    bytes++;
                }
                if (n >= 3)
                {
                    output[bytes] = (char)c3;
                    bytes++;
                }
            }
            bp += 4;
            n -= 3;
        }

        *eoln = keep;
        ptr = eoln+strlen(MIME_EOLN);
        if (*ptr == '\0') break;


    }




/*
    while (*ptr)
    {
        eoln = nextEoln(ptr);
        if (eoln)
        {
            keep = *eoln;
            *eoln = '\0';

            line_length = eoln-ptr;

            octets = UULookup(*ptr);
            ptr++;
            if (octets > 0 && octets == (3*(line_length-1)/4))
            {
                for (i=0; i<octets; i+=3)
                {
                    octet1 = octet2 = octet3 = 0;

                    ptr = nextValidUUCode(ptr,code);
                    octet1 = (code & 0x3f) << 2;

                    if (*ptr == '\0') break;
                    ptr++;
                    if (*ptr == '\0') break;

                    ptr = nextValidUUCode(ptr,code);
                    octet1 += (code & 0x30) >> 4;
                    octet2  = (code & 0x0f) << 4;

                    if (*ptr == '\0') break;
                    ptr++;
                    if (*ptr == '\0') break;

                    ptr = nextValidUUCode(ptr,code);
                    octet2 += (code & 0x3c) >> 2;
                    octet3  = (code & 0x03) << 6;

                    if (*ptr == '\0') break;
                    ptr++;
                    if (*ptr == '\0') break;

                    ptr = nextValidUUCode(ptr,code);
                    octet3 += (code & 0x3f);

                    // have 3 bytes in octet1, 2 & 3.
                    output[bytes] = octet1;
                    bytes++;
                    output[bytes] = octet2;
                    bytes++;
                    output[bytes] = octet3;
                    bytes++;

                    if (*ptr == '\0') break;
                    ptr++;
                    if (*ptr == '\0') break;
                }
            }
            else
            {
//                printf("Octets == %d, should be %d\n",octets,3*(line_length-1)/4);
            }
        }
        else
        {
            break;  // can't find next line??!
        }

        *eoln = keep;
        ptr = eoln+strlen(MIME_EOLN);


    }
*/

    delete[] data[index];
    data[index] = NULL;

    // was anything decoded ?
    if (bytes > 0)
    {
        data[index] = new char[bytes];
        data_size[index] = bytes; // (0 to bytes-1)
        ptr = data[index];
///        for (int i=0; i<bytes; i++, ptr++)
//        {
//            *ptr = output[i];
//        }

        memcpy(ptr,output,bytes);
    }

    delete[] output;
}
