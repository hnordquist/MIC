// JulianT.h

#if !defined (CJULIANTIME)
#define CJULIANTIME

#define BASE_YEAR			1952
#define BASE_OFFSET_DAYS	(-63.)
#define HALF_MILLISECOND	(.5 / ( 1000. * 60. * 60. * 24.))
#define SECONDS_PER_YEAR	86400

class CJulianTime
{
public:
	CJulianTime(char *);
	CJulianTime(CString);   //MIC_CB_028 to MIC 1.907 PJM 1/20/05
	CJulianTime();
	void Init(char*);
	void Init(unsigned Input);
	void FinishInit();
	int Year();
	int Month();
	int Day();
	int Hour();
	int Minute();
	int Second();
	unsigned int TotalSeconds();
	char *Date();
	char *Time();
	bool IsValid();
	~CJulianTime();
	// ClassWizard generate virtual function overrides
	char	m_pDate[11];
	char	m_pTime[14];
protected:
	char	m_pCharacter[32];
	double	m_dDouble;
//	_i64	m_I64;
	bool	m_bIsValid;
	int		m_Year;
	int		m_Month;
	int		m_Day;
	int		m_Hour;
	int		m_Minute;
	int		m_Second;
	int		m_MSecond;
	unsigned int m_dTotalSeconds;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif