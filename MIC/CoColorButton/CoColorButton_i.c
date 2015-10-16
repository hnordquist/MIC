/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Nov 27 12:10:08 2007
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\CoColorButton\CoColorButton.idl:
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

const IID IID_ICB = {0xCE6E7E5C,0x77FA,0x433F,{0xAE,0x7E,0x86,0x84,0xCB,0xA4,0x60,0xF2}};


const IID LIBID_COCOLORBUTTONLib = {0x440D14EB,0x7014,0x48D8,{0xB9,0xCC,0xF8,0xA7,0x9A,0xA2,0xAF,0x7D}};


const CLSID CLSID_ColorButton = {0x2B1A162B,0x1FBC,0x49EC,{0xA9,0xC7,0x5D,0xE1,0x3A,0x2E,0x10,0x61}};


#ifdef __cplusplus
}
#endif

