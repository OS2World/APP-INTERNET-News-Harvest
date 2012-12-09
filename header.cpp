
/***********************************************************************

header.cpp,v 1.4 2002/03/26 02:51:38 root Exp

Header decoding

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2002 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	09 Jan 02 SHL - decodeSubject: avoid fool by extra brackets
		09 Jan 02 SHL - isMulti: correct ancient logic error
		23 Feb 02 SHL - decodeSubject: try to handle (#/#) [#/#] intelligently

***********************************************************************/

#include <regex.h>

#include "header.h"

Header::Header(char *blk)
{
    init();
    block = blk;

    if (blk)
    {
	str_len = strlen(blk);
	lower_block = new char[1+str_len];
	strcpy(lower_block,blk);

	for (unsigned int i=0; i<str_len; i++)
	    lower_block[i] = tolower(lower_block[i]);
    }

#ifdef DEBUG
#ifndef NOPRINT
    fprintf(stderr,"New Header[%p](char *blk \"%s\")\n",this,(blk?blk:"<null>"));
    fprintf(stderr,"\tblock      = \"%s\"\n",(block?block:"<null>"));
    fprintf(stderr,"\tlower_block= \"%s\"\n",(lower_block?lower_block:"<null>"));
    fprintf(stderr,"\tpart_name  = \"%s\"\n",(part_name?part_name:"<null>"));
    fprintf(stderr,"\tpart_line  = \"%s\"\n",(part_line?part_line:"<null>"));
    fprintf(stderr,"\tlines      = \"%d\"\n",lines);
    fprintf(stderr,"\tpart_num   = \"%d\"\n",part_num);
    fprintf(stderr,"\tpart_total = \"%d\"\n",part_total);
    fflush(stderr);
#endif
#endif // DEBUG

} // Header::Header


Header::Header(char *blk,int art)
{
    init();
    block = blk;
    article = art;

    if (blk)
    {
	str_len = strlen(blk);
	lower_block = new char[1+str_len];
	strcpy(lower_block,blk);

	for (unsigned int i=0; i<str_len; i++)
	    lower_block[i] = tolower(lower_block[i]);
    }

#ifdef DEBUG
#ifndef NOPRINT
    fprintf(stderr,"New Header[%p](char *blk \"%s\")\n",this,(blk?blk:"<null>"));
    fprintf(stderr,"\tblock      = \"%s\"\n",(block?block:"<null>"));
    fprintf(stderr,"\tlower_block= \"%s\"\n",(lower_block?lower_block:"<null>"));
    fprintf(stderr,"\tpart_name  = \"%s\"\n",(part_name?part_name:"<null>"));
    fprintf(stderr,"\tpart_line  = \"%s\"\n",(part_line?part_line:"<null>"));
    fprintf(stderr,"\tlines      = \"%d\"\n",lines);
    fprintf(stderr,"\tpart_num   = \"%d\"\n",part_num);
    fprintf(stderr,"\tpart_total = \"%d\"\n",part_total);
    fflush(stderr);
#endif
#endif // DEBUG

} // Header::Header


void Header::init()
{
    lines = -2;				// Mark unknown
    block = 0;
    lower_block = 0;
    str_len = 0;
    article = 0;

    part_name = 0;
    part_line = 0;
    part_num = -2;			// Mark unknown
    part_total = -2;			// Mark unknown

    subject = 0;
    lower_subject = 0;
    sender = 0;
    lower_sender = 0;

} // Header::init

Header::~Header(void)
{

#ifdef DEBUG
#ifndef NOPRINT
    fprintf(stderr,"Delete Header[%p](void)\n",this);
    fprintf(stderr,"\tblock      = \"%s\"\n",(block?block:"<null>"));
    fprintf(stderr,"\tlower_block= \"%s\"\n",(lower_block?lower_block:"<null>"));
    fprintf(stderr,"\tpart_name  = \"%s\"\n",(part_name?part_name:"<null>"));
    fprintf(stderr,"\tpart_line  = \"%s\"\n",(part_line?part_line:"<null>"));
    fprintf(stderr,"\tlines      = \"%d\"\n",lines);
    fprintf(stderr,"\tpart_num   = \"%d\"\n",part_num);
    fprintf(stderr,"\tpart_total = \"%d\"\n",part_total);
    fflush(stderr);
#endif
#endif // DEBUG

    delete[] block;
    delete[] part_name;
    delete[] part_line;
    delete[] lower_block;
    delete[] subject;
    delete[] lower_subject;
    delete[] sender;
    delete[] lower_sender;

} // Header::~Header



int Header::containsString(char *s)
{
    if (block && s && *s != '\0' && strlen(s) <= str_len)
    {
	return strstr(block,s) != 0;
    }
    else
	return 0;
}


int Header::containsStringNoCase(char *s)
{
    if (block && s && *s != '\0' && strlen(s) <= str_len)
    {
	char *ptr = s;
	while(*ptr)
	{
	    *ptr = tolower(*ptr);
	    ptr++;
	}
	return strstr(lower_block,s) != 0;
    }
    else
	return 0;
}


//=== Header::decodeSubject - decode subject line for filename and part info ===
// return true if multipart

int Header::decodeSubject(char *subjectLine)
{
    // Forget prior decode

    delete []part_line;		// residual subject line
    part_line = 0;

    delete []part_name;		// filename
    part_name = 0;

    part_num = 0;		// Mark checked
    part_total = 0;		// Mark checked

    // expect:
    //  blah blah blah filename.ext (partnum/partcnt) optional blah blah
    // are getting like:   filename.ext (1/4)  12321 bytes
    // try to fix these:
    // filename-01.mpg (02/20) [1/3] - filename-01.mpg
    //   choose ( over [?

    char *pHere = subjectLine;
    char ch;

    while ((ch = *pHere) == ' ')
	pHere++;			// Skip leading blanks

    enum {GetLine, GetFileName, GetPartNum, GetPartCnt, GetDone} idState = GetLine;

    char *pLine = pHere;

    int cFileName = 0;			// Filename string length (0=none)
    int cPartNum = 0;			// Part number string length
    int cPartCnt = 0;			// Part count string length

    char *pFileName;			// Filename string pointer
    char *pPartNum;			// Part number string pointer
    char *pPartCnt;			// Part count string pointer

    int cNewPartNum = 0;		// Potential part number string length
    char *pNewPartNum;			// Potential part number string pointer
    char *pNewPartCnt;			// Potential part count string pointer

    int cDigits = 0;			// In current number
    int gotOf = 0;			// of string seen after number

    char *pStart = pHere;		// Start of current string
    int cWord;				// Characters in current word

    while ((ch = *pHere++) != '\0')
    {
	switch (ch)
	{
	case '(':
	case '[':
	    switch (idState)
	    {
	    case GetLine:
		// If lead in is [ and already have num/cnt with ( leadin ignore this
		if (ch != '[' ||
		    pPartNum == 0 ||
		    *(pPartNum - 1) != '(')
		{
		  idState = GetPartNum;
		  pNewPartNum = pHere;
		}
		break;

	    case GetFileName:
		cFileName = pHere - pFileName;
		if (cFileName < 5)
		    cFileName = 0;	// Forget it
		idState = GetPartNum;
		pNewPartNum = pHere;
		break;
	    default:
		idState = GetPartNum;
		pNewPartNum = pHere;
		break;
	    }
	    break;
	case ')':
	case ']':
	    switch (idState)
	    {
	    case GetFileName:
		cFileName = pHere - pFileName;
		idState = GetLine;
		break;
	    case GetPartNum:
		idState = GetLine;
		pStart = pHere;
		break;
	    case GetPartCnt:
		// Got valid part number and count
		pPartNum = pNewPartNum;
		cPartNum = cNewPartNum;
		pPartCnt = pNewPartCnt;
		cPartCnt = pHere - pPartCnt;
		idState = GetLine;
		cNewPartNum = 0;
		pStart = pHere;
		break;
	    }
	    break;
	case '|':
	    switch (idState)
	    {
	    case GetFileName:
		cFileName = pHere - pFileName;
		idState = GetLine;
		break;
	    case GetPartNum:
		cNewPartNum = pHere - pNewPartNum;
		idState = GetPartCnt;
		pNewPartCnt = pHere;
		break;
	    case GetPartCnt:
		idState = GetLine;	// Cancel
		cNewPartNum = 0;
		pStart = pHere;
		break;
	    default:
		pStart = pHere;
	    }
	    break;
	case '/':
	case '\\':
	    switch (idState)
	    {
	    case GetFileName:
		pStart = pHere;
		idState = GetLine;
		break;
	    case GetPartNum:
		cNewPartNum = pHere - pNewPartNum;
		idState = GetPartCnt;
		pNewPartCnt = pHere;
		break;
	    case GetPartCnt:
		idState = GetLine;	// Cancel
		cNewPartNum = 0;
		pStart = pHere;
		break;
	    default:
		pStart = pHere;
	    }
	    break;
	case '.':			// filename dot maybe
	    switch (idState)
	    {
	    case GetLine:
		pFileName = pStart;
		idState = GetFileName;
		break;
	    case GetPartNum:
		idState = GetLine;
		pStart = pNewPartNum;	// Set string start
		break;
	    case GetPartCnt:
		idState = GetLine;	// Cancel
		cNewPartNum = 0;
		pStart = pNewPartCnt;
		break;
	    }
	    break;

	case '<':
	case '>':
	case '"':
	    switch (idState)
	    {
	    case GetPartNum:
		idState = GetLine;	// Cancel
		pStart = pHere;
		break;
	    case GetPartCnt:
		idState = GetLine;	// Cancel
		cNewPartNum = 0;
		pStart = pHere;
		break;
	    case GetFileName:
		cFileName = pHere - pFileName;
		idState = GetLine;
		break;
	    default:
		pStart = pHere;
	    }
	    break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    switch (idState)
	    {
	    case GetPartCnt:
	    case GetPartNum:
	    case GetFileName:
		break;			// Include in string
	    default:
		cDigits++;		// Count digits in case free standing number
		break;
	    }
	    break;

	case ' ':			// Blank
	    // Ends something
	    switch (idState)
	    {
	    case GetLine:
		cWord = pHere - pStart;
		if (cDigits && cWord == cDigits + 1)
		{
		    // Don't have part number or count yet
		    if (!cNewPartNum)
		    {
			cNewPartNum = cWord;
			pNewPartNum = pStart;
		    }
		    else
		    {
			if (gotOf)
			{
			    cPartNum = cNewPartNum;
			    pPartNum = pNewPartNum;
			    cPartCnt = cWord;
			    pPartCnt = pStart;
			    gotOf = 0;
			}
			cNewPartNum = 0;	// Forget it
		    }
		}
		// Check for "of" or "fo" (i.e. of misspelled)
		if (cNewPartNum)
		{
		    gotOf = cWord == 3 &&
			    (strnicmp(pStart, "of", 2) == 0 ||
			     strnicmp(pStart, "fo", 2) == 0);
		}
		pStart = pHere;
		cDigits = 0;
		break;
	    case GetFileName:
		cFileName = pHere - pFileName;
		idState = GetLine;
		break;
	    case GetPartNum:
		idState = GetLine;	// Cancel
		pStart = pHere;
		break;
	    case GetPartCnt:
		idState = GetLine;	// Cancel
		cNewPartNum = 0;
		pStart = pHere;
		break;
	    } // switch
	    break; // space

	default:
	    // All other characters
	    switch (idState)
	    {
	    case GetFileName:
		if (ch < 32)
		{
		    // End of file name
		    cFileName = pHere - pFileName;
		    idState = GetLine;
		}
		break;
	    case GetPartNum:
		idState = GetLine;	// Cancel
		pStart = pNewPartNum;
		break;
	    case GetPartCnt:
		idState = GetLine;	// Cancel
		cNewPartNum = 0;
		pStart = pNewPartCnt;
		break;
	    }
	    break;

	} // switch ch
    } // while scanning

    // Check if header indicates multipart
    // String length counts are all plus 1

    if (cFileName && cPartNum && cPartCnt)
    {
	part_num = atol(pPartNum);
	part_total = atol(pPartCnt);

	int cLine = pPartNum - pLine;
	if (cLine == 0)
	  cLine = 1;

	part_line = new char[cLine];
	cLine--;
	strncpy(part_line, pLine, cLine);
	*(part_line + cLine) = '\0';

	part_name = new char[cFileName];
	cFileName--;
	strncpy(part_name, pFileName, cFileName);
	*(part_name + cFileName) = '\0';
    }

    return part_num > 0 && part_total > 1 && part_num <= part_total && part_name;

} // Header::decodeSubject


//=== Header::filter - check if filters matched else -1 ===
// Assume filter array contains lower case strings only

int Header::filter(array<String>& filters, int num_filters)
{
    int rc;
    char *szFilter;
    int fHaveInclude = 0;
    int iExclude = -1;
    int iInclude = -1;

    for (int iFilter= 0; iFilter < num_filters; iFilter++)
    {
	// Filter syntax
	// string - substring exclude
	// ^string - wildcard exclude
	// +string - substring include
	// *string - wildcard include
	// "string" - quoted substring exclude
	// Exclude overrides include match

	szFilter = filters[iFilter].as_charstar();

	regex_t RegEx;
	int haveRegEx = 0;

	if (*szFilter == '*' || *szFilter == '^')
	{
	    // Wildcarded - support extended RE syntax
	    rc = regcomp(&RegEx, szFilter + 1, REG_EXTENDED | REG_NOSUB);
	    if (rc == 0)
		haveRegEx = 1;
	    else
	    {
		log("error %d parsing regex \"%s\"", rc, szFilter + 1);
		continue;
	    }
	}

	// it really only seems necessary to filter on sender
	// & subject.  None of the other fields are pertinant

	for (int iPass = 0; iPass < 2; iPass++)
	{
	    char *pszString;

	    switch (iPass)
	    {
	    case 0:
		if (!lower_subject)
		    lower_subject = new char[1024];
		strcpy(lower_subject,getSubject());
		strlwr(lower_subject);
		pszString = lower_subject;
		break;
	    default:
		if (!lower_sender)
		    lower_sender = new char[1024];
		strcpy(lower_sender,getSender());
		strlwr(lower_sender);
		pszString = lower_sender;
	    }

	    if (!*pszString)
		continue;		// Bypass if empty - probably xhdr sender

	    if (*szFilter == '"')
	    {
		// Quoted substring exclude
		if (strstr(pszString,szFilter + 1))
		{
		    iExclude = iFilter;		// Say excluded
		    break;
		}
	    }
	    else if (*szFilter == '+')
	    {
		// Substring Include
		fHaveInclude = 1;
		if (strstr(pszString,szFilter + 1))
		{
		    iInclude = iFilter;		// Say included
		    break;
		}
	    }
	    else if (*szFilter == '*')
	    {
		// Wildcard Include
		fHaveInclude = 1;
		rc = regexec(&RegEx, pszString, 0, 0, 0);
		if (rc == 0)
		{
		    iInclude = iFilter;		// Say included
		    break;
		}

	    }
	    else if (*szFilter == '^')
	    {
		// Wildcard exclude
		int rc = regexec(&RegEx, pszString, 0, 0, 0);
		if (rc == 0)
		{
		    iExclude = iFilter;		// Say included
		    break;
		}

	    }
	    else if (strstr(pszString,szFilter))
	    {
		// Exclude
		iExclude = iFilter;		// Say excluded
		break;
	    }

	    if (iExclude != -1)
		break;			// Stop at 1st exclude

	} // for iPass

	if (haveRegEx)
	    regfree(&RegEx);

	if (iExclude != -1)
	    break;			// Stop at 1st exclude

    } // for iFilter

    // Return match type
    // fixme to be cleaner
    if (iExclude != -1)
	rc = - iExclude - 1;		// Say specific exclude
    else if (iInclude != -1)
	rc = iInclude + 1;		// Say specific include
    else if (fHaveInclude)
	rc = - num_filters - 1;		// Say non-specific exclude
    else
	rc = 0;				// Say non-specific include

    return rc;

} // Header::filter


int Header::getArticleNum()
{
    return article;
}


char *Header::getContent(void)
{
    return block;
}


//=== Header::getSubject: return subject or empty string ===

char *Header::getSubject(void)
{
    if (subject)
	return subject;
    else
    {
	subject = new char[1024];
	*subject='\0';
	char *ptr = strstr(block,"Subject:");
	if (ptr && strlen(ptr) > 10)
	{
	    ptr = strchr(ptr,':');
	    if (ptr)
	    {
		ptr++;
		while (*ptr == ' ')
		    ptr++;  // trim left whitespace
		char *end = strchr(ptr,'\r');
		if (end)
		    *end='\0';		// Allow strcpy - fixme
		strcpy(subject,ptr);
		if (end)
		    *end='\r';		// Restore
	    }
	}

	// be sure we don't have non-7bit chars in subject
	ptr = subject;
	while (*ptr)
	{
	    if (*ptr < 32 || *ptr > 126)
		*ptr = '_';
	    ptr++;
	}
    }

    return subject;
}


//=== Header::getSender: return sender or empty string ===

char *Header::getSender(void)
{
    if (sender)
	return sender;
    else
    {
	sender = new char[1024];
	*sender='\0';
	char *ptr = strstr(block,"From:");
	if (ptr && strlen(ptr) > 7)
	{
	    ptr = strchr(ptr,':');
	    if (ptr)
	    {
		ptr++;
		while (*ptr == ' ')
		    ptr++;  // trim left whitespace
		char *end = strchr(ptr,'\r');
		if (end) *end='\0';	// Allow strcpy
		strcpy(sender,ptr);
		if (end) *end='\r';	// Restore
	    }
	}

	// be sure we don't have non-7bit chars in sender
	ptr = sender;
	while (*ptr)
	{
	    if (*ptr < 32 || *ptr > 126)
		*ptr = '_';
	    ptr++;
	}
    }

    return sender;

} // Header::getSender



//=== Header::isMultiPart: return TRUE if multipart else FALSE ===
// Decode subject line as side effect

int Header::isMultiPart()
{
    // Quick check, if this is a second call, just return the
    // values we have already calculated
    if (!block || part_num == 0 || part_num > part_total || part_total == 1)
	return 0;	// Say no

    if (part_num > 0 && part_num <= part_total && part_total > 1)
	return 1;	// Say yes

    part_num = 0;			// Assume not multipart

    // Find subject in block

    int isMulti = 0;
    char *ptr = strstr(block,"Subject:");

    if (ptr && strlen(ptr) > 15)
    {
	char *end = strchr(ptr,'\n');
	if (end)
	{
	    int c = end - ptr - 9;
	    char *buffer = new char[c + 1];
	    strncpy(buffer, ptr + 9, c);
	    *(buffer + c) = '\0';
	    isMulti = decodeSubject(buffer);
	    delete []buffer;
	}
    }

    return isMulti;

} // Header::isMultiPart


int Header::numLines(void)
{
    if (block && lines == -2)
    {
	char *ptr = strstr(block,"Lines:");
	if (ptr && strlen(ptr) > 6)
	{
	    ptr+=6;  // skip 6 chars
	    lines = atoi(ptr);
	}
	else lines = 0;
    }
    else
    {
	lines = 0;
    }

    return lines;
}

int Header::partNumber()
{
    if (part_num < 0)
	isMultiPart();

    return part_num;
}


char *Header::partName()
{
    if (part_num < 0)
	isMultiPart();

    return (part_name?part_name:"");
}


char *Header::partString()
{
    if (part_num < 0)
	isMultiPart();

    return (part_line?part_line:"");
}


int Header::partTotal()
{
    if (part_total < 0)
	isMultiPart();

    return part_total;
}


