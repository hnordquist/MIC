/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Nov 27 12:11:36 2007
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\CoILONCSO\CoILONCSO.idl:
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

const IID IID_ICSO = {0xB7180771,0x895F,0x417E,{0x82,0x39,0x35,0x3E,0xE0,0x9F,0x70,0x09}};


const IID LIBID_COILONCSOLib = {0x973132C6,0x2A0B,0x44DD,{0x93,0xDC,0x7A,0x88,0x69,0xF4,0xAD,0xBE}};


const CLSID CLSID_ILONCSO = {0xAEE74781,0x06DC,0x4928,{0x8E,0x8F,0xD1,0xE2,0x81,0xCF,0x27,0xAD}};


#ifdef __cplusplus
}
#endif

