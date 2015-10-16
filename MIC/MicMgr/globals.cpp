// globals.cpp

//DR00200 - Error messages are needed when installation is not as expected.
//DR00200 - Added global registration anomoly message pjm 13-feb-2006 

#include "stdafx.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Global Data

CString	g_csVersion;
CString g_csNameVersionDate; //SCR 118

char    g_szIniFile[_MAX_PATH];               //configuration file
char	g_szClsidIniFile[_MAX_PATH];		  //CLSID file for DLL's
int     g_iInstCount;                         //how many instruments
int     g_iCommCount;                         //how many comm objects
int     g_iDlgCloseMillisec;                  //autodialog close at 
char    g_szUtilPath[_MAX_PATH];
bool    g_bHideWindows;                       //minimize the number of windows on screen
bool    g_blogarchiveact;                     //logarchive actions
bool    g_bNoPauseRestart;                    //fast restart -- instruments not paused
bool    g_bUseShortName;                      //use short form (8.3) file name convention
CString g_csLongName;                          //form of the long name
int     g_iButtonColumns;                       //number of button columns to instantiate
int     g_iButtonWidth;                       //width of an instrument button
int     g_iButtonHeight;                      //height of an instrument button
int     g_iButtonFlagSize;                    //height of an instrument button's flag
float   g_fMaxPercentDiskUse;                        //disk usage percentage
int     g_iMinutesToCheckArchive;
int     g_iDaysUntilDeleteFiles;                          //days until del files
char    g_sFacilityName[64];                  //facility name
bool    g_bDoingNetwork;                      //transmit button data to network addresses
int     g_iMaxRepetition;                     //maximum repetition of selected commands in the pfm file
bool    g_bInIAEA;
char    g_pVal_Name[64];
COleDateTime g_cLastGoodValidate;

CColorButton*             g_pColorButton[MAX_INSTRUMENTS];

IClassFactory            *g_pCoColorButtonFactory;
ISO_FUNCTIONAL_CATEGORY   g_eISOCategory[MAX_INSTRUMENTS];
IISO*                     g_pIISO[MAX_INSTRUMENTS];
ICB*                      g_pICB[MAX_INSTRUMENTS];
ICSO*                     g_pICSO[MAX_COMM];

CString m_csRegistrationAnomalyMessage; // DR00200
CString g_csColorButtonVersionString;
