//FileMgr.h
#ifndef FILEMGR_H
#define FILEMGR_H

// FileMgr.h : header file
//

#include "COM_Utilities.h"

struct FileRec 
{
	FILETIME Date;
	bool Deleted;
	int Size;
	char *PathName;
	FileRec* Older;
	FileRec* Newer;
};

class CMicMgrDlg;

/////////////////////////////////////////////////////////////////////////////
// CFileMgr thread

class CFileMgr : public CWinThread
{
	DECLARE_DYNCREATE(CFileMgr)
protected:
	CFileMgr();           // protected constructor used by dynamic creation

// Attributes
public:
	virtual ~CFileMgr();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileMgr)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	void DoTimeCheck(WPARAM,LPARAM);
	void DoCopyData(WPARAM,LPARAM);
	void DoArchiveCheck();
	int  m_dTimeInCopy;
	int  m_dTimeFromAge;
	void DoShutdown(WPARAM,LPARAM);

	void SetParentPointer(CMicMgrDlg *p);
	void SetIDC_COM_STOPRegisteredMessageValue(UINT value);

	CMicMgrDlg *m_pMicMgrDlgParent;

// Implementation
protected:

	FILE* OutFile;
	FileRec* Head;
	FileRec* CreateRec(char* Path,WIN32_FIND_DATA FindFileData);
	void Insert(FileRec* NewRec, FileRec* At);
	void CleanUp(FileRec* At);
	__int64 VisitSize(FileRec* At, __int64 BytesToDelete, FILE* OutFile = NULL);
	void VisitAge (FileRec* At, COleDateTime TodayMx, FILE* OutFile = NULL);

	bool m_bDoingCopyFile;
	bool m_bDoingAgeFile;
	// Generated message map functions
	//{{AFX_MSG(CFileMgr)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // FILEMGR_H
