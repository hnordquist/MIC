/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Nov 27 12:13:05 2007
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\CoWatchISO\CoWatchISO.idl:
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


const IID LIBID_COWATCHISOLib = {0xE35BFFE4,0xBB7F,0x4D18,{0x9D,0x5F,0x3B,0x29,0x22,0xAA,0x8F,0xCE}};


const CLSID CLSID_WatchISO = {0xFA40B144,0xC1F8,0x4e4b,{0xA7,0x29,0xF4,0x26,0x11,0x95,0xC4,0xC7}};


#ifdef __cplusplus
}
#endif

