

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Mar 15 16:02:32 2012
 */
/* Compiler settings for .\src\ATSAlarmBridgeUtility.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

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

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IATSAlarmActionManager,0x4FE13D05,0x55DA,0x4320,0xBC,0x06,0x6A,0x4F,0xA6,0x14,0x8A,0xB7);


MIDL_DEFINE_GUID(IID, LIBID_ATSAlarmBridgeUtilityLib,0x90318F9E,0x3C96,0x497E,0xBB,0x7C,0xBC,0x5D,0x72,0x47,0xB8,0x81);


MIDL_DEFINE_GUID(IID, DIID__IATSAlarmActionManagerEvents,0xB13691A1,0x614C,0x4B5F,0xB8,0x21,0xEA,0xF1,0x5C,0x70,0x6B,0x66);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSAlarmActionManager,0xE8A7F762,0x8797,0x4E89,0x81,0xE3,0x11,0xC0,0x80,0x64,0x23,0x87);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



