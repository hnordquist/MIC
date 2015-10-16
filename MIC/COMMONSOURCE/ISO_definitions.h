// ISO_definitions.h

#ifndef ISO_DEFINITIONS_H
#define ISO_DEFINITIONS_H

#define RFIDDEMO
//#undef RFIDDEMO


#define SLOWRESET       900000

#define CHECKSUM_GOOD   1
#define CHECKSUM_BAD    0

#define TYPE_GID2		1
#define TYPE_INST		2
#define TYPE_COMP		3
#define TYPE_TIME		4
#define	TYPE_INVTIME	5
#define TYPE_START		6
#define TYPE_DELETE		7
#define TYPE_DUMP		8
#define TYPE_ABNORMAL	9
#define TYPE_NEWDAY		10

#define TO_CEV			0x00000001
#define TO_PFM			0x00000002
#define TO_DMP			0x00000004

#define BOMB_CHAR      'M'
#define SKULL_CHAR     'N'
#define FLAG_CHAR      'O'
#define CHICK_CHAR     'q'  //MIC_CB_028 to MIC 1.907 PJM 1/19/05

#define OOBFLAG			0x00010000

#define TIMER_PAUSE				1
#define TIMER_TICK				2
//#define TIMER_UPDATE			4
#define TIMER_CAMERA_START		5
#define TIMER_CAMERA_END		6
#define TIMER_EXCLUSION			7
#define TIMER_ANALYSIS_DWELL	8
#define TIMER_REALTIME_DWELL	9
#define TIMER_AUTOCLOSE			10

#define COLLECTINGBEGIN			"Begin\nCollecting"
#define COLLECTINGPAUSE			"Pause\nCollecting"

#define BUMP_EXTEND	300
#define MAX_EXTEND 7200

#define CEV_SUFFIX		"CEV"
#define PFM_SUFFIX		"PFM"
#define DMP_SUFFIX		"DMP"
#define BID_SUFFIX		"BID"
#define ISR_SUFFIX		"ISR"
#define JSR_SUFFIX		"JSR"
#define MCA_SUFFIX		"MCA"
#define BNY_SUFFIX		"BNY"
#define GPS_SUFFIX		"GPS"
#define VCS_SUFFIX		"VCS"
#define EVT_SUFFIX		"EVT"
#define CHN_SUFFIX		"CHN"
#define MGD_SUFFIX		"MGD" //microGrand
#define SOH_SUFFIX		"SOH"
#define PSR_SUFFIX		"PSR"
#define HMR_SUFFIX		"HMR"
#define ESS_SUFFIX		"ESS"
#ifdef RFIDDEMO
#define RFI_SUFFIX		"RFI"
#define RAD_SUFFIX		"RAD"
#endif
#define ID_CLOSE_BUTTON                 9000
#define ID_PAUSE_BUTTON                 9001
#define ID_PRINT_BUTTON                 9002
#define ID_ACKFL_BUTTON                 9003
#define ID_TOCLOSE                      9004

//<<QA>> put this somewhere else eventually.
#define IDC_EXTENDCLOSE		35999    //MIC_CB_028 to MIC 1.907 PJM 1/19/05


#endif //ISO_DEFINITIONS_H