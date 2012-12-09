
/***********************************************************************

newsserver.cpp,v 1.4 2004/12/05 00:59:57 root Exp

Newsserver class methods

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2003 Steven Levine and Associates, Inc.
All rights reserved.

Revisions	04 Jul 01 SHL - getBody: change Bandwidth limit faster
		15 Dec 03 SHL - Rename BUFFERSIZE to NS_BUFFER_SIZE

***********************************************************************/

#include "newsserver.h"

int is_sleeping = 0;

#ifdef WIN32
// used to end the groups download prematurely
int enddownload=0;	
#endif // WIN32

// Allocate rx() queue size as buffer size plus 1 line (approx) to avoid Charqueue resize
const unsigned long QUEUE_SIZE = NS_BUFFER_SIZE + 82;

NewsServer::NewsServer(char *net_address,int port_number) : incoming(QUEUE_SIZE)
{
#ifdef WIN32
    if (!initWinsock())
        fprintf(stderr, "Winsock not initialised\n");
    if (enddownload)
        return;
#endif
    set(net_address,port_number);
}

NewsServer::NewsServer(char *net_address) : incoming(QUEUE_SIZE)
{
#ifdef WIN32
    if (!initWinsock())
        fprintf(stderr, "Winsock not initialised\n");
#endif
    set(net_address,119);
}

#ifdef WIN32
// gavin: i added this to get status of connect back to gui app
NewsServer::NewsServer(char *net_address, int *status) : incoming(QUEUE_SIZE)
{
    if (!initWinsock())
        fprintf(stderr, "Winsock not initialised\n");
    set(net_address,119, status);
}
#endif

NewsServer::NewsServer()
{
#ifdef WIN32
    if (!initWinsock())
        fprintf(stderr, "Winsock not initialised\n");
#endif
    address = 0;
    group = 0;
    set("localhost",119);
    memset(socks_server,0,sizeof(socks_server));
}

NewsServer::~NewsServer()
{
    if (sock > 0)
        close();
    delete[] address;
    delete[] group;

#ifdef WIN32
    removeWinSock();
#endif

}

void NewsServer::set(char *addr,int p)
{
    debug_file = 0;
    
    memset(server_signon_message,'\0',sizeof(server_signon_message));
    memset(socks_server,0,sizeof(socks_server));
    
    if (!addr)
    {
        address = new char[12];
        strcpy(address,"localhost");
    }
    else
    {
        address = new char[1+strlen(addr)];
        strcpy(address,addr);
    }
    
    if (p == 0)
        port = 119;
    else
        port = p;
    
    sock = -1;
    group = 0;
    resetCPS();
    
    end_of_return = ".\n";
    
#ifdef OS2
    sock_init();
#endif
    
    long theaddr=inet_addr(addr); 
    if (theaddr==INADDR_NONE) 
	host = gethostbyname(addr); 
    else 
	host=gethostbyaddr((char *)&theaddr,4,AF_INET); 

    if (host)
    {
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        memcpy(&server.sin_addr,host->h_addr,host->h_length);
    }
    
    bandwidth_limit = 0; // no limit
    
    timeout = 60;
    no_xhdr = 0;
    xhdr_cache_size = 16;
    xhdr_lines_cache[0].set(0,NULL,0);
    xhdr_subject_cache[0].set(0,NULL,0);

//    xhdr_type_cache[0].set(0,NULL,0);
//    xhdr_encoding_cache[0].set(0,NULL,0);
//    xhdr_disposition_cache[0].set(0,NULL,0);

}


// gavin: added this so that harvest does not exit if the
// host is not found! (simply returns an error)

#ifdef WIN32
void NewsServer::set(char *addr,int p, int* status)
{
    debug_file = 0;
    memset(socks_server,0,sizeof(socks_server));
    
    if (!addr)
    {
        address = new char[strlen("localhost")+1];
        strcpy(address,"localhost");
    }
    else
    {
        address = new char[1+strlen(addr)];
        strcpy(address,addr);
    }
    
    if (p == 0)
        port = 119;
    else
        port = p;
    
    sock = -1;
    group = 0;
    resetCPS();
    
    end_of_return = ".\n";

    long theaddr=inet_addr(addr); 

    if (theaddr==INADDR_NONE) 
        host = gethostbyname(addr); 
    else 
        host=gethostbyaddr((char *)&theaddr,4,AF_INET); 

    if (host)
    {
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        memcpy(&server.sin_addr,host->h_addr,host->h_length);
    }
#ifndef NOPRINT
    else
    {
        fprintf(stderr,"%s: unknown host\n",addr);
        *status = 0;
        return;
    }
#endif
    
    bandwidth_limit = 0; // no limit    
    
    timeout = 60;
    no_xhdr = 0;
    xhdr_cache_size = 16;
    xhdr_lines_cache[0].set(0,NULL,0);
    xhdr_subject_cache[0].set(0,NULL,0);
    //    xhdr_type_cache[0].set(0,NULL,0);
    //    xhdr_encoding_cache[0].set(0,NULL,0);
    //    xhdr_disposition_cache[0].set(0,NULL,0);
    
    *status=1;
}
#endif

void NewsServer::setSocksServer(char *addr)
{
    if (addr)
    {
        strncpy(socks_server,addr,80);
    }
    else
    {
        memset(socks_server,0,sizeof(socks_server));
    }
}


int NewsServer::open()
{
    if (sock > 0)
        close();
    
    sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock < 0)
    {
#ifdef OS2
	log("Error: open - open failed (%d)", sock_errno());
        DosBeep(200,200);
#endif

#ifndef NOPRINT
#ifdef OS2
        psock_errno("connecting socket");
#else
        printf("Error connecting socket");
#endif
        printf("\n");
#endif
        return 0;
    }
    
    // set socket input buffer size
    long buf_size = 8192;
    setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char*)&buf_size,sizeof(buf_size));
    // turn on the keepalive
    int toggle = 1;
    setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,(char*)&toggle,sizeof(toggle));
    
    // make non-blocking

//    int dontblock = 1;
//    if (ioctl(sock,FIONBIO,(char*)&dontblock,sizeof(dontblock)))
//    {
//        psock_errno("ioctl()");
//        printf("\n");
//        return 0;
//    }
    
    if (connect(sock,(struct sockaddr *)&server,sizeof(server)) < 0)
    {
#ifdef OS2
#endif

#ifndef NOPRINT
#ifdef OS2
        psock_errno("connect()");
#else
        printf("Error on connect()");
#endif
        printf("\n");
#endif
        return 0;
    }
    else
    {
        // recieve the sign-on banner from the server -> and junk it
        char *banner = rxLine();
        if (banner)
        {
#ifndef NOPRINT
            printf("%s",banner);
#endif
            // remember the banner to get the sigon message
            strncpy(server_signon_message,banner+4,511);   // +4 to skip the result code
            server_signon_message[511] = '\0';
            
            int result_code = atoi(banner);
            delete[] banner;
            
            if (result_code == 400)
            {
                //rx():
                //34 30 30 20  53 79 73 74  65 6d 20 73  74 61 72 74    400 System start
                //69 6e 67 20  75 70 20 2d  20 54 72 79  20 61 67 61    ing up - Try aga
                //69 6e 20 69  6e 20 61 20  66 65 77 20  6d 69 6e 75    in in a few minu
                //74 65 73 20  28 54 79 70  68 6f 6f 6e  20 76 31 2e    tes (Typhoon v1.
                //31 2e 38 29  0d 0a                                    1.8)..
                
#ifndef NOPRINT
                printf("Server not ready to respond...\n");
#endif
                log("Error: open - server reporting not ready (400)");
                return OPEN_WAIT;
                
            }
        }
        else
        {
            // no banner -> did proxy or summat fail ?
            return OPEN_FAIL;
        }
    }
    
    // used to limit bandwidth
    time(&bandwidth_start_time);
    bandwidth_chars_read = 0;
    bandwidth_chars_read_hist = 0;
    
    return OPEN_OK;
}

int NewsServer::closed()
{
    return (sock <= 0);
}

int NewsServer::close()
{
    if (sock > 0)
    {
#ifdef OS2
        soclose(sock);
#endif
        
#ifdef UNIX
        ::close(sock);
#endif
        
#ifdef WIN32
        _close(sock);
#endif
    }
    
    sock = 0;

    return 0;
}

unsigned long NewsServer::getCPS()
{
    time_t t = time(NULL);
    long delta = (long)((unsigned long)t-(unsigned long)cps_start_time);
    
    if (delta <= 0) return 0;
    if (cps_chars_read == 0) return 0;
    
    return (cps_chars_read / delta);
}

void NewsServer::resetCPS()
{
    cps_start_time = time(NULL);
    cps_chars_read = 0;
}

int NewsServer::tx(char *buffer)
{
    return NewsServer::tx(buffer,strlen(buffer));
}

void NewsServer::setDebugFile(char *filename)
{
    debug_file = filename;
}

int NewsServer::tx(char *buffer,unsigned long length)
{
    unsigned int num;
    
    //    printf("[33mtx() - %s[0m ",buffer);

    if (sock <= 0)
    {
	log("   Error: tx - socket closed");
        return 0;
    }
    else
    {
        if ((num = send(sock,buffer,length,0)) == length)
        {
            if (debug_file && *debug_file)
            {
                FILE *dout = fopen(debug_file,"a");
                if (dout)
                {
                    fprintf(dout,"tx():\n");
                    binCat(dout,buffer,length);
                    fprintf(dout,"\n");
                    fclose(dout);
                }
            }
            
            bandwidth_chars_read += num;
            
            return 1;
        }
        else
        {
            if (debug_file && *debug_file)
            {
                FILE *dout = fopen(debug_file,"a");
                if (dout)
                {
                    fprintf(dout,"tx(): failed, only %d of %ld\n",num,length);
                    if (num != 0)
                    {
                        binCat(dout,buffer,num);
                        fprintf(dout,"\n");
                    }
                    fclose(dout);
                }
            }
            bandwidth_chars_read += num;
	    log("   Error: tx - send failed %u/%u", num, length);
            return 0;
        }
    }
}

// NewsServer::rx: try to receive, return true if something arrived

int NewsServer::rx()
{
    if (sock <= 0)
    {
	log("   Error: rx - socket closed");
        return 0;
    }
    else
    {
        int rc = recv(sock,buffer,sizeof(buffer) - 1,0);
        if (rc > 0)
        {
            if (debug_file && *debug_file)
            {
                FILE *dout = fopen(debug_file,"a");
                if (dout)
                {
                    fprintf(dout,"rx():\n");
                    binCat(dout,buffer,rc);
                    fprintf(dout,"\n");
                    fclose(dout);
                }
            }
            
            bandwidth_chars_read += rc;
            cps_chars_read += rc;

            imposeBandwidthLimit();
            
            incoming.Enqueue(buffer,rc);
            
            return 1;
        }
        else
        {
            if (rc == 0)
            {
#ifndef NOPRINT
                printf("\nConnection terminated by %s\n",address);
#endif
                if (debug_file && *debug_file)
                {
                    FILE *dout = fopen(debug_file,"a");
                    if (dout)
                    {
                        fprintf(dout,"rx():\n");
                        fprintf(dout,"Connection terminated\n");
                        fprintf(dout,"\n");
                        fclose(dout);
                    }
                }
	        log("   Error: rx - socket closed by no data");
                sock = 0;
            }
            else if (rc == -1)
            {
                char *err_msg = "Error: rx - unknown error\n";
#ifdef OS2
                int   err_num = sock_errno();
	        log("   Error: rx - socket error (%u)", err_num);
#else
                int   err_num = 0;
#endif
/*                switch (err_num)
                {
                    case SOCKENOTSOCK:
                        err_msg = "Socket is not a valid socket\n";
                        break;
                        
                    case SOCKEFAULT:
                        err_msg = "Socket buffer over-run\n";
                        break;
                        
                    case SOCKINTR:
                        err_msg = "Socket recv() interrupted\n";
                        break;
                        
                    case SOCKINTR:
                        err_msg = "Socket invalid argument\n";
                        break;
    
                    case SOCKEWOULDBLOCK:
                        err_msg = "Socket timed out\n";
                        break;
    
                    default:
                        err_msg = "Socket - Unknown Error\n";
                }
*/
#ifndef NOPRINT
                printf("\nSocket Receive Error %d\n\t\"%s\"\n",err_num,err_msg);
#endif
                if (debug_file && *debug_file)
                {
                    FILE *dout = fopen(debug_file,"a");
                    if (dout)
                    {
                        fprintf(dout,"rx():\n");
                        fprintf(dout,"Error %d: %s",err_num,err_msg);
                        fprintf(dout,"\n");
                        fclose(dout);
                    }
                }
            }
            else
            {
	        log("   Error: rx - unknown socket error");
#ifndef NOPRINT
                printf("\nUnknown Socket Receive Error\n");
#endif
                if (debug_file && *debug_file)
                {
                    FILE *dout = fopen(debug_file,"a");
                    if (dout)
                    {
                        fprintf(dout,"rx():\n");
                        fprintf(dout,"Socket rx() error\n");
                        fprintf(dout,"\n");
                        fclose(dout);
                    }
                }
            }
        }
    }

    return 0;

} // NewsServer::rx

char *NewsServer::lineSansRx()
{
    if (incoming.Empty())
        return 0;
    else
    {
        CharQueue line(1024);
        char *line_buffer = 0;
        int   count = 0;
        char  ch = 0;
        
        // Try to build a whole line from input
        while (!incoming.Empty() && ch != '\n')
        {
            ch = incoming.Dequeue();
            count++;
            line.Enqueue(ch);
        }
        
        if (incoming.Empty() && ch != '\n')
        {
            // The incoming Queue is empty, and did not contain a full line.
            // Put all the data back into the incoming queue, because it will
            // eventually form a full line.
            
            // maybe optimise this
            
            while (!line.Empty())
                incoming.Enqueue(line.Dequeue());
            
            return 0;
        }
        else
        {
            // We have a full line in the line queue, that is <count> long
            line_buffer = new char[count+2];
            for (int i=0; i < count; i++)
                line_buffer[i] = line.Dequeue();
            line_buffer[count] = '\0';
            
            return line_buffer;
        }
    }
}

char *NewsServer::rxLine()
{
    char *result;
    
    result = lineSansRx();
    if (!result)
    {
        // There was no valid line in the buffer, see
        // if we can get another block from the server.
        rx();
        result = lineSansRx();
    }
    
    return result;
}

// NewsServer::attempt: send command, get response

char *NewsServer::attempt(char *command)
{
    char *result;
    tx(command,strlen(command));
    result = rxLine();

    // Check if userid/password wanted
    if (result && strlen(result) >= 3 && strncmp(result,"480",3)==0)
    {
        delete[] result;
        reLogin();
        tx(command,strlen(command));
        result = rxLine();
    }
    
    return result;
}


NewsGroup *NewsServer::setGroup(char *name)
{
    // imposeBandwidthLimit();
    
    // time_t t = time(NULL)+timeout;
    NewsGroup *ng = 0;
    char *result = 0;
    
    xhdr_cached_from = -1;
    xhdr_cached_upto = -1;
    
    if (sock <= 0)
	log("Error: setGroup - socket closed");
    else if (!name)
	log("Error: setGroup - group name null");
    else
    {
        char group_command[] = {"group %s\r\n"};
        int   command_len = strlen(group_command)+strlen(name);
        char *command = new char[1+command_len];
        sprintf(command,group_command,name);
        
        result = attempt(command);
            
        delete[] command;

        if (!result)
        {
	    log("Error: setGroup - no reply to GROUP command");
#ifndef NOPRINT
            fprintf(stderr,"No reply from server (trying again)...\n");
#endif
        }
        else
        {
            // printf("The newsserver responded %s",result);
            unsigned long code,num,first,last;
            
            sscanf(result,"%ld",&code);
            
            if (code == 211)
            {
                // printf("Able to scan %d values from group line\n",sscanf(result,"%d %d %d %d ",&code,&num,&first,&last));
                // printf("These are code=%d, num=%d, first=%d, last=%d\n",code,num,first,last);

                if (sscanf(result,"%ld %ld %ld %ld ",&code,&num,&first,&last) == 4)
                    ng = new NewsGroup(address,name,num,first,last);
		else
                    log("Error: setGroup - GROUP command response garbled - \"%s\"", result);
            }
	    else
	    {
                if (code == 400)
                    log("Server not ready for GROUP command (400)");
                else
                    log("Error: setGroup - GROUP command rejected - \"%s\"", result);
	    }
            
            delete[] result;
        }
    }

    return ng;
}

int NewsServer::usingXhdr(void)
{
   return !no_xhdr;
}

char *NewsServer::getHead(unsigned long article)
{
    char *result;
    
    if (!no_xhdr)
    {
        // xhdr still turned ON?
        result = getXhdrHead(article);
        // reasons for result == NULL:
        //     the xhdr command fails   (no_xhdr should toggle)
        //     the article does not exist
        
        // did the xhdr command fail - use old method
        if (!result && no_xhdr)
            result = getLongHead(article);  // use the old method
    }
    else
        result = getLongHead(article);
    
    return result;
}

char *NewsServer::getLongHead(unsigned long article)
{
    char *result = 0;
    char *head = 0;
    CharQueue full_head(4096);
    int finished = 0;
    int line = 0;
    
    // imposeBandwidthLimit();
    
    if (!article)
        log("   Attempting to fetch article 0");
    else if (sock <= 0)
        log("   Error: getLongHead - socket closed");
    else
    {
        char command_base[] = {"head %lu\r\n"};
        char *command = new char[21+strlen(command_base)];
        sprintf(command,command_base,article);
        result = attempt(command);
        delete[] command;
        
        resetCPS();
        
        // news Server returns article status first
        if (!result)
	{
            log("   Error: getLongHead - no response to head command");
            finished = 1;
	}
        else
        {
            if (strlen(result) < 3)
	    {
                log("   Error: getLongHead - short response to head command - \"%s\"", result);
                finished = 1;
	    }
            else if (strncmp(result,"221",3))
            {
		int c = strlen(result);
                log("   Error: getLongHead - unexpected response to head command \"%.*s\"", c, result);
                finished = 1;
            }
        }
        
        delete[] result;
        result = 0;

        // Record header
        while (!result && !finished)
        {
            result = rxLine();
#ifndef NOPRINT
            if (line % 100 == 0)
            {
                float cps = (float)getCPS();
                cps /= 1024;
                printf("%4.1f k/sec\b\b\b\b\b\b\b\b\b\b",cps);
                fflush(stdout);
            }
#endif
            
            if (!result)
            {
#ifdef WIN32
	        if (running == 0)
		    return 0;
#endif
                if (sock == 0)
                {
#ifndef NOPRINT
                    fprintf(stderr,"\nNo reply from server (giving up)...\n");
#endif
                    log("   Error: getLongHead - socket closed");
                    return 0;
                }
                else
                {
		    // since we got no response, sleep so that the thread is not
		    // looping and red-lining man!
		    msecSleep(200);
                }
            } 
            else
            {
                line++;
                if (strlen(result) >= 2 && result[0] == '.' &&
		    (result[1] == 10 || result[1] == 13))
                {
                    finished = 1;
                }
                else
                {
                    char *ptr = result;
		    // fixme to handle ..
                    while (*ptr != '\0')
                    {
                        full_head.Enqueue(*ptr);
                        ptr++;
                    }
                }
                
                delete[] result;
                result = 0;
            }
        } // while !result and !finished

        if (!full_head.Empty())
        {
            head = new char[10+full_head.ContentSize()];
            if (!head)
            {
                log("   Error: getLongHead - new failed");
            }
            else
            {
                // memset(head,0,10+full_head.ContentSize());
                int index = 0;
                while (!full_head.Empty())
                {
                    char ch = full_head.Dequeue();
                    head[index] = ch;
                    index++;
                }
                head[index]='\0';
            }
        }
    }

    return head;

} // NewsServer::getLongHead


char *NewsServer::getXhdrHead(unsigned long article)
{
    array<XhdrPacket> list;
    int num_results;
    char *format = "Lines: %d\r\nSubject: %s\r\n";
    int i;
    
    if (xhdr_cache_size != 0 && article != 0)
    {
        long articleAsLong = (long)article;
        
        // If cache not loaded or article not in cache reload cache
        if (xhdr_cached_from == -1 ||
            articleAsLong < xhdr_cached_from ||
	    articleAsLong > xhdr_cached_upto)
        {
            num_results = getArticleLines(list,article,article+(xhdr_cache_size==1?1:xhdr_cache_size-1));
          
            // fill the cache with article numbers
            for (i = 0; i < xhdr_cache_size; i++)
            {
                if (i < num_results)
                {
                    XhdrPacket &xpr = list[i];
                    xhdr_lines_cache[i] = xpr;  // uses XhdrPacket.operator= ()
                }
                else
                    xhdr_lines_cache[i].set(0, 0, 0);
	    }

            if (num_results > 0)
	    {
	        // Get subjects for cached article numbers
		// fixme to handle inconsistent article number list
                num_results = getArticleSubjects(list,article,article+(xhdr_cache_size==1?1:xhdr_cache_size-1));
	    }

            // fill the cache with article subjects
            for (i = 0; i < xhdr_cache_size; i++)
            {
		if (i < num_results)
		{
                    XhdrPacket &xpr = list[i];
                    xhdr_subject_cache[i] = xpr; // uses XhdrPacket.operator= ()
                    // printf("Cache[%d] loded with %ld->%d/%s\n",i,list[i].getArticleNum(),xhdr_lines_cache[i].getDataAsInt(),list[i].getDataAsChar());
		}
		else
		    xhdr_subject_cache[i].set(0, 0, 0);
            }
        } // if empty

        // Check if possible for the article to be in the cache ??
        // i.e.: Have we read the details for a range including this article

        if (xhdr_cached_from != -1 &&
            articleAsLong >= xhdr_cached_from &&
	    articleAsLong <= xhdr_cached_upto)
        {
            // check if it's in the cache
            for (i = 0; i < xhdr_cache_size; i++)
            {
                if (article == xhdr_lines_cache[i].getArticleNum())
                {
		    // Found article header in cache
                    /*
                    printf("Hit on cache[%d] loded with %ld->%d/%s\n",i,
                           xhdr_lines_cache[i].getArticleNum(),
                           xhdr_lines_cache[i].getDataAsInt(),
                           xhdr_subject_cache[i].getDataAsChar());
                    */
                    
                    int formatlen = strlen(format);
                    int subjectlen = strlen(xhdr_subject_cache[i].getDataAsChar());
                    
                    char *buffer = new char[formatlen + 10 + subjectlen + 10];
                    sprintf(buffer,format,xhdr_lines_cache[i].getDataAsInt(),xhdr_subject_cache[i].getDataAsChar());
                    return buffer;
                }
            }
        }
    } // if cache defined
    
    if (!no_xhdr)
        log("   Article cancelled (xhdr)");
    return 0;				// Say not in xhdr cache

} // NewsServer::getXhdrHead


char *NewsServer::getBody(unsigned long article,
		          int target_lines,
			  int (* callback)(const float, const int, const int),
			  Configuration* pCfg)
{
    char *result = 0;
    char *body = 0;
    CharQueue full_body(target_lines * 80);
    int finished = 0;
    int line = 0;
    
    // imposeBandwidthLimit();

    // Return pointer to body or 0
    
    if (!article)
        log("   getBody: attempting to fetch article 0");
    else if (!sock)
        log("   Error: getBody - socket closed");
    else
    {
        char command_base[] = {"body %lu\r\n"};
        char *command = new char[21+strlen(command_base)];
        sprintf(command,command_base,article);
        result = attempt(command);
        
        delete[] command;

        resetCPS();
        
        // news Server returns article status first
        if (!result)
	{
            log("   Error: getBody - no response to body command");
            finished = 1;
	}
	else
        {
            if(strlen(result) < 3)
	    {
                log("   Error: getBody - short response to body command");
                finished = 1;
	    }
            else if(strncmp(result,"222",3))
	    {
	        int c = strlen(result) - 2;
                log("   Error: getBody - unexpected response to body command - \"%.*s\"", c, result);
                finished = 1;
            }
        }
        
        delete[] result;
        result = 0;

        // Record article body
        while (!result && !finished)
        {
            result = rxLine();

	    // Show progress
            if (line % 100 == 0)
            {
                float cps = (float)getCPS();
                cps /= 1024;
                
                if (!callback)
                {
#ifndef NOPRINT
                    if (target_lines == 0)
                        printf("%4.1f k/sec %5d lines\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",cps,line);
                    else
                        printf("%4.1f k/sec %3d%%        \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",cps,100*line/target_lines);
                    fflush(stdout);
#endif
                    
                }
                else
                {
                    if (!callback(cps,(target_lines?(100*line/target_lines):100),line))
                    {
                        // we have been asked to terminate
                        delete[] result;
                        log("   Terminating on request");
                        return 0;
                    }
                }

		// In case changed
                setBandwidthLimit(pCfg->getBandwidthLimit());
            }
            
            if (!result)
            {
		// Waiting for line to arrive
#ifdef WIN32
	        if (!running)
		    return 0;
#endif
                if (sock == 0)
                {
#ifndef NOPRINT
                    fprintf(stderr,"\nNo reply from server (giving up)...\n");
#endif
                    log("   Error: getBody - socket closed");
                    return 0;
                }
                else
                {
//                  printf("\nServer timed out (trying again)...\n");

                    // since we got no response, sleep so that the thread is not
		    // looping and red-lining man!
		    msecSleep(200);
                }
            } 
            else
            {
		// Got line
                line++;
                int result_len = strlen(result);
		// Body ends with single .
                if (result_len >= 2 && result[0] == '.' &&
		    (result[1] == 10 || result[1] == 13))
                {
                    finished = 1;
                }
                else
                {
		    // fixme to handle leading .. correctly
                    full_body.Enqueue(result,result_len);
                }
                
                delete[] result;
                result = 0;
            }
        } // while !result and !finished

        if (!full_body.Empty())
        {
            body = new char[10+full_body.ContentSize()];
            if (body)
            {
                //  memset(body,0,10+full_body.ContentSize());
                int index = 0;
                while (!full_body.Empty())
                {
                    char ch = full_body.Dequeue();
                    body[index] = ch;
                    index++;
                }
                body[index] = '\0';
            }
        }
    }

    return body;

} // NewsServer::getBody


int NewsServer::getGroupList(array<char *> &list)
{
    return getGroupList(list,NULL,NULL);
}

// since should be in form "YYMMDD HHMMSS"
int NewsServer::getGroupList(array<char *> &list,int *num_groups,char *since)
{
    char *result = 0;
    int group_count = 0;
    int skipped_header = 0;
    char command[80] = "list\r\n";
    
    if (since)
        sprintf(command,"newgroups %s\r\n",since);
    
    if (sock <= 0)
	log("Error: getGroupList - socket closed");
    else if (tx(command))
    {
        int finished = 0;
        while (!finished && sock > 0)
        {
#ifdef WIN32 
	    // check WIN32  pre-mature termination
	    if (enddownload)
		return 0;
#endif
	    result = rxLine();

#ifdef WIN32 
	    // check WIN32  pre-mature termination
	    if (enddownload)
		return 0;
#endif
            if (sock <= 0)
            {
#ifndef NOPRINT
                printf("\nNo reply from server (giving up)...\n");
#endif
	        log("Error: getGroupList - socket closed");
                finished = 1;
            }
            else if (!result)
	    {
	        log("Error: getGroupList - no response to LIST/NEWGROUPS command");
                finished = 1;
	    }
            else
            {
	        // Got response - chop CR/LF
	        char *ptr = strchr(result,'\r');
	        if (ptr)
		    *ptr='\0';
	        ptr = strchr(result,'\n');
	        if (ptr)
		    *ptr='\0';
                
                if (!skipped_header)
	        {
                    if (strlen(result) < 3)
		    {
	                log("Error: getGroupList - garbled response to LIST/NEWGROUPS command");
                        finished = 1;	// give up
		    }
		    // fixme to be smarter
		    else if (!since && strncmp(result,"215",3) == 0)
                        skipped_header = 1;
                    else if (since && strncmp(result,"231",3) == 0)
                        skipped_header = 1;
                    else if (strncmp(result,"480",3) == 0)
		    {
	                log("getGroupList logging in again.  Server responded: %s", result);
                        reLogin();
		    }
		    else
		    {
	                log("Error: getGroupList - server responded - \"%s\"", result);
                        finished = 1;	// give up
		    }
	        }
                else if (*result == '.' && *(result+1)==0)
                {
	            // List ends with single .
                    finished = 1;
                }
                else
                {
		    // Validate group name
                    int ok = 1;
                    char *chptr = result;
                    
                    // should start with alpha/number
                    if (!isalnum(*result))
                        ok = 0;
                    else
                    {
                        // check all characters are printable
                        while (*chptr)
                        {
                            // should be [A-z0-9.]+
                            if (*chptr == '.' || *chptr == '-' || *chptr == '_' || *chptr == ' ' || isalnum(*chptr))
                            {
                            }
                            else
                            {
                                ok = 0;
                                break;
                            }
                            chptr++;
                        }
                    }
                    
                    if (ok)
                    {
                        list[group_count] = result;
                        group_count++;
                        if (num_groups)
			    *num_groups = group_count;
                        // printf("%5d\b\b\b\b\b",group_count);
                    }
                } // if not end of list
            } // if got response
        } // while !finished

        delete[] result;
    }
    
    list[group_count] = 0;
    
    return group_count;

} // NewsServer::getGroupList


// NewsServer::getArticleLines: get list of article line counts using xhdr

int NewsServer::getArticleLines(array<XhdrPacket> &list,unsigned int low,unsigned int high)
{
    char *result;
    char command[1024];
    int finished = 0;
    int article_count = -1;
    int skipped_header = 0;
    
    xhdr_cached_from = low;
    xhdr_cached_upto = high;
    
    // imposeBandwidthLimit();
    
    if (sock > 0)
    {
        sprintf(command,"xhdr lines %u-%u\r\n",low,high);
        
        tx(command);
        while (!finished)
        {
            result = rxLine();
            if (!result || sock == 0)
            {
#ifndef NOPRINT                    
                printf("\nNo reply from server (giving up)...\n");
#endif
                delete[] result;
                finished = 1;
            }
            else
            {
                if (*result == '.' && *(result+1) !='.')
                    finished = 1;
                else
                {
                    char *ptr = strchr(result,'\r'); if (ptr) *ptr='\0';
                    ptr = strchr(result,'\n'); if (ptr) *ptr='\0';
                    
                    if (strlen(result) > 2 && skipped_header)
                    {
                        ptr = strchr(result,' ');
                        if (ptr)
                        {
                            unsigned long art_num;
                            int data;
                            
                            // read tuple like "21248 304"  ie: <artticle #> <lines>
                            art_num = atol(result);
                            data = atoi(ptr+1);
                            
                            if (article_count == -1)
                                article_count = 0;
                            
                            list[article_count].set(art_num,(void*)&data,sizeof(int));
                                
                            article_count++;
#ifndef NOPRINT
                            printf("%5d\b\b\b\b\b",article_count);
#endif
                        }
                    }
                    else
                    {
                        // read the "221 <type> fields follow"
                        if (strncmp(result,"221",3) == 0)
                        {
//                            for (int x=0; x<(high-low); x++)  // prime cache with zeros
//                                list[i].set(0,NULL,0);
                            skipped_header = 1;
                        }
                        else if (strncmp(result,"480",3) == 0)
                            reLogin();
                        
                        // read "50* What?/instructions
                        else if (strncmp(result,"50",2) == 0)
                        {
                            finished = 1;
                            no_xhdr = 1;
                        }
                    }
                }
                delete[] result;
            } // if rx line
        }
    }
    
    return article_count;

} // NewsServer::getArticleLines


// NewsServer::getArticleSubjects: get list of article subjects via xhdr

int NewsServer::getArticleSubjects(array<XhdrPacket> &list,unsigned int low,unsigned int high)
{
    char *result;
    char command[1024];
    int finished = 0;
    int article_count = -1;
    int skipped_header = 0;
    
    // imposeBandwidthLimit();
    
    if (sock > 0)
    {
        sprintf(command,"xhdr subject %u-%u\r\n",low,high);
        if (tx(command))
        {
            while (!finished)
            {
                result = rxLine();
                if (!result || sock == 0)
                {
#ifndef NOPRINT                    
                    printf("\nNo reply from server (giving up)...\n");
#endif
                    delete[] result;
                    finished = 1;
                }
                else
                {
                    if (*result == '.' && *(result+1)!='.')
                    {
                        finished = 1;
                    }
                    else
                    {
                        char *ptr = strchr(result,'\r'); if (ptr) *ptr='\0';
                        ptr = strchr(result,'\n'); if (ptr) *ptr='\0';
                        
                        if (strlen(result) > 2 && skipped_header)
                        {
                            ptr = strchr(result,' ');
                            if (ptr)
                            {
                                unsigned long art_num;
                                
                                // read tuple like "21248 the quick brown fox"  ie: <artticle #> <subject>
                                art_num = atol(result);
                                
                                if (article_count == -1)
                                    article_count = 0;
                                
                                list[article_count].set(art_num,ptr+1,1+strlen(ptr+1));
                                
                                article_count++;
#ifndef NOPRINT
                                printf("%5d\b\b\b\b\b",article_count);
#endif
                            }
                        }
                        else
                        {
                            // read the "221 <type> fields follow"
                            if (strncmp(result,"221",3) == 0)
                                skipped_header = 1;
                            if (strncmp(result,"480",3) == 0)
                                reLogin();
                            // read "50* What?/instructions
                            else if (strncmp(result,"50",2) == 0)
                            {
                                finished = 1;
                                no_xhdr = 1;
                            }
                        }
                    }
                    delete[] result;
                }
            } // while !finished
        } // if tx OK
    } // if socket exists
    
    return article_count;
}

void NewsServer::imposeBandwidthLimit()
{
    if (bandwidth_limit > 0)
    {
	// Check every 100 characters
        
        if (bandwidth_chars_read - bandwidth_chars_read_hist > 100)
	{
	    bandwidth_chars_read_hist = bandwidth_chars_read;
            time_t now = time(NULL);

            if (now > bandwidth_start_time)
            {
                // Calculate transfer rate over 30 second moving average
	        // This ensures changes bandwidth changes are honored quickly
	        // without going to sleep forever

                float elapsed_seconds  = (float)(now - bandwidth_start_time);
                float chars_read = (float)bandwidth_chars_read;

                float cps = chars_read / elapsed_seconds;
                
                float max_cps = (float)bandwidth_limit;

                // So did we go over the limit
                if (cps > max_cps)
                {
                    // YES!
                    // But how many seconds _should_ it have taken
                    float expected_seconds = chars_read / max_cps;
                    float wait_seconds = expected_seconds - elapsed_seconds;

                    float m_sec = 1000 * wait_seconds;
                    unsigned long int_m_sec = (unsigned long)m_sec;

		    if (int_m_sec > 5000)
		        int_m_sec = 5000;	// Avoid long sleep

		    /* fixme
                    log("Actual bandwidth == %5.2lf b/s, should be %5.2lf b/s",cps,bandwidth_limit);
                    log("Sleeping for %d milliseconds",int_u_sec);
		    */

		    // Recalc 20 sec moving average - approximately
		    if (elapsed_seconds > 20)
		    {
		        // Calc chars per 20 sec
		        bandwidth_chars_read = (unsigned long)(cps * 20);
		        bandwidth_start_time = now - 20;
		    }
                    
                    is_sleeping = 1;
                    doSleep(int_m_sec);
                    is_sleeping = 0;
                    
                }
            } // if at least 1 sec elapsed
        }
    } // if limiting
} // NewsServer::imposeBandwidthLimit

int NewsServer::login(char *username,char *password)
{
    // authinfo user me pass test
    char *format1 = "authinfo user %s\r\n";
    char *format2 = "authinfo pass %s\r\n";
    char command[1024];
    char *result;
    int finished = 0;
    int success = 0;
    
    strcpy(last_username,(username?username:""));
    strcpy(last_password,(password?password:""));
    
    if (sock > 0 && username && *username != '\0' && password && *password!='\0')
    {
        sprintf(command,format1,username);
        if (tx(command))
        {
            while (!finished)
            {
                result = rxLine();
                if (!result || sock == 0)
                {
#ifndef NOPRINT                    
                    printf("\nNo reply from server (giving up)...\n");
#endif
                    delete[] result;
                    finished = 1;
                }
                else
                {
                    // read the "381 Enter pass"
                    if (strncmp(result,"381",3) == 0)
                    {
                        delete[] result;
                        sprintf(command,format2,password);
                        if (tx(command))
                            result = rxLine();
                        
                        if (*result == '2')
                        {
                            success = 1;
                        }
                        else
                        {
                            // 502 Authentication error
                            success = 0;
                        }
                     }
                    
                    finished = 1;
                    delete[] result;
                }
            }
        }
    }
    
    // password fails -> server closes connection
    return success;
    
}


#ifdef WIN32

// returns 0 on failure, 1 on success
int NewsServer::initWinsock()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 1, 1 );
    if (WSAStartup(wVersionRequested, &wsaData) != 0) return 0;
    else return 1;
}

// returns 0 on failure, 1 on success
int NewsServer::removeWinSock()
{
    if (WSACleanup() != 0) return 0;
    else return 1;
}


#endif // WIN32


int NewsServer::reLogin()
{
    return login(last_username,last_password);
}


void NewsServer::setBandwidthLimit(double bytes_per_sec)
{
    bandwidth_limit = bytes_per_sec;
}

