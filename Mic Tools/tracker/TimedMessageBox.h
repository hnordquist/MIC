//TimedMessageBox.h

class CTimedMessageBox : public CWnd
{
public:
	CTimedMessageBox::CTimedMessageBox(
		const char *title,
		const char *message,
		int  seconds,
		CWnd *parent);

	CTimedMessageBox::CTimedMessageBox(
		CString& cstitle,
		CString& csmessage,
		int seconds,
		CWnd *parent);

	char m_cTitle[32];
	char m_cMessage[256];
	int  m_iTimeToGoInSeconds;
	UINT m_uTimerID;
	int  m_iResult;

	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	DECLARE_MESSAGE_MAP()
};