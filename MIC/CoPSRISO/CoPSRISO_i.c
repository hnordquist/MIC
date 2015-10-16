/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Jun 15 10:08:50 2012
 */
/* Compiler settings for E:\N-Legacy\trunk\UNARM\MIC DAQ\MIC\CoPSRISO\CoPSRISO.idl:
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


const IID LIBID_COPSRISOLib = {0xFC64E5D5,0x1A8A,0x49b5,{0x94,0x78,0x8E,0xF1,0x95,0x8E,0xE3,0x19}};


const CLSID CLSID_PSRISO = {0x07FCAEB4,0x073A,0x40a2,{0x87,0xFD,0x35,0x1A,0xE0,0x86,0x12,0x21}};


#ifdef __cplusplus
}
#endif

