
/***********************************************************************

decoder.cpp,v 1.5 2003/12/16 03:56:02 root Exp

MIME and UU decoders

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2001 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	22 Jun 01 SHL - Drop dead code
		25 Mar 02 SHL - decode: report unknown decode type
		05 Dec 02 KRT - add yEnc support

***********************************************************************/


#include "decoder.h"

// Should we use the BinaryTree or Array
// to remember past decoded files?
// #define USE_BINARY_TREE_FILE_MEMORY

#ifdef UNIX
#define SEPERATOR_STR  "/"
#define SEPERATOR_CHAR '/'
#define WRITE_BINARY   "w"
#else
#define SEPERATOR_STR  "\\"
#define SEPERATOR_CHAR '\\'
#define WRITE_BINARY   "wb"
#endif




Decoder::Decoder(void)
{
    output_path = 0;
    exit_exe = 0;
    hist_size = 0;
    output_path = 0;
    save_text = 0;
    loadDecodes();

#ifdef DEBUG
#ifndef NOPRINT
    fprintf(stderr, "New Decoder[%p](void)\n", this);
    fprintf(stderr, "\toutput_path = \"%s\"\n", (output_path ? output_path : "<null>"));
    fprintf(stderr, "\texit_exe    = \"%s\"\n", (exit_exe ? exit_exe : "<null>"));
    fprintf(stderr, "\thist_size   = \"%d\"\n", hist_size);
    fflush(stderr);
#endif
#endif // DEBUG
}

Decoder::Decoder(char *path)
{
    output_path = 0;
    exit_exe = 0;
    hist_size = 0;
    save_text = 0;
    setOutputPath(path);
    loadDecodes();

#ifdef DEBUG
#ifndef NOPRINT
    fprintf(stderr, "New Decoder[%p](char *path = \"%s\")\n", this, (path ? path : "<null>"));
    fprintf(stderr, "\toutput_path = \"%s\"\n", (output_path ? output_path : "<null>"));
    fprintf(stderr, "\texit_exe    = \"%s\"\n", (exit_exe ? exit_exe : "<null>"));
    fprintf(stderr, "\thist_size   = \"%d\"\n", hist_size);
    fflush(stderr);
#endif
#endif // DEBUG

}

Decoder::~Decoder()
{

#ifdef DEBUG
#ifndef NOPRINT
    fprintf(stderr, "Delete Decoder[%p](void)\n", this);
    fprintf(stderr, "\toutput_path = \"%s\"\n", (output_path ? output_path : "<null>"));
    fprintf(stderr, "\texit_exe    = \"%s\"\n", (exit_exe ? exit_exe : "<null>"));
    fprintf(stderr, "\thist_size   = \"%d\"\n", hist_size);
    fflush(stderr);
#endif
#endif // DEBUG

    if (output_path)
        delete[]output_path;
    if (exit_exe)
        delete[]exit_exe;

    // show free decodes array/tree
#ifdef USE_BINARY_TREE_FILE_MEMORY
#else
    for(int i = 0; i < hist_size; i++)
    {
        if (history[i])
        {
            delete  history[i];
            history[i] = NULL;
        }
    }
#endif
}

void Decoder::loadDecodes(void)
{
    FILE   *in = fopen("decodes", "r");
#ifndef NOPRINT
    printf("Loading History   ");
#endif
    int lines = 0;
    if (in)
    {
#ifndef NOPRINT
        int c = 0;
#endif
        char buffer[1024];
        while (!feof(in))
        {
#ifndef NOPRINT
            c++;
            if (c == 0)
                printf("\b\b\b   ");
            else if (c == 100)
                printf("\b\b\b.  ");
            else if (c == 200)
                printf("\b\b\b.. ");
            else if (c == 300)
                printf("\b\b\b...");
            else if (c >= 400)
                c = 0;
            fflush(stdout);
#endif

            fgets(buffer, 1024, in);
            lines++;
            if (!feof(in))
            {
                char   *ptr = strchr(buffer, '\n');
                if (ptr)
                    *ptr = '\0';
                ptr = strchr(buffer, '\r');
                if (ptr)
                    *ptr = '\0';
                if (strlen(buffer) > 3)
                {
                    unsigned long size = (unsigned long)atol(buffer);
                    ptr = strchr(buffer, ' ');
                    if (ptr)
                        ptr = strchr(ptr + 1, ' ');
                    if (!ptr)
                    {
#ifndef NOPRINT
                        printf("Invalid entry in \"decodes\", line %d\n", lines);
#endif
                    }
                    else
                    {
                        Item   *i = new Item(size, ptr + 1);
                        if (i)
                        {
#ifdef USE_BINARY_TREE_FILE_MEMORY
                            decodes.add(i);
#else
                            history[hist_size] = i;
#endif
                            hist_size++;
                        }
                    }
                }
            }
        }
        fclose(in);
    }
#ifndef NOPRINT
    printf("\r                               \r");
    fflush(stdout);
#endif

    //assert(history[hist_size] == NULL);
}

void Decoder::validateHistory()
{
    for(int i = 0; i < hist_size; i++)
    {
        history[i]->validate();
    }
}


// find the next non-blank line
char   *Decoder::nextLine(char *base)
{
    char   *p = base;
    if (p)
    {
        // find the end of the 'used' line (the first one)
        while (*p != '\0' && *p != '\r' && *p != '\n')
            p++;

        // skip past the end-of-line character(s)
        while (*p != '\0' && (*p == '\r' || *p == '\n'))
            p++;
    }

    return p;
}

int Decoder::anyIn(char *in_this, char *of_these)
{
    int rc = 0;

    if (of_these && in_this)
    {
        while (*of_these != '\0')
        {
            if (strchr(in_this, *of_these))
            {
                rc = 1;
                break;
            }
            of_these++;
        }
    }

    return rc;
}

char   *Decoder::getEncodingName(int encoding_type)
{
    switch (encoding_type)
    {
        case UU:
            return "UUencoded";
        case XX:
            return "XXencoded";
        case BASE64:
            return "Base64";
        case YENC:
            return "Y-Enc";
        default:
            return "Unknown";
    }
}


int Decoder::determineType(char *string_block)
{
    int done = 0;
    int encoding_type = UNKNOWN;
    int found_begin = 0;
    int line_length;
    char   *ptr;
    char   *line = 0;
    char   *eoln = 0;
    int line_count = 0;
    char keep;

    ptr = string_block;

    while (!done && *string_block != '\0' && line_count < 100)
    {
        if (ptr)
        {
            eoln = strchr(ptr, '\r');
            if (!eoln)
                eoln = strchr(ptr, '\n');
            if (!eoln)
            {
                line_length = strlen(ptr);
            }
            else
            {
                line_length = eoln - ptr;   // we truncate the last char
                keep = *eoln;
                *eoln = '\0';
            }

            if (line_length > 0)
            {
                line = ptr;

                if (strstr(line, "=ybegin ") != NULL)
                {
                    encoding_type = YENC;
                    done = 1;
                }
                else if (line_length >= 6 && strstr(line, "begin ") != 0)
                {
                    found_begin = 1;
                }
                else if (found_begin)   // now we have non-blank line, start to analyse it
                {
                    // we know it's either UU or XX encoded
                    if (anyIn(line, "`!\"#$%&'()*,.;<=>?[]^"))
                    {
                        encoding_type = UU;
                        done = 1;
                    }
                    else if (anyIn(line, "abcdefghijklmnopqrstuvwxyz"))
                    {
                        encoding_type = XX;
                        done = 1;
                    }
                }
                else
                {
                    // Haven't a clue what it may be yet
                    if (strstr(line, "Content-Transfer-Encoding"))
                    {
                        encoding_type = BASE64;
                        done = 1;
                    }
                }

                if (eoln)
                    *eoln = keep;
            }
            else
            {
                if (eoln)
                    *eoln = keep;
            }
        }
        else
        {
            done = 1;
        }
        ptr = nextLine(string_block);
        line_count++;
        string_block = ptr;
    }
    return encoding_type;
}

char   *Decoder::getDecodeFilename(char *string_block, int encoding_type)
{
    char   *filename = NULL;
    char   *ptr;

    switch (encoding_type)
    {
        case UU:
        case XX:
            // begin 644 filename.ext \n
            ptr = strstr(string_block, "begin ");
            if (ptr)
            {
                ptr = strchr(ptr, ' ');
                if (ptr)
                {
                    ptr++;
                    ptr = strchr(ptr, ' ');
                    if (ptr)
                    {
                        while (*ptr == ' ')
                            ptr++;
                        char   *end = ptr;
                        while (*end && *end != '\n' && *end != '\r')    // filename extends to eoln
                            end++;

                        char ch = *end;
                        *end = '\0';
                        if (end > ptr)
                        {
                            filename = new char[20 + strlen(ptr)];
                            if (!filename)
                            {
                                log("Error: getDecodeFilename - can not allocate %d bytes at %d", end - ptr + 1, __LINE__);
                                exit(2);
                            }
                            else
                            {
                                strcpy(filename, ptr);
                            }
                        }
                        *end = ch;
                    }
                }
            }

            break;
        case YENC:
            {
                char   *start = strstr(string_block, "=ybegin ");
                if (start)
                {
                    ptr = strchr(start, '\r');
                    if (!ptr)
                        ptr = strchr(start, '\n');
                    if (ptr)
                    {
                        char keep = *ptr;
                        *ptr = '\0';

                        char   *name = strstr(start, "name=");
                        if (name)
                        {
                            // log("YENC: name= (%s)",name);
                            name = name + 5;
                            int linelen = strlen(name);
                            if (linelen > 0)
                            {
                                filename = new char[20 + linelen];
                                if (!filename)
                                {
                                    log("Error: getDecodeFilename (YENC)- can not allocate %d bytes at %d", 1 + linelen, __LINE__);
                                    exit(2);
                                }
                                else
                                {
                                    strcpy(filename, name);
                                    // log("YENC: filename= (%s)",filename);
                                }
                            }
                        }

                        *ptr = keep;
                    }
                }
            }
            break;

        case BASE64:
            // Content-Disposition: inline; filename="AirFiltr.gif"
            // Content-Type: image/gif; name="AirFiltr.gif"
            ptr = strstr(string_block, " filename=\"");
            if (!ptr)
                ptr = strstr(string_block, " name=\"");
            if (!ptr)
                ptr = strstr(string_block, " FILENAME=\"");
            if (!ptr)
                ptr = strstr(string_block, " NAME=\"");
            // Content-Disposition: attachment; filename=asi5-15.jpg
            if (!ptr)
                ptr = strstr(string_block, " filename=");
            if (!ptr)
                ptr = strstr(string_block, " name=");
            if (!ptr)
                ptr = strstr(string_block, " NAME=");
            if (!ptr)
                ptr = strstr(string_block, " FILENAME=");

            if (ptr)
            {
                char   *ptr2 = strchr(ptr, '\"');
                char   *end = 0;
                if (ptr2)
                {
                    ptr2++;
                    ptr = ptr2;
                    end = strchr(ptr, '\"');
                }
                else
                {
                    // no " around filename, take from = to eoln
                    ptr2 = strchr(ptr, '=');
                    if (ptr2)
                    {
                        ptr2++;
                        ptr = ptr2;
                        end = strchr(ptr, '\r');
                        if (!end)
                            end = strchr(ptr, '\n');
                    }
                }

                if (end)
                {
                    char keep = *end;
                    *end = '\0';
                    int linelen = strlen(ptr);
                    if (linelen > 0)
                    {
                        filename = new char[20 + linelen];
                        if (!filename)
                        {
                            log("Error: getDecodeFilename - can not allocate %d bytes at %d", 1 + linelen, __LINE__);
                            exit(2);
                        }
                        else
                        {
                            strcpy(filename, ptr);
                        }
                    }
                    *end = keep;
                }
            }
            break;
    }

    if (filename)
    {
        // in mime.cpp
        cleanFilename(filename);
    }

    return filename;
}

// get the next non-blank line
char   *Decoder::extractLine(char **strblock)
{
    char   *block = *strblock;
    char   *line;
    char   *p;

    if (*block == '\0')
        return block;

    // don't return an empty line, unless we're at the end of
    // the block
    while (*block == '\r' || *block == '\n')
    {
        block++;
    }

    if (*block == '\0')
        return block;

    // line points to beginning of line
    line = block;

    // p will eventually point to the next line
    p = block;
    while (*p != '\0' && *p != '\r' && *p != '\n')
    {
        p++;
    }
    while (*p != '\0' && (*p == '\r' || *p == '\n'))
    {
        *p = '\0';
        p++;
    }

    // KT 31-JUL-2002
    // A ".." at the beginning of the line is an 'escaped' "."
    // (becuase posted messages end with ".\r\n")
    // so nuke it
    if (*line == '.' && *(line + 1) == '.')
        line++;

    *strblock = p;

    return line;


}

char   *Decoder::strrstr(char *haystack, char *needle)
{
    char   *result = NULL;
    if (needle && haystack)
    {
        // we're going to find the rightmost occurance of
        // the first character of the needle, then see
        // if this has the rest of the needle after it
        char ch = *needle;
        int needle_len = strlen(needle);
        int haystack_len = strlen(haystack);
        if (haystack_len >= needle_len)
        {
            char   *ptr = haystack + haystack_len - 1;  // point at last character
            while (ptr >= haystack)
            {
                if (*ptr == ch)
                {
                    if (strncmp(ptr, needle, needle_len) == 0)
                    {
                        result = ptr;
                        break;
                    }
                }
                ptr--;
            }
        }
    }

    return result;
}



unsigned long Decoder::YEncDecode(FILE * out, char **the_block)
{
    char   *ptr;
    char   *block = *the_block;
    unsigned long byte_count = 0;
    unsigned long specified_bytes = 0;

    if (block != NULL)
    {
        CharQueue output(strlen(block) * 2 / 3);    // Holds decoded output

        char   *start, *end;
        start = strstr(block, "=ybegin ");

        // try to get the size of the header if available
        if (start)
        {
            ptr = strstr(start, " size=");
            if (ptr && strlen(ptr) > 6 && isdigit(*(ptr + 6)))
                specified_bytes = atol(ptr + 6);
        }

        if (strstr(block, "=ypart") != NULL)
            end = strrstr(block, "=yend");  // is multipart, has >1 '=yend', get the last one
        else
            end = strstr(start, "=yend");   // not multipart, but may have >1 file in block

        // start to decode blocks
        ptr = start;
        log("   YEnc Decoding... ");
        while (end && ptr && ptr < end)
        {
            switch (*ptr)
            {
                case '\r':
                case '\n':
                    break;
                case '=':
                    ptr++;      // skip the '=' escape char
                    switch (*ptr)
                    {
                        case '\0':  // catch a truncated block that ends with '='
                            ptr = end;
                            break;
                        case 'y':  // looks like a y-enc data line, skip it
                            if (strncmp(ptr, "yend", 4) == 0)
                            {
                                // hit an '=yend' but not at EOF, so is probably multipart
                                // skip data until we see another '=ybegin'
                                ptr = strstr(ptr, "=ybegin ");
                                if (!ptr)
                                    ptr = end;
                            }
                            ptr = strstr(ptr, "\r\n");
                            if (!ptr)
                                ptr = end;
                            break;
                        default:
                            output.Enqueue((char)*ptr - 64 - 42);
                           byte_count++;
                    }
                    break;
                case '.':      // a dot in the first column must be doubled (dunno why, maybe messages end in ".\r\n")
                    if (ptr < end - 1 && *(ptr + 1) == '.' && *(ptr - 1) == '\n')
                        ptr++;
                    // NB: dropping thru to default on purpose

                default:
                    output.Enqueue((char)*ptr - 42);
                    byte_count++;
            }
            ptr++;
        }


        // if anything was EnQueued, write it to the file
        if (output.ContentSize() != 0)
        {
            if (specified_bytes > 0 && byte_count != specified_bytes)
                log("   YEnc Warning - should be %lu bytes, but found %lu", specified_bytes, byte_count);

            fwrite(output.getContentAsString(), 1, (size_t) output.ContentSize(), out);
        }
    }
    *the_block = (char *)ptr;

    return byte_count;
}



char Decoder::XXLookup(char x)
{
    if (x == '+')
        return 0;
    if (x == '-')
        return 1;
    if (x >= '0' && x <= '9')
        return (char)(x - 46);
    if (x >= 'A' && x <= 'Z')
        return (char)(x - 53);
    if (x >= 'a' && x <= 'z')
        return (char)(x - 59);

    return 0;
}

#define DEC(c) (((c) - ' ') & 077)


unsigned long Decoder::UUXXDecode(FILE * out, char **the_block, int e_type)
{
    char   *line;
    char   *block = *the_block;
    unsigned long byte_count = 0;

    // implement a quick-buffering

    if (block != NULL)
    {
        CharQueue output(strlen(block) * 2 / 3);    // Holds decoded output

        line = extractLine(&block);

        // our data begins after a 'begin'

        while (*line != '\0')
        {
            if (strncmp(line, "begin ", 6) == 0)
            {
                log("   UUDecoding... ");
                break;
            }
            line = extractLine(&block);
        }
        // skip the begin line


        char   *buf;
        char   *bp;
        int n, expected, actual;


        while (1)
        {
            /* for each input line */
            do
            {
                buf = extractLine(&block);

                n = 0;
                if (buf == NULL)
                    break;
                if (buf[0] == '\0')
                    break;
                n = DEC(buf[0]);

                /* Calculate expected # of chars and pad if necessary */
                expected = ((n + 2) / 3) << 2;

                //   for (i = strlen(buf)-1; i <= expected; i++) buf[i] = ' ';

                actual = strlen(buf) - 1;

                if (actual != expected)
                {
                    // bad data, but try to wing it.
                    // me suspects that some uuencoder dont encode the last line
                    // properly if data is not an even 3 bytes, so pad with zeros
                    if (!strstr(buf, "CUT"))    // common to get lines ---CUT-HERE----
                    {
                        if (actual < expected)
                        {
                            if (expected - actual == 1)
                            {
                                // log("PATCHING.");
                                strcat(buf, " ");
                                break;
                            }
                            else if (expected - actual == 2)
                            {
                                // log("PATCHING..");
                                strcat(buf, "  ");
                                break;
                            }
                            else if (actual - expected < 3)
                            {
                                // too many bytes, but not a huge difference, use anyway
                                log("   Warning: too much data, %d octets, should be %d, using anyway", actual, expected);
                                log("   Data: %s", buf);
                                break;
                            }
                        }
                    }
                    // Illegal line length, skip it
                    log("    Warning: corrupt binary data, %d octets, should be %d.", actual, expected);
                    log("    Data: %s", buf);
                }

            } while (expected != actual);

            if (n <= 0 || buf[0] == '\0' || buf[0] == '\n')
                break;

            bp = &buf[1];
            char   *p;
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
                        output.Enqueue((char)c1);
                        byte_count++;
                    }
                    if (n >= 2)
                    {
                        output.Enqueue((char)c2);
                        byte_count++;
                    }
                    if (n >= 3)
                    {
                        output.Enqueue((char)c3);
                        byte_count++;
                    }
                }
                bp += 4;
                n -= 3;
            }
        }

        // if anything was EnQueued, write it to the file
        if (output.ContentSize() != 0)
        {
            fwrite(output.getContentAsString(), 1, (size_t) output.ContentSize(), out);
        }
    }
    *the_block = block;

    return byte_count;
}


int Decoder::UUXXSkip(char **the_block)
{
    char   *line;
    char   *block = *the_block;

    if (block != NULL)
    {
        line = extractLine(&block);

        // our data begins after a 'begin'
        while (*line != '\0')
        {
            //if (line[0] == 'b' && line[1] == 'e' && line[2] == 'g' && line[3] == 'i' && line[4] == 'n' && line[5] == ' ')
            if (strncmp(line, "begin ", 6) == 0)
                break;
            line = extractLine(&block);
        }
        // skip the begin line
        line = extractLine(&block);
        // now decode the block
        while (*line != '\0')
        {
            line = extractLine(&block);
        }
    }
    *the_block = block;

    return 1;
}

int Decoder::isValidB64(char x)
{
    if ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || (x >= '0' && x <= '9') || (x == '+') || (x == '/'))
        return 1;
    else
        return 0;
}

char Decoder::B64Lookup(char x)
{

    if (x >= 'A' && x <= 'Z')
        return (char)(x - 65);
    if (x >= 'a' && x <= 'z')
        return (char)(x - 71);
    if (x >= '0' && x <= '9')
        return (char)(x + 4);
    if (x == '+')
        return 62;
    if (x == '/')
        return 63;

    return 0;
}


unsigned long Decoder::B64Decode(FILE * out, char **the_block)
{
    // function has been replaced by Mime class.
    // should NEVER be called.

    assert(0);

    return 0;

}


void Decoder::addToDecodes(Item * i)
{
    FILE   *out = fopen("decodes", "a");
    if (out)
    {
        fprintf(out, "%lu %s %s\n", i->getSize(), i->getTimeStamp(), i->getFilename());
        fclose(out);
    }
    else
    {
#ifndef NOPRINT
        printf("Failed to open \"decodes\"\n");
#endif
    }
}

#undef TESTING_BLOCK
#define SAVE_ARTICLE
int Decoder::decode(char *string_block)
{
    int rc = OK;
    int encoding_type;
    int run_count = 0;
    int decodes = 0;
    char   *filename = NULL;
    char   *orig_filename = NULL;
    char   *path = NULL;
    FILE   *out = NULL;
    Item   *itm;
    unsigned long osize;

    items_processed = 0;

    files[0].clear();
    files[0].setError(UNKNOWN_TYPE);

#ifdef TESTING_BLOCK
    int content_gulped = 0;
    FILE *gulp = fopen("h:\\crap\\ART1032662396.src","rb");
    char *old_string_block = NULL;
    if (gulp)
    {
        fseek(gulp,0,SEEK_END);
        long size = ftell(gulp);
        fseek(gulp,0,SEEK_SET);
        char *new_string_block = (char *)malloc((size+1) * sizeof(char));
        if (new_string_block)
        {
            old_string_block = string_block;
            string_block = new_string_block;
            fread(string_block,size,1,gulp);
            content_gulped = 1;
        }
        fclose(gulp);
    }
#endif    


    if (string_block == NULL)
    {
        return UNKNOWN_TYPE;
    }

    encoding_type = determineType(string_block);

#ifdef SAVE_ARTICLE
    char *raw_data_filename = (char *)calloc(sizeof(char),2048);
    if (raw_data_filename)
    {
        sprintf(raw_data_filename,"%s" SEPERATOR_STR "ART%u.src",(output_path?output_path:"."),time(NULL));
        FILE *raw_data = fopen(raw_data_filename,"w");
        if (raw_data)
        {
            fwrite(string_block,strlen(string_block),1,raw_data);
            fclose(raw_data);
        }
    }
#endif

    if (encoding_type == BASE64)
    {
        Mime   *m = new Mime();
        if (m == NULL)
        {
            log("FATAL: failed to allocate a new Mime object %s line %d",__FILE__,__LINE__);
        }
        else
        {
            log("   Base64/MIME Decoding... ");
            m->preScan(string_block);
            for(int i = 0; i < m->getNumSections(); i++)
            {

                // If there's some data, AND
                // the data is binary, OR we've said to save text stuff as well.
                if (m->getDataSize(i) > 8 && (save_text || (m->getContentType(i) == MIME_BASE64 || m->getContentType(i) == MIME_UU)))
                {
                    files[items_processed].clear();
                    files[items_processed].setEncodingMethod(encoding_type);
                    files[items_processed].setSize(m->getDataSize(i));
                    orig_filename = strdup(m->getFilename(i));
                    path = buildFullPath(m->getFilename(i), output_path);
                    files[items_processed].setFullPath(path);
                    files[items_processed].setName(m->getFilename(i));

                    write(path, m->getDataSize(i), m->getDataBlock(i));

                    // delete it if it's a repeat, else remember it
                    if (orig_filename)
                    {
                        itm = new Item(m->getDataSize(i), orig_filename);
                        deleteIfSeenBefore(itm, path);
                        free(orig_filename);
                        orig_filename = NULL;
                    }

                    // add extension to FileStat block
                    char   *dot = findExtension(path);
                    if (dot != path)
                        files[items_processed].setExtension(dot + 1);

                    if (path) 
                    {
                        delete[]path;
                        path = NULL;
                    }

                    items_processed++;
                    decodes++;
                }
            }
            delete m;
        }
    }
    else if (encoding_type == UU || encoding_type == XX || encoding_type == YENC)   // do the UUdecoding
    {

        while (*string_block != '\0')
        {
            run_count++;
            filename = NULL;
            path = NULL;

            encoding_type = determineType(string_block);
            files[items_processed].clear();
            files[items_processed].setEncodingMethod(encoding_type);

            if (encoding_type == UNKNOWN)
            {
                rc = UNKNOWN_TYPE;
                files[items_processed].setError(UNKNOWN_TYPE);
                if (items_processed == 0)
                    items_processed = 1;

                break;
            }

            filename = getDecodeFilename(string_block, encoding_type);

            if (filename == NULL)
            {
                files[items_processed].setError(NO_FILENAME);
                rc = NO_FILENAME;
                if (items_processed == 0)
                    items_processed = 1;

                break;
            }

            orig_filename = strdup(filename);
            path = buildFullPath(filename, output_path);
            files[items_processed].setFullPath(path);
            files[items_processed].setName(filename);

            out = fopen(path, WRITE_BINARY);
            if (out == NULL)
            {
                log("Error: decode - can not write to \"%s\"", path);
                break;
            }

            // Do the actual decode
            decodes++;
            osize = 0;
            if (encoding_type == UU || encoding_type == XX)
            {
                osize = UUXXDecode(out, &string_block, encoding_type);
                fclose(out);
            }
            else if (encoding_type == YENC)
            {
                osize = YEncDecode(out, &string_block);
                fclose(out);
            }
            else
            {
                fclose(out);
                break;
            }

            if (osize == 0)
            {
                deleteFile(path);
                files[items_processed].setError(EMPTY);
                if (items_processed == 0)
                    items_processed = 1;
                break;
            }

            files[items_processed].setSize(osize);
            if (orig_filename)
            {
                itm = new Item(osize, orig_filename);
                free(orig_filename);
                orig_filename = NULL;
            }

            // delete it if it's a repeat, else remember it
            deleteIfSeenBefore(itm, path);

            // add extension to FileStat block
            char   *dot = findExtension(path);
            if (dot != path)
                files[items_processed].setExtension(dot + 1);

            if (filename)
            {
                delete[]filename;
                filename = NULL;
            }
            if (path)
            {
                delete[]path;
                path = NULL;
            }

            items_processed++;
        }
    }
    if (filename) 
    {
        delete[]filename;
        filename = NULL;
    }
    if (path) 
    {
        delete[]path;
        path = NULL;
    }

#ifdef SAVE_ARTICLE
    if (raw_data_filename)
    {
        deleteFile(raw_data_filename);
        free(raw_data_filename);
    }
#endif 

#ifdef TESTING_BLOCK
    if (content_gulped)
    {
        free(string_block);
        string_block = old_string_block;
    }
#endif        

    if (decodes > 0)
        return OK;
    else
        return rc;
}


void Decoder::setOutputPath(char *path)
{
    delete[]output_path;

    if (path == NULL)
        path = ".";

    output_path = new char[1 + strlen(path)];
    if (!output_path)
    {
        log("Error: setOutputPath - can not %d bytes", 1 + strlen(path));
        exit(2);
    }

    strcpy(output_path, path);
}

void Decoder::setOutputPostProcessor(char *exe_name)
{
    if (exit_exe != NULL)
        delete[]exit_exe;

    if (!exe_name)
    {
        exit_exe = NULL;
    }
    else
    {
        exit_exe = new char[1 + strlen(exe_name)];
        if (!exit_exe)
        {
            log("FATAL Error: setOutputPostProcessor - cannot allocate %d bytes", 1 + strlen(exe_name));
        }
        else
        {
            strcpy(exit_exe, exe_name);
        }
    }
}


int Decoder::checkNameSeenBefore(char *fileName)
{
    // SET DEFINE UP THERE A BIT
#ifdef USE_BINARY_TREE_FILE_MEMORY

    // Have we seen this file already ?
    int rc = Decoder::decodes.find(itm);

#else
// USE_PLAIN_ARRAY_FILE_MEMORY
    int was_found = 0;
    for(int x = 0; x < hist_size && !was_found; x++)
    {
        was_found = stricmp(history[x]->filename, fileName) == 0;
    }

    return was_found;

#endif // #ifdef USE_BINARY_TREE_FILE_MEMORY

}                               // checkNameSeenBefore


char   *Decoder::checkMangle(char *path)
{
    int mangle = 0;
    FILE   *out = fopen(path, "r");
    char   *original_path = new char[16 + strlen(path)];

    if (original_path == NULL)
    {
        log("   FATAL: Failed to allocate original_path at %s line %d",__FILE__,__LINE__);
    }
    else
    {
        strcpy(original_path, path);

        while (out)
        {
            fclose(out);
            // filename already exists, should mangle
            strcpy(path, original_path);
            char   *buffer = new char[1024 + strlen(path)];
            if (buffer == NULL)
            {
                log("   FATAL: Failed to allocate buffer at %s line %d",__FILE__,__LINE__);
            }
            else
            {
                char   *dot = findExtension(path);

                if (dot == path)
                {
                    // there is not '.' in the filename
                    sprintf(buffer, "%s(%03d)", path, mangle);
                    strcpy(path, buffer);
                }
                else
                {
                    sprintf(buffer, "(%03d)", mangle);
                    strcat(buffer, dot);
                    *dot = '\0';
                    strcat(path, buffer);
                }

                log("   Mangling filename to \"%s\"", path);

                mangle++;
                delete[]buffer;
                out = fopen(path, "r");
            }
        }

        delete[]original_path;
    }

    return path;
}




int Decoder::checkFullDisk(void)
{
    char buffer[1024];
    sprintf(buffer, "%s" SEPERATOR_STR "_.tst", (output_path ? output_path : "."));
    FILE   *out = fopen(buffer, WRITE_BINARY);
    if (out)
    {
        unsigned int rc = fprintf(out, buffer);
        fclose(out);
        deleteFile(buffer);
        if (rc == strlen(buffer))
            return 0;
    }

    return 1;
}



int Decoder::deleteIfSeenBefore(Item * itm, char *path)
{
    // SET DEFINE UP THERE A BIT
#ifdef USE_BINARY_TREE_FILE_MEMORY

    // Have we seen this file already ?
    if (Decoder::decodes.find(itm))
    {
        // Yes, piss it off
        delete itm;
        files[items_processed].setAction(SKIPPED);
        deleteFile(path);
    }
    else
    {
        // No!  Make a note of it
        if (itm)
        {
            Decoder::decodes.add(itm);
            history[hist_size] = itm;
            hist_size++;
        }
        Decoder::addToDecodes(itm);
        files[items_processed].setAction(DECODED);
    }

#else
// USE_PLAIN_ARRAY_FILE_MEMORY
    int was_found = 0;
    int x;
    for(x = 0; x < hist_size; x++)
    {
        if (Item::compare(history[x], itm) == 0)
        {
            was_found = 1;
            break;
        }
    }

    if (was_found)
    {
        // Yes, piss it off
        delete itm;
        files[items_processed].setAction(SKIPPED);
        deleteFile(path);
    }
    else
    {
        // No!  Make a note of it
        if (itm)
        {
            history[hist_size] = itm;
            hist_size++;
        }
        Decoder::addToDecodes(itm);
        files[items_processed].setAction(DECODED);
    }
#endif // #ifdef USE_BINARY_TREE_FILE_MEMORY

    return 0;
}

// return poinbter to the bit starting at the last dot in the filename
// or the beginning of the filename if no dot.  (eg: ".gif")
char   *Decoder::findExtension(char *path)
{
    char   *dot = path + strlen(path);
    while (dot > path && *dot != '.')
    {
        // if we hit a directory break, no '.' to be found
        if (*dot == '/' || *dot == '\\')
        {
            dot = path;
            break;
        }
        dot--;
    }

    return dot;
}


char   *Decoder::buildFullPath(char *filename, char *output_path)
{
    char   *path = new char[1024 + (output_path ? strlen(output_path) : 0) + strlen(filename)];
    if (path != NULL)
    {
        *path = '\0';
        if (output_path && *output_path)
        {
            strcpy(path, output_path);
            strcat(path, SEPERATOR_STR);    // either "/" or "\\"
        }
        strcat(path, filename);
        checkMangle(path);

        // filename may have changed, must get it
        char   *nfp = path + strlen(path) - 1;
        while (nfp > path && *nfp != SEPERATOR_CHAR)
            nfp--;
        if (nfp > path && *nfp == SEPERATOR_CHAR && *(nfp + 1) != '\0')
        {
            strcpy(filename, nfp + 1);
        }
    }
    else
    {
        log("FATAL: Failed to allocate %d bytes in buildFullPath", 1024 + (output_path ? strlen(output_path) : 0) + strlen(filename));
    }

    return path;

}


int Decoder::write(char *filename, int size, char *block)
{
    int rc = 0;

    FILE   *out = fopen(filename, WRITE_BINARY);
    if (out)
    {
        rc = fwrite(block, 1, size, out);
        fclose(out);
    }
    return rc;
}

// The end
