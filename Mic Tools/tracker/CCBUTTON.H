//CColorButton Class Header File

#if !defined(COLORBUTTON)
#define COLORBUTTON

#include <winsock.h>

static	WORD s_wVer;
static	WSADATA s_WSAData;
static	SOCKET s_sd_dg;
static	struct sockaddr_in s_sAddr_Loc;
static	struct sockaddr_in s_sAddr_Rem;
static	int s_dDefPort;
static  int s_iRc;
static int m_dNetCount;
 
class CColorButton: public CButton {
public:
	CColorButton(bool ParticipateInNet);//:CButton(){};
	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, 
		         const RECT& rect, CWnd* pParentWnd, UINT nID );
	COLORREF SetFaceColor(UINT, bool redraw = true);
	COLORREF GetFaceColor();
	COLORREF SetTextColor(UINT, bool redraw = true);
	COLORREF SetFlagColor(UINT, bool redraw = true);
	int  GetNumeric();
	void SetFlash(bool set);
	void SetNumeric(int, bool redraw = true);
	void SetFlagSize(UINT, bool redraw = true);
	void SetFlag(bool set, bool redraw = true);
	void SetBell(bool set, bool redraw = true);
	void SetTime(bool set, bool redraw = true);
	void SetOthr(bool set, bool redraw = true);
	void SetDisk(bool set, bool redraw = true);
	void SetQuest(bool set, bool redraw = true);
	void SetBase(int, bool redraw = true);
	void SetFlagChar(char ch, bool redraw = true);
	void SetBellChar(char ch, bool redraw = true);
	void SetTimeChar(char ch, bool redraw = true);
	void SetOthrChar(char ch, bool redraw = true);
	void SetDiskChar(char ch, bool redraw = true);
	void SetQuestChar(char ch, bool redraw = true);
	void SetPreNum(char *txt, bool redraw = true);
	void ReCount(bool reset = false);
	void Redraw(BOOL redraw);
	void GetFlags(char*);
	void GetNumericLine(char*, size_t count);
	bool GetFlash();
	char GetBell() {return bell;};

private:
	COLORREF face_color;
	COLORREF text_color;
	COLORREF flag_color;
	UINT     flag_size;
	char	 m_szPreNum[32];
	CFont    flag_font;
	CFont	*pflag_font;
	int      numeric;
	int		 base;
	char	 flag;
	char	 bell;
	char	 time;
	char	 othr;
	char	 quest;
	char	 space;
	char	 disk;
	bool	 m_bFlash;
	bool	 m_bFlashState;
	bool	 flag_flag;
	bool	 flag_bell;
	bool	 flag_time;
	bool	 flag_othr;
	bool	 flag_disk;
	bool	 flag_quest;
	bool	 m_bParticipateInNetwork;

	int		 m_dNetIndex;
	
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
//};

//#endif
/////////////////////////////////////////////////////////////////////////////
// CColorButton window

//class CColorButton : public CButton
//{
// Construction
//public:
//	CColorButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
#endif
/////////////////////////////////////////////////////////////////////////////
