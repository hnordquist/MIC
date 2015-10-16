//$History: DirDialog.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Pelowitz     Date: 3/12/08    Time: 3:54p
 * Created in $/WatchCom II
 * At or near retirement
// 
// *****************  Version 1  *****************
// User: Pelowitz     Date: 9/07/06    Time: 1:34p
// Created in $/WatchCom II
// 
// *****************  Version 2  *****************
// User: Pelowitz     Date: 1/11/06    Time: 7:43a
// Updated in $/MIC 1.910
// 
// *****************  Version 2  *****************
// User: Moore        Date: 12/01/05   Time: 6:05p
// Updated in $/MIC 2.000/CommonSource
// Changed DIRDLG.cpp to keep error message from coming up when
// running from an MPSS startup on a Windows XP operating system.
// This was a problem Kelly Michel and Peggy Moore observed in Feb 2004
// in Vienna during the Baseline 1 training.  The students had installed
// MIC 1.907 on Windows XP laptops and kept getting an error message
// whenever the Browse buttons were pressed. Thereafter, however, the
// function was successful in presenting the correct dialog.  That is, the
// problem was a cosmetic one, but very ugly.  The error message did not
// happen when running with Windows 2000.
// 
// *****************  Version 1  *****************
// User: Moore        Date: 8/09/05    Time: 4:14p
// Created in $/MIC 2.000/CommonSource
// 

///////////////////////////////////////////////////////////////////////////
// DirDialog.cpp: implementation of the CDirDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirDialog.h"
#include "shlobj.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirDialog::CDirDialog(CWnd *pParent)
{
	//save the parent
	m_pParent = pParent;
	m_strInitDir.Empty();
}

CDirDialog::~CDirDialog()
{
}

//do the browse window
int CDirDialog::DoBrowse()
{
  LPMALLOC pMalloc;
  if (SHGetMalloc (&pMalloc)!= NOERROR)
  {
      return 0;
  }

  BROWSEINFO bInfo;
  LPITEMIDLIST pidl;
  ZeroMemory ( (PVOID) &bInfo,sizeof (BROWSEINFO));

  //
  //Deleted the following line to keep error message from coming up when
  //running from an MPSS startup on a Windows XP operating system.
  //This was a problem Kelly Michel and Peggy Moore observed in February 2004
  //in Vienna during the Baseline 1 training.  The students had installed
  //MIC 1.907 on Windows XP laptops and kept getting an error message whenever
  //the Browse buttons were pressed. Thereafter, however, the function was
  //successful in presenting the correct dialog.  That is, the problem was
  //a cosmetic one, but very ugly.  The error message did not happen when
  //running with Windows 2000.
  //
  //The fix makes it so the desktop is evoked not via a NULL in the BROWSEINFO
  //structure, but by using an empty string to determine the location of the
  //root folder.
  //
  //PJM 12/1/2005
  //
  //if (!m_strInitDir.IsEmpty ())
  {
    OLECHAR       olePath[MAX_PATH];
    ULONG         chEaten;
    ULONG         dwAttributes;
    HRESULT       hr;
    LPSHELLFOLDER pDesktopFolder;
    // 
    // Get a pointer to the Desktop's IShellFolder interface. 
    //
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
    {
      //
      // IShellFolder::ParseDisplayName requires the file name be in Unicode.
      //
      MultiByteToWideChar(CP_ACP, 
		  MB_PRECOMPOSED, m_strInitDir.GetBuffer(MAX_PATH), -1,
                          olePath, MAX_PATH);

      m_strInitDir.ReleaseBuffer (-1);
      //
      // Convert the path to an ITEMIDLIST.
      //
      hr = pDesktopFolder->ParseDisplayName(NULL,
			NULL,olePath,&chEaten,&pidl,&dwAttributes);

      if (FAILED(hr))
      {
        pMalloc ->Free (pidl);
        pMalloc ->Release ();
        return 0;
      }

      bInfo.pidlRoot = pidl;

    }
  }
  bInfo.hwndOwner = m_pParent->m_hWnd;
  bInfo.pszDisplayName = m_strPath.GetBuffer (MAX_PATH);
  bInfo.lpszTitle = (m_strTitle.IsEmpty()) ? "Open":m_strTitle;
  bInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;

  if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
  {
	  return 0;
  }
  m_strPath.ReleaseBuffer();
  m_iImageIndex = bInfo.iImage;

  if (::SHGetPathFromIDList(pidl,m_strPath.GetBuffer(MAX_PATH)) == FALSE)
  {
    pMalloc ->Free(pidl);
    pMalloc ->Release();
    return 0;
  }

  m_strPath.ReleaseBuffer();

  pMalloc ->Free(pidl);
  pMalloc ->Release();

  return 1;
}