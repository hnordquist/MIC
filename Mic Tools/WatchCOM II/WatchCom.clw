; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWatchComDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "WatchCom.h"

ClassCount=4
Class1=CWatchComApp
Class2=CWatchComDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_WATCHCOM_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_DIALOG1
Class4=CSelect
Resource4=IDD_ABOUTBOX
Resource5=IDD_DATAWATCH_DIALOG

[CLS:CWatchComApp]
Type=0
HeaderFile=WatchCom.h
ImplementationFile=WatchCom.cpp
Filter=N

[CLS:CWatchComDlg]
Type=0
HeaderFile=WatchComDlg.h
ImplementationFile=WatchComDlg.cpp
Filter=D
LastObject=IDC_BUTTON4
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=WatchComDlg.h
ImplementationFile=WatchComDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_WATCHCOM_DIALOG]
Type=1
Class=CWatchComDlg
ControlCount=12
Control1=IDC_STATIC_COMA,static,1342181633
Control2=IDC_STATIC_COMB,static,1342181633
Control3=IDC_RICHEDIT1,RICHEDIT,1353713860
Control4=IDC_STATIC_COMC,static,1342181633
Control5=IDC_STATIC_COMD,static,1342181633
Control6=IDC_BUTTON1,button,1342242880
Control7=IDC_BUTTON2,button,1342242880
Control8=IDC_BUTTON3,button,1342242880
Control9=IDC_BUTTON4,button,1342242880
Control10=IDC_CHECK1,button,1342242819
Control11=IDC_EDIT1,edit,1350639744
Control12=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG1]
Type=1
Class=CSelect
ControlCount=60
Control1=IDC_DESTINATIONNAME_EDIT,edit,1350631552
Control2=IDC_BUTTON_BROWSE,button,1342242816
Control3=IDC_EDIT_LONGNAME,edit,1350631552
Control4=IDC_RADIO_BAUD300,button,1342308361
Control5=IDC_RADIO_BAUD600,button,1342177289
Control6=IDC_RADIO_BAUD1200,button,1342177289
Control7=IDC_RADIO_BAUD1800,button,1342177289
Control8=IDC_RADIO_BAUD2400,button,1342177289
Control9=IDC_RADIO_BAUD4800,button,1342177289
Control10=IDC_RADIO_BAUD7200,button,1342177289
Control11=IDC_RADIO_BAUD9600,button,1342177289
Control12=IDC_RADIO_BAUD14400,button,1342177289
Control13=IDC_RADIO_BAUD19200,button,1342177289
Control14=IDC_RADIO_BAUD38400,button,1342177289
Control15=IDC_RADIO_BAUD56000,button,1342177289
Control16=IDC_RADIO_BAUD57600,button,1342177289
Control17=IDC_RADIO_BAUD115200,button,1342177289
Control18=IDC_RADIO_BAUD128000,button,1342177289
Control19=IDC_RADIO15,button,1342308361
Control20=IDC_RADIO16,button,1342177289
Control21=IDC_RADIO17,button,1342177289
Control22=IDC_RADIO18,button,1342177289
Control23=IDC_RADIO19,button,1342177289
Control24=IDC_RADIO20,button,1342308361
Control25=IDC_RADIO21,button,1342177289
Control26=IDC_RADIO22,button,1342177289
Control27=IDC_LIST1,listbox,1352728835
Control28=IDC_RADIO23,button,1342308361
Control29=IDC_RADIO24,button,1342177289
Control30=IDC_RADIO25,button,1342177289
Control31=IDC_RADIO26,button,1342177289
Control32=IDOK,button,1342242817
Control33=IDCANCEL,button,1342242816
Control34=IDC_STATIC,button,1342177287
Control35=IDC_STATIC,button,1342177287
Control36=IDC_STATIC,button,1342177287
Control37=IDC_STATIC,button,1342177287
Control38=IDC_STATIC,button,1342177287
Control39=IDC_STATIC,button,1342177287
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,static,1342308352
Control42=IDC_STATIC,static,1342308352
Control43=IDC_STATIC_EXAMPLE,static,1342312448
Control44=IDC_STATIC,static,1342308352
Control45=IDC_STATIC,static,1342308352
Control46=IDC_STATIC,static,1342308352
Control47=IDC_STATIC,static,1342308352
Control48=IDC_STATIC,static,1342308352
Control49=IDC_STATIC,static,1342308352
Control50=IDC_STATIC,static,1342308352
Control51=IDC_STATIC,static,1342308352
Control52=IDC_STATIC,static,1342308352
Control53=IDC_STATIC,button,1342177287
Control54=IDC_STATIC,button,1342177287
Control55=IDC_IPADDRESS1,SysIPAddress32,1342242816
Control56=IDC_STATIC,static,1342308352
Control57=IDC_STATIC,static,1342308352
Control58=IDC_EDIT_TCPPORTNUMBER,edit,1350639744
Control59=IDC_BUTTON_DEACTIVATE,button,1342242816
Control60=IDC_STATIC,static,1342308352

[CLS:CSelect]
Type=0
HeaderFile=Select.h
ImplementationFile=Select.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSelect

[DLG:IDD_DATAWATCH_DIALOG]
Type=1
Class=?
ControlCount=51
Control1=IDC_CHART1V_MAX,static,1342308354
Control2=IDC_CHART1V_MIN,static,1342308354
Control3=IDC_CHART2V_MAX,static,1342308354
Control4=IDC_CHART2V_MIN,static,1342308354
Control5=IDC_CHART3V_MAX,static,1342308354
Control6=IDC_CHART3V_MIN,static,1342308354
Control7=IDC_CHART1H_MAX,static,1342308354
Control8=IDC_CHART1H_MIN,static,1342308352
Control9=IDC_CHART2H_MAX,static,1342308354
Control10=IDC_CHART2H_MIN,static,1342308352
Control11=IDC_CHART3H_MAX,static,1342308354
Control12=IDC_CHART3H_MIN,static,1342308352
Control13=IDC_STATIC_CHART1,static,1342308353
Control14=IDC_STATIC_CHART2,static,1342308353
Control15=IDC_STATIC_CHART3,static,1342308353
Control16=IDC_STATIC_CHART4,static,1342308353
Control17=IDC_CHECK_LOCKED,button,1342242819
Control18=IDC_STATIC_CHART1B,static,1342308353
Control19=IDC_STATIC_CHART2B,static,1342308353
Control20=IDC_STATIC_CHART3B,static,1342308353
Control21=IDC_CHART4V_MAX,static,1342308354
Control22=IDC_CHART4V_MIN,static,1342308354
Control23=IDC_CHART5V_MAX,static,1342308354
Control24=IDC_CHART5V_MIN,static,1342308354
Control25=IDC_CHART4H_MAX,static,1342308354
Control26=IDC_CHART5H_MAX,static,1342308354
Control27=IDC_CHART4H_MIN,static,1342308352
Control28=IDC_CHART5H_MIN,static,1342308352
Control29=IDC_STATIC_CHART5,static,1342308353
Control30=IDC_STATIC_CHART4B,static,1342308353
Control31=IDC_STATIC_CHART5B,static,1342308353
Control32=IDC_BUTTON_EXPAND,button,1342242816
Control33=IDC_BUTTON_CONTRACT,button,1342242816
Control34=IDC_STATIC_CHART6,static,1342308353
Control35=IDC_STATIC_CHART7,static,1342308353
Control36=IDC_STATIC_CHART8,static,1342308353
Control37=IDC_CHART6H_MIN,static,1342308352
Control38=IDC_CHART7H_MIN,static,1342308352
Control39=IDC_CHART8H_MIN,static,1342308352
Control40=IDC_CHART6H_MAX,static,1342308354
Control41=IDC_CHART7H_MAX,static,1342308354
Control42=IDC_CHART8H_MAX,static,1342308354
Control43=IDC_CHART6V_MAX,static,1342308354
Control44=IDC_CHART6V_MIN,static,1342308354
Control45=IDC_CHART7V_MAX,static,1342308354
Control46=IDC_CHART7V_MIN,static,1342308354
Control47=IDC_CHART8V_MAX,static,1342308354
Control48=IDC_CHART8V_MIN,static,1342308354
Control49=IDC_STATIC_CHART6B,static,1342308353
Control50=IDC_STATIC_CHART7B,static,1342308353
Control51=IDC_STATIC_CHART8B,static,1342308353

