/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Feb 13 16:14:59 2008
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\CoDSPECISO\CoDSPECISO.idl:
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


const IID LIBID_CODSPECISOLib = {0xB5D835F6,0x7ADF,0x4BB7,{0x88,0xD5,0x73,0xE2,0x7C,0xA8,0x95,0x80}};


const CLSID CLSID_DSPECISO = {0x6AC4B463,0xA2F3,0x408A,{0xB7,0x7D,0x72,0x5E,0x67,0xAF,0x18,0xB8}};


#ifdef __cplusplus
}
#endif

