#ifndef DOFILECOPY_H
#define DOFILECOPY_H

// DoFileCopy.h : header file
//

#include "resource.h"
#include "COM_Utilities.h"

/////////////////////////////////////////////////////////////////////////////
// CDoFileCopy dialog

class CDoFileCopy : public CDialog
{
// Construction
public:
	CDoFileCopy(CWnd *parent);
	~CDoFileCopy();
	void  CleanUpName(CString*);
	volatile int  m_dNewMediaState;

// Dialog Data
	//{{AFX_DATA(CDoFileCopy)
	enum { IDD = IDD_COPYFILES4_DIALOG };
	CListCtrl	m_pFileList;
	//}}AFX_DATA

	void UnpackFileExtensionsList(
			BSTR& bstrFileExtensions,
			char **pFileExtensionList,
			int  *iNumberOfFileExtensions);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDoFileCopy)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	public:
	virtual int DoModal(CString,CString,CString,CString,CString,CString);

// Implementation
protected:
	HICON m_hSelectState;
	HICON m_hInsertState;
	HICON m_hRemoveState;
	HICON m_hVerifyState;
	HICON m_hIconGreen;
	HICON m_hIconRed;
	HICON m_hIconYellow1;
	HICON m_hIconYellow2;
	CString m_pDestination; 
	
	//<<QA>> these should all be renamed.  They
	// are not pointers.
	CString m_pPrevious;
	CString	m_pDateTime;
	CString m_pInspectorName;
	CString m_pInspectorID;
	CString m_pInspection;
	CString m_pIPName;
	CString m_csDLGName;

	CImageList* m_pImageList;
	bool  m_bWaitingForMedia;
	bool  m_bDirectionUp;
	int   m_dTimeInDialog;
	int	  m_dEntryCount;
	int   m_dSortedCol;
	int   m_dTotal;
	int   m_dSelected;
	int	  m_dToClose;
	bool  m_bDoingSort;
	bool  m_bDoneSetup;
	bool  m_bDoingSwap;
	bool  m_bSelectionChangable;
	bool  m_bRemovable;
	DWORD m_dTotalNumberOfClusters;
	BOOL  SortTextItems( int nCol, BOOL bAscending, 
                                 int low = 0, int high = -1 );
//	bool  DoCopyCat(char* Suffix, CString* FullPath, char* Source,char* Id,int Inst);
	bool  DoCopy   (char* Suffix, CString* FullPath, char* Source,char* Id,int Inst);
	bool  FileMkDir(CString path);
	bool  DoCopyAFile(CString path);
	void  DoMove();
	void  PrintINISection(char *szSection, FILE*outfile);
	void  GetYMDFromName(char* szName, bool bNotAShortFileName, int* piYear, int* piMonth, int* piDay); //DR 359
	void  BuildLongFileName(CString &cResult, //DR 359
				CString cFormat,CString cPath,
				CString cType,CString cID,
				int iYear,	int iMonth,	int iDay,
//				int iHour,	int iMinute,int iSecond,
				CString cSuffix	);
	int	  Eject(char* EjectDrive);

	DWORD GetMediaSize();


	int   m_bFinishState;
	bool  TestDrive();
	FILE* m_pIPFile;
	FILE* m_pDLGFile;
	// bool  m_bShowAllSuffixMatches;  // todo: incomplete copy override, need to rewrite the details in DopCopy to use this flag too
	bool  m_bFirstPass;
	bool  m_bShortName; //DR 359
	CString m_csNamePattern; //DR 359
	// Generated message map functions
	//{{AFX_MSG(CDoFileCopy)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnUndoButton();
	afx_msg void OnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegincopyButton();
	virtual void OnOK();
	afx_msg void OnItemchangingFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickFileList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	afx_msg void OnTocloseButton();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnQueryCancelAutoPlay(WPARAM wParam, LPARAM lParam);//DR00175
	//}}AFX_MSG
	afx_msg void OnClickFileList(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);	//DR00175
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // DOFILECOPY_H)
