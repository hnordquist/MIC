// WinDumpDlg.cpp : implementation file
//
//shellexe
#include "stdafx.h"
#include <afxole.h>
#include "WinDump.h"
#include "WinDumpDlg.h"
#include "DlgProxy.h"
#include "cgfiltyp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VERSION "1.0.0.0"

#pragma pack(2)
struct AcquireRecord
{
	unsigned int JulianSeconds;	//4 bytes
	short	StatusByte;		//2 bytes
	float	NeutronChA;		//4 bytes
	float	NeutronChB;		//4 bytes
	float	NeutronChC;		//4 bytes
	float	GammaCh1;		//4 bytes
	float	GammaCh1Sigma;	//4 bytes
	float	GammaCh2;		//4 bytes
	float	GammaCh2Sigma;	//4 bytes
	short	CountTime;		//2 bytes
};
//pack structure at the byte level
#pragma pack(1)
struct AcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	//char	unused2[47];
	char FullYear[4];
	char unused2[43];
};
#pragma pack()

//pack structure at the byte level
#pragma pack(1)
struct MCAAcquireRecord
{
	unsigned int JulianSeconds;	//4 bytes
	unsigned char	StatusByte1;	//1 byte
	unsigned char	StatusByte2;	//1 byte
	unsigned char	Alarm;			//1 byte
	float	GrossCounts;	//4 bytes
	float	U235;			//4 bytes
	float	CS137;			//4 bytes
	float	U238;			//4 bytes
	float	Scaler;			//4 bytes
	float	U235Ratio;		//4 bytes
	float	CS137Ratio;		//4 bytes
	int		ElapsedTime;	//4 bytes
};
//pack structure at the byte level
#pragma pack(1)
struct MCAAcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
// or replace unused2 with...
//	char	Year4[4];
//	char	unused2[43];
};

#pragma pack(2)
struct ISRAcquireRecord
{
	unsigned int	JulianSeconds;	//4 bytes
	unsigned short	StatusByte;		//2 bytes
	double			TotalsCount;	//8 bytes
	double			Totals2Count;	//8 bytes
	double			Totals3Count;	//8 bytes
	double			RealAccCount;	//8 bytes
	double			AccidenCount;	//8 bytes
	double			ElapsedTime;	//8 bytes 1/10 sec increments
};

#pragma pack(1)
struct ISRAcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
// or replace unused2 with...
//	char	Year4[4];
//	char	unused2[43];
};
#pragma pack()

int CWinDumpDlg::ReadALine(FILE* infile, char *szLineBuff, int iBufSize)
{
	int iAt = 0;
	unsigned char ucC;
	while (!feof(infile) && (iAt < iBufSize-1))
	{
		ucC = fgetc(infile);
		szLineBuff[iAt] = ucC;
		if (ucC == '\n') 
			break;
		else
			iAt++;
	}
	szLineBuff[iAt] = NULL;
	iAt--;
	if (szLineBuff[iAt] == '\r')
		szLineBuff[iAt] = NULL;
	if (feof(infile)) iAt = -iAt;
	return iAt;
}


void CWinDumpDlg::DoIt(char* szFileName, Types eThisType)
{
	FILE* infile;

	AcquireHeader Header;
	AcquireRecord	 BIDRecord;
	ISRAcquireRecord ISRRecord;
	MCAAcquireRecord MCARecord;

	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	bool bCountTimeProvided = false;

	char szOutFile[_MAX_PATH];

	if ((infile = fopen(szFileName,"rt")) == NULL)
	{
		m_cMessage += "Cannot open input file!";
		return;
	}

	//read a line
	char ch = NULL;
	char szLineBuff[512];
	char *pszAt;
	int iIndex;
	if (ReadALine(infile,szLineBuff,512) > 0)
	{
		int iMonth = -1;
		int iDay = -1;
		int iYear = -1;
		int iHours = -1;
		int iMinutes = -1;
		int iSeconds = -1;
		int iIntervalSec = -1;
		int iStationID = -1;
		char szType[32];
		char szVersion[64];
		szType[0] = NULL;
		szVersion[0] = NULL;
		pszAt = strtok(szLineBuff,"\",");
		while (pszAt != NULL)
		{
			if (strcmp(pszAt,"date=")==0)
			{
				pszAt = strtok(NULL,"\", ");
				sscanf(pszAt,"%d/%d/%d",&iMonth,&iDay,&iYear);
			} 
			else if (strcmp(pszAt,"time bin interval=")==0)
			{
				//get the value
				pszAt = strtok(NULL,"\", ");
				sscanf(pszAt,"%d",&iIntervalSec);
				//get the multiplier "sec"
				pszAt = strtok(NULL,"\", ");
			}
			else if (strcmp(pszAt,"instrument number=")==0)
			{
				pszAt = strtok(NULL,"\", ");
				sscanf(pszAt,"%d",&iStationID);
			}
			else if (strcmp(pszAt,"instrument type=")==0)
			{
				pszAt = strtok(NULL,"\",");
				strcpy(szType,pszAt);
			}
			else if (strcmp(pszAt,"ISR")== 0)
			{
				strcpy(szType,pszAt);
				pszAt = strtok(NULL,"\",");
			}
			else if (strcmp(pszAt,"MCA")== 0)
			{
				strcpy(szType,pszAt);
				pszAt = strtok(NULL,"\",");
			}
			else if (strcmp(pszAt,"BID")== 0)
			{
				strcpy(szType,pszAt);
				pszAt = strtok(NULL,"\",");
			}
			else if (strcmp(pszAt,"JSR")== 0)
			{
				strcpy(szType,pszAt);
				pszAt = strtok(NULL,"\",");
			}
			else if (strcmp(pszAt,"ISR")== 0)
			{
				strcpy(szType,pszAt);
				pszAt = strtok(NULL,"\",");
			}
			else if (strcmp(pszAt,"PSR")== 0)
			{
				strcpy(szType,pszAt);
				pszAt = strtok(NULL,"\",");
			}
			else if (strcmp(pszAt,"version=")==0)
			{
				pszAt = strtok(NULL,"\",");
				strcpy(szVersion,pszAt);
			}
			//get the next token
			else
				pszAt = strtok(NULL,"\",");
		}
		bool bExit = false;
		if (iMonth < 1 || iMonth > 12 || iDay < 1 || iDay > 31 || iYear < 0 || iYear > 2100)
		{
			m_cMessage += "Invalid text file, line 1 date\r\n";
			bExit = true;
		}

		if (iIntervalSec < 1 || iIntervalSec > 100000)
		{
			m_cMessage += "Invalid text file, line 1 time bin interval\r\n";
			bExit = true;
		}
		if (iStationID < 0 || iStationID > 99)
		{
			m_cMessage += "Invalid text file, line 1 instrument number\r\n";
			bExit = true;
		}
		if (strlen(szType) == 0)
		{
			m_cMessage += "Invalid text file, line 1 type\r\n";
			bExit = true;
		}
		if (strlen(szVersion) == 0)
		{
			m_cMessage += "Invalid text file, line 1 version\r\n";
			bExit = true;
		}
		if (bExit)
			return;

		//get data columns names
		if (ReadALine(infile,szLineBuff,512) > 0)
		{
			//split path
			_splitpath(szFileName,drive,dir,fname,ext);
			//build new name
			_makepath(path_buffer,drive,dir,NULL,NULL);

			sprintf(szOutFile,"%s%02d%c%c%c%c%02d.%3s",
				path_buffer,	
				iStationID,
				((iYear-1990)<10)?
					((iYear-1990<0)?'#':'0'+(iYear-1990)):
					'A'+(iYear-2000),
				((iMonth<10)?
					'0'+(iMonth):
					'A'+(iMonth-10)),
				((iDay <10)?
					'0'+(iDay):
					'A'+(iDay-10)),
				'A',0,szType);

			m_cMessage += szOutFile;
			m_cMessage += "\r\n";

			//open output for append
			FILE *outfile = fopen(szOutFile,"ab");
			if (outfile == NULL)
			{
				m_cMessage += "Cannot open output file!\r\n";
				return;
			}
			else
			{
				//get file position
				fpos_t position;
				fseek(outfile,0,SEEK_END);
				if (fgetpos( outfile, &position )==0)
				{
					//if new file (e.g. no header there yet)
					if (position == 0)
					{
						//initialize header
						char temp[32];
						memset( &Header, ' ', sizeof(AcquireHeader));

						//fill header
						//size of header beyond first four bytes
						sprintf(temp,"%4d",sizeof(AcquireHeader)-4);
						memcpy(Header.SizeOfHeader,temp,4);

						memcpy(Header.Version,szVersion,5);

						sprintf(temp,"%3d",iStationID);
						memcpy(Header.StationId,temp,3);
						sprintf(temp,"%3d",iYear%100);
						memcpy(Header.Year,temp,3);
						sprintf(temp,"%3d",iMonth);
						memcpy(Header.Month,temp,3);
						sprintf(temp,"%3d",iDay);
						memcpy(Header.Day,temp,3);
						sprintf(temp,"%4d",iYear);
						memcpy(Header.unused2,temp,4);
						//write header
						fwrite(&Header,sizeof(AcquireHeader),1,outfile);
					}
				}

				//what order are the values in the table
				char* pszAt;
				//cross reference table
				int iCrossRef[9];
				//flag all as not used
				for (iIndex=0; iIndex < 8; iIndex++) iCrossRef[iIndex] = -1;
				pszAt = strtok(szLineBuff,"\",");
				int iStringIndex = 0;
				while (pszAt)
				{
					if (strcmp(szType,"ISR")==0) //JSR PSR
					{
						if (strcmp(pszAt,"Time")==0)
							iCrossRef[iStringIndex] = 0;
						else if (strcmp(pszAt,"Singles") == 0)
							iCrossRef[iStringIndex] = 1;
						else if (strcmp(pszAt,"AUX1") == 0)
							iCrossRef[iStringIndex] = 2;
						else if (strcmp(pszAt,"AUX2") == 0)
							iCrossRef[iStringIndex] = 3;
						else if (strcmp(pszAt,"Reals") == 0)
							iCrossRef[iStringIndex] = 4;
						else if (strcmp(pszAt,"Doubles") == 0)
							iCrossRef[iStringIndex] = 4;
						else if (strcmp(pszAt,"RealsAccidentals") == 0)
							iCrossRef[iStringIndex] = 4;
						else if (strcmp(pszAt,"Accidentals") == 0)
							iCrossRef[iStringIndex] = 5;
					}
					else if (strcmp(szType,"BID")==0)
					{
						if (strcmp(pszAt,"Time") == 0)
							iCrossRef[iStringIndex] = 7;
						else if (strcmp(pszAt,"NeutA") == 0)
							iCrossRef[iStringIndex] = 8;
						else if (strcmp(pszAt,"NeutB") == 0)
							iCrossRef[iStringIndex] = 9;
						else if (strcmp(pszAt,"NeutC") == 0)
							iCrossRef[iStringIndex] = 10;
						else if (strcmp(pszAt,"Gamma1") == 0)
							iCrossRef[iStringIndex] = 11;
						else if (strcmp(pszAt,"Gamma1s") == 0)
							iCrossRef[iStringIndex] = 12;
						else if (strcmp(pszAt,"Gamma2") == 0)
							iCrossRef[iStringIndex] = 13;
						else if (strcmp(pszAt,"Gamma2s") == 0)
							iCrossRef[iStringIndex] = 14;
						else if (strcmp(pszAt,"Duration") == 0)
						{
							bCountTimeProvided = true;
							iCrossRef[iStringIndex] = 15;
						}
					}
					else if (strcmp(pszAt,"MCA") == 0)
					{
						if (strcmp(pszAt,"Time") == 0)
							iCrossRef[iStringIndex] = 16;
						else if (strcmp(pszAt,"Counts") == 0)
							iCrossRef[iStringIndex] = 17;
						else if (strcmp(pszAt,"U235") == 0)
							iCrossRef[iStringIndex] = 18;
						else if (strcmp(pszAt,"CS137") == 0)
							iCrossRef[iStringIndex] = 19;
						else if (strcmp(pszAt,"U238") == 0)
							iCrossRef[iStringIndex] = 20;
						else if (strcmp(pszAt,"Scaler") == 0)
							iCrossRef[iStringIndex] = 21;
						else if (strcmp(pszAt,"U235Ratio") == 0)
							iCrossRef[iStringIndex] = 22;
						else if (strcmp(pszAt,"CS137Ratio") == 0)
							iCrossRef[iStringIndex] = 23;
					}
					pszAt = strtok(NULL,"\",");
					iStringIndex++;
				}
				bExit = true;
				for (iIndex = 0; iIndex < 8; iIndex++) 
					if (iCrossRef[iIndex] != -1) bExit = false;
				if (bExit)
				{
					m_cMessage += "Invalid text file, line 2 no data names\r\n";
					return;
				}
		
/*
varible mapping
TimeCountsU235CS137U238ScalerU235RatioCS137Ratio
TimeNeutANeutBGamma1Gamma1Gamma1sGamma2Gamma2s
bid file
	unsigned int JulianSeconds;	//4 bytes		7	//Time
	short	StatusByte;		//2 bytes
	float	NeutronChA;		//4 bytes			8	//NeutA
	float	NeutronChB;		//4 bytes			9	//NeutB
	float	NeutronChC;		//4 bytes			10	//NeutC
	float	GammaCh1;		//4 bytes			11	//Gamma1
	float	GammaCh1Sigma;	//4 bytes			12	//Gamma1s
	float	GammaCh2;		//4 bytes			13	//Gamma2
	float	GammaCh2Sigma;	//4 bytes			14	//Gamma2s
mca file
	unsigned int JulianSeconds;	//4 bytes		15	//Time
	unsigned char	StatusByte1;	//1 byte
	unsigned char	StatusByte2;	//1 byte
	unsigned char	Alarm;			//1 byte
	float	GrossCounts;	//4 bytes			16	//Counts
	float	U235;			//4 bytes			17	//U235
	float	CS137;			//4 bytes			18	//CS137
	float	U238;			//4 bytes			19	//U238
	float	Scaler;			//4 bytes			20	//Scaler
	float	U235Ratio;		//4 bytes			21	//U235Ratio
	float	CS137Ratio;		//4 bytes			22	//CS137Ratio
	int		ElapsedTime;	//4 bytes			
isr file

	unsigned int	JulianSeconds;	//4 bytes	0
	unsigned short	StatusByte;		//2 bytes
	double			TotalsCount;	//8 bytes	1
	double			Totals2Count;	//8 bytes	2
	double			Totals3Count;	//8 bytes	3
	double			RealAccCount;	//8 bytes	4
	double			AccidenCount;	//8 bytes	5
	double			ElapsedTime;	//8 bytes	6
			1/10 sec increments

*/
				//while not end of file
				bool bDayChange = false;
				double dTemp;
				int iPreviousHours = 0;
				COleDateTime cOffset(1952,1,1,0,0,0);
				COleDateTimeSpan cOneDay(1,0,0,0);
				COleDateTimeSpan cEpoc;
				COleDateTime cRecord;
				while (!feof(infile))
				{
					memset(&BIDRecord,NULL,sizeof(AcquireRecord));
					memset(&ISRRecord,NULL,sizeof(ISRAcquireRecord));
					memset(&MCARecord,NULL,sizeof(MCAAcquireRecord));
					//read a record
					if (ReadALine(infile,szLineBuff,512) > 0)
					{
						pszAt = strtok(szLineBuff,"\",");
						for (int iIndex = 0; iIndex < iStringIndex; iIndex++)
						{
							switch (iCrossRef[iIndex]) {
							case 0:
								//read seconds
								sscanf(pszAt,"%d:%d:%d",&iHours,&iMinutes,&iSeconds);
								cRecord = COleDateTime(iYear,iMonth,iDay,iHours,iMinutes,iSeconds);
								cEpoc = cRecord - cOffset;
TRACE("iHours: %d, iPerviousHours %d\r\n",iHours,iPreviousHours);
								if (iHours < iPreviousHours)
								{
									bDayChange = true;
									cEpoc += cOneDay;
									cRecord += cOneDay;
									iYear = cRecord.GetYear();
									iMonth = cRecord.GetMonth();
									iDay = cRecord.GetDay();
									iMinutes = cRecord.GetMinute();
									iSeconds = cRecord.GetSecond();
								}
								else
									bDayChange = false;
								iPreviousHours = iHours = cRecord.GetHour();
								//save seconds
								ISRRecord.JulianSeconds = (unsigned int)cEpoc.GetTotalSeconds();
								break;
							case 1:
								sscanf(pszAt,"%lf",&dTemp);//Singles
								ISRRecord.TotalsCount = dTemp * iIntervalSec;
								break;
							case 2:
								sscanf(pszAt,"%lf",&dTemp);//AUX1
								ISRRecord.Totals2Count = dTemp * iIntervalSec;
								break;
							case 3:
								sscanf(pszAt,"%lf",&dTemp);//AUX2
								ISRRecord.Totals3Count = dTemp * iIntervalSec;
								break;
							case 4:
								sscanf(pszAt,"%lf",&dTemp);//Reals or Doubles
								ISRRecord.RealAccCount = dTemp * iIntervalSec;
								break;
							case 5:
								sscanf(pszAt,"%lf",&dTemp);
								ISRRecord.AccidenCount = dTemp * iIntervalSec;
								break;
							//case 6:
							//	break;
							case 7:
								sscanf(pszAt,"%d:%d:%d",&iHours,&iMinutes,&iSeconds);
								cRecord = COleDateTime(iYear,iMonth,iDay,iHours,iMinutes,iSeconds);
								cEpoc = cRecord - cOffset;
								if (iHours < iPreviousHours)
								{
									bDayChange = true;
									cEpoc += cOneDay;
									cRecord += cOneDay;
									iYear = cRecord.GetYear();
									iMonth = cRecord.GetMonth();
									iDay = cRecord.GetDay();
									iPreviousHours = iHours = cRecord.GetHour();
									iMinutes = cRecord.GetMinute();
									iSeconds = cRecord.GetSecond();
								}
								else
									bDayChange = false;
								//save seconds
								BIDRecord.JulianSeconds = (unsigned int)cEpoc.GetTotalSeconds();
								break;
							case 8:
								sscanf(pszAt,"%f",&BIDRecord.NeutronChA);
								break;
							case 9:
								sscanf(pszAt,"%f",&BIDRecord.NeutronChB);
								break;
							case 10:
								sscanf(pszAt,"%f",&BIDRecord.NeutronChC);
								break;
							case 11:
								sscanf(pszAt,"%f",&BIDRecord.GammaCh1);
								break;
							case 12:
								sscanf(pszAt,"%f",&BIDRecord.GammaCh1Sigma);
								break;
							case 13:
								sscanf(pszAt,"%f",&BIDRecord.GammaCh2);
								break;
							case 14:
								sscanf(pszAt,"%f",&BIDRecord.GammaCh2Sigma);
								break;
							case 15:
								sscanf(pszAt,"%d",&BIDRecord.CountTime);
								break;
							case 16:
								sscanf(pszAt,"%d:%d:%d",&iHours,&iMinutes,&iSeconds);
								cRecord = COleDateTime(iYear,iMonth,iDay,iHours,iMinutes,iSeconds);
								cEpoc = cRecord - cOffset;
								if (iHours < iPreviousHours)
								{
									bDayChange = true;
									cEpoc += cOneDay;
									cRecord += cOneDay;
									iYear = cRecord.GetYear();
									iMonth = cRecord.GetMonth();
									iDay = cRecord.GetDay();
									iPreviousHours = iHours = cRecord.GetHour();
									iMinutes = cRecord.GetMinute();
									iSeconds = cRecord.GetSecond();
								}
								else
									bDayChange = false;
								//save seconds
								MCARecord.JulianSeconds = (unsigned int)cEpoc.GetTotalSeconds();
								break;
							case 17:
								sscanf(pszAt,"%f",&MCARecord.GrossCounts);
								break;
							case 18:
								sscanf(pszAt,"%f",&MCARecord.U235);
								break;
							case 19:
								sscanf(pszAt,"%f",&MCARecord.CS137);
								break;
							case 20:
								sscanf(pszAt,"%f",&MCARecord.U238);
								break;
							case 21:
								sscanf(pszAt,"%f",&MCARecord.Scaler);
								break;
							case 22:
								sscanf(pszAt,"%f",&MCARecord.U235Ratio);
								break;
							case 23:
								sscanf(pszAt,"%f",&MCARecord.CS137Ratio);
								break;
							}
							pszAt = strtok(NULL,",");
						}

						//if bDayChange then do a new file or move do file name down here
						if (bDayChange)
						{

							fclose(outfile);

							sprintf(szOutFile,"%s%02d%c%c%c%c%02d.%3s",
								path_buffer,	
								iStationID,
								((iYear-1990)<10)?
									((iYear-1990<0)?'#':'0'+(iYear-1990)):
									'A'+(iYear-2000),
								((iMonth<10)?
									'0'+(iMonth):
									'A'+(iMonth-10)),
								((iDay <10)?
									'0'+(iDay):
									'A'+(iDay-10)),
								'A',0,szType);

							m_cMessage += "\r\n";
							m_cMessage += szOutFile;
							m_cMessage += "\r\n";

							//open output for append
							outfile = fopen(szOutFile,"ab");
							if (outfile == NULL)
							{
								m_cMessage += "Cannot open output file!\r\n";
								return;
							}
							else
							{
								//get file position
								fpos_t position;
								fseek(outfile,0,SEEK_END);
								if (fgetpos( outfile, &position )==0)
								{
									//if new file (e.g. no header there yet)
									if (position == 0)
									{
										//initialize header
										char temp[32];
										memset( &Header, ' ', sizeof(AcquireHeader));

										//fill header
										//size of header beyond first four bytes
										sprintf(temp,"%4d",sizeof(AcquireHeader)-4);
										memcpy(Header.SizeOfHeader,temp,4);

										memcpy(Header.Version,szVersion,5);

										sprintf(temp,"%3d",iStationID);
										memcpy(Header.StationId,temp,3);
										sprintf(temp,"%3d",iYear%100);
										memcpy(Header.Year,temp,3);
										sprintf(temp,"%3d",iMonth);
										memcpy(Header.Month,temp,3);
										sprintf(temp,"%3d",iDay);
										memcpy(Header.Day,temp,3);
										sprintf(temp,"%4d",iYear);
										memcpy(Header.unused2,temp,4);
										//write header
										fwrite(&Header,sizeof(AcquireHeader),1,outfile);
									}
								}
							}
						}
						//switch on type
						//case JSR
						//case ISR
						//case PSR
						if (strcmp(szType,"JSR")==0 || 
							strcmp(szType,"ISR")==0 ||
							strcmp(szType,"PSR")==0)
						{
							//write a record
							ISRRecord.RealAccCount += ISRRecord.AccidenCount;
							ISRRecord.ElapsedTime = iIntervalSec;
							fwrite(&ISRRecord,sizeof(ISRAcquireRecord),1,outfile);
						}
						else if (strcmp(szType,"BID")==0)
						{
						//case BID
							//write a record
							if (!bCountTimeProvided)
								BIDRecord.CountTime = iIntervalSec;
							fwrite(&BIDRecord,sizeof(AcquireRecord),1,outfile);
						}
						else if (strcmp(szType,"MCA")==0)
						{
						//case MCA
							//write a record
							MCARecord.ElapsedTime = iIntervalSec;
							fwrite(&MCARecord,sizeof(MCAAcquireRecord),1,outfile);
						}
					}
				}
			}
			//close output file
			fclose(outfile);
		}
	}
	//close input file
	fclose(infile);
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlg dialog

IMPLEMENT_DYNAMIC(CWinDumpDlg, CDialog);

CWinDumpDlg::CWinDumpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinDumpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinDumpDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CWinDumpDlg::~CWinDumpDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CWinDumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinDumpDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWinDumpDlg, CDialog)
	//{{AFX_MSG_MAP(CWinDumpDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinDumpDlg message handlers

BOOL CWinDumpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	DragAcceptFiles( TRUE );

	char *pszAt;
//	if ((szAt = strtok(m_szCmdLine,"\""))!= NULL) //find first quote
//	{
//		if ((szAt = strtok(NULL,"\""))	!= NULL) //find second quote
//		{
//			if ((szAt = strtok(NULL,"\"")) != NULL) //find start of second parameter
//			{
//				DoAFile(szAt);
//				EndDialog(0);
//			}
//		}
//	}

	if ((pszAt = strstr(m_szCmdLine,"file=")) != NULL)
	{
		pszAt += 5;
		while ((pszAt[0] == '"') || (pszAt[0] == ' '))
			pszAt++;
		if (pszAt[strlen(pszAt)-1] == '"')
			pszAt[strlen(pszAt)-1] = NULL;
		DoAFile(pszAt);
		SetTimer(1000,4000,NULL);
		SetDlgItemText(IDC_EDIT_LOG,m_cMessage);
	}
	else if (strlen(m_szCmdLine) > 0)
	{
		DoAFile(m_szCmdLine);
		SetDlgItemText(IDC_EDIT_LOG,m_cMessage);
	}
	else
	{
		CString cTemp;

		cTemp += "CSV File Format:\r\n"
			"Line 1: date=,4/20/2006,time bin interval=,10,secs,instrument number=,99,ISR,version=,1.5MCA May 2006\r\n"
			"Where \"ISR\" may be replaced with one of ISR, JSR, PSR, BID, or MCA\r\n\r\n"
			"Line 2: Contains comma delimited names of data variables in following lines (no spaces).\r\n"
			"To generate an ISR file line 2 may be any combination or order of: \r\n"
			"Time, Singles, AUX1, AUX2, Reals or Doubles or RealsAccidentals, Accidentals\r\n"
			"To generate a BID file line 2 may be any combination or order of:\r\n"
			"Time, NeutA, NeutB, Gamma1, Gamma1s, Gamma2, Gamma2s, Duration\r\n"
			"To generate a MCA file line 2 may be any combination or order of:\r\n"
			"Time, Counts, U235, CS137, U238, Scaler, U235Ratio, CS137Ratio\r\n\r\n"
			"Line 3 to end of file contain the data.  There must be as many comma delimited\r\n"
			"data elements in each line as indicated in line 2";
		SetDlgItemText(IDC_EDIT_LOG,cTemp);
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWinDumpDlg::OnDropFiles(HDROP hDropInfo)
{
    TCHAR szFileName[_MAX_PATH + 1];

    if (NULL != hDropInfo)
    {
        UINT nFiles = DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

		m_cMessage.Empty();

        for(UINT nNames = 0; nNames < nFiles; nNames++)
        {
            ZeroMemory(szFileName, _MAX_PATH + 1);
            DragQueryFile
                (hDropInfo, nNames, (LPTSTR)szFileName, _MAX_PATH + 1);

			DoAFile(szFileName);

			SetDlgItemText(IDC_EDIT_LOG,m_cMessage);

        }
	}
}

void CWinDumpDlg::DoAFile(char*  szFileName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(szFileName, drive, dir, fname, ext);
	_strupr(ext);
	if (strcmp(ext,".CSV")==0)
	{
		m_cMessage += "Translating ";
		m_cMessage += szFileName;
		m_cMessage += "\r\nto\r\n";
		SetDlgItemText(IDC_EDIT_LOG,m_cMessage);
		DoIt(szFileName,eThisType);
	}
	else
	{
		m_cMessage += "Improper file type: ";
		m_cMessage += szFileName;
		m_cMessage += "\r\nNeed file extension .CSV";
		SetDlgItemText(IDC_EDIT_LOG,m_cMessage);
	}
}

void CWinDumpDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWinDumpDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWinDumpDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CWinDumpDlg::OnClose() 
{
	if (CanExit())
		CDialog::OnClose();
}

void CWinDumpDlg::OnOK() 
{
	if (CanExit())
		CDialog::OnOK();
}

void CWinDumpDlg::OnCancel() 
{
	if (CanExit())
		CDialog::OnCancel();
}

BOOL CWinDumpDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}


void CWinDumpDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1000)
	{
		KillTimer(1000);
		EndDialog(0);
	}
	CDialog::OnTimer(nIDEvent);
}
