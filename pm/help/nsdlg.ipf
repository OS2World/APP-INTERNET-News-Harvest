.* nsdlg.ipf,v 1.3 2007/04/26 03:05:29 root Exp
.* 13 Oct 05 SHL Document mulitpart limit disable

:userdoc.
.nameit symbol=NHV text='News Harvest v1.5'
.nameit symbol=NH text='News Harvest'
:title.&NHV.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=1000 group=1.Copyright
:i1 id=toc.Table of Contents
:artwork fit align=center name='nh_cube.bmp'.
:lines align=center.
:font facename=Helv size=28x18.
&NHV.
:font facename=Helv size=0x0.
Copyright (c) 1997-1999 KRT Pty. Ltd.
Copyright (c) 2000, 2001 Steven Levine &amp. Associates, Inc.
:elines.
:sl.
:li.:link reftype=hd res=1500.Introduction:elink.
:li.:link reftype=hd res=2000.The Main Window:elink.
:li.:link reftype=hd res=3000.Adding a Host:elink.
:li.:link reftype=hd res=4000.Groups to Harvest:elink.
:li.:link reftype=hd res=4500.The Group List:elink.
:li.:link reftype=hd res=5000.Configuration:elink.
:li.:link reftype=hd res=6000.Mime Types:elink.
:li.:link reftype=hd res=7000.Setup Walkthrough:elink.
:li.:link reftype=hd res=8000.Questions and Answers:elink.
:li.:link reftype=hd res=9000.Software Licence:elink.
:esl.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=1500 group=1.News Harvest
:i1 id=intro.News Harvest
:artwork fit align=center name='nh_cube.bmp'.
:lines align=center.
:font facename=Helv size=28x18.
News Harvest
:font facename=Helv size=0x0.
Copyright (C) 1997-1999 KRT Pty. Ltd.
Copyright (c) 2000, 2001 Steven Levine &amp. Associates, Inc.
:elines.
:p.
News Harvest is an OS/2 Presentation Manager application for the automatic
retrieval of Usenet News binary articles.
:p.
The main design impetus was to create a :hp5.Fire and Forget:ehp5. utility that 
would process any number of groups over any number of servers with no 
user-intervention.  This is indeed the case - once News Harvest is configured 
with host and group information, a single command will run the 
process to completion.
:p.
A binary news harvester with such simplicity of operation is often low on
features.  This is not the case with News Harvest&colon.
:lines.
:artwork runin name='nh20.bmp'.Multi-threaded, fully 32bit executable.
:artwork runin name='nh20.bmp'.Completly automatic operation.
:artwork runin name='nh20.bmp'.Rebuilds multipart articles.
:artwork runin name='nh20.bmp'.Remembers past downloads.
:artwork runin name='nh20.bmp'.Utilises SPAM-keyword filtering.
:artwork runin name='nh20.bmp'.Fully configurable output paths.
:artwork runin name='nh20.bmp'.Handles abrupt disconnections (re-connects).
:artwork runin name='nh20.bmp'.Updates Netscape Navigator news file (optional).
:artwork runin name='nh20.bmp'.Very low, average CPU load.
:artwork runin name='nh20.bmp'.Comprehensive logging information (optional).
:artwork runin name='nh20.bmp'.WPS Drag and Drop.
:artwork runin name='nh20.bmp'.Output displayed in ordered and intuitive manner.
:artwork runin name='nh20.bmp'.On-the-fly changing of configuration.
:artwork runin name='nh20.bmp'.Complete online manual.
:elines.
:p.
For detailed information about the operation of News Harvest, please refer to
on of the hyperlinked sections below.
:lines.:hp2.Contents&colon.:ehp2.
:elines.
:sl.
:li.:link reftype=hd res=7000.Setup Walkthrough (quick-start):elink.
:li.:link reftype=hd res=2000.The Main Window:elink.
:li.:link reftype=hd res=3000.Adding a Host:elink.
:li.:link reftype=hd res=4000.Groups to Harvest:elink.
:li.:link reftype=hd res=4500.The Group List:elink.
:li.:link reftype=hd res=5000.Configuration:elink.
:li.:link reftype=hd res=6000.Mime Types:elink.
:li.:link reftype=hd res=8000.Questions and Answers:elink.
:li.:link reftype=hd res=9000.Software Licence:elink.
:esl.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=2000 group=1.Using the Main Panel
:i1 id=main.The Main Panel
:lines.
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.The Main Window
:elines.
:font facename=Helv size=0x0.
:artwork fit align=center name='MainPanel.bmp'.
:p.
Most of the usual running of News Harvest is done via this main Window.
Control is essentially the Go/Stop button towards the bottom-left of the
panel.  Setting up of the harvesting path is done via a popup menu in
the right-hand section of the window.
:p.
:hp2.Starting and Stopping:ehp2.
:p.
News Harvest uses a dual-purpose button to both Start and Stop.  If harvesting
is in process, the button becomes a "Stop".  If it is not running, the button is 
marked "Go".  Occasionally the button becomes unavailable ("Greyed out").  This
is because News Harvest is busy operating on another thread, usually starting
up or closing the conversation with the News Server.
:p.
Note&colon. The command line options "-auto" and "-loop" have now been moved
to the Configuration Panel.
:p.
The Finish button also stops harvesting.
However, if a article body download in in progress, the stop is delayed until the
download finishes.
:p.
:hp2.The Output/Configuration Window.:ehp2. 
:p.
News Harvest has a set of Servers, servers have Groups, groups have Articles.
This hierarchary is displayed on the right of the main panel.  Most actions
on this setup is through "context" or pop-up menus.  The default way to activate
these is with a left-mouse-button-click.
:p.
:hp2.News Host Context Menu:ehp2.
:artwork fit align=center name='Context_Host.bmp'.
:p.
:hp5.Add Group:ehp5.&colon. Add a Group to the selected host via the :link reftype=hd res=4000.Group:elink. window.
.br
:hp5.Delete:ehp5.&colon. Removes this Host and associated Groups, but not harvested articles.
.br
:hp5.Add Host:ehp5.&colon. Add a News Server via the :link reftype=hd res=3000.Host:elink. window.
.br
:hp5.Mime Types:ehp5.&colon. Opens the :link reftype=hd res=6000.MIME Configuration:elink. window.
:p.
:hp2.Group Context Menu:ehp2.
:artwork fit align=center name='Context_Group.bmp'.
:p.
:hp5.Refresh:ehp5.&colon. This option reloads the list of articles from the group output directory.
.br
:hp5.Skip Processing:ehp5.&colon. Mark the group so it will not be harvested.
You can mark a group while harvesting is in progress.
News Harvest will proceed to the next group when it finishes with the current header or
body in the group.
.br
:hp5.GoTo:ehp5.&colon. Stops processing the current newsgroup and causes it to begin
with the selected group.
.br
:hp5.Open:ehp5.&colon. Opens the WPS window for the output directory of this group.
.br
:hp5.Delete:ehp5.&colon. Removes the group entry, but not harvested articles.
.br
:hp5.Add Host:ehp5.&colon. Add a News Server via the :link reftype=hd res=3000.Host:elink. window.
.br
:hp5.Mime Types:ehp5.&colon. Opens the :link reftype=hd res=6000.MIME Configuration:elink. window.
:p.
:hp2.Article Context Menu:ehp2.
:artwork fit align=center name='Context_Article.bmp'.
:p.
:hp5.Delete:ehp5.&colon. Removes the harvested article file from the output disk.
.br
:hp5.Add Host:ehp5.&colon. Add a News Server via the :link reftype=hd res=3000.Host:elink. window.
.br
:hp5.Mime Types:ehp5.&colon. Opens the :link reftype=hd res=6000.MIME Configuration:elink. window.
:p.
News Harvest will process starting at the top of the list and work downwards.
:p.
To change to position of a Group/Host highlight the entry to move, and use the
Shift-Arrow keys to move it up or down.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=3000 group=1.Adding a Host
:i1 id=host.Adding a Host
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.Adding a Host
:font facename=Helv size=0x0.
.br
:artwork fit align=center name='AddNewsServer.bmp'.
:p.
Enter the name or IP address of your local News Server.
If you are unsure as to what this is, a good guess would be
to type 'news', and then hit the lookup button.  If that does
not work, perhaps try 'news' and append to that the domain
of your Internet Service Provider (e.g.&colon. news.krt.com.au).
:p.
The lookup button will fetch the cannonical name for your News Server.
This can be the source of a potention problem, as some ISPs rotate
(or at least change occasionally) the news host, and redirect the alias.
As an example of this, the Host may be called 'RED.krt.com.au', and
the alias 'NEWS.krt.com.au' points to RED.  But then the ISP moves
the server onto a newer machine called 'GREEN.krt.com.au' and redirects
the alias.  If the hostname News Harvest uses is just plain 'NEWS.krt.com.au', 
there will be no problem.  But if the lookup feature was used to determine 
the cannonical name of the host, it would be 'RED', and would have to be
changed to GREEN.  So, if in doubt, use the alias.
:p.
Some usenet news servers require user authentication before use.  If this
is necessary for your host, fill in your username and password.  Password
characters will be displayed, and are stored in your 'groups' file.
When the news host is initially contacted, the password will be sent 
automatically.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=4000 group=1.Adding Groups
:i1 id=group.Adding Groups
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.Adding Groups
:font facename=Helv size=0x0.
:artwork fit align=center name='AddGroup.bmp'.
:p.
:hp2.Group Name:ehp2.
:p.
Simply enter the name of the news-group from which you wish to harvest binaries.
Group names are case insensitive, and are usually of the form
something.something.something.something.  A few examples are something
like alt.binaries.clip-art, comp.binaries.os2, or alt.binaries.sounds.midi.
:p.
Not all groups are available on all News Servers.  Contact you system 
administrator (or ISP) if you think you are missing some groups, or if the 
news service has been censored (quite common in university environments).
:p.
If you wish to browse an available list of groups, and/or add multiple 
groups simultaneously, refer to :link reftype=hd res=4500.Using The Group List:elink..
:p.
:hp2.Cull Size:ehp2.
:p.
This value sets the default limit for the size of articles to skip. 
News Harvest is able to quickly guess the size of an attachment by looking
at the line count in the article header.  A calculation based on this allows
the speedy removal of articles that are too small to produce a decent binary.
:p.
To use this feature effectively, think about the usual size of what you 
intend to harvest.  For example, if you limit 100k on :hp1.alt.binaries.clip-art:ehp1., you would rarely 
get anything.  To skip banner ads, grotty little images,  set this
value to around 20.
:p.
A very rough guide&colon.
:sl.
:li. 0k -   5k Clip Art, Text files
:li. 5k -  15k MIDI Files
:li.15k -  30k Banner Ads
:li.30k - 200k JPEG Pictures
:li.200k ++    Gif Pictures, MPEG Animations
:esl.
:p.
So to extract quality JPEG images from a group, use a value of around 30-40.
For MIDI music files (zipped) don't use a value much over 5.  For animations
a value over 100 should work ok.  There's always going to be 80k banner ads,
and 15k JPEG images, using a cull size will never be better than a rule of thumb.
:p.
Note that tails of multipart articles are never culled.
:p.
:hp2.Next Article Number:ehp2.
:p.
All Usenet News articles are indexed by an integer number.  This is
local to both the News Server, and the Group.  Each new article posted
to the group gets an (ever increasing) index.
To keep track of where the last extraction was performed, News Harvest
records this index with the group name.
:p.
News Harvest adds a few twists to this simple concept.  If you wish to
start at the first available article in the group, use '0' for the Next 
Article.  The effect of this is that extraction starts at the beginning
or within 5000 articles, whichever is the lesser.  The 5000 limit was
introduced due to a problem found by our (most excellent) Beta Testers.
For reasons unknown, it is common for news servers to keep one particular
article from a long time ago.  This becomes the first article. After this 
there comes a huge block of expired articles (in one case over 12000), which 
News Harvest must process before it gets to the next non-expired posting.
It would be unusual for a server to keep more than 5000 non-expired articles,
so this became the defacto limit.
:p.
At the other end of the Servers' articles is there feature to use a negative 
index.  This instructs News Harvest to begin extraction however many from the 
most recently posted article.  For example, if -100 was entered, News Harvest
would process the last 100 articles.  This is a good tool for processing a
news group for the first time.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=4500 group=1.Using The Group List
:i1 id=list.Using The Group List
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.Using The Group List
:font facename=Helv size=0x0.
.br
:artwork fit align=center name='GroupList.bmp'.
:p.
:hp2.Selecting Groups:ehp2.
:p.
Usenet News offers the user a huge array of different subject areas.
With the well-designed tools incorperated into News Harvest's Group Selection
Panel, it's possible to quickly find the subjects of interest.
:p.
:hp2.Removing Groups From Selection:ehp2.
:p.
To remove a group from the selection list, simply highlight the
undesired group name in the selection list amd click the Remove 
button.
:p.
:hp2.Filtering:ehp2.
:p.
There are thousands of Usenet News Groups, but not all are available 
on every server.  However most servers still have in the order of thousands
of groups.  Finding the right group can sometimes be difficult.  
:p.
This is where being able to find specific groups on keyword matching becomes
handy.  The way it works is by listing news-groups that contain only the
entered string in their name.
:p.
For example, to limit the display to only groups dedicated to binaries,
enter the word 'binaries' into the filter field, and click the Filter button.
(NB&colon. There are quite a few binaries groups that do not have the
word 'binaries' in their name too).
:p.
The list of groups will be constrained to only those with the keyword.
This list is still quite extensive.  To shorten it further, enter a
different word, e.g. 'sounds'.  Now the list contains only news-groups
that have both 'binaries' AND 'sounds' in their name.
:p.
To clear all filtering, delete any text from the filter field and click
the filter button.  That is, have the filter keyword field blank.  All groups
will be returned to the list.
:p.
:hp2.Refreshing The Group List:ehp2.
:p.
Initially the group list will always be empty.  News Harvest must download one
from the target News Server.  This can be quite a lengthy process, as some
news servers carry in excess of 35000 groups. 
:p.
To begin the list refresh (even for the first time) click the Refresh button.
News Harvest will start the downloading process.  Note that this process
is totally independant, and will continue even if News Harvest is exited.
(Thus it's possible to download a news-group list without starting News Harvest).
This operation may take a considerable length of time, it's possible to close
the Group List window, and then re-open it later when the refresh is complete.
.br
:artwork fit align=center name='refresh.bmp'.
.br
Once the group-list download tool has fetched the complete list, they will
be sorted and written to a file of the same name as your News Server.
For example, the group-list for news.krt.com.au is stored in a file called
"news.krt.com.au".  The new list is then displayed on the selection panel.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=5000 group=1.Configuration
:i1 id=config.Configuration
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.Configuration
:font facename=Helv size=0x0.
:p.
:hp2.Priority:ehp2.
.br
:artwork fit align=center name='config_priority.bmp'.
:p.
News Harvest's priority configutation allows users with very fast connections
to reduce the processor load on the system.  The priority is only changed
for the downloading thread.  The User Interface priority is left at the OS/2
default.
:p.
Two settings are allowed, "Normal" and "Idle Time".  Within each setting, there
are 64 scales (-32 to +32), so priority may be assigned with respect to processes
at the same priority level also.  
:p.
It is beyond the scope of this document to explain how OS/2 priority settings
work, but this is covered quite well in your system documentation.  Care should
be taken using excessive changes in priority, as very high settings may starve
other (perhaps critical) processes of timeslices.  Note also, that if you
run in idle-time, a long-running process (e.g. compilations) will starve
News Harvest of CPU time, and your news server could hang up.
:p.
To limit the average bandwidth News Harvest consumes, enter a non-zero value
in the "Limit bandwidth to" entry field.  Please note though, that it is not
possible to reduce peak bandwidth consumption.  
:p.
Article headers and content will still download at the same speed.  This is
because news servers transmit at maximum available bandwidth, and this volume
is not configurable from the client-side.  But what it is possible, is 
to wait for a short period before asking for the next block of data.
This produces an automatically correcting average bandwidth, where peak rates
are often above and below the mean, but average bandwidth used is very close to the
given rate.
:p.
:hp2.Filtering:ehp2.
.br
:artwork fit align=center name='config_filtering.bmp'.
.br
Unfortunately Usenet News has become a medium of choice for promulgating 
unsolicited advertising.  Using a file of filter strings, it is 
possible to limit the processing of these articles.
:p.
The filters file is a plain text file of keywords, one to a line.  The
keywords can be anything you like.  If that keyword appears in the header
of an article, the article is not (further) processed by News Harvest.
For example&colon.
:sl compact.
:li.www.fratboys.com
:li.www.news-archive.com
:li..wmf
:li.warez
:li..exe
:li.(etc...)
:esl.
:p.
There are several types of filters&colon.
:ul compact.
:li.substring exclude
:li.wildcard exclude
:li.substring include
:li.wildcard include
:li.quoted substring exclude
:eul.
:p.
The first character of the filter string indicates the filter type.
All filter matches are case-insensitive.
:p.
A filter string does not start with a one of the lead-in characters, it is a 
substring exclude filter. The filter excludes an article from further 
processing if the filter string matches any substring in the article's 
sender or subject header field.
:p.
A wildcard exclude filter starts with the '^' character. The filter excludes 
an article from further processing if the regular expression filter string 
matches any substring in the article's sender or subject header field. The 
regular expressions are of the extended unix style.  The lead-in character is 
not part of the filter string.
:p.
A substring include filter string starts with the '+' character. The filter 
selects an article for further processing if the filter string matches any 
substring in the article's sender or subject header field.
:p.
A wildcard include filter string starts with the '*' character. The filter 
selects an article for further processing if the regular expression filter 
string matches any substring in the article's sender or subject header field.
:p.
A quoted substring exclude filter starts with the double-quote charactger 
(i.e. "). The filter excludes an article from further processing if the 
filter string matches any substring in the article's sender or subject 
header field. This filter type allows exclude substrings to start with one 
of the special lead-in characters.
:p.
Include and exclude filters may be intermixed; however, an exclude filter 
match overrides an include filter match.
:p.
Hopefully SPAM from a particular company has the URL of their web site 
embedded somewhere in the header (usually the subject).  The first two
lines of our filter file specifically target two of them.  Since it is
possible to have any keyword, the ".wmf" will remove most of the postings with
the a ".wmf" extension. (Windows Meta Files - the bane of :hp1.alt.binaries.clip-art:ehp1.)
Similarly the "warez" tries to keep down the binaries of a more illicit nature.
:p.
Be careful when adding entries to a filter file, for example if the keyword "jpg"
was included (jpeg-image extension), around 80% of all image articles would be filtered
out.
:p.
To disable all filtering uncheck the Enable Filtering box.
:p.
To filter based on duplicate filenames check the Fast Filename Filter box.
By default, &NH. checks for duplicates based on size only.
:p.
:hp2.User Interface:ehp2.
.br
:artwork fit align=center name='config_userinterface.bmp'.
.br
This panel is used to set the various options for working with News Harvest.
:p.
Running in "Unattended Mode" is usually associated with batch processing.
No message boxes are displayed, and as soon as News Harvest has started up
processing will begin.  Once processing is complete, News Harvest will exit.
Of course if News Harvest is unregistered, auto-starting cannot occurr, as
the user has to be present to click the "Dismiss" button on the welcome
dialog.  It's best to record a logfile if you are using this feature.
:p.
The files extracted from news are displayed in a directory-tree like structure.
The user may opt to have News Harvest load a reference to all previously 
downloaded files.  This can give slower startup times if you have many 
files extracted previously.  If "Show New Files" is selected, only files
extracted during this current run will be shown.  Of course it's possible
to get the complete list at any time by selecting "Refresh" from the popup
windows.
:p.
Another batch-like option is "Restarting".  Essentially this is when News Harvest
has finished processing all it's servers/groups and would normally finish.
However you may wish to run again.  Entering a looping value (> 0) will
make News Harvest start again - up to the number of times entered.  It is also
possible to cause a delay before restarting begins. 
:p.
:hp2.Logging:ehp2.
.br
:artwork fit align=center name='config_logging.bmp'.
.br
News Harvest is able to create an extensive report log on processing.  This log
will contain information such as processing, decoding and updates.  It is
very useful for a variety of purposes.
:p.
If a log is wanted, simply enter a filename into the field.  All data will
be appended in a plain text format to the file.  It is not necessary to stop 
News Harvest processing to turn on (or off) the logging feature.
:p.
It is often beneficial in setting up MIME types, to see what News Harvest
is doing with your configuration.  To see the commands used as a file is
opened, select the "Log File-Open Spawns" checkbox.
:p.
:hp2.Multipart Processing:ehp2.
.br
:artwork fit align=center name='config_multipart.bmp'.
.br
Some binaries are posted to Usenet News in multiple sections.
News Harvest is able to detect and properly decode most of these.  The situations
where News Harvest fails include things like missing sections, or articles
improperly marked.
:p.
To decode these binaries, News Harvest must store part articles until all
member parts have been downloaded.  To accomplish this a directory is created
called 'parts'.  Any article section will be placed here until they are
needed for decoding.  If for some reason not all parts are found, (which is quite
common on large multi-part postings) News Harvest will eventually erase these files. 
See "Maximum Part Age" below for more information on this.  If you need to
reclaim some disk space urgently, delete the content of the 'parts' directory
including (and especially) the index file.  Novice Usenet News users tend to
post very large files in a gazillion parts, only a few of which ever make it
to distribution.  Unfortunately for us news harvesters, all this rubbish tends
to visit the 'parts' directory sooner or later.  In order to minimise this, 
News Harvest includes a feature to ignore multipart articles where the total
number of parts is over some threshold.  This can increase processing speed
dramatically, as News Harvest does not have to download 20 parts of a 300 part
post.
Enter 0 to disable this feature if you have a high speed link and a reliable news source.
:p.
The drive on which this is contained may be on any partition except that 
:hp5.It must support long filenames!:ehp5.  It is not necessary to create
the 'parts' directory, this is automatic.
:p.
:hp2.Maximum Part Age:ehp2.
:p.
Article parts from multi-part postings are stored until all sections are found.
For various reasons, News Harvest may never see all parts, and thus the orphaned
bits hang around.
:p.
This value specifies the duration in days that News Harvest will keep 
part articles for.  This value should be set with respect to how often News Harvest
is run.  If this is every day, a value of 7 days works well.  But if the period
between successive runs extends to a week, it would be better to use a value
of 14 days.
:p.
If this value is set to 0, News Harvest will not store part articles, and therefore
not decode multi-part postings.
:p.
:hp2.Post Processing:ehp2.
.br
:artwork fit align=center name='config_postprocessing.bmp'.
.br
Immediately after writing the decoded article to disk,  News Harvest can execute a 
program to act on the new file.  The program will be started with two arguments, the
first is the extension of the filename (e.g. gif, jpg, html, exe, etc.),
and the second is the full filename.  If the filename has no recognisable 
extension, a '?' is substituted.  The extension will always be given in lower case.
:p.
This is a very powerful feature, it's possible to call a virus scanner on exe, 
play midi or mp3 files, view images as they download, it's limited only by 
imagination.  NOTE&colon. KRT recomends erasing all executable files extracted from 
news, as they are a massive security risk.
:p.
News Harvest will execute the post decode process simultaneously with its
normal processing.  This can cause problems if the process does not complete
before a long succession of downloads occur.  For example, if the post decode process
was to use the OS/2 image viewer (ib.exe) which will not exit without user intervention.
Many instances of the viewer would eventually be running, thus grinding the
system to a halt.
:p.
Therefore it is necessary to use a post-decode process in such a way so that
it exits (or knows how to have only one instance running) when the next call
arrives.  This is easy with a simple REXX script, utilising a few (easily
available) tools. 
:p.
:lines.
:font facename=Courier size=0x0.
:hp5.DISPLAY.CMD:ehp5.

    /* --- My first ever REXX Program --- */
    parse arg TYPE FILENAME
    '@echo off'
    
    if TYPE = 'jpg' | type = 'gif' then
    do
        'kill pmview.exe > nul'
        /* call PMView, the best OS/2 Image Viewer */
        /* Note&colon. PMView _must_ be set to NOT mention errors */
        'detach c&colon.\pmview\pmview.exe ' FILENAME
    end
            
    /* erase all .EXE 'less they be a trojan horse */
    if TYPE = 'exe' then
    do
        'del ' FILENAME
    end
                
    exit 0

:font facename=Helv size=0x0.
:elines.
:p.
Obviously for this to work you need both a 'kill' and PMView installed. 
DISPLAY.CMD is easily extended to handle other image types.
News Harvest will also call normal OS/2 executables, as a matter of 
fact any executable than can be started with a single command.
:p.
:hp2.Output:ehp2.
.br
:artwork fit align=center name='config_output.bmp'.
.br
The Output Path is where New Harvest will store any files it extracts from
news.  This may be on any drive except that :hp5.It must support long filenames!:ehp5.
News Harvest will automatically create a directory hierarchary to match the
news groups that are being processed.  
:p.
Examples&colon.
:sl.
:li.\NH_Output\
:li.N&colon.\users\jeff\Files\
:li.E&colon.\
:esl.
:p.
If the field is left blank, News Harvest will output files into the current
directory.
Paths should end in a '\' character, but
if this is omitted, News Harvest will append one automatically.
:p.
:hp2.Maximum Download Size:ehp2.
:p.
This numeric value limits the amount of files that can be extracted.
News Harvest will always stop before its output device fills up (within 3 Mbytes).
But this option allows greater control on how much news content is written.
:p.
For example, if this option is set to 20, News Harvest will stop processing 
when the size of decoded output reaches 20 Mbytes.  If you are extracting
very large files from Usenet News (greater than a few megabytes), it may be
necessary to set this value a little lower than the target value.  This is
because the total size of files might be something like 19.5 Mbytes, but 
the next article is 2.5Mb, thereby exceeding the quota by 2 Mb.
:p.
If no size is specified (i.e. a value of 0), News Harvest will run until
it fills the output device or processes all groups.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=6000 group=1.Mime Types
:i1 id=mime.Mime Types
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.Mime Types
:font facename=Helv size=0x0.
:artwork fit align=center name='MimeTypes.bmp'.
.br
For each of the file types listed it is possible to change the
double-click (Open) action.  Simply select the object type, and adjust the
associated program.  If the action is left blank, the default OS/2
association will be used.
:p.
The command should be entered as if it were typed at an OS/2 command
prompt, except that the filename should be replaced by a '%1', and the
program name extension should be included (.EXE .CMD .COM).
It is not necessary to specify the full path name if the executable is
in the path, but this gives slightly better performance.
:p.
For example, to use the OS/2 Multimedia Extensions to play a MIDI file,
use the following command in conjunction with the MIDI file type&colon. 
:lines.
  c&colon.\mmos2\play.cmd FILE=%1
:elines.
:p.
Similarly to use the inbuilt OS/2 Warp 4 Image Viewer for JPEG images&colon.
:lines.
  ib.exe %1
:elines.
:p.
or the much faster PMView...
:lines.
  D&colon.\PMView\PMView.exe %1
:elines.
:p.
One final example - Using Netscape Navigator to view HTML&colon.
:lines.
  F&colon.\Netscape\Netscape.exe FILE&colon.///%1
:elines.
:p.
Some filenames container embedded spaces, in this case News Harvest will
automatically quote the filename, it is not necessary to add these.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=7000 group=1.Setup Walkthrough
:i1 id=setup.Setup Walkthrough
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.Setup Walkthough
:font facename=Helv size=0x0.
:p.
:hp2.1. Add a News Host:ehp2.
:p.
The main window of News Harvest has an empty space to the right, activate the
context (pop-up) menu here.  The default OS/2 action for this is a right-mouse-button 
click.
:p.
From the pop-up menu, select 'Add Host".
:artwork fit align=center name='context_none.bmp'.
.br
When the new host dialog appears, enter the name of your News Server.
If you do not know the name of your news server, contact your System
Administrator, or Internet Service Provider.  At a pinch, try 'news'
and hit the 'Lookup' button.
.br
:artwork fit align=center name='AddNewsServer.bmp'.
.br
In this example we are using the news server 'news.krt.com.au'.
When the Server Name is satisfactory, click 'Ok'.
:p.
:hp2.2. Add some Groups to the Host:ehp2.
:p.
Now that the News Server is in place, it is necessary to add a group (or more)
to the host for harvesting.  To this end, activate the context menu for
the Server.
:artwork fit align=center name='Walk_group1.bmp'.
.br
Select 'Add Group', the new group dialog will appear.
:artwork fit align=center name='Walk_group2.bmp'.
.br
Enter the name of the group you wish to harvest from.  There are thousands of
such groups, later on you may wish to use the group-list feature.  But
for now just the group 'alt.binaries.pictures.animals' will do.
Click 'Enter'.  The new group will be atatched to your server.
:artwork fit align=center name='Walk_group3.bmp'.
:p.
:hp2.3. Configuration:ehp2.
:p.
Click the 'Config' button.  Click on the 'Output' tab.
Enter some suitable paths into the 
output path on the "Output" panel and part path on the "Multipart" panel.  
It's probably best to use the same path for both.
.br
:artwork fit align=center name='config_output.bmp'.
:artwork fit align=center name='config_multipart.bmp'.
:p.
:hp2.4. Starting:ehp2.
:p.
Click :artwork runin name='Go.bmp'.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=8000 group=1.Questions and Answers
:i1 id=qna.Questions and Answers
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.Questions and Answers
:font facename=Helv size=0x0.
:p.
:hp5.Q&colon.:ehp5. How can I speed up News Harvest ?
:p.
:hp5.A&colon.:ehp5. News Harvest spends most of its time downloading the article.
(Around 97% of total time.)  The best way to speed it us is to get a faster connection to
your news server.  Failing that, using accurate cull-sizes and a good filter file will 
remove some of the dross from processing.
:p.
:hp5.Q&colon.:ehp5. What are all these Xyz.jpg and Xyz000.jpg images ?
:p.
:hp5.A&colon.:ehp5. News Harvest maintains a list of files it has previously
harvested.  If a repost is detected, the file will be
skipped.  However it is quite common for two (or more)
totally different images to share the same filename.
In this case News Harvest will not overwrite the existing
file with a new (different) one.  The name of the second
download has a number appended to make it different.
:p.
:hp5.Q&colon.:ehp5. So why do I still get an Xyz.jpg anf Xyz000.jpg that are identical?
:p.
:hp5.A&colon.:ehp5. This is because a single news article contained two copies of the binary.
The occurances of this are quite uncommon, and to prevent this News Harvest
would have to compare every binary from a single article against every
other binary from the same article.  Putting up with the very-occasional
duplicate is a far better solution.
:p.
:hp5.Q&colon.:ehp5. How can I use News Harvest in a batch process ?
:p.
:hp5.A&colon.:ehp5. Select the "Run in unattended mode" checkbox in the Configuration.
:p.
:hp5.Q&colon.:ehp5. Why can't I run multiple instances of News Harvest ?
:p.
:hp5.A&colon.:ehp5. Multiple News Harvest processes do not know how to properly
share configuration and history files.
:p.
:hp5.Q&colon.:ehp5. How can I cut the initialisation time ?
:p.
:hp5.A&colon.:ehp5. Most time starting up is taken loading an entry into the 
output container for each file in the output file area.
Try moving some of these files, or select the "Show New Files" in the User Interface
section of the Configuration.
:p.
:hp5.Q&colon.:ehp5. What are these NoFileName.* files ?
:p.
:hp5.A&colon.:ehp5. Some news readers do not encode a filename with the
article being posted (Netscape Navigator 3.x for example).
News Harvest takes a 'best guess' at the content, and creates
a filename.
:p.
:h1 x=left y=bottom width=100% height=100% titlebar=none rules=none res=9000 group=1.Software Licence
:i1 id=licence.Software Licence
:lines.
:font facename=Helv size=28x12.
:artwork runin name='nh64.bmp'.Software Licence
:font facename=Helv size=0x0.
:elines.
:lines.
Copyright (C) 1997,1998  KRT Pty. Ltd.  All Rights Reserved
Copyright (C) 1997,1998  Kingsley Turner  All Rights Reserved
Copyright (c) 2000, 2001 Steven Levine &amp. Associates, Inc.
:elines.
:p.
Use of this software by any person or organisation for any purpose
requires that a usage licence be obtained by that person or organisation.
:p.
Commercial redistribution of this software, by itself or as part of
another application is allowed only under express written permission of
KRT Pty. Ltd.
:p.
KINGSLEY R. TURNER AND KRT PTY. LTD. DISCLAIM ALL WARRANTIES WITH REGARD 
TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL KINGSLEY R. TURNER OR KRT PTY. LTD. BE
LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, OR OTHER ACTION, ARISING OUT OF OR IN CONNECTION 
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.




:euserdoc.