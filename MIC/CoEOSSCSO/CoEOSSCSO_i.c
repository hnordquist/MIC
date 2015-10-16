/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Mar 26 13:47:42 2008
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\CoEOSSCSO\CoEOSSCSO.idl:
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


const IID LIBID_COEOSSCSOLib = {0x81AA1A65,0x55A7,0x41c4,{0xB1,0x64,0xB0,0x3F,0x7D,0xFF,0xD8,0x3E}};


const CLSID CLSID_EOSSCSO = {0x6710F5EB,0x794B,0x485b,{0x94,0x5E,0x34,0x5E,0xBB,0xA3,0xFF,0xC3}};


#ifdef __cplusplus
}
#endif

