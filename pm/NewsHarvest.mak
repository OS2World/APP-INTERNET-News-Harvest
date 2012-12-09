# NewsHarvest.mak,v 1.2 2002/01/05 19:28:16 root Exp

# NewsHarvest.mak
# Created by IBM WorkFrame/2 MakeMake at 20:28:04 on 12 July 2000
#
# The actions included in this make file are:
#  Compile::Resource Compiler
#  Compile::C++ Compiler
#  Link::Linker
#  Bind::Resource Bind

.SUFFIXES:

.SUFFIXES: \
    .cpp .obj .rc .res 

.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|fF.RES

{D:\SLA_DEV2\NHSHL}.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|fF.RES

{D:\SLA_DEV2\NHSHL\pm}.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|fF.RES

.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /I.\.. /DANSI /DOS2 /DOS2PM /DSOCKS /DTCPV40HDRS /Ti /Gm /G4 /C %s

{D:\SLA_DEV2\NHSHL}.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /I.\.. /DANSI /DOS2 /DOS2PM /DSOCKS /DTCPV40HDRS /Ti /Gm /G4 /C %s

{D:\SLA_DEV2\NHSHL\pm}.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /I.\.. /DANSI /DOS2 /DOS2PM /DSOCKS /DTCPV40HDRS /Ti /Gm /G4 /C %s

all: \
    .\NewsHarvest.exe

.\NewsHarvest.exe: \
    .\unlink.obj \
    .\xhdrpacket.obj \
    .\bincat.obj \
    .\decoder.obj \
    .\filestat.obj \
    .\header.obj \
    .\item.obj \
    .\log.obj \
    .\mime.obj \
    .\multipart.obj \
    .\netscape.obj \
    .\newsgroup.obj \
    .\newsserver.obj \
    .\obj_string.obj \
    .\part.obj \
    .\Class_Notebook.obj \
    .\config.obj \
    .\container.obj \
    .\dlg_log.obj \
    .\dlg_multi.obj \
    .\Dlg_Notebook.obj \
    .\dlg_out.obj \
    .\dlg_post.obj \
    .\Dlg_Priority.obj \
    .\dlg_processfilter.obj \
    .\Dlg_rego.obj \
    .\dlg_socks.obj \
    .\dlg_user.obj \
    .\download.obj \
    .\filelist.obj \
    .\groupdefinition.obj \
    .\help.obj \
    .\hostlist.obj \
    .\ledgend.obj \
    .\mimedlg.obj \
    .\mimeset.obj \
    .\newgroup.obj \
    .\newhost.obj \
    .\nsdlg.obj \
    .\pmtools.obj \
    .\serverdefinition.obj \
    .\splash.obj \
    .\sleep.obj \
    .\strutils.obj \
    .\nsdlg.res \
    {$(LIB)}tcp32dll.lib \
    {$(LIB)}so32dll.lib \
    {$(LIB)}libsock5.lib
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /pmtype:pm /noe"
     /FeNewsHarvest.exe 
     tcp32dll.lib 
     so32dll.lib 
     libsock5.lib 
     .\unlink.obj
     .\xhdrpacket.obj
     .\bincat.obj
     .\decoder.obj
     .\filestat.obj
     .\header.obj
     .\item.obj
     .\log.obj
     .\mime.obj
     .\multipart.obj
     .\netscape.obj
     .\newsgroup.obj
     .\newsserver.obj
     .\obj_string.obj
     .\part.obj
     .\Class_Notebook.obj
     .\config.obj
     .\container.obj
     .\dlg_log.obj
     .\dlg_multi.obj
     .\Dlg_Notebook.obj
     .\dlg_out.obj
     .\dlg_post.obj
     .\Dlg_Priority.obj
     .\dlg_processfilter.obj
     .\Dlg_rego.obj
     .\dlg_socks.obj
     .\dlg_user.obj
     .\download.obj
     .\filelist.obj
     .\groupdefinition.obj
     .\help.obj
     .\hostlist.obj
     .\ledgend.obj
     .\mimedlg.obj
     .\mimeset.obj
     .\newgroup.obj
     .\newhost.obj
     .\nsdlg.obj
     .\pmtools.obj
     .\serverdefinition.obj
     .\splash.obj
     .\sleep.obj
     .\strutils.obj
<<
    rc.exe .\nsdlg.res NewsHarvest.exe

.\nsdlg.res: \
    D:\SLA_DEV2\NHSHL\pm\nsdlg.rc \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}splash.dlg \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}splash.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}unreg_splash.dlg \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}version.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}ledgend.dlg \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}ledgend.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}mime.dlg \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}mime.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}hostlist.dlg \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}hostlist.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}newgroup.dlg \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}newgroup.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}newhost.dlg \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}newhost.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}nsdlg.dlg \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}nsdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}server.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}suggest.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}png.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}html.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}au.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}wav.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}tif.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}tga.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}pcx.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}mpg.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}mp3.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}midi.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}gif.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}fli.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}bmp.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}avi.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}jpeg.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}unknoe.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}processd.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}drag.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}f_close.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}f_open.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}host.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}base.ico \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}resource.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}dlg_socks.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}user.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}post.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}out.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;$(INCLUDE)}icons.h

.\unlink.obj: \
    D:\SLA_DEV2\NHSHL\unlink.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}unlink.h

.\strutils.obj: \
    D:\SLA_DEV2\NHSHL\strutils.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}strutils.h

.\sleep.obj: \
    D:\SLA_DEV2\NHSHL\sleep.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}sleep.h

.\splash.obj: \
    D:\SLA_DEV2\NHSHL\pm\splash.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}pmtools.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}splash.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}splashdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}threads.h

.\serverdefinition.obj: \
    D:\SLA_DEV2\NHSHL\pm\serverdefinition.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h

.\pmtools.obj: \
    D:\SLA_DEV2\NHSHL\pm\pmtools.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}pmtools.h

.\nsdlg.obj: \
    D:\SLA_DEV2\NHSHL\pm\nsdlg.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}pmtools.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}help.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}obj_string.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}download.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}nsdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}decoder.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newsserver.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newsgroup.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}header.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filestat.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multipart.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}netscape.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}item.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}binarytree.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}mime.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}charqueue.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}libsock5.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}xhdrpacket.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}bincat.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}sleep.h \
    win32\stdafx.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}part.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}ledgenddlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}ledgend.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}mimedlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}mimeset.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newgroup.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newhost.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}hostdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}splash.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}splashdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}threads.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}version.h

.\newhost.obj: \
    D:\SLA_DEV2\NHSHL\pm\newhost.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}help.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newhost.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}hostdlg.h

.\newgroup.obj: \
    D:\SLA_DEV2\NHSHL\pm\newgroup.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}help.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newsserver.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newsgroup.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}header.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}charqueue.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}libsock5.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}xhdrpacket.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}bincat.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}sleep.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}hostlistdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}hostlist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newgroup.h

.\mimeset.obj: \
    D:\SLA_DEV2\NHSHL\pm\mimeset.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}mimeset.h

.\mimedlg.obj: \
    D:\SLA_DEV2\NHSHL\pm\mimedlg.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}help.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}mime.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}mimedlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}mimeset.h

.\ledgend.obj: \
    D:\SLA_DEV2\NHSHL\pm\ledgend.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}ledgenddlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}ledgend.h

.\hostlist.obj: \
    D:\SLA_DEV2\NHSHL\pm\hostlist.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}help.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newsserver.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newsgroup.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}header.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}charqueue.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}libsock5.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}xhdrpacket.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}bincat.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}sleep.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}hostlistdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}hostlist.h

.\help.obj: \
    D:\SLA_DEV2\NHSHL\pm\help.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}help.h

.\groupdefinition.obj: \
    D:\SLA_DEV2\NHSHL\pm\groupdefinition.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h

.\filelist.obj: \
    D:\SLA_DEV2\NHSHL\pm\filelist.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h

.\download.obj: \
    D:\SLA_DEV2\NHSHL\pm\download.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}pmtools.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}obj_string.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}download.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}nsdlg.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}decoder.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newsserver.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}newsgroup.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}header.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filestat.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multipart.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}netscape.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}item.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}binarytree.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}mime.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}charqueue.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}libsock5.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}xhdrpacket.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}bincat.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}sleep.h \
    win32\stdafx.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}part.h

.\dlg_user.obj: \
    D:\SLA_DEV2\NHSHL\pm\dlg_user.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}obj_string.h

.\dlg_socks.obj: \
    D:\SLA_DEV2\NHSHL\pm\dlg_socks.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h

.\Dlg_rego.obj: \
    D:\SLA_DEV2\NHSHL\pm\Dlg_rego.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h

.\dlg_processfilter.obj: \
    D:\SLA_DEV2\NHSHL\pm\dlg_processfilter.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h

.\Dlg_Priority.obj: \
    D:\SLA_DEV2\NHSHL\pm\Dlg_Priority.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h

.\dlg_post.obj: \
    D:\SLA_DEV2\NHSHL\pm\dlg_post.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h

.\dlg_out.obj: \
    D:\SLA_DEV2\NHSHL\pm\dlg_out.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h

.\Dlg_Notebook.obj: \
    D:\SLA_DEV2\NHSHL\pm\Dlg_Notebook.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Class_Notebook.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}pmtools.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}help.h

.\dlg_multi.obj: \
    D:\SLA_DEV2\NHSHL\pm\dlg_multi.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h

.\dlg_log.obj: \
    D:\SLA_DEV2\NHSHL\pm\dlg_log.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Resource.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}priority.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}rego.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}multi.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}out.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filter.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}post.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}user.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}dlg_socks.h

.\container.obj: \
    D:\SLA_DEV2\NHSHL\pm\container.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h

.\config.obj: \
    D:\SLA_DEV2\NHSHL\pm\config.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}config.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}serverdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}groupdefinition.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}container.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}icons.h \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}filelist.h

.\Class_Notebook.obj: \
    D:\SLA_DEV2\NHSHL\pm\Class_Notebook.cpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Basic.hpp \
    {D:\SLA_DEV2\NHSHL\pm;.\..;$(INCLUDE);}Class_Notebook.hpp

.\part.obj: \
    D:\SLA_DEV2\NHSHL\part.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}part.h

.\obj_string.obj: \
    D:\SLA_DEV2\NHSHL\obj_string.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}obj_string.h

.\newsserver.obj: \
    D:\SLA_DEV2\NHSHL\newsserver.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}xhdrpacket.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}bincat.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}charqueue.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}newsserver.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}newsgroup.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}libsock5.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}sleep.h

.\newsgroup.obj: \
    D:\SLA_DEV2\NHSHL\newsgroup.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}newsgroup.h

.\netscape.obj: \
    D:\SLA_DEV2\NHSHL\netscape.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}netscape.h

.\multipart.obj: \
    D:\SLA_DEV2\NHSHL\multipart.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}xhdrpacket.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}bincat.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}decoder.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}filestat.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}item.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}binarytree.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}header.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}mime.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}charqueue.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}strutils.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}multipart.h \
    win32\stdafx.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}newsserver.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}newsgroup.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}filelist.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}part.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}libsock5.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}sleep.h

.\mime.obj: \
    D:\SLA_DEV2\NHSHL\mime.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}mime.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h

.\log.obj: \
    D:\SLA_DEV2\NHSHL\log.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h

.\item.obj: \
    D:\SLA_DEV2\NHSHL\item.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}item.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}strutils.h

.\header.obj: \
    D:\SLA_DEV2\NHSHL\header.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}header.h

.\filestat.obj: \
    D:\SLA_DEV2\NHSHL\filestat.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}decoder.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}filestat.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}item.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}binarytree.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}header.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}mime.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}charqueue.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}strutils.h

.\decoder.obj: \
    D:\SLA_DEV2\NHSHL\decoder.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}decoder.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}filestat.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}array.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}item.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}binarytree.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}header.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}mime.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}charqueue.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}unlink.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}log.h \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}strutils.h

.\bincat.obj: \
    D:\SLA_DEV2\NHSHL\bincat.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}bincat.h

.\xhdrpacket.obj: \
    D:\SLA_DEV2\NHSHL\xhdrpacket.cpp \
    {D:\SLA_DEV2\NHSHL;.\..;$(INCLUDE);}xhdrpacket.h
