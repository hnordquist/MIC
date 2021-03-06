/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
   conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
   conditions and the following disclaimer in the documentation and/or other materials provided
   with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
   may be used to endorse or promote products derived from this software without specific prior
   written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
// globals.h 

//DR00200 - Error messages are needed when installation is not as expected.
//DR00200 - Added global registration anomoly message pjm 13-Feb-2006 

#ifndef GLOBALS_H
#define GLOBALS_H

//the value of 128 only allows for 64 instruments with watchers
//one site may have as many as approximately 72 instruments
#define MAX_INSTRUMENTS			256  //(was 50) MIC_CB_028 to MIC 1.907 PJM 1/20/05 DGP changed to 256 17Oct06
#define MAX_COMM				256  //(was 32) MIC_CB_028 to MIC 1.907 PJM 1/20/05 DGP changed to 256 17Oct06

#include "stdafx.h"

#include "..\Interfaces\ICB.h"
#include "..\Interfaces\ICSO.h"
#include "..\Interfaces\IISO.h"

#include "ccbutton.h"

/////////////////////////////////////////////////////////////////////////////
// Global Data

extern CString g_csVersion;
extern CString g_csNameVersionDate; //SCR 118

extern char    g_szIniFile[_MAX_PATH];               //configuration file
extern char    g_szClsidIniFile[_MAX_PATH];
extern int     g_iInstCount;                         //how many instruments
extern int     g_iCommCount;                         //how many comm objects
extern int     g_iDlgCloseMillisec;                  //autodialog close at 
extern char    g_szUtilPath[_MAX_PATH];
extern bool    g_bHideWindows;                       //minimize the number of windows on screen
extern bool    g_blogarchiveact;                     //logarchive actions
extern bool    g_bNoPauseRestart;   //fast restart -- instruments not paused
extern bool    g_bUseShortName;                      //use short form (8.3) file name convention
extern CString g_csLongName;                          //form of the long name
extern int     g_iButtonColumns;                       //number of button columns to instantiate
extern int     g_iButtonWidth;                       //width of an instrument button
extern int     g_iButtonHeight;                      //height of an instrument button
extern int     g_iButtonFlagSize;                    //height of an instrument button's flag
extern float   g_fMaxPercentDiskUse;                        //disk usage percentage
extern int     g_iMinutesToCheckArchive;
extern int     g_iDaysUntilDeleteFiles;                          //days until del files
extern char    g_sFacilityName[64];                  //facility name
extern bool    g_bDoingNetwork;                      //transmit button data to network addresses
extern int     g_iMaxRepetition;                     //maximum repetition of selected commands in the pfm file
extern bool    g_bInIAEA;
extern char    g_pVal_Name[64];
extern COleDateTime g_cLastGoodValidate;

extern CColorButton*  g_pColorButton[MAX_INSTRUMENTS];

extern IClassFactory *g_pCoColorButtonFactory;
extern ISO_FUNCTIONAL_CATEGORY g_eISOCategory[MAX_INSTRUMENTS];
extern IISO*          g_pIISO[MAX_INSTRUMENTS];
extern ICB*           g_pICB[MAX_INSTRUMENTS];
extern ICSO*          g_pICSO[MAX_COMM];

extern CString m_csRegistrationAnomalyMessage; // DR00200
extern CString g_csColorButtonVersionString;	//SCR00161

#endif // GLOBALS_H