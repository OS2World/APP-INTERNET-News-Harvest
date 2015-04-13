ipfc nsdlg.ipf
@if exist nh.hlp del nh.hlp > nul 2> nul
@if exist nsdlg.hlp rename nsdlg.hlp nh.hlp > nul 2> nul
@if exist nh.hlp copy nh.hlp .. > nul 2> nul
