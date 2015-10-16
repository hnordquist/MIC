//CJulianTime Class Header File

//DR 243: Correct processing in CJulian of year < 1982
//8 Mar 2006 pjm (most of changes are by dgp, pjm re-inserted MIC 1.907 changes)


#if !defined (CJULIANTIME)
#define CJULIANTIME

#define BASE_YEAR			1952
#define BASE_MONTH			1
#define BASE_DAY			1
#define BASE_HOUR			0
#define BASE_MINUTE			0
#define BASE_SECOND			0
#define HALF_MILLISECOND	(.5 / ( 1000. * 60. * 60. * 24.))
#define SECONDS_PER_YEAR	86400L
#define HALF_SECOND			5.7870370370370370e-6

class CJulianTime
{
public:
	CJulianTime(char *);
	CJulianTime(CString);   //MIC_CB_028 to MIC 1.907 PJM 1/20/05
	CJulianTime(unsigned int uiTime);
	CJulianTime(double dTime);
	CJulianTime();
	void Init(char*);
	void Init(unsigned Input);
	void Init(double Input);
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

	char	m_pDate[11];
	char	m_pTime[14];

protected:

	char	m_pCharacter[32];
	double	m_dDouble;
	bool	m_bIsValid;
	int		m_Year;
	int		m_Month;
	int		m_Day;
	int		m_Hour;
	int		m_Minute;
	int		m_Second;
	int		m_MSecond;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif