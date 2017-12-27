

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Thu Nov 02 18:01:59 2017
 */
/* Compiler settings for src/SoundManager.idl:
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


#ifndef __SoundManager_h__
#define __SoundManager_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISoundInterface_FWD_DEFINED__
#define __ISoundInterface_FWD_DEFINED__
typedef interface ISoundInterface ISoundInterface;
#endif 	/* __ISoundInterface_FWD_DEFINED__ */


#ifndef ___ISoundInterfaceEvents_FWD_DEFINED__
#define ___ISoundInterfaceEvents_FWD_DEFINED__
typedef interface _ISoundInterfaceEvents _ISoundInterfaceEvents;
#endif 	/* ___ISoundInterfaceEvents_FWD_DEFINED__ */


#ifndef __SoundInterface_FWD_DEFINED__
#define __SoundInterface_FWD_DEFINED__

#ifdef __cplusplus
typedef class SoundInterface SoundInterface;
#else
typedef struct SoundInterface SoundInterface;
#endif /* __cplusplus */

#endif 	/* __SoundInterface_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __SOUNDMANAGERLib_LIBRARY_DEFINED__
#define __SOUNDMANAGERLib_LIBRARY_DEFINED__

/* library SOUNDMANAGERLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SOUNDMANAGERLib;

#ifndef __ISoundInterface_INTERFACE_DEFINED__
#define __ISoundInterface_INTERFACE_DEFINED__

/* interface ISoundInterface */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISoundInterface;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A03E7601-4386-4B87-9AC1-FDC81B2C163D")
    ISoundInterface : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setPriority( 
            BSTR category,
            int priority) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setSilence( 
            BSTR category,
            VARIANT_BOOL isSilenced) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setSilenceAll( 
            VARIANT_BOOL isSilenced) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE joinCategories( 
            BSTR primaryCategory,
            BSTR secondaryCategory) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE isSilenced( 
            BSTR category,
            /* [retval][out] */ VARIANT_BOOL *isSilenced) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE isAllSilenced( 
            /* [retval][out] */ VARIANT_BOOL *isSilenced) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISoundInterfaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISoundInterface * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISoundInterface * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISoundInterface * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISoundInterface * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISoundInterface * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISoundInterface * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISoundInterface * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setPriority )( 
            ISoundInterface * This,
            BSTR category,
            int priority);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setSilence )( 
            ISoundInterface * This,
            BSTR category,
            VARIANT_BOOL isSilenced);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setSilenceAll )( 
            ISoundInterface * This,
            VARIANT_BOOL isSilenced);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *joinCategories )( 
            ISoundInterface * This,
            BSTR primaryCategory,
            BSTR secondaryCategory);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *isSilenced )( 
            ISoundInterface * This,
            BSTR category,
            /* [retval][out] */ VARIANT_BOOL *isSilenced);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *isAllSilenced )( 
            ISoundInterface * This,
            /* [retval][out] */ VARIANT_BOOL *isSilenced);
        
        END_INTERFACE
    } ISoundInterfaceVtbl;

    interface ISoundInterface
    {
        CONST_VTBL struct ISoundInterfaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISoundInterface_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISoundInterface_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISoundInterface_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISoundInterface_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISoundInterface_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISoundInterface_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISoundInterface_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISoundInterface_setPriority(This,category,priority)	\
    ( (This)->lpVtbl -> setPriority(This,category,priority) ) 

#define ISoundInterface_setSilence(This,category,isSilenced)	\
    ( (This)->lpVtbl -> setSilence(This,category,isSilenced) ) 

#define ISoundInterface_setSilenceAll(This,isSilenced)	\
    ( (This)->lpVtbl -> setSilenceAll(This,isSilenced) ) 

#define ISoundInterface_joinCategories(This,primaryCategory,secondaryCategory)	\
    ( (This)->lpVtbl -> joinCategories(This,primaryCategory,secondaryCategory) ) 

#define ISoundInterface_isSilenced(This,category,isSilenced)	\
    ( (This)->lpVtbl -> isSilenced(This,category,isSilenced) ) 

#define ISoundInterface_isAllSilenced(This,isSilenced)	\
    ( (This)->lpVtbl -> isAllSilenced(This,isSilenced) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISoundInterface_INTERFACE_DEFINED__ */


#ifndef ___ISoundInterfaceEvents_DISPINTERFACE_DEFINED__
#define ___ISoundInterfaceEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISoundInterfaceEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__ISoundInterfaceEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B1C9D994-E1A8-44da-B985-B72A3A9AF1EF")
    _ISoundInterfaceEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISoundInterfaceEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISoundInterfaceEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISoundInterfaceEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISoundInterfaceEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISoundInterfaceEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISoundInterfaceEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISoundInterfaceEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISoundInterfaceEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISoundInterfaceEventsVtbl;

    interface _ISoundInterfaceEvents
    {
        CONST_VTBL struct _ISoundInterfaceEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISoundInterfaceEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _ISoundInterfaceEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _ISoundInterfaceEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _ISoundInterfaceEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _ISoundInterfaceEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _ISoundInterfaceEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _ISoundInterfaceEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISoundInterfaceEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SoundInterface;

#ifdef __cplusplus

class DECLSPEC_UUID("0F20771A-E29B-412C-B81E-147B97E32321")
SoundInterface;
#endif
#endif /* __SOUNDMANAGERLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


