

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __ATSAlarmBridgeUtility_i_h__
#define __ATSAlarmBridgeUtility_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IATSAlarmActionManager_FWD_DEFINED__
#define __IATSAlarmActionManager_FWD_DEFINED__
typedef interface IATSAlarmActionManager IATSAlarmActionManager;
#endif 	/* __IATSAlarmActionManager_FWD_DEFINED__ */


#ifndef ___IATSAlarmActionManagerEvents_FWD_DEFINED__
#define ___IATSAlarmActionManagerEvents_FWD_DEFINED__
typedef interface _IATSAlarmActionManagerEvents _IATSAlarmActionManagerEvents;
#endif 	/* ___IATSAlarmActionManagerEvents_FWD_DEFINED__ */


#ifndef __ATSAlarmActionManager_FWD_DEFINED__
#define __ATSAlarmActionManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSAlarmActionManager ATSAlarmActionManager;
#else
typedef struct ATSAlarmActionManager ATSAlarmActionManager;
#endif /* __cplusplus */

#endif 	/* __ATSAlarmActionManager_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_ATSAlarmBridgeUtility_0000 */
/* [local] */ 

typedef /* [helpstring][version][uuid] */  DECLSPEC_UUID("FC367CFD-7C1F-494e-BA1E-BAB0B59CA16E") struct AlarmActionInfo
    {
    /* [helpstring] */ BSTR bstrAlarmID;
    /* [helpstring] */ LONG lLeftPosition;
    } 	AlarmActionInfo;

typedef /* [helpstring][version][uuid] */  DECLSPEC_UUID("F5534AA7-F739-4aca-872E-BFE95315C94A") struct SchematicDisplayInfo
    {
    /* [helpstring] */ BSTR bstrSchematic;
    /* [helpstring] */ LONG lLeftPosition;
    } 	SchematicDisplayInfo;



extern RPC_IF_HANDLE __MIDL_itf_ATSAlarmBridgeUtility_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ATSAlarmBridgeUtility_0000_v0_0_s_ifspec;

#ifndef __IATSAlarmActionManager_INTERFACE_DEFINED__
#define __IATSAlarmActionManager_INTERFACE_DEFINED__

/* interface IATSAlarmActionManager */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSAlarmActionManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4FE13D05-55DA-4320-BC06-6A4FA6148AB7")
    IATSAlarmActionManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE showIP( 
            /* [in] */ AlarmActionInfo *pAlarmActionInfo,
            /* [retval][out] */ SHORT *nRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE showSchematic( 
            /* [in] */ AlarmActionInfo *pAlarmActionInfo,
            /* [retval][out] */ SHORT *nRetVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSAlarmActionManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSAlarmActionManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSAlarmActionManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSAlarmActionManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSAlarmActionManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSAlarmActionManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSAlarmActionManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSAlarmActionManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *showIP )( 
            IATSAlarmActionManager * This,
            /* [in] */ AlarmActionInfo *pAlarmActionInfo,
            /* [retval][out] */ SHORT *nRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *showSchematic )( 
            IATSAlarmActionManager * This,
            /* [in] */ AlarmActionInfo *pAlarmActionInfo,
            /* [retval][out] */ SHORT *nRetVal);
        
        END_INTERFACE
    } IATSAlarmActionManagerVtbl;

    interface IATSAlarmActionManager
    {
        CONST_VTBL struct IATSAlarmActionManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSAlarmActionManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IATSAlarmActionManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IATSAlarmActionManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IATSAlarmActionManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IATSAlarmActionManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IATSAlarmActionManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IATSAlarmActionManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IATSAlarmActionManager_showIP(This,pAlarmActionInfo,nRetVal)	\
    (This)->lpVtbl -> showIP(This,pAlarmActionInfo,nRetVal)

#define IATSAlarmActionManager_showSchematic(This,pAlarmActionInfo,nRetVal)	\
    (This)->lpVtbl -> showSchematic(This,pAlarmActionInfo,nRetVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IATSAlarmActionManager_showIP_Proxy( 
    IATSAlarmActionManager * This,
    /* [in] */ AlarmActionInfo *pAlarmActionInfo,
    /* [retval][out] */ SHORT *nRetVal);


void __RPC_STUB IATSAlarmActionManager_showIP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IATSAlarmActionManager_showSchematic_Proxy( 
    IATSAlarmActionManager * This,
    /* [in] */ AlarmActionInfo *pAlarmActionInfo,
    /* [retval][out] */ SHORT *nRetVal);


void __RPC_STUB IATSAlarmActionManager_showSchematic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IATSAlarmActionManager_INTERFACE_DEFINED__ */



#ifndef __ATSAlarmBridgeUtilityLib_LIBRARY_DEFINED__
#define __ATSAlarmBridgeUtilityLib_LIBRARY_DEFINED__

/* library ATSAlarmBridgeUtilityLib */
/* [helpstring][version][uuid] */ 




EXTERN_C const IID LIBID_ATSAlarmBridgeUtilityLib;

#ifndef ___IATSAlarmActionManagerEvents_DISPINTERFACE_DEFINED__
#define ___IATSAlarmActionManagerEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IATSAlarmActionManagerEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IATSAlarmActionManagerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B13691A1-614C-4B5F-B821-EAF15C706B66")
    _IATSAlarmActionManagerEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IATSAlarmActionManagerEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IATSAlarmActionManagerEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IATSAlarmActionManagerEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IATSAlarmActionManagerEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IATSAlarmActionManagerEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IATSAlarmActionManagerEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IATSAlarmActionManagerEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IATSAlarmActionManagerEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IATSAlarmActionManagerEventsVtbl;

    interface _IATSAlarmActionManagerEvents
    {
        CONST_VTBL struct _IATSAlarmActionManagerEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IATSAlarmActionManagerEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IATSAlarmActionManagerEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IATSAlarmActionManagerEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IATSAlarmActionManagerEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IATSAlarmActionManagerEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IATSAlarmActionManagerEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IATSAlarmActionManagerEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IATSAlarmActionManagerEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ATSAlarmActionManager;

#ifdef __cplusplus

class DECLSPEC_UUID("E8A7F762-8797-4E89-81E3-11C080642387")
ATSAlarmActionManager;
#endif
#endif /* __ATSAlarmBridgeUtilityLib_LIBRARY_DEFINED__ */

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


