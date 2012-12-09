/* Class_Notebook.hpp,v 1.2 2001/06/20 00:42:40 root Exp */

// Class_Notebook.hpp - generic PM notebook support class

#ifndef CLASS_NOTEBOOK_HPP
#define CLASS_NOTEBOOK_HPP

//------------------------------------------------------------------------

class Class_Notebook {
public:
     Class_Notebook( HWND hwnd, ULONG ulNotebookDialogResource );
     void AddMajor( ULONG ulNotebookPageResource, char *szTabText = "", PFNWP fnwpNotebookDialog = WinDefDlgProc, char *szStatusText = "" );
     void AddMinor( ULONG ulNotebookPageResource, char *szTabText = "", PFNWP fnwpNotebookDialog = WinDefDlgProc, char *szStatusText = "" );

     MRESULT SendMessage( ULONG ulPageID, ULONG msg, MPARAM mp1, MPARAM mp2 );
     BOOL PostMessage( ULONG ulPageID, ULONG msg, MPARAM mp1, MPARAM mp2 );

     USHORT getPageNo(void);
     void gotoPageNo(USHORT uPageNo);

private:
     void AddTab( USHORT usType, ULONG ulNotebookPageResource, char *szTabText = "", PFNWP fnwpNotebookDialog = WinDefDlgProc, char *szStatusText = "" );

     HWND hwndNotebook;
};

#endif // CLASS_NOTEBOOK_HPP
