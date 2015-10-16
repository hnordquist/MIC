/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Nov 27 12:12:27 2007
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\CoMCAISO\CoMCAISO.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IISO = {0xC1AD5FF6,0xB607,0x4454,{0x9E,0x7A,0x03,0x52,0x35,0xD4,0x95,0x91}};


const IID LIBID_COMCAISOLib = {0xB94956E1,0xE605,0x4A13,{0xB0,0x5E,0xFD,0x3A,0x92,0x62,0x7F,0xC3}};


const CLSID CLSID_MCAISO = {0x462A7ED9,0x50F6,0x4208,{0xBE,0x1F,0xCD,0x5A,0x66,0xCD,0x1F,0x22}};


#ifdef __cplusplus
}
#endif

