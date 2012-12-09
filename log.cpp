/* log.cpp,v 1.2 2001/06/20 00:42:19 root Exp */

#include "log.h"

/*
 * Write to a logfile if we have one defined, otherwise skip it
 *
 */

static char logfile_path[_MAX_PATH];
static FILE *out;
static int log_on;
static char *buffer;


void setLogfilePath(char *p)
{
    if (out)
    {
	fclose(out);
	out = 0;
    }

    if (buffer)
    {
        delete[] buffer;
	buffer = 0;
    }

    if (p)
        strcpy(logfile_path,p);
    else
        *logfile_path = 0;

} // setLogfilePath


void toggleLog(int x)
{
    log_on = x;

    if (!log_on)
    {
        if (out)
        {
	    fclose(out);
	    out = 0;
        }
        if (buffer)
        {
            delete[] buffer;
	    buffer = 0;
        }
    }
} // toggleLog

void log(char *format,...)
{
    if (log_on && *logfile_path)
    {
        va_list  arguments;

	if (!buffer)        
            buffer = new char[4096];

        if (format && buffer) {
            // build error message using argument list on format
            va_start(arguments,format);
            vsprintf(buffer,format,arguments);
            va_end(arguments);
        
	    if (!out)
                out = fopen(logfile_path,"a");		// fixme to complain

            if (out)
            {
                fprintf(out,"%s\n",buffer);
                fflush(out);
            }
        
        }
	else if (out)
	{
	    // Null pointer requests close
	    fclose(out);
	    out = 0;
	}
    }

} // log
