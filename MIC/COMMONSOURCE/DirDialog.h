// DirDialog.h

#ifndef DIRDIALOG_H
#define DIRDIALOG_H

class CDirDialog
{
  public:
    CDirDialog(CWnd*);
    virtual ~CDirDialog();
    int DoBrowse();

	CWnd *m_pParent;
    CString m_strPath;
    CString m_strInitDir;
    CString m_strSelDir;
    CString m_strTitle;
    int  m_iImageIndex;
};

#endif // DIRDIALOG_H


