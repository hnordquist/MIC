/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Apr 16 18:44:13 2008
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\CoLOCNUTCSO\CoLOCNUTCSO.idl:
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


const IID LIBID_COLOCNUTCSOLib = {0xD740EA1D,0xE9B5,0x410f,{0x89,0x3D,0x9B,0x19,0xCB,0xBC,0xF7,0x86}};


const CLSID CLSID_LOCNUTCSO = {0x1DEF8026,0x4870,0x4d4b,{0x80,0xA2,0x21,0x74,0x3A,0x56,0xD5,0x58}};


#ifdef __cplusplus
}
#endif

