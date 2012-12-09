
/***********************************************************************

download.cpp,v 1.6 2007/04/26 03:07:22 root Exp

Newsgroup and Download threads

Copyright (c) 1997, 1999 Pty. Limited
Copyright (c) 2000, 2005 Steven Levine and Associates, Inc.
All rights reserved.

04 Jul 01 SHL processGroupThread: pass Configuration to getBody
30 Nov 01 SHL processGroupThread: quit fast if skip toggles on
30 Nov 01 SHL processGroupThread: allow buffers to clear before switch to
18 Nov 03 SHL SKIP_AHEAD: disable - add to config someday
03 Oct 05 SHL processGroupThread: doc long name issue
13 Oct 05 SHL processGroupThread: use 0 to disable part count limit

***********************************************************************/

//=== download.cpp - download progress monitor thread

#include "download.h"

// interface data for download and group threads

class ExtractPack
{
public:
    NewsServer   *ns;
    UserData     *the_server;
    UserData     *the_group;
    Decoder      *the_decoder;
    DownloadArgs *args;
    int           return_code;
    int		  stopGroup;		// Set by download thread
    int           groupStopped;		// Set by group thread when stopped clean
};

// logging filename fro newsserver tx/rx log
extern char *tx_rx_logname;

// save the "groups" file every <SAVE_AT> _decodes_
// not articles processed, actual decodes
#define SAVE_AT 10

// If too many articles, start before the latest one
#define SKIP_AHEAD 5000

#define ZERO_SERV serv_extracted=serv_reposts=serv_culled=serv_filtered=0;serv_extracted_mb=serv_reposts_mb=serv_culled_mb=serv_filtered_mb=0.0
#define ZERO_GROUP group_extracted=group_reposts=group_culled=group_filtered=0;group_extracted_mb=group_reposts_mb=group_culled_mb=group_filtered_mb=0.0

// internal funcs
static int loadFilter(char *filter_file_name,array<String> &filters, int *num_filters);
static void _Optlink processGroupThread(void *pVoid);

static int current_line;
static int current_article;

static long total_extracted;
static double total_extracted_mb;

static long serv_extracted;
static double serv_extracted_mb;
static long serv_reposts;
static double serv_reposts_mb;
static long serv_culled;
static double serv_culled_mb;
static long serv_filtered;
static double serv_filtered_mb;

static long group_extracted;
static double group_extracted_mb;
static long group_reposts;
static double group_reposts_mb;
static long group_culled;
static double group_culled_mb;
static long group_filtered;
static double group_filtered_mb;

static int counts_changed;
static time_t counts_checked_clk;

static DownloadArgs *callback_args;

//=== callback: called by getBody to show progress ====

static int callback(const float cps,const int percent, const int lines_sofar)
{
    char *psz;

    if (callback_args)
    {
	psz = new char[32];
	sprintf(psz,"%1.1f%s",cps, is_sleeping ? "*" : "");
	WinPostMsg(callback_args->parent, DL_SHOW_SPEED, MPFROMP(psz), MPFROMSHORT(0));

	psz = new char[32];
	sprintf(psz,"%d",percent);
	WinPostMsg(callback_args->parent, DL_SHOW_PERCENT, MPFROMP(psz), MPFROMSHORT(0));
    }

    current_line = lines_sofar;

    return running != 0;
}


enum { eOK, eSKIP_HOST, eSKIP_GROUP, eLOST_CONN } results;

#define MAX_TRIES 15

//== DownloadThread: Download control thread ===

// invoked via Start button ===

void _Optlink DownloadThread(void *pVoid)
{
    HAB hab;
    HMQ hmq;
    HWND hwnd;
    Container *tree;
    Configuration *config;
    UserData *server;
    UserData *group;
    NewsServer *ns;
    ExtractPack *ep;
    TID group_thread_id;
    int switched = 0;
    char *psz;

    ep = new ExtractPack;

    /* initialise so we can talk to the PM */
    hab = WinInitialize(0);
    if (hab)
	hmq = WinCreateMsgQueue(hab,0);
    else
	hmq = 0;

    /* start up the decoder */
    Decoder d;

    /* unpack our arguments */
    DownloadArgs *args = (DownloadArgs*)pVoid;
    hwnd   = args->parent;
    tree   = args->tree;
    config = args->config;
    callback_args = args;

    counts_changed = 0;
    counts_checked_clk = clock();

    total_extracted = 0;
    total_extracted_mb = 0.0;

    /* do the processing */
    server = 0;
    int rc;
    int not_responding = 0;
    int num_tries;

    while (tree->getNextServer(&server) && running)
    {
	WinPostMsg(hwnd,DL_SHOW_SERVER,MPFROMP(server->getName()),MPFROMSHORT(1));
	ZERO_SERV;
	WinPostMsg(hwnd,DL_SHOW_SEXTRACT,MPFROMP("0"),MPFROMSHORT(1));
	WinPostMsg(hwnd,DL_SHOW_SEXTRACT_MB,MPFROMP("0.0"),MPFROMSHORT(1));
	WinPostMsg(hwnd,DL_SHOW_SREPOST,MPFROMP("0"),MPFROMSHORT(1));
	WinPostMsg(hwnd,DL_SHOW_SREPOST_MB,MPFROMP("0.0"),MPFROMSHORT(1));
	WinPostMsg(hwnd,DL_SHOW_SCULL,MPFROMP("0"),MPFROMSHORT(1));
	WinPostMsg(hwnd,DL_SHOW_SCULL_MB,MPFROMP("0.0"),MPFROMSHORT(1));
	WinPostMsg(hwnd,DL_SHOW_SFILTER,MPFROMP("0"),MPFROMSHORT(1));
	WinPostMsg(hwnd,DL_SHOW_SFILTER_MB,MPFROMP("0.0"),MPFROMSHORT(1));

	psz = new char[1024];
	sprintf(psz,"%s (connecting)",server->getName());
	WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

	ns = new NewsServer(server->getName());

	// Open connection and log in to server

	for (num_tries = 0;num_tries < MAX_TRIES && running; num_tries++)
	{
	    int open_rc = ns->open();
	    long delay=1000;

	    if (open_rc == OPEN_FAIL)
	    {
		if (!running || num_tries == MAX_TRIES)
		{
		    psz = new char[1024];
		    sprintf(psz,"%s (connection failed)",server->getName());
		    WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

		    log("Error: DownloadThread - %s connection failed",server->getName());
		    delete ns;
		    ns = 0;
		}
		else
		{
		    psz = new char[1024];
		    sprintf(psz,"%s (no response, retrying)",server->getName());
		    WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

		    doSleep(delay);
		    delay *= 2;
		    log("Error: DownloadThread - %s not responding - retrying",server->getName());
		}
	    }
	    else if (open_rc == OPEN_WAIT)
	    {
		// server is answering connection, but not ready to accept queries
		psz = new char[1024];
		sprintf(psz,"%s (not ready, waiting...)",server->getName());
		WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

		doSleep(delay);
		delay *= 2;  // 1 second, 2 secs, 4 secs, ...
		log("%s alive, but not ready to accept queries, waiting...",server->getName());
	    }
	    else
	    {
		ns->setDebugFile(tx_rx_logname);
		if (*(server->getUsername()) !='\0' && *(server->getPassword()) != '\0')
		{
		    psz = new char[1024];
		    sprintf(psz,"%s (login)",server->getName());
		    WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

		    if (!ns->login(server->getUsername(),server->getPassword()))
		    {
			psz = new char[1024];
			sprintf(psz,"%s (login failed)",server->getName());
			WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

			log("Error: DownloadThread - login failed");
			message("Login Failed!");
			delete ns;
			ns = 0;
		    }
		    else
			log("Sending username & password - OK");

		}
		else
		    log("No username & password needed");

		break;
	    }
	} // while num_tries and running

	// If got access to server proces newsgroups on server

	if (ns && running)
	{
	    psz = new char[1024];
	    sprintf(psz,"%s (connected)",server->getName());
	    WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

	    log("Connected to %s",server->getName());
	    log(ns->getSignOnBanner());

	    group = 0;
	    tree->getNextGroup(server,&group);

	    while (group && running)
	    {
		if (!group->getSkipped())
		{
		    // Process newsgroup

		    WinPostMsg(hwnd, DL_SHOW_GROUP, MPFROMP(group->getName()), MPFROMSHORT(1));
		    ZERO_GROUP;
		    WinPostMsg(hwnd, DL_SHOW_GEXTRACT, MPFROMP("0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_GEXTRACT_MB, MPFROMP("0.0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_GREPOST, MPFROMP("0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_GREPOST_MB, MPFROMP("0.0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_GCULL, MPFROMP("0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_GCULL_MB, MPFROMP("0.0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_GFILTER, MPFROMP("0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_GFILTER_MB, MPFROMP("0.0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_ARTICLE, MPFROMP("Article"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_SUBJECT, MPFROMP(" "), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_LINES, MPFROMP(" "), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_SPEED, MPFROMP("0.0"), MPFROMSHORT(1));
		    WinPostMsg(hwnd, DL_SHOW_PERCENT, MPFROMP("0"), MPFROMSHORT(1));

		    psz = new char[20+strlen(group->getName())];
		    sprintf(psz,"News Harvest: %s",group->getName());
		    WinPostMsg(hwnd,DL_SET_TITLE, MPFROMP(psz), MPFROMSHORT(0));

		    ep->ns = ns;
		    ep->the_server = server;
		    ep->the_group = group;
		    ep->the_decoder = &d;
		    ep->args = args;
		    ep->stopGroup = 0;
		    ep->groupStopped = 0;

		    int last_line = 0;
		    int last_article = 0;
		    int no_response_since = 0;
		    unsigned long seconds_since_last_block = 0;
		    group_thread_id = 0;

		    // Loop til done with group

		    while (!ep->groupStopped && running)
		    {
			// if thread not running, start it
			if (group_thread_id == 0)
			{
			    group_thread_id = _beginthread(processGroupThread,
							      0,
							      32*4096,
							      (void*)ep);
			    if (group_thread_id == -1)
			    {
				log("Error: DownloadThread - cannot start group thread");
				DosBeep(200,200);
				group_thread_id = 0;
				break;
			    }
			    seconds_since_last_block = 0;
			    last_line = 0;
			    last_article = 0;
			}

			// have we recieved *any* data

			if (last_line == current_line &&
			    last_article == current_article)
			{
			    seconds_since_last_block += 2;

			    // is_sleeping is externed int from news server, which is 1 during
			    // bandwidth limit sleep, 0 otherwise
			    if (is_sleeping)
				seconds_since_last_block = 0;

			    if (seconds_since_last_block == 120)
			    {
				no_response_since = current_line;
				log("Error: DownloadThread - %s not responding",server->getName());
				not_responding = 1;
				psz = new char[1024];
				sprintf(psz,"%s (not responding)",server->getName());
				WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));
			    }
			    else if (seconds_since_last_block >= 180 && no_response_since == current_line)
			    {
				log("Error: DownloadThread - no response from %s - re-connecting ",server->getName());
				psz = new char[1024];
				sprintf(psz,"%s (re-connecting)",server->getName());
				WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

				DosBeep(200,200);
				if (0 == DosKillThread(group_thread_id))
				{
				    log("Error: DownloadThread - group thread not responding.  Terminated.");
				    group_thread_id = 0;
				}
				else
				    log("Error: DownloadThread - can not terminate group thread");

				seconds_since_last_block = 0;
				last_line = 0;
				last_article = 0;
			    }
			}
			else
			{
			    // Got input
			    seconds_since_last_block = 0;
			    no_response_since = 0;
			    last_line = current_line;
			    last_article = current_article;
			    if (not_responding)
			    {
				not_responding = 0;
				log("%s now responding",server->getName());
				psz = new char[1024];
				sprintf(psz,"%s (connected)",server->getName());
				WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));
			    }
			}

			for (int delay = 0; delay < 4; delay++)
			{
			    msecSleep(500);  // wait for 1/2 sec

			    // Check group switch requested from user interface
			    if (args->switch_to)
			    {
				psz = new char[1024];
				sprintf(psz,"%s (switching)",server->getName());
				WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

				// Set the group to process, tell loop not to move on to next group
				switched = 1;
				group = args->switch_to;
				args->switch_to = 0;
				log("Switching to group %s",group->getName());

				ep -> stopGroup = 1;	// Tell group thread to stop

				int last_line = current_line;
				int last_article = current_article;

				// Allow thread to finish gracefully
				// Give up if thread inactive to 10 seconds
				int counter = 0;
				while (counter++ < 50 && !ep->groupStopped)
				{
				    if (current_line > last_line ||
					current_article > last_article)
				    {
					last_line = current_line;
					last_article = current_article;
					counter = 0;		// Restart timeout
				    }
				    msecSleep(200);
				}

				if (!ep->groupStopped)
				{
				    // kill the bugger
				    DosBeep(200,200);
				    if (0 == DosKillThread(group_thread_id))
				    {
					log("Error: DownloadThread - killed group thread after 10 seconds without response");
					group_thread_id = 0;
				    }
				    else
					log("Error: DownloadThread - can not terminate group thread");
				}

				ep -> stopGroup = 0;
				seconds_since_last_block = 0;
				last_line = 0;
				last_article = 0;

			    } // if switching

			    if (args -> finish_up)
				ep -> stopGroup = 1;

			} // for delay

		    } // while !groupStopped

		    // If connect to host failed, skip other groups on this host
		    if (ep->return_code == eSKIP_HOST)
			break;

		    WinSendMsg(hwnd,DL_SAVESTATE,MPFROMSHORT(0),MPFROMSHORT(0));

		} // if group not skipped

		if (args -> finish_up)
		    break;

		if (!switched)
		    tree->getNextGroup(server,&group);
		else
		    switched = 0;

	    } // while got group and running

	    WinPostMsg(hwnd, DL_SET_TITLE, MPFROMP("News Harvest"), MPFROMSHORT(1));

	    psz = new char[1024];
	    sprintf(psz,"%s (disconnecting)",server->getName());
	    WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

	    ns->close();

	    if (!running || rc == eSKIP_HOST)
	    {
		psz = new char[1024];
		sprintf(psz,"%s (disconnected)",server->getName());
		WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));
	    }
	    else
	    {
		log("--------------------\nFinished\n--------------------",server->getName());
		time_t fin; time(&fin);
		log(ctime(&fin));
		psz = new char[1024];
		sprintf(psz,"%s (finished)",server->getName());
		WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));
	    }

	} // if ns and running

	delete ns;
	ns = 0;

	if (looping && running)
	{
	    if (looping_times != -1)
		looping_times--;

	    if (looping_times == -1 || looping_times > 0)
	    {
		UserData *sir = server;
		int x;
		if (!tree->getNextServer(&sir) && running)
		{

		    for (x = 0; x < looping; x++)
		    {
			psz = new char[1024];
			sprintf(psz,"Restarting in %d sec",looping - x);
			WinPostMsg(hwnd, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));
			if (!running)
			    break;
			doSleep(1000);
		    }

		    server = 0;  // cause it to start at the beginning again
		}
	    }
	} // if looping and running

    } // while server and running

    delete ep;

    if (!running)
	WinSendMsg(hwnd,DL_TERM,MPFROMSHORT(0),MPFROMSHORT(0));
    else
	WinSendMsg(hwnd,DL_FINISHED,MPFROMSHORT(0),MPFROMSHORT(0));

    /* clean up & exit */
    if (hmq)
	WinDestroyMsgQueue(hmq);
    if (hab)
	WinTerminate(hab);

    log(0);		// Close log

    _endthread();

} // DownloadThread


void logDecodeFailReason(int reason)
{
     switch (reason)
     {
	 case OK:
	    break;
	 case UNKNOWN_TYPE:
	     log("   Error: malformed post - indeterminable encoding type"); break;
	 case NO_FILENAME:
	     log("   Error: malformed post - no binary filename"); break;
	 case MALFORMED:
	     log("   Error: malformed post - Not a binary"); break;
	 case EMPTY:
	     log("   Error: malformed post - empty Article body");break;
	 default:
	     log("   Error: Malformed post - unknown reason"); break;
     }
}


//=== processGroupThread: get articles from selected Newsgroup thread ===

static void _Optlink processGroupThread(void *pVoid)
{
    HAB hab;
    HMQ hmq;

    /* initialise so we can talk to the PM */
    hab = WinInitialize(0);
    if (hab)
	hmq = WinCreateMsgQueue(hab,0);
    else
	hmq = 0;

    NewsGroup *ng = 0;
    char *body = 0;
    Header *head = 0;
    int isLongHead;
    long firstArticle;
    long num_articles = 0;
    long lastArticle;
    char *output_path;
    int should_cull;
    int should_fetch;
    long thisArticle;
    int article_lines;
    array<String> filters;
    int num_filters = 0;
    time_t the_time;
    int made_dir = 0;
    int open_rc;
    time_t config_last_update;
    int j;
    char *psz;

    ExtractPack *ep = (ExtractPack *)pVoid;

    NewsServer   *ns         = ep->ns;
    UserData     *the_server = ep->the_server;
    UserData     *the_group  = ep->the_group;
    Decoder      *the_decoder = ep->the_decoder;
    DownloadArgs *args       = ep->args;
    ep->return_code = eOK;

    writeLastGroup(the_server->getName(),the_group->getName());

    output_path = new char[4096];
    strcpy(output_path,the_group->getName());

    // Create News Server, if not created

    if (!ns)
    {
	psz = new char[1024];
	sprintf(psz,"%s (connecting)",the_server->getName());
	WinPostMsg(args->parent, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

	ns = new NewsServer(the_server->getName());
    }

    if (ns)
    {
	int num_loops = 0;
	open_rc = ns->open();
	long delay = 1000;

	while (running && open_rc == OPEN_WAIT)
	{
	    // server is answering connection, but not ready to accept queries
	    psz = new char[1024];
	    sprintf(psz,"%s (not ready, waiting...)",the_server->getName());
	    WinPostMsg(args->parent, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

	    doSleep(delay);
	    delay *= 2;
	    num_loops++;
	    if (num_loops == MAX_TRIES)
	    {
		log("Error: processGroupThread - giving up on %s.  It is never going to be ready",the_server->getName());
		open_rc = OPEN_FAIL;
		break;
	    }

	    log("%s is alive, but not ready to accept queries, waiting...",the_server->getName());
	    // retry
	    open_rc = ns->open();
	}
    }

    if (!ns || open_rc == OPEN_FAIL)
    {
	log("Error: processGroupThread - can not connect to %s",the_server->getName());
	psz = new char[1024];
	sprintf(psz,"%s (no response)",the_server->getName());
	WinPostMsg(args->parent, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

	delete[] output_path;
	delete ng;

	ep->return_code = eSKIP_HOST;
	ep->groupStopped = 1;

	 /* clean up & exit */
	if (hmq)
	    WinDestroyMsgQueue(hmq);
	if (hab)
	    WinTerminate(hab);

	_endthread();
	return;
    }
    else
    {
	psz = new char[1024];
	sprintf(psz,"%s (connected)",the_server->getName());
	WinPostMsg(args->parent, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));
    }

    // set thread priority
    if (DosSetPriority(PRTYS_THREAD,
		       (args->config->getPriorityClass()==PRIORITY_NORMAL?PRTYC_REGULAR:PRTYC_IDLETIME),
		       (long)args->config->getPriorityValue(),
		       0) != NO_ERROR)
    {
	log("Error: processGroupThread - can not adjust priority");
    }


    // If have set a base dir setup our output path = BASE + GROUPNAME
    if (*(args->config->getBaseDir()) != '\0')
    {
	delete[] output_path;

	output_path = new char[3+strlen(args->config->getBaseDir())+strlen(the_group->getName())];
	strcpy(output_path,args->config->getBaseDir());
	strcat(output_path,the_group->getName());
    }

    // Remember when the config was last updated so we can reload it
    config_last_update = args->config->getTimeStamp();

    // tell the decoder where to put files
    the_decoder->setOutputPath(output_path);

    // If we have a post-process command, tell the decoder about it
    if (args->config->getProcessUse() && *(args->config->getProcessCommand()) != '\0')
    {
	the_decoder->setOutputPostProcessor(args->config->getProcessCommand());
    }

    // Seek out our group
    ns->setDebugFile(tx_rx_logname);
    ng = ns->setGroup(the_group->getName());
    time_t change;
    time(&change);

    if (ng)
    {
	log("\n---------------------------------------------------------------\n"
	    "Processing group %s at %s"
	    "---------------------------------------------------------------",
	    the_group->getName(),ctime(&change));

	// work out where we are up to
	lastArticle = ng->lastArticle();

	// ::message_buffer("Start = %ld, first = %ld, last = %ld",the_group->getStart(),ng->firstArticle(),lastArticle);

	// if we have a negative start, that means start <start> from the
	// newest article
	if (the_group->getStart() < 0)
	    the_group->setStart(lastArticle+the_group->getStart());
	if (the_group->getStart() < 0)
	    the_group->setStart(0);

	// check our start value is realistic
	if (the_group->getStart() > lastArticle)
	    the_group->setStart(lastArticle);

	if (the_group->getStart() < ng->firstArticle())
	    the_group->setStart(ng->firstArticle()-1);

	// If there are way too many articles to be extracted, start SKIP_AHEAD
	// from the latest article
	if (ng->lastArticle() - the_group->getStart() > SKIP_AHEAD)
	{
	    // the_group->setStart(ng->lastArticle()-SKIP_AHEAD);
	    //log("%ld Articles is way too many to process",ng->lastArticle()-the_group->getStart());
	    // log("Starting at article %ld (%d from the latest article)",the_group->getStart(),SKIP_AHEAD);
	    log("Warning: %ld articles is a lot to process",ng->lastArticle()-the_group->getStart());
	}

	firstArticle = 1+the_group->getStart();

	if (firstArticle < ng->firstArticle())
	    firstArticle = ng->firstArticle();

	the_group->setStart(firstArticle);

	// are there any NEW articles ?
	if (firstArticle <= lastArticle)
	{
	    // if using filters load now
	    if (args->config->getFilterUse() && *(args->config->getFilter()) != '\0')
		loadFilter(args->config->getFilter(),filters,&num_filters);

//            mkdir(output_path);

	    num_articles = 1+lastArticle - firstArticle;

	    // loop through each article

	    for (thisArticle = firstArticle;thisArticle <= lastArticle;thisArticle++)
	    {
		// Throttle the bandwidth limit
		ns->setBandwidthLimit(args->config->getBandwidthLimit());

		// Avoid starving screen if filtering xhdrs
		if (counts_changed)
		{
		    time_t new_clock = clock();
		    if (new_clock - counts_checked_clk > 2 * CLOCKS_PER_SEC)
		    {
			counts_checked_clk = new_clock;
			counts_changed = 0;
			DosSleep(1);		// Allow screen update
		    }
		}

		// see if we need to reload any new config changes
		if (args->config->getTimeStamp() > config_last_update)
		{
		    config_last_update = args->config->getTimeStamp();

		    // reload filters
		    if (args->config->getFilterUse() && *(args->config->getFilter()) != '\0')
			loadFilter(args->config->getFilter(),filters,&num_filters);
		}

		if (!running)
		    break;		// Cancel requested

		ns->setXhdrCacheSize(args->config->getHeaderCacheSize());

		ns->setDebugFile(tx_rx_logname);

		// did the connection drop out ?
		if (ns->closed())
		{
		    log("Error: processGroupThread - connection lost - reconnecting");

		    psz = new char[1024];
		    sprintf(psz,"%s (connecting)",the_server->getName());
		    WinPostMsg(args->parent, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

		    unsigned long delay = 1000;

		    ns->setDebugFile(tx_rx_logname);

		    for (j=0; j < MAX_TRIES; j++)
		    {
			open_rc = ns->open();
			if (open_rc == OPEN_OK)
			    break;
			else if (open_rc == OPEN_WAIT)
			    log("Server is alive, but says it's not ready to handle queries - retrying");
			else
			    log("Error: processGroupThread - can not re-establish connection - retrying");

			doSleep(delay);
			if (!running)
			{
			    open_rc = OPEN_FAIL;
			    break;
			}
			delay *= 2;
		    }

		    if (j == MAX_TRIES)
			open_rc = OPEN_FAIL;

		    if (open_rc == OPEN_FAIL)
		    {
			log("Error: processGroupThread - cannot re-establish connection");

			psz = new char[1024];
			sprintf(psz,"%s (disconnected)",the_server->getName());
			WinPostMsg(args->parent, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

			delete ns;
			delete[] output_path;
			delete ng;

			ep->return_code = eLOST_CONN;
			ep->groupStopped = 1;

			/* clean up & exit */
			if (hmq)
			    WinDestroyMsgQueue(hmq);
			if (hab)
			    WinTerminate(hab);

			_endthread();
			return;
		    }
		    else
		    {
			// Seek out our group
			delete ng;
			log("Re-Connected");

			psz = new char[1024];
			sprintf(psz,"%s (re-connected)",the_server->getName());
			WinPostMsg(args->parent, DL_SHOW_SERVER, MPFROMP(psz), MPFROMSHORT(0));

			ns->setDebugFile(tx_rx_logname);

			ng = ns->setGroup(the_group->getName());
			if (ng)
			{
			    log("Re-processing group %s",the_group->getName());
			    thisArticle--;  // dont skip this article
			}
			else
			{
			    log("Error: processGroupThread - cannot set group to %s",the_group->getName());
			}
		    }
		} // if ns closed

		// Display the article # on the GUI
		current_article = thisArticle;

		psz = new char[64];
		sprintf(psz,"Article %ld",thisArticle);
		WinPostMsg(args->parent, DL_SHOW_ARTICLE, MPFROMP(psz), MPFROMSHORT(0));

		char *psz = new char[1024];
		sprintf(psz,"%s (%ld/%ld)",
			the_group->getName(), 1 + thisArticle - firstArticle, num_articles);
		WinPostMsg(args->parent, DL_SHOW_GROUP, MPFROMP(psz), MPFROMSHORT(0));

		log("Processing article %ld:",thisArticle);

		article_lines = 0;
		should_cull = 0;
		should_fetch = 0;

		// Write the subject & num lines to the GUI
		WinPostMsg(args->parent, DL_SHOW_PERCENT, MPFROMP("0"), MPFROMSHORT(1));
		WinPostMsg(args->parent, DL_SHOW_SPEED, MPFROMP("0.0"), MPFROMSHORT(1));

		if (!running)
		    break;		// Cancel requested

		psz = new char[64];
		sprintf(psz,"Article %ld (fetching header)",thisArticle);
		WinPostMsg(args->parent, DL_SHOW_ARTICLE, MPFROMP(psz), MPFROMSHORT(0));

		ns->setDebugFile(tx_rx_logname);

		psz = new char[64];
		sprintf(psz,"Article %ld",thisArticle);
		WinPostMsg(args->parent, DL_SHOW_ARTICLE, MPFROMP(psz), MPFROMSHORT(0));

		char *header_data = ns->getHead(thisArticle);
		isLongHead = !ns->usingXhdr();

		if (!header_data)
		{
		    should_cull = 1;  // expired/cancelled or connection lost
		}
		else
		{
		    delete head;
		    head = new Header(header_data);
		    // don't delete header_data, as the head object uses & deletes it
		    article_lines = head->numLines();

		    // Write the subject & num lines to the GUI
		    if (!(head->getSubject()))
		    {
			psz = "(no subject)";
			WinPostMsg(args->parent, DL_SHOW_SUBJECT, MPFROMP(psz), MPFROMSHORT(1));
		    }
		    else
		    {
			psz = head->getSubject();
			if (strnicmp(psz, "Re: ", 4) == 0)
			    psz += 4;
			else if (strnicmp(psz, "Re: ", 3) == 0)
			    psz += 3;
			int c = strlen(psz);
			const int cMax = 40;
			if (c > cMax)
			{
			    char *psz2 = new char[cMax + 1];
			    sprintf(psz2, "%12.12s...%25.25s", psz, psz + c - 25);
			    WinPostMsg(args->parent, DL_SHOW_SUBJECT, MPFROMP(psz2), MPFROMSHORT(0));
			}
			else
			    WinPostMsg(args->parent, DL_SHOW_SUBJECT, MPFROMP(psz), MPFROMSHORT(1));
		    }

		    psz = new char[32];
		    sprintf(psz,"%ld",article_lines);
		    WinPostMsg(args->parent, DL_SHOW_LINES, MPFROMP(psz), MPFROMSHORT(0));

		    log("   %s",(head->getSubject()?head->getSubject():"(no subject)"));
		    log("   %ld lines",article_lines);

		    if (article_lines < 0)
		    {
			log("   Line count odd - culled");
			should_cull = 1;
		    }

		    if (args->config->getMultipartMaxLines() &&
			article_lines > args->config->getMultipartMaxLines())
		    {
			log("   Culled by line count limit");
			should_cull = 1;
		    }

		    // does it have enough lines in it? or multipart?
		    if (!should_cull)
		    {
			// are we ignoring multi-part posts?
			if (head->isMultiPart())
			{
			    // if (args->config->getMaxPartAge() == 0)
			    if (args->config->getMultipartUse() == 0)
			    {
				should_cull = 1;
				log("   Is multipart - culled");
			    }
			    // way too many parts ?
			    else if (args->config->getMultipartMaxParts() > 0 &&
			             head->partTotal() > args->config->getMultipartMaxParts())
			    {
				should_cull = 1;
				log("   Culled, recovery of %d parts unlikely (> %d parts)",
				    head->partTotal(),args->config->getMultipartMaxParts());
			    }
			}
			// Not multipart - check per group line limit if defined
			else if (the_group->getCull())
			{
			    if (article_lines < the_group->getCullLines())
			    {
				should_cull = 1;
				log("   Culled by group minimum");
			    }
			}
			// If not group limit, check server line limit if defined
			else if (the_server->getCull() && article_lines < the_server->getCullLines())
			{
			    should_cull = 1;
			    log("   Culled by base minimum");
			}

			// Report culls

			if (should_cull)
			{
			    group_culled++;

			    psz = new char[32];
			    sprintf(psz,"%ld",group_culled);
			    WinPostMsg(args->parent, DL_SHOW_GCULL, MPFROMP(psz), MPFROMSHORT(0));

			    group_culled_mb += 40.0 * (double)article_lines;

			    psz = new char[32];
			    sprintf(psz,"%1.1f",group_culled_mb/1024.0/1024.0);
			    WinPostMsg(args->parent, DL_SHOW_GCULL_MB, MPFROMP(psz), MPFROMSHORT(0));

			    serv_culled++;

			    psz = new char[32];
			    sprintf(psz,"%ld",serv_culled);
			    WinPostMsg(args->parent, DL_SHOW_SCULL, MPFROMP(psz), MPFROMSHORT(0));

			    serv_culled_mb += 40.0 * (double)article_lines;

			    psz = new char[32];
			    sprintf(psz,"%1.1f",group_culled_mb/1024.0/1024.0);
			    WinPostMsg(args->parent, DL_SHOW_SCULL_MB, MPFROMP(psz), MPFROMSHORT(0));
			}
		    }

		} // if have header_data

		if (!running)
		    break;

		// If have xhdr and using filters check for fast filter
		if (!should_cull &&
		    ns->usingXhdr() &&
		    args->config->getFilterUse())
		{
		    j = head->filter(filters, num_filters);
		    if (j > 0)
		    {
			log("   Fast selected with \"%s\"",filters[j - 1].asCharStar());
			should_fetch = 1;
		    }
		    else if (j < 0)
		    {
			should_cull = 1;

			group_filtered++;

			psz = new char[32];
			sprintf(psz,"%ld",group_filtered);
			WinPostMsg(args->parent, DL_SHOW_GFILTER, MPFROMP(psz), MPFROMSHORT(0));

			group_filtered_mb += 62.0 * (double)article_lines;

			psz = new char[32];
			sprintf(psz,"%1.1f",group_filtered_mb/1024.0/1024.0);
			WinPostMsg(args->parent, DL_SHOW_GFILTER_MB, MPFROMP(psz), MPFROMSHORT(0));

			serv_filtered++;

			psz = new char[32];
			sprintf(psz,"%ld",serv_filtered);
			WinPostMsg(args->parent, DL_SHOW_SFILTER, MPFROMP(psz), MPFROMSHORT(0));

			serv_filtered_mb += 62.0 * (double)article_lines;

			psz = new char[32];
			sprintf(psz,"%1.1f",group_filtered_mb/1024.0/1024.0);
			WinPostMsg(args->parent, DL_SHOW_SFILTER_MB, MPFROMP(psz), MPFROMSHORT(0));

			j = -j - 1;

			if (j < num_filters)
			    log("   Fast filtered with \"%s\"",filters[j].asCharStar());
			else
			    log("   Fast filtered by include match fail");

			counts_changed = 1;	// Avoid starving screen

		    } // if culled

		    // If multipart and fast name filters enabled check if file name seen before
		    // Does not work well if subject line name does not match name in MIME part - fixme
		    if (!should_cull &&
			 ns->usingXhdr() &&
			 head->isMultiPart() &&
			 args->config->getFastNameFilterUse() &&
			 the_decoder -> checkNameSeenBefore(head -> partName())
		       )
		    {
			should_cull = 1;

			group_filtered++;

			psz = new char[32];
			sprintf(psz,"%ld",group_filtered);
			WinPostMsg(args->parent, DL_SHOW_GFILTER, MPFROMP(psz), MPFROMSHORT(0));

			group_filtered_mb += 62.0 * (double)article_lines;

			psz = new char[32];
			sprintf(psz,"%1.1f",group_filtered_mb/1024.0/1024.0);
			WinPostMsg(args->parent, DL_SHOW_GFILTER_MB, MPFROMP(psz), MPFROMSHORT(0));

			serv_filtered++;

			psz = new char[32];
			sprintf(psz,"%ld",serv_filtered);
			WinPostMsg(args->parent, DL_SHOW_SFILTER, MPFROMP(psz), MPFROMSHORT(0));

			serv_filtered_mb += 62.0 * (double)article_lines;

			psz = new char[32];
			sprintf(psz,"%1.1f",group_filtered_mb/1024.0/1024.0);
			WinPostMsg(args->parent, DL_SHOW_SFILTER_MB, MPFROMP(psz), MPFROMSHORT(0));

			log("   Fast filtered by filename match \"%s\"",head -> partName());

			counts_changed = 1;	// Avoid starving screen
		    }


		    // If not culled need full header to filter on sender
		    if (!should_cull && !should_fetch)
		    {
			char *full_header = ns->getLongHead(thisArticle);
			if (full_header)
			{
			    delete head;
			    head = new Header(full_header);
			    isLongHead = 1;
			    // don't delete full_header as the head objects does
			}
		    }
		} // if need filter

		// If undecided must filter again on full header
		if (!should_cull &&
		    !should_fetch &&
		    args->config->getFilterUse())
		{
		    // if (head->filter(filters[j].as_charstar() ))
		    j = head->filter(filters, num_filters);
		    if (j > 0)
		    {
			log("   Selected by filter \"%s\"",filters[j - 1].asCharStar());
		    }
		    else if (j < 0)
		    {
			should_cull = 1;

			group_filtered++;

			psz = new char[32];
			sprintf(psz,"%ld",group_filtered);
			WinPostMsg(args->parent, DL_SHOW_GFILTER, MPFROMP(psz), MPFROMSHORT(0));

			group_filtered_mb += 62.0 * (double)article_lines;

			psz = new char[32];
			sprintf(psz,"%1.1f",group_filtered_mb/1024.0/1024.0);
			WinPostMsg(args->parent, DL_SHOW_GFILTER_MB, MPFROMP(psz), MPFROMSHORT(0));

			serv_filtered++;

			psz = new char[32];
			sprintf(psz,"%ld",serv_filtered);
			WinPostMsg(args->parent, DL_SHOW_SFILTER, MPFROMP(psz), MPFROMSHORT(0));

			serv_filtered_mb += 62.0 * (double)article_lines;

			psz = new char[32];
			sprintf(psz,"%1.1f",group_filtered_mb/1024.0/1024.0);
			WinPostMsg(args->parent, DL_SHOW_SFILTER_MB, MPFROMP(psz), MPFROMSHORT(0));

			j = -j - 1;

			if (j < num_filters)
			    log("   Filtered by \"%s\"",filters[j].asCharStar());
			else
			    log("   Filtered by include match fail");

			counts_changed = 1;

		    } // if culled
		} // if neet 2nd filter check

		if (!should_cull)
		{
		    if (!*head->getSender())
			log("   (sender unknown)");
		    else
			log("   %s",head->getSender());
		}

		if (!running)
		    break;

		// do the processing
		if (!should_cull)
		{
		    psz = new char[64];
		    sprintf(psz,"Article %ld (fetching body)",thisArticle);
		    WinPostMsg(args->parent, DL_SHOW_ARTICLE, MPFROMP(psz), MPFROMSHORT(0));

		    ns->setDebugFile(tx_rx_logname);

		    delete[] body;
		    // getBody will log errors
		    body = ns->getBody(thisArticle,article_lines,callback,args->config);

		    if (!running)
			break;

		    if (body)
		    {
			if (head->isMultiPart())
			{
			    unsigned long dummy;
			    log("   Processing as part-multipart");

			    psz = new char[64];
			    sprintf(psz,"Article %ld (multipart processing)",thisArticle);
			    WinPostMsg(args->parent, DL_SHOW_ARTICLE, MPFROMP(psz), MPFROMSHORT(0));

			    if (!made_dir)
			    {
				made_dir = 1;
				mkdir(output_path);
			    }

			    // check we haven't filled the parts disk
			    char drive;
			    if (*(args->config->getPartsDir()) && *(args->config->getPartsDir()+1) == ':')
				drive = *(args->config->getPartsDir());
			    else
				drive = '0';

			    if (getFreeSpace(drive) < 3)
			    {
				// partClean(args->config->getPartsDir());

				switch(args->config->getMultipartFullStop())
				{
				    case 0:
					args->config->setMultipartUse(0);
					log("Parts drive is near full - no longer processing parts");
					break;
				    case 2:
					log("Parts drive is near full - removing 25%% of oldest parts");
					if (partCull25PercentOldest(args->config->getPartsDir()) == 0)
					{
					    log("Error: processGroupThread - failed to remove any parts - stopping");
					    running = 0;
					}
					break;
				    default:
					log("Error: processGroupThread - parts drive is near full - stopping");
					running = 0;
				}
			    }

			    body = handlePartPosts(head,
						   body,
						   args->config->getPartsDir(),
						   &dummy,
						   &dummy,
						   args->config->getMaxPartAge());
			}
			else
			{
			    // Not multipart
			    char *head_n_body = 0;

			    // fixme to avoid strcpy
			    if (ns->usingXhdr() && !isLongHead)
			    {
				// often the header contains encoding info
				// must get all of it
				char *full_header = ns->getLongHead(thisArticle);
				if (full_header)
				{
				    head_n_body = new char[strlen(full_header) + strlen(body) + 3];
				    strcpy(head_n_body,full_header);
				    delete[] full_header;
				    full_header = 0;
				}
				else
				{
				  log("   Error: processGroupThread - getLongHead failed");
				}
			    }
			    else
			    {
				head_n_body = new char[strlen(head->getContent()) + strlen(body) + 3];
				strcpy(head_n_body,head->getContent());
			    }

			    if (head_n_body)
			    {
				strcat(head_n_body,"\r\n");
				strcat(head_n_body,body);
				delete[] body;
				body = head_n_body;
				log("   Processing");
			    }
			    else
			    {
				delete[] body;
				body = 0;
			    }
			} // if not multipart

			WinPostMsg(callback_args->parent, DL_SHOW_PERCENT, MPFROMP("100"), MPFROMSHORT(1));

			//body may be freed by multipart logic
			if (body)
			{
			    psz = new char[64];
			    sprintf(psz,"Article %ld (decoding)",thisArticle);
			    WinPostMsg(args->parent, DL_SHOW_ARTICLE, MPFROMP(psz), MPFROMSHORT(0));

			    if (!made_dir)
			    {
				made_dir = 1;
				mkdir(output_path);
			    }

	// Plug filename in here & run to test
	//                    FILE *ooo = fopen("c:\\tmp\\x\\z.news","r");
	//                    if (ooo)
	//                    {
	//                        unsigned long oolen ;
	//                        fseek (ooo, 0L, SEEK_END);
	//                        oolen = ftell (ooo);
	//                        rewind(ooo);
	//
	//                        body = new char[oolen+10000];
	//                        fread(body,1,oolen,ooo);
	//                        log("read %lu bytes from ooo",oolen);
	//                        fclose(ooo);
	//                    }

			    int decode_ret = the_decoder->decode(body);
			    logDecodeFailReason(decode_ret);

			    // check we haven't filled the disk
			    char drive;
			    if (output_path[1] == ':')
				drive = output_path[0];
			    else
				drive = '0';
			    if (getFreeSpace(drive) < 3)
			    {
				log("Output partition has only %lu MBytes left - stopping",getFreeSpace(drive));
				running = 0;
			    }
	//                    else
	//                    {
	//                        log("Output partition has %lu MBytes free\n",getFreeSpace(drive));
	//                    }

			    psz = new char[64];
			    sprintf(psz,"Article %ld",thisArticle);
			    WinPostMsg(args->parent, DL_SHOW_ARTICLE, MPFROMP(psz), MPFROMSHORT(0));

			    // show some output
			    for (int kkk = 0; kkk < the_decoder->getItemsProcessed(); kkk++)
			    {
				if (!running)
				    break;

				FileStat *fst = the_decoder->getFileStat(kkk);
				if (fst->getError() == OK)
				{
				    if (fst->getAction() == DECODED)
				    {
					args->tree->insert(fst->getName(),the_group,8888L,8888L);  // 8-8 magic num to say is a file
					log("   Decoded \"%s\"",fst->getName());

					// do stats
					total_extracted++;  total_extracted_mb += fst->getSize();
					serv_extracted++;  serv_extracted_mb += fst->getSize();
					group_extracted++; group_extracted_mb += fst->getSize();

					// save where we are up to every SAVE_AT decodes
					if (group_extracted && group_extracted % SAVE_AT == 0)
					    WinPostMsg(args->parent,DL_SAVESTATE,MPFROMSHORT(0),MPFROMSHORT(0));

					psz = new char[32];
					sprintf(psz,"%ld",serv_extracted);
					WinPostMsg(args->parent, DL_SHOW_SEXTRACT, MPFROMP(psz), MPFROMSHORT(0));

					psz = new char[32];
					sprintf(psz,"%1.1f",serv_extracted_mb/1024.0/1024.0);
					WinPostMsg(args->parent, DL_SHOW_SEXTRACT_MB, MPFROMP(psz), MPFROMSHORT(0));

					psz = new char[32];
					sprintf(psz,"%ld",group_extracted);
					WinPostMsg(args->parent, DL_SHOW_GEXTRACT, MPFROMP(psz), MPFROMSHORT(0));

					psz = new char[32];
					sprintf(psz,"%1.1f",group_extracted_mb/1024.0/1024.0);
					WinPostMsg(args->parent, DL_SHOW_GEXTRACT_MB, MPFROMP(psz), MPFROMSHORT(0));

					// if we have USER EXIT command do it
					if (args->config->getProcessUse() && *(args->config->getProcessCommand()) != '\0')
					{
					    if (args->config->getLogSpawn())
						log("   Post processing with \"%s\"",args->config->getProcessCommand());

					    char *program = new char[4096];
					    if (strchr(fst->getFullPath(),' '))
						sprintf(program,"detach %s %s \"%s\"",args->config->getProcessCommand(),(fst->hasExtension()?fst->getExtension():"?"),fst->getFullPath());
					    else
						sprintf(program,"detach %s %s %s",args->config->getProcessCommand(),(fst->hasExtension()?fst->getExtension():"?"),fst->getFullPath());

					    // run it!
					    if (args->config->getLogSpawn())
						log(program+7);
					    system(program);

					    delete[] program;
					}

					// Have we reached the number of items download limit
					if (args->config->getMaxArticles() != 0 && args->config->getMaxArticles() <= total_extracted)
					{
					    // message("Max %lu > extarcted %ld",args->config->getMaxArticles(),total_extracted);
					    log("Reached maximum number of extracted articles (%lu)",total_extracted);
					    running = 0;
					    break;
					}

					// Have we reached the MB download limit
	//                                log("Max MB %lu (stop if <) extracted MB %lu %3.2f %3.2f",args->config->getMaxDownload(),1L*(ULONG)((double)total_extracted_mb/(double)1024.0/(double)1024.0),total_extracted_mb,total_extracted_mb/(double)1024.0/(double)1024.0);
					if (args->config->getMaxDownload() != 0 && args->config->getMaxDownload() <= 1L*(ULONG)(total_extracted_mb/1024.0/1024.0))
					{
					    // message("Max MB %lu > extracted MB %lu",args->config->getMaxDownload(),1L*(ULONG)(total_extracted_mb/1024.0/1024.0));
					    log("Reached maximum amount of extracted articles %lu Mb",args->config->getMaxDownload());
					    running = 0;
					    break;
					}

				    }
				    else if (fst->getAction() == SKIPPED)
				    {
					log("   A repost");
					// do stats
					serv_reposts++;  serv_reposts_mb += fst->getSize();
					group_reposts++; group_reposts_mb += fst->getSize();

					psz = new char[32];
					sprintf(psz,"%ld",serv_reposts);
					WinPostMsg(args->parent, DL_SHOW_SREPOST, MPFROMP(psz), MPFROMSHORT(0));

					psz = new char[32];
					sprintf(psz,"%1.1f",serv_reposts_mb/1024.0/1024.0);
					WinPostMsg(args->parent, DL_SHOW_SREPOST_MB, MPFROMP(psz), MPFROMSHORT(0));

					psz = new char[32];
					sprintf(psz,"%ld",group_reposts);
					WinPostMsg(args->parent, DL_SHOW_GREPOST, MPFROMP(psz), MPFROMSHORT(0));

					psz = new char[32];
					sprintf(psz,"%1.1f",group_reposts_mb/1024.0/1024.0);
					WinPostMsg(args->parent, DL_SHOW_GREPOST_MB, MPFROMP(psz), MPFROMSHORT(0));
				    }
				}
			    } // for decoded
			} // if body
		    } // if body
		} // if not culled

		delete[] body;
		body = 0;
		delete head;
		head = 0;

		if (!running)
		    break;

		the_group->setStart(thisArticle);

		if (ep -> stopGroup)
		    break;			// Download thread requesting clean stop

	    } // for thisArticle

	    // if update netscape .rc file - do it!
	    if (*(args->config->getNetscapeDir()) != '\0')
	    {
		updateNetscapeNews(args->config->getNetscapeDir(),
				   the_server->getName(),
				   the_group->getName(),
				   thisArticle==0?0:thisArticle-1);
	    }

	} // if some articles

    } // if ng
    else
    {
	log("Error: processGroupThread - can not set group %s on %s",the_group->getName(),the_server->getName());
	log("   Login failed?, Group does not exist?, Proxy Failed?");
    }

    delete[] body;
    delete head;

    delete ng;

    // remove the directory if it is empty
    FileList *fls;
#ifndef SLAINC
    char *command_buf = new char[4096];
    sprintf(command_buf,"rmdir %s",output_path);
    strcat(output_path,"\\*");
    fls = new FileList(output_path);
    if (fls->getNumFiles() == 0)
    {
	// log("%s has 0 files, removng empty dir",output_path);
	system(command_buf);
    }
    delete[] command_buf;
#else
    strcat(output_path,"\\*");
    fls = new FileList(output_path);
    if (fls->getNumFiles() == 0)
	rmdir(output_path);
#endif

    delete fls;

    delete[] output_path;

    /* clean up & exit */
    if (hmq)
	WinDestroyMsgQueue(hmq);
    if (hab)
	WinTerminate(hab);

    ep->groupStopped = 1;

    _endthread();

} // processGroupThread


//=== loadFilter: load filters from file to memory array, return 1 if OK ===

int loadFilter(char *filter_file_name,array<String> &filters, int *num_filters)
{
    int rc = 0;
    int i = 0;

    FILE *f = fopen(filter_file_name,"r");
    if (f)
    {
	char *buffer = new char[4096];
	while(!feof(f))
	{
	    fgets(buffer,4096,f);
	    if (!feof(f))
	    {
		// Chop cr/nf
		char *p = strchr(buffer,'\n'); if (p) *p='\0';
		      p = strchr(buffer,'\r'); if (p) *p='\0';
		if (strlen(buffer) > 2)
		{
		    // string object should automatically free any existsing string on reload
		    filters[i] = buffer;
		    filters[i].lower();
		    i++;
		    rc = 1;
		}
	    }
	}
	fclose(f);
	delete[] buffer;
    }

    *num_filters = i;

    return rc;
}


void writeLastGroup(char *server,char *group)
{
    FILE *out = fopen("lastgroup","w");
    if (out)
    {
	fprintf(out,"%s:%s\n",server,group);
	fclose(out);
    }
}
