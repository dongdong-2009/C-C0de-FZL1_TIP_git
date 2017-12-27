

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Dec 26 20:31:22 2017
 */
/* Compiler settings for .\src\RippleControlStation.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __RippleControlStation_i_h__
#define __RippleControlStation_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISchematicDisplay_FWD_DEFINED__
#define __ISchematicDisplay_FWD_DEFINED__
typedef interface ISchematicDisplay ISchematicDisplay;
#endif 	/* __ISchematicDisplay_FWD_DEFINED__ */


#ifndef ___ISchematicDisplayEvents_FWD_DEFINED__
#define ___ISchematicDisplayEvents_FWD_DEFINED__
typedef interface _ISchematicDisplayEvents _ISchematicDisplayEvents;
#endif 	/* ___ISchematicDisplayEvents_FWD_DEFINED__ */


#ifndef __SchematicDisplay_FWD_DEFINED__
#define __SchematicDisplay_FWD_DEFINED__

#ifdef __cplusplus
typedef class SchematicDisplay SchematicDisplay;
#else
typedef struct SchematicDisplay SchematicDisplay;
#endif /* __cplusplus */

#endif 	/* __SchematicDisplay_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ISchematicDisplay_INTERFACE_DEFINED__
#define __ISchematicDisplay_INTERFACE_DEFINED__

/* interface ISchematicDisplay */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISchematicDisplay;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("63E6413C-1849-4259-9791-C0F330388B23")
    ISchematicDisplay : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE displayAtsSchematic( 
            BSTR filePath,
            long leftPosition) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getCurrentSessionId( 
            /* [retval][out] */ BSTR *sessionId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getCurrentProfile( 
            /* [retval][out] */ BSTR *profile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getCurrentLocation( 
            /* [retval][out] */ BSTR *location) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getCurrentUserName( 
            /* [retval][out] */ BSTR *userName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISchematicDisplayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchematicDisplay * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchematicDisplay * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchematicDisplay * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchematicDisplay * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchematicDisplay * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchematicDisplay * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchematicDisplay * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *displayAtsSchematic )( 
            ISchematicDisplay * This,
            BSTR filePath,
            long leftPosition);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getCurrentSessionId )( 
            ISchematicDisplay * This,
            /* [retval][out] */ BSTR *sessionId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getCurrentProfile )( 
            ISchematicDisplay * This,
            /* [retval][out] */ BSTR *profile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getCurrentLocation )( 
            ISchematicDisplay * This,
            /* [retval][out] */ BSTR *location);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getCurrentUserName )( 
            ISchematicDisplay * This,
            /* [retval][out] */ BSTR *userName);
        
        END_INTERFACE
    } ISchematicDisplayVtbl;

    interface ISchematicDisplay
    {
        CONST_VTBL struct ISchematicDisplayVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISchematicDisplay_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISchematicDisplay_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISchematicDisplay_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISchematicDisplay_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISchematicDisplay_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISchematicDisplay_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISchematicDisplay_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISchematicDisplay_displayAtsSchematic(This,filePath,leftPosition)	\
    ( (This)->lpVtbl -> displayAtsSchematic(This,filePath,leftPosition) ) 

#define ISchematicDisplay_getCurrentSessionId(This,sessionId)	\
    ( (This)->lpVtbl -> getCurrentSessionId(This,sessionId) ) 

#define ISchematicDisplay_getCurrentProfile(This,profile)	\
    ( (This)->lpVtbl -> getCurrentProfile(This,profile) ) 

#define ISchematicDisplay_getCurrentLocation(This,location)	\
    ( (This)->lpVtbl -> getCurrentLocation(This,location) ) 

#define ISchematicDisplay_getCurrentUserName(This,userName)	\
    ( (This)->lpVtbl -> getCurrentUserName(This,userName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISchematicDisplay_INTERFACE_DEFINED__ */



#ifndef __RippleControlStationLib_LIBRARY_DEFINED__
#define __RippleControlStationLib_LIBRARY_DEFINED__

/* library RippleControlStationLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_RippleControlStationLib;

#ifndef ___ISchematicDisplayEvents_DISPINTERFACE_DEFINED__
#define ___ISchematicDisplayEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISchematicDisplayEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISchematicDisplayEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("549EC73F-77CF-4DF9-8ECA-7B73E0F72326")
    _ISchematicDisplayEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISchematicDisplayEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISchematicDisplayEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISchematicDisplayEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISchematicDisplayEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISchematicDisplayEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISchematicDisplayEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISchematicDisplayEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISchematicDisplayEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISchematicDisplayEventsVtbl;

    interface _ISchematicDisplayEvents
    {
        CONST_VTBL struct _ISchematicDisplayEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISchematicDisplayEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _ISchematicDisplayEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _ISchematicDisplayEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _ISchematicDisplayEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _ISchematicDisplayEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _ISchematicDisplayEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _ISchematicDisplayEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISchematicDisplayEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SchematicDisplay;

#ifdef __cplusplus

class DECLSPEC_UUID("9F3187D4-5454-4C55-9A42-F0EEBCC5EA72")
SchematicDisplay;
#endif
#endif /* __RippleControlStationLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


