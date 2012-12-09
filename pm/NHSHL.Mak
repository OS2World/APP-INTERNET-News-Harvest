# NHSHL.Mak,v 1.5 2004/12/05 00:41:31 root Exp

# NHSHL.mak
# Created by IBM WorkFrame/2 MakeMake at 20:28:04 on 12 July 2000
# Modified by hand after this to be self relative

# Revisions	09 May 01 SHL - build NHSHL
#		11 Jul 01 SHL - add config.h, priority.dlg
#		15 Dec 03 SHL - Add .def and .mak dependencies

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
    rc.exe -DSLAINC -r %s %|fF.RES

{..}.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -DSLAINC -r %s %|fF.RES

.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /I.\.. /DANSI /DOS2 /DOS2PM /DSOCKS /DTCPV40HDRS /DNOPRINT /DSLAINC /Ti /Gm /G4 /C %s

{..}.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /I.\.. /DANSI /DOS2 /DOS2PM /DSOCKS /DTCPV40HDRS /DNOPRINT /DSLAINC /Ti /Gm /G4 /C %s

all: \
    .\NHSHL.exe

.\NHSHL.exe: \
    .\nhshl.mak \
    .\nhshl.def \
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
     /B" /de /noe /ma /l"
     /Fenhshl.exe
     .\nhshl.def
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
    rc.exe .\nsdlg.res NHSHL.exe

.\nsdlg.res: \
    .\nsdlg.rc \
    {$(INCLUDE)}filter.dlg \
    {$(INCLUDE)}multi.dlg \
    {$(INCLUDE)}priority.dlg \
    {$(INCLUDE)}splash.dlg \
    {$(INCLUDE)}splash.h \
    {$(INCLUDE)}unreg_splash.dlg \
    {$(INCLUDE)}version.h \
    {$(INCLUDE)}ledgend.dlg \
    {$(INCLUDE)}ledgend.h \
    {$(INCLUDE)}mime.dlg \
    {$(INCLUDE)}mime.h \
    {$(INCLUDE)}hostlist.dlg \
    {$(INCLUDE)}hostlist.h \
    {$(INCLUDE)}newgroup.dlg \
    {$(INCLUDE)}newgroup.h \
    {$(INCLUDE)}newhost.dlg \
    {$(INCLUDE)}newhost.h \
    {$(INCLUDE)}nsdlg.dlg \
    {$(INCLUDE)}nsdlg.h \
    {$(INCLUDE)}server.ico \
    {$(INCLUDE)}suggest.ico \
    {$(INCLUDE)}png.ico \
    {$(INCLUDE)}html.ico \
    {$(INCLUDE)}au.ico \
    {$(INCLUDE)}wav.ico \
    {$(INCLUDE)}tif.ico \
    {$(INCLUDE)}tga.ico \
    {$(INCLUDE)}pcx.ico \
    {$(INCLUDE)}mpg.ico \
    {$(INCLUDE)}mp3.ico \
    {$(INCLUDE)}midi.ico \
    {$(INCLUDE)}gif.ico \
    {$(INCLUDE)}fli.ico \
    {$(INCLUDE)}bmp.ico \
    {$(INCLUDE)}avi.ico \
    {$(INCLUDE)}jpeg.ico \
    {$(INCLUDE)}unknoe.ico \
    {$(INCLUDE)}processd.ico \
    {$(INCLUDE)}drag.ico \
    {$(INCLUDE)}f_close.ico \
    {$(INCLUDE)}f_open.ico \
    {$(INCLUDE)}host.ico \
    {$(INCLUDE)}base.ico \
    {$(INCLUDE)}resource.h \
    {$(INCLUDE)}dlg_socks.h \
    {$(INCLUDE)}user.h \
    {$(INCLUDE)}post.h \
    {$(INCLUDE)}filter.h \
    {$(INCLUDE)}filter.h \
    {$(INCLUDE)}dlg_log.h \
    {$(INCLUDE)}out.h \
    {$(INCLUDE)}multi.h \
    {$(INCLUDE)}rego.h \
    {$(INCLUDE)}priority.h \
    {$(INCLUDE)}icons.h

.\unlink.obj: \
    ..\unlink.cpp \
    {..;$(INCLUDE);}unlink.h

.\strutils.obj: \
    ..\strutils.cpp \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}strutils.h

.\sleep.obj: \
    ..\sleep.cpp \
    {..;$(INCLUDE);}sleep.h

.\splash.obj: \
    .\splash.cpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}pmtools.h \
    {..;$(INCLUDE);}splash.h \
    {..;$(INCLUDE);}splashdlg.h \
    {..;$(INCLUDE);}threads.h

.\serverdefinition.obj: \
    .\serverdefinition.cpp \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h

.\pmtools.obj: \
    .\pmtools.cpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}pmtools.h

.\nsdlg.obj: \
    .\nsdlg.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h \
    {..;$(INCLUDE);}pmtools.h \
    {..;$(INCLUDE);}help.h \
    {..;$(INCLUDE);}obj_string.h \
    {..;$(INCLUDE);}download.h \
    {..;$(INCLUDE);}nsdlg.h \
    {..;$(INCLUDE);}decoder.h \
    {..;$(INCLUDE);}newsserver.h \
    {..;$(INCLUDE);}newsgroup.h \
    {..;$(INCLUDE);}header.h \
    {..;$(INCLUDE);}filestat.h \
    {..;$(INCLUDE);}multipart.h \
    {..;$(INCLUDE);}netscape.h \
    {..;$(INCLUDE);}item.h \
    {..;$(INCLUDE);}binarytree.h \
    {..;$(INCLUDE);}mime.h \
    {..;$(INCLUDE);}charqueue.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}libsock5.h \
    {..;$(INCLUDE);}xhdrpacket.h \
    {..;$(INCLUDE);}bincat.h \
    {..;$(INCLUDE);}sleep.h \
    win32\stdafx.h \
    {..;$(INCLUDE);}part.h \
    {..;$(INCLUDE);}ledgenddlg.h \
    {..;$(INCLUDE);}ledgend.h \
    {..;$(INCLUDE);}mimedlg.h \
    {..;$(INCLUDE);}mimeset.h \
    {..;$(INCLUDE);}groupdlg.h \
    {..;$(INCLUDE);}newgroup.h \
    {..;$(INCLUDE);}newhost.h \
    {..;$(INCLUDE);}hostdlg.h \
    {..;$(INCLUDE);}splash.h \
    {..;$(INCLUDE);}splashdlg.h \
    {..;$(INCLUDE);}threads.h \
    {..;$(INCLUDE);}version.h

.\newhost.obj: \
    .\newhost.cpp \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}help.h \
    {..;$(INCLUDE);}newhost.h \
    {..;$(INCLUDE);}hostdlg.h

.\newgroup.obj: \
    .\newgroup.cpp \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}help.h \
    {..;$(INCLUDE);}newsserver.h \
    {..;$(INCLUDE);}newsgroup.h \
    {..;$(INCLUDE);}header.h \
    {..;$(INCLUDE);}charqueue.h \
    {..;$(INCLUDE);}libsock5.h \
    {..;$(INCLUDE);}xhdrpacket.h \
    {..;$(INCLUDE);}bincat.h \
    {..;$(INCLUDE);}sleep.h \
    {..;$(INCLUDE);}hostlistdlg.h \
    {..;$(INCLUDE);}hostlist.h \
    {..;$(INCLUDE);}groupdlg.h \
    {..;$(INCLUDE);}newgroup.h

.\mimeset.obj: \
    .\mimeset.cpp \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}mimeset.h

.\mimedlg.obj: \
    .\mimedlg.cpp \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}help.h \
    {..;$(INCLUDE);}mime.h \
    {..;$(INCLUDE);}mimedlg.h \
    {..;$(INCLUDE);}mimeset.h

.\ledgend.obj: \
    .\ledgend.cpp \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}ledgenddlg.h \
    {..;$(INCLUDE);}ledgend.h

.\hostlist.obj: \
    .\hostlist.cpp \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}help.h \
    {..;$(INCLUDE);}newsserver.h \
    {..;$(INCLUDE);}newsgroup.h \
    {..;$(INCLUDE);}header.h \
    {..;$(INCLUDE);}charqueue.h \
    {..;$(INCLUDE);}libsock5.h \
    {..;$(INCLUDE);}xhdrpacket.h \
    {..;$(INCLUDE);}bincat.h \
    {..;$(INCLUDE);}sleep.h \
    {..;$(INCLUDE);}hostlistdlg.h \
    {..;$(INCLUDE);}hostlist.h

.\help.obj: \
    .\help.cpp \
    {..;$(INCLUDE);}help.h

.\groupdefinition.obj: \
    .\groupdefinition.cpp \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}groupdefinition.h

.\filelist.obj: \
    .\filelist.cpp \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}filelist.h

.\download.obj: \
    .\download.cpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}pmtools.h \
    {..;$(INCLUDE);}obj_string.h \
    {..;$(INCLUDE);}download.h \
    {..;$(INCLUDE);}nsdlg.h \
    {..;$(INCLUDE);}decoder.h \
    {..;$(INCLUDE);}newsserver.h \
    {..;$(INCLUDE);}newsgroup.h \
    {..;$(INCLUDE);}header.h \
    {..;$(INCLUDE);}filestat.h \
    {..;$(INCLUDE);}multipart.h \
    {..;$(INCLUDE);}netscape.h \
    {..;$(INCLUDE);}item.h \
    {..;$(INCLUDE);}binarytree.h \
    {..;$(INCLUDE);}mime.h \
    {..;$(INCLUDE);}charqueue.h \
    {..;$(INCLUDE);}libsock5.h \
    {..;$(INCLUDE);}xhdrpacket.h \
    {..;$(INCLUDE);}bincat.h \
    {..;$(INCLUDE);}sleep.h \
    win32\stdafx.h \
    {..;$(INCLUDE);}part.h

.\dlg_user.obj: \
    .\dlg_user.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h \
    {..;$(INCLUDE);}obj_string.h

.\dlg_socks.obj: \
    .\dlg_socks.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h

.\Dlg_rego.obj: \
    .\Dlg_rego.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h

.\dlg_processfilter.obj: \
    .\dlg_processfilter.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h

.\Dlg_Priority.obj: \
    .\Dlg_Priority.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h

.\dlg_post.obj: \
    .\dlg_post.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h

.\dlg_out.obj: \
    .\dlg_out.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h

.\Dlg_Notebook.obj: \
    .\Dlg_Notebook.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}Class_Notebook.hpp \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h \
    {..;$(INCLUDE);}pmtools.h \
    {..;$(INCLUDE);}help.h

.\dlg_multi.obj: \
    .\dlg_multi.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h

.\dlg_log.obj: \
    .\dlg_log.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}Resource.h \
    {..;$(INCLUDE);}dlg_log.h \
    {..;$(INCLUDE);}priority.h \
    {..;$(INCLUDE);}rego.h \
    {..;$(INCLUDE);}multi.h \
    {..;$(INCLUDE);}out.h \
    {..;$(INCLUDE);}filter.h \
    {..;$(INCLUDE);}post.h \
    {..;$(INCLUDE);}user.h \
    {..;$(INCLUDE);}dlg_socks.h

.\container.obj: \
    .\container.cpp \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h

.\config.obj: \
    .\config.cpp \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}serverdefinition.h \
    {..;$(INCLUDE);}groupdefinition.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}container.h \
    {..;$(INCLUDE);}icons.h \
    {..;$(INCLUDE);}filelist.h

.\Class_Notebook.obj: \
    .\Class_Notebook.cpp \
    {..;$(INCLUDE);}Basic.hpp \
    {..;$(INCLUDE);}Class_Notebook.hpp

.\part.obj: \
    ..\part.cpp \
    {..;$(INCLUDE);}part.h

.\obj_string.obj: \
    ..\obj_string.cpp \
    {..;$(INCLUDE);}obj_string.h

.\newsserver.obj: \
    ..\newsserver.cpp \
    {..;$(INCLUDE);}xhdrpacket.h \
    {..;$(INCLUDE);}bincat.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}charqueue.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}newsserver.h \
    {..;$(INCLUDE);}newsgroup.h \
    {..;$(INCLUDE);}libsock5.h \
    {..;$(INCLUDE);}config.h \
    {..;$(INCLUDE);}sleep.h

.\newsgroup.obj: \
    ..\newsgroup.cpp \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}newsgroup.h

.\netscape.obj: \
    ..\netscape.cpp \
    {..;$(INCLUDE);}netscape.h

.\multipart.obj: \
    ..\multipart.cpp \
    {..;$(INCLUDE);}xhdrpacket.h \
    {..;$(INCLUDE);}bincat.h \
    {..;$(INCLUDE);}decoder.h \
    {..;$(INCLUDE);}filestat.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}item.h \
    {..;$(INCLUDE);}binarytree.h \
    {..;$(INCLUDE);}header.h \
    {..;$(INCLUDE);}mime.h \
    {..;$(INCLUDE);}charqueue.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}strutils.h \
    {..;$(INCLUDE);}multipart.h \
    win32\stdafx.h \
    {..;$(INCLUDE);}newsserver.h \
    {..;$(INCLUDE);}newsgroup.h \
    {..;$(INCLUDE);}filelist.h \
    {..;$(INCLUDE);}part.h \
    {..;$(INCLUDE);}libsock5.h \
    {..;$(INCLUDE);}sleep.h

.\mime.obj: \
    ..\mime.cpp \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}mime.h \
    {..;$(INCLUDE);}log.h

.\log.obj: \
    ..\log.cpp \
    {..;$(INCLUDE);}log.h

.\item.obj: \
    ..\item.cpp \
    {..;$(INCLUDE);}item.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}strutils.h

.\header.obj: \
    ..\header.cpp \
    {..;$(INCLUDE);}header.h

.\filestat.obj: \
    ..\filestat.cpp \
    {..;$(INCLUDE);}decoder.h \
    {..;$(INCLUDE);}filestat.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}item.h \
    {..;$(INCLUDE);}binarytree.h \
    {..;$(INCLUDE);}header.h \
    {..;$(INCLUDE);}mime.h \
    {..;$(INCLUDE);}charqueue.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}strutils.h

.\decoder.obj: \
    ..\decoder.cpp \
    {..;$(INCLUDE);}decoder.h \
    {..;$(INCLUDE);}filestat.h \
    {..;$(INCLUDE);}array.h \
    {..;$(INCLUDE);}item.h \
    {..;$(INCLUDE);}binarytree.h \
    {..;$(INCLUDE);}header.h \
    {..;$(INCLUDE);}mime.h \
    {..;$(INCLUDE);}charqueue.h \
    {..;$(INCLUDE);}unlink.h \
    {..;$(INCLUDE);}log.h \
    {..;$(INCLUDE);}strutils.h

.\bincat.obj: \
    ..\bincat.cpp \
    {..;$(INCLUDE);}bincat.h

.\xhdrpacket.obj: \
    ..\xhdrpacket.cpp \
    {..;$(INCLUDE);}xhdrpacket.h
