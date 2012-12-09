/***********************************************************************

multipart.cpp,v 1.4 2005/10/04 01:08:28 root Exp

Multipart assemble and decode

Copyright (c) 1997, 2003 Pty. Limited
Copyright (c) 2000, 2005 Steven Levine and Associates, Inc.
All rights reserved.

12 Dec 03 SHL reAssembleParts: rework to avoid disk full traps
03 Oct 05 SHL reAssembleParts: avoid death if index contains dups

***********************************************************************/


#include "multipart.h"

#ifdef UNIX
#define SEPERATOR '/'
#define WRITE_BINARY "w"
#else
#define SEPERATOR '\\'
#define WRITE_BINARY "wb"
#endif

//
// added by gavin 20/04/1999
// Multiparts critical section access implementation
//

#ifdef WIN32
CSemaphore multipartsem;
#endif

void gainAccessToParts()
{
#ifdef WIN32
 multipartsem.Lock();
#endif
}

void releaseAccessToParts()
{
#ifdef WIN32
 multipartsem.Unlock();
#endif
}



char *handlePartPosts(Header *head,char *body,char *part_path,unsigned long *dl_cnt,unsigned long *dl_g_cnt,unsigned long days)
{
    static time_t t;
    static int count;

    // get a unique part name
    if (t == time(NULL))
    {
	count++;
    }
    else
    {
	t = time(NULL);
	count=0;
    }

    char *filename = new char[1024];
    sprintf(filename,"%sparts",part_path);
    MKDIR(filename);

    // added by gavin 20/04/1999 for critical section protection
    gainAccessToParts();

    sprintf(filename,"%sparts%c%08ld%03d",part_path,SEPERATOR,t,count);
    FILE *out = fopen(filename,WRITE_BINARY);
    if (!out)
    {
	log("Failed to open \"%s\", out of space?\n",filename);
#ifndef NOPRINT
	printf("Failed to open \"%s\", out of space?\n",filename);
#endif
    }
    else
    {
	// write the sucker to file
	fwrite(body,strlen(body),1,out);	// gulp

	fclose(out);
	log("   Part %d of %d \"%s\"",head->partNumber(),head->partTotal(),head->partName());
#ifndef NOPRINT
	printf("%s %d/%d                      \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b ",head->partName(),head->partNumber(),head->partTotal());
#endif
    }

    char *indexname = new char[1024];
    sprintf(indexname,"%sparts%cindex",part_path,SEPERATOR);
    out = fopen(indexname,"a");
    if (out)
    {
	fprintf(out,"%ld %d %d x%08ld%03d\n%s\n",time(NULL),head->partNumber(),head->partTotal(),t,count,head->partString());
	fclose(out);
    }
    else
    {
#ifndef NOPRINT
	printf("Failed to open \"%s\"\n",indexname);
#endif
	log("Failed to open \"%s\"\n",indexname);
    }

    delete[] indexname;
    delete[] filename;
    delete[] body;
    body = reAssembleParts(head->partString(),part_path,dl_cnt,dl_g_cnt,days);

    // added by gavin 20/04/1999 for critical section protection
    releaseAccessToParts();

    return body;
}

// ------- Do *ALL* multipart processing, maintain index, return assembled parts or null ---------------------

#define Max_NUM_PARTS 500

char *reAssembleParts(char *part_idstr,char *part_path,unsigned long *dl_count,unsigned long *dl_grp_count,unsigned long days)
{
    time_t cutoff_date = time(NULL) - days * 86400;	// 60 * 60 * 24
    FILE *index_in = 0;
    FILE *index_out = 0;
    FILE *part_in = 0;
    FILE *parts_out = 0;
    int parts_found = 0;
    int max_part_num = 0;
    int all_found = 0;
    int i;
    char *body = 0;
    char part_in_filename[_MAX_PATH];
    char index_in_filename[_MAX_PATH];
    char index_out_filename[_MAX_PATH];
    const unsigned short LINE_MAX = 1023;
    char index_line[LINE_MAX+1];				// fixme to be smaller
    Part **parts = new PartPtr[Max_NUM_PARTS+1];	// fixme to auto expand

    sprintf(index_in_filename,"%sparts%cindex",part_path,SEPERATOR);
    sprintf(index_out_filename,"%sparts%cindex.new",part_path,SEPERATOR);

    index_in  = fopen(index_in_filename,"r");

    for (;;) {

	if (!index_in) {
	   log("   Can not open %s",i, index_in_filename);
	   break;
	}

	index_out = fopen(index_out_filename,"w");

	if (!index_out) {
	   log("   Can not create %s",i, index_out_filename);
	   break;
	}

	int  part_num;
	int  parts_total;
	char *index_idstr;
	char *ptr;

	// Read index.  Check if have all parts now
	// 2 lines per entry

	for (;;) {

	    // Get part descriptor line
	    fgets(index_line,LINE_MAX,index_in);
	    if (feof(index_in))
		break;			// Done

	    int c = strlen(index_line);
	    if (c > 0) {
		ptr = index_line + c - 1;
		*ptr = '\0';		// Chop newline
	    }

	    time_t part_date;

	    // Format: 972849088 2 2 x972849088000
	    if (sscanf(index_line,"%lu %d %d",&part_date,&part_num,&parts_total) != 3)
		continue;		// Silently ignore malformed lines - fixme

	    ptr = strchr(index_line,'x');
	    if (!ptr)
		break;		// Silently ignore malformed line - fixme

	    ptr++;
	    sprintf(part_in_filename,"%sparts%c%s",part_path,SEPERATOR,ptr);
	    char *name_ptr = part_in_filename+strlen(part_path)+6;

	    // Get subject line
	    fgets(index_line,LINE_MAX,index_in);

	    if (feof(index_in))
		break;			// Silently ignore missing line - fixme

	    c = strlen(index_line);
	    ptr = index_line + c - 1;
	    *ptr = '\0';	// Chop newline

	    index_idstr = index_line;

	    // has this part been sitting around for ever?
	    if (part_date < cutoff_date) {
		log("   Removing old Parts... (%s)",part_in_filename);
#		ifndef NOPRINT
		printf("Removing old Parts... (%s)\n",part_in_filename);
#		endif
		if (deleteFile(part_in_filename) != DELETE_OK)
		    log("   Error removing part \"%s\"",part_in_filename);
		continue;
	    }

	    if (strcmp(part_idstr,index_idstr) == 0) {
		// Found another part of what we're looking for
		if (part_num <= parts_found && parts[part_num-1]) {
		    // we have a duplicate
		    log("   Hmmm... we've seen this part %d before (removing)", part_num);
		    if (deleteFile(part_in_filename) != DELETE_OK)
		       log("   Error removing part \"%s\"",part_in_filename);
		}
		else {
		    // Remember
		    if (part_num > max_part_num) {
			for (i = max_part_num; i < part_num; i++)
			    parts[i] = 0;	// fill to allow delete[]
			max_part_num = part_num;
		    }
		    // If user reset counter, index can have dups - avoid death
		    if (!parts[part_num-1]) {
		      parts[part_num-1] = new Part(index_idstr,name_ptr,part_num,parts_total);
		      parts_found++;
		      if (parts_found ==  parts_total)
			  all_found = TRUE;
		    }
		}
	    }
	    else {
		// just some part of something else that isn't too old to delete
		fprintf(index_out,"%ld %d %d x%s\n%s\n",part_date,part_num,parts_total,name_ptr,index_idstr);
	    }
	} // for index items

	// unsigned int size_check = 0;

	if (!all_found)	{
	    log("   Incomplete post, %d part%s so far",parts_found,(parts_found!=1?"s":""));
	}
	else {
	    log("   Found all parts");
	    // have all parts, in sorted order [1..parts_found]
	    // concatenate them together
	    if (!parts[0]->getFilename()) {
    #           ifndef NOPRINT
		printf("No filename for multi-part\n");
    #           endif
		log("No filename for multi-part\n");
	    }
	    else {
		parts_out = tmpfile();
		if (!parts_out)	{
		    log("Failed to create a temporary file!");
		    all_found = 0;		// keep parts to allow retry
		}
		else {
		    log("   Concatenating %d parts",parts_found);
		    for (i = 0; i < parts_found; i++) {
			sprintf(part_in_filename,"%sparts%c%s",part_path,SEPERATOR,parts[i]->getFilename());
			part_in = fopen(part_in_filename,"rb");

			if (!part_in) {
			    log("   Part %d (%s) is missing from the parts directory",i,part_in_filename);
			}
			else {
			    char *huge_buffer;

			    fseek(part_in,0L,SEEK_END);
			    long part_size = ftell(part_in);
			    if (part_size == -1) {
				log("   Can not size part %d (%s)",i, part_in_filename);
				all_found = FALSE;
			    }
			    else {
				fseek (part_in,0L,SEEK_SET);

				// size_check += part_size;

				huge_buffer = new char[1024+part_size];
				if (!huge_buffer) {
				    log("   Can not allocate %d bytes to read %s",1024+part_size,part_in_filename);
				    all_found = FALSE;
				}
				else {
				    long c = fread(huge_buffer,1,part_size,part_in);
				    if (c != part_size) {
					log("   Can not read %d bytes from %s",1024+part_size,part_in_filename);
					all_found = FALSE;
				    }
				    else {
					c = fwrite(huge_buffer,1,part_size,parts_out);
					delete[] huge_buffer;
					if (c != part_size) {
					    log("   Can not write %d bytes to tmpfile",part_size);
					    all_found = FALSE;
					}
					else {
					    log("   Adding %ld bytes from part %d",part_size,i+1);
					}
				    }
				}
			    }

			    fclose(part_in);
			    part_in = 0;
			}

		    } // for parts

		    int rc = fputs("\r\n.\r\n",parts_out);
		    if (rc == EOF) {
			log("   Can not write end bytes to tmpfile");
			all_found = FALSE;
		    }
		} // if concat

		if (all_found) {
		    // made a temp file, now read it all back into a mem buffer
		    fseek(parts_out,0L,SEEK_END);	//should n't need to do this...
		    long size = ftell(parts_out);
		    rewind(parts_out);

		    // log("   Check sizes: %u vs %u\n",size_check,size);

		    if (size == -1) {
			log("   Can not size tmpfile");
			all_found = FALSE;
		    }
		    else {
			log("   Multipart article is %u bytes",size);
			body = new char[1024+size];
			if (!body) {
			    log("   Can not allocate %d bytes to read tmpfile",1024+size);
			    all_found = FALSE;
			}
			else {
			    memset(body,0,1024+size);
			    // read it all back
			    fread(body,1,size,parts_out);
			    fclose(parts_out);
			    parts_out = 0;

			    // UNCOMMENT this to save a copy of the multiaprt before decoding
			    // FILE *kt = fopen("C:\\desktop\\part.concatenated","wb");
			    // fwrite(rc,1,size,kt);
			    // fclose(kt);
			}
		    }
		} // if still all_found

		(*dl_count)++;
		(*dl_grp_count)++;
	    }

	    // If concat OK, delete parts files
	    if (all_found) {
		for (i = 0; i < parts_found; i++) {
		    sprintf(part_in_filename,"%sparts%c%s",part_path,SEPERATOR,parts[i]->getFilename());
		    if (deleteFile(part_in_filename) != DELETE_OK)
		       log("   Error removing part \"%s\"",part_in_filename);
		}
	    }
	} // if all_found

	// If still need some more parts
	// put the remembered, but *unused* parts back
	// touch their times, we just got a new part!
	if (!all_found)	{
	     for (i = 0; i < max_part_num; i++)	{
		 if (parts[i]) {
		     part_num = parts[i]->getNumber();
		     parts_total = parts[i]->getTotal();
		     char *name_ptr = parts[i]->getFilename();
		     index_idstr = parts[i]->getIdString();

		     fprintf(index_out,"%ld %d %d x%s\n%s\n",time(NULL),part_num,parts_total,name_ptr,index_idstr);
		 }
	     }
	}

	fclose(index_in);
	fclose(index_out);
	if (deleteFile(index_in_filename) != DELETE_OK)
	   log("   Error removing old index");

	rename(index_out_filename,index_in_filename);

	break;				// Just once

    } /* for */

    if (index_in)
	fclose(index_in);

    if (index_out)
	fclose(index_in);

    if (part_in)
	fclose(part_in);

    if (parts_out)
	fclose(parts_out);

    for (i = 0; i < max_part_num; i++) {
	delete parts[i];
    }

    delete[] parts;

    return body;

} // reAssembleParts


#ifdef OS2

int isAllDigits(char *string)
{
    char *ptr = string;
    int rc = 1;

    while (*ptr != '\0')
    {
	if (!isdigit(*ptr))
	{
	    rc = 0;
	    break;
	}
	ptr++;
    }
    return rc;
}

int partClean(char *basepath)
{
    char *index;
    array<char *> files;
    int deleted = 0;

    char *indexfilename = new char[4096];


    // basepath should already end in '\' (or '/'
    sprintf(indexfilename,"%sparts%cindex",basepath,SEPERATOR);

    FILE *in = fopen(indexfilename,"r");
    if (in)
    {
	fseek(in, 0L, SEEK_END);
	unsigned long size = ftell(in);
	rewind(in);

	index = new char[1024+size];

	fread(index,1,size,in);
	fclose(in);
    }
    else
    {
	delete[] indexfilename;
	return 0;
    }

    char *path = new char[4096];
    strcpy(path,basepath);
    strcat(path,"*");

    FileList fl(path);
    for (int i=0; i<fl.getNumFiles(); i++)
    {
	if (isAllDigits(fl.getFile(i)))
	{
	    if (strstr(index,fl.getFile(i)) == NULL)
	    {
		// file not in index - remove
		strcpy(path,basepath);
		strcat(path,fl.getFile(i));
		deleteFile(path);
		deleted++;
	    }
	}

    }

    delete[] index;
    delete[] indexfilename;
    delete[] path;

    return deleted;
}


#endif


int partCull25PercentOldest(char *basepath)
{
    int deleted = 0;
    char *indexfilename = new char[4096];
    array<Part> all_parts;
    int num_read = 0;


    // basepath should already end in '\' (or '/'
    sprintf(indexfilename,"%sparts%cindex",basepath,SEPERATOR);

 gainAccessToParts();


    FILE *in = fopen(indexfilename,"r");
    if (in)
    {

	// read in the whole index
	while(!feof(in))
	{
	    if (all_parts[num_read].read(in))
		num_read++;
	    else
		break;
	}
	fclose(in);
    }

    // if file not opened, or no parts - give up
    if (num_read == 0)
    {
	delete[] indexfilename;
	return 0;
    }


    // entries are sorted by timestamp already - yay!
    // so throw away top 25%

    int skipper = num_read / 4;
    int i;

    FILE *out = fopen(indexfilename,"w");
    if (!out)
    {
	delete[] indexfilename;
	return 0;
    }


    for (i=0; i<num_read; i++)
    {
	if (i < skipper)
	{
	    // in top 25% - delete it
	    sprintf(indexfilename,"%sparts%c%s",basepath,SEPERATOR,all_parts[i].getFilename());
	    if (deleteFile(indexfilename) != DELETE_OK)
		log("25%% Clean up - failed to delete \'%s\'",indexfilename);
	    deleted++;
	}
	else
	{
	    // write back to index
	    fprintf(out,"%ld %d %d x%s\n%s\n",all_parts[i].getTimestamp(),
		    all_parts[i].getNumber(),
		    all_parts[i].getTotal(),
		    all_parts[i].getFilename(),
		    all_parts[i].getIdString());

	}

    }

    fclose(out);

 releaseAccessToParts();


    delete[] indexfilename;

    return deleted;
}


// return a block of concatenated parts, or NULL
// give path to parts directory, and id string of
// the part.

// This will only work where you have parts
// 0-N of X parts, where N < X, and all parts
// numbered 0..N are available.

// And probably then, only if they are UU-encoded
char *buildIncomplete(char *basepath,char *tag)
{
    char *indexfilename = new char[4096];
    array<Part> all_parts;
    int num_read = 0;
    char *block = NULL;


    if (tag == NULL || *tag == '\0')
	return NULL;

    // basepath should already end in '\' (or '/'
    sprintf(indexfilename,"%sparts%cindex",basepath,SEPERATOR);

 gainAccessToParts();


    FILE *in = fopen(indexfilename,"r");
    if (in)
    {

	// read in the whole index
	while(!feof(in))
	{
	    if (all_parts[num_read].read(in))
		num_read++;
	    else
		break;
	}
	fclose(in);
    }

    // if file not opened, or no parts - give it up
    if (num_read == 0)
    {
	delete[] indexfilename;
	return NULL;
    }

    int i;
    int first_index,last_index,previous_index;

    first_index = -1;	// index into array of first part for this article
    last_index  = -1;	// index into array of the last consecutive part for this article
    previous_index = -1;	// part number of previous part used to check consecutiveness

    // entries are sorted by description and then part number - yay!
    // find the index of the first part matching the given tag

    for (i=0; i<num_read; i++)		// for all loaded parts
    {
	// is it a matching tag
	if (strcmp(tag,all_parts[i].getIdString()) == 0)
	{
	    // found a matching part
	    if (first_index == -1)
	    {
		// we have found the first part in this block of parts
		first_index = last_index = i;
		previous_index = all_parts[i].getNumber();
	    }
	    else
	    {
		// already found the first row, this is another row

		// check that this part is the next consecutive part,
		// and we haven't jumped a (few) parts
		if (previous_index == all_parts[i].getNumber()-1)
		{
		    last_index = i;
		    previous_index = all_parts[i].getNumber();
		}
		else
		{
		    // if it's not consecutive then it's time to stop
		    break;
		}
	    }
	}
	else
	{
	    // parts must be consecutive, once a different part is found
	    // we have run out of parts for this block.
	    if (first_index != -1)
	    {
		break;
	    }
	}
    }

    // if no matching parts were found, or the part number of the
    // first part is not 1, then give up
    if (first_index == -1 || all_parts[first_index].getNumber() != 1)
    {
	// no matching parts found
	delete[] indexfilename;
	return NULL;
    }

    // So now we have a start index and a finish index, such that
    // no parts are skipped in between...
    // EG: we have parts 1,2,3,5,7 - last_index == 3 (well 2, 'cause it's 0->N-1 not 1->N)

    FILE *out;
    unsigned int size_check = 0;
    out = tmpfile();
    log("MULTIPART - Concatenating 1 to %d parts",all_parts[last_index].getNumber());
    if (out)
    {
	char *filename = new char[4096];
	for (i=first_index; i<=last_index; i++)
	{
	    sprintf(filename,"%sparts%c%s",basepath,SEPERATOR,all_parts[i].getFilename());
	    in = fopen(filename,"rb");

	    if (in == NULL)
	    {
		log("MULTIPART - Part %d (%s) is missing from the parts directory",all_parts[i].getNumber(),filename);
	    }
	    else
	    {
		char *huge_buffer;
		long part_size;

		fseek(in,0L,SEEK_END);
		part_size = ftell(in);
		fseek (in,0L,SEEK_SET);

		size_check += part_size;

		huge_buffer = new char[1024+part_size];
		fread(huge_buffer,1,part_size,in);
		fwrite(huge_buffer,1,part_size,out);
		delete[] huge_buffer;
		log("MULTIPART - Adding %ld bytes from part %d",part_size,all_parts[i].getNumber());
	    }


	    fclose(in);
	}
	fputs("\r\n.\r\n",out);

	delete[] filename;
    }
    else
    {
	log("MULTIPART - Failed to create a temporary file!");
    }


    // made a temp file, now read it all back into a mem buffer
    fseek(out,0L,SEEK_END);		//should n't need to do this...
    unsigned int size = (unsigned int)ftell(out);
    rewind(out);

    log("MULTIPART - Check sizes: %u vs %u\n",size_check,size);

    if (size)
    {
	log("MULTIPART - multipart article is %u bytes",size);
	block = new char[1024+size];
	memset(block,0,1024+size);
	// read it all back
	fread(block,size,1,out);

// UNCOMMENT this to save a copy of the multiaprt before decoding
//                FILE *kt = fopen("C:\\desktop\\part.concatenated","wb");
//                fwrite(block,1,size,kt);
//                fclose(kt);

    }
    fclose(out);  // file should delete itself



    // note Array object must delete all Parts inside

 releaseAccessToParts();


    delete[] indexfilename;

    return block;
}

