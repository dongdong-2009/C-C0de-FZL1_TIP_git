

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Wed Apr 15 11:32:26 2015
 */
/* Compiler settings for .\src\ATSBridgeUtil.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
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

#ifndef __ATSBridgeUtil_i_h__
#define __ATSBridgeUtil_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IATSIntList_FWD_DEFINED__
#define __IATSIntList_FWD_DEFINED__
typedef interface IATSIntList IATSIntList;
#endif 	/* __IATSIntList_FWD_DEFINED__ */


#ifndef __IATSStringList_FWD_DEFINED__
#define __IATSStringList_FWD_DEFINED__
typedef interface IATSStringList IATSStringList;
#endif 	/* __IATSStringList_FWD_DEFINED__ */


#ifndef __IATSDateTime_FWD_DEFINED__
#define __IATSDateTime_FWD_DEFINED__
typedef interface IATSDateTime IATSDateTime;
#endif 	/* __IATSDateTime_FWD_DEFINED__ */


#ifndef __IATSEvent_FWD_DEFINED__
#define __IATSEvent_FWD_DEFINED__
typedef interface IATSEvent IATSEvent;
#endif 	/* __IATSEvent_FWD_DEFINED__ */


#ifndef __IATSEventList_FWD_DEFINED__
#define __IATSEventList_FWD_DEFINED__
typedef interface IATSEventList IATSEventList;
#endif 	/* __IATSEventList_FWD_DEFINED__ */


#ifndef __IATSEventSystem_FWD_DEFINED__
#define __IATSEventSystem_FWD_DEFINED__
typedef interface IATSEventSystem IATSEventSystem;
#endif 	/* __IATSEventSystem_FWD_DEFINED__ */


#ifndef __IATSEventSystemList_FWD_DEFINED__
#define __IATSEventSystemList_FWD_DEFINED__
typedef interface IATSEventSystemList IATSEventSystemList;
#endif 	/* __IATSEventSystemList_FWD_DEFINED__ */


#ifndef __IATSFilterCriteria_FWD_DEFINED__
#define __IATSFilterCriteria_FWD_DEFINED__
typedef interface IATSFilterCriteria IATSFilterCriteria;
#endif 	/* __IATSFilterCriteria_FWD_DEFINED__ */


#ifndef __IATSBridgeUtility_FWD_DEFINED__
#define __IATSBridgeUtility_FWD_DEFINED__
typedef interface IATSBridgeUtility IATSBridgeUtility;
#endif 	/* __IATSBridgeUtility_FWD_DEFINED__ */


#ifndef __IATSEventSubscription_FWD_DEFINED__
#define __IATSEventSubscription_FWD_DEFINED__
typedef interface IATSEventSubscription IATSEventSubscription;
#endif 	/* __IATSEventSubscription_FWD_DEFINED__ */


#ifndef __ATSIntList_FWD_DEFINED__
#define __ATSIntList_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSIntList ATSIntList;
#else
typedef struct ATSIntList ATSIntList;
#endif /* __cplusplus */

#endif 	/* __ATSIntList_FWD_DEFINED__ */


#ifndef __ATSStringList_FWD_DEFINED__
#define __ATSStringList_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSStringList ATSStringList;
#else
typedef struct ATSStringList ATSStringList;
#endif /* __cplusplus */

#endif 	/* __ATSStringList_FWD_DEFINED__ */


#ifndef __ATSEvent_FWD_DEFINED__
#define __ATSEvent_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSEvent ATSEvent;
#else
typedef struct ATSEvent ATSEvent;
#endif /* __cplusplus */

#endif 	/* __ATSEvent_FWD_DEFINED__ */


#ifndef __ATSDateTime_FWD_DEFINED__
#define __ATSDateTime_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSDateTime ATSDateTime;
#else
typedef struct ATSDateTime ATSDateTime;
#endif /* __cplusplus */

#endif 	/* __ATSDateTime_FWD_DEFINED__ */


#ifndef __ATSEventList_FWD_DEFINED__
#define __ATSEventList_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSEventList ATSEventList;
#else
typedef struct ATSEventList ATSEventList;
#endif /* __cplusplus */

#endif 	/* __ATSEventList_FWD_DEFINED__ */


#ifndef __ATSEventSystem_FWD_DEFINED__
#define __ATSEventSystem_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSEventSystem ATSEventSystem;
#else
typedef struct ATSEventSystem ATSEventSystem;
#endif /* __cplusplus */

#endif 	/* __ATSEventSystem_FWD_DEFINED__ */


#ifndef __ATSEventSystemList_FWD_DEFINED__
#define __ATSEventSystemList_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSEventSystemList ATSEventSystemList;
#else
typedef struct ATSEventSystemList ATSEventSystemList;
#endif /* __cplusplus */

#endif 	/* __ATSEventSystemList_FWD_DEFINED__ */


#ifndef __ATSFilterCriteria_FWD_DEFINED__
#define __ATSFilterCriteria_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSFilterCriteria ATSFilterCriteria;
#else
typedef struct ATSFilterCriteria ATSFilterCriteria;
#endif /* __cplusplus */

#endif 	/* __ATSFilterCriteria_FWD_DEFINED__ */


#ifndef __ATSBridgeUtility_FWD_DEFINED__
#define __ATSBridgeUtility_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSBridgeUtility ATSBridgeUtility;
#else
typedef struct ATSBridgeUtility ATSBridgeUtility;
#endif /* __cplusplus */

#endif 	/* __ATSBridgeUtility_FWD_DEFINED__ */


#ifndef ___IATSEventSubscriptionEvents_FWD_DEFINED__
#define ___IATSEventSubscriptionEvents_FWD_DEFINED__
typedef interface _IATSEventSubscriptionEvents _IATSEventSubscriptionEvents;
#endif 	/* ___IATSEventSubscriptionEvents_FWD_DEFINED__ */


#ifndef __ATSEventSubscription_FWD_DEFINED__
#define __ATSEventSubscription_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATSEventSubscription ATSEventSubscription;
#else
typedef struct ATSEventSubscription ATSEventSubscription;
#endif /* __cplusplus */

#endif 	/* __ATSEventSubscription_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IATSIntList_INTERFACE_DEFINED__
#define __IATSIntList_INTERFACE_DEFINED__

/* interface IATSIntList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSIntList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4C007031-097D-499B-AD44-A44C03CAA6E1")
    IATSIntList : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ LONG lVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG lIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSize( 
            /* [retval][out] */ LONG *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItem( 
            /* [in] */ LONG lIndex,
            /* [retval][out] */ LONG *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetItemAt( 
            /* [in] */ LONG lIndex,
            /* [in] */ LONG lnewVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSIntListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSIntList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSIntList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSIntList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSIntList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSIntList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSIntList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSIntList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IATSIntList * This,
            /* [in] */ LONG lVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IATSIntList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IATSIntList * This,
            /* [in] */ LONG lIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IATSIntList * This,
            /* [retval][out] */ LONG *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IATSIntList * This,
            /* [in] */ LONG lIndex,
            /* [retval][out] */ LONG *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetItemAt )( 
            IATSIntList * This,
            /* [in] */ LONG lIndex,
            /* [in] */ LONG lnewVal);
        
        END_INTERFACE
    } IATSIntListVtbl;

    interface IATSIntList
    {
        CONST_VTBL struct IATSIntListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSIntList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSIntList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSIntList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSIntList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSIntList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSIntList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSIntList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSIntList_Add(This,lVal)	\
    ( (This)->lpVtbl -> Add(This,lVal) ) 

#define IATSIntList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IATSIntList_Remove(This,lIndex)	\
    ( (This)->lpVtbl -> Remove(This,lIndex) ) 

#define IATSIntList_GetSize(This,plRetVal)	\
    ( (This)->lpVtbl -> GetSize(This,plRetVal) ) 

#define IATSIntList_GetItem(This,lIndex,plRetVal)	\
    ( (This)->lpVtbl -> GetItem(This,lIndex,plRetVal) ) 

#define IATSIntList_SetItemAt(This,lIndex,lnewVal)	\
    ( (This)->lpVtbl -> SetItemAt(This,lIndex,lnewVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSIntList_INTERFACE_DEFINED__ */


#ifndef __IATSStringList_INTERFACE_DEFINED__
#define __IATSStringList_INTERFACE_DEFINED__

/* interface IATSStringList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSStringList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E69ED143-BFFB-45B7-B02E-1BB53F979A83")
    IATSStringList : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG lIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSize( 
            /* [retval][out] */ LONG *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItem( 
            /* [in] */ LONG lIndex,
            /* [retval][out] */ BSTR *pbstrRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetItemAt( 
            /* [in] */ LONG lIndex,
            /* [in] */ BSTR bstrNewVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSStringListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSStringList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSStringList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSStringList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSStringList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSStringList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSStringList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSStringList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IATSStringList * This,
            /* [in] */ BSTR bstrVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IATSStringList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IATSStringList * This,
            /* [in] */ LONG lIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IATSStringList * This,
            /* [retval][out] */ LONG *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IATSStringList * This,
            /* [in] */ LONG lIndex,
            /* [retval][out] */ BSTR *pbstrRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetItemAt )( 
            IATSStringList * This,
            /* [in] */ LONG lIndex,
            /* [in] */ BSTR bstrNewVal);
        
        END_INTERFACE
    } IATSStringListVtbl;

    interface IATSStringList
    {
        CONST_VTBL struct IATSStringListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSStringList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSStringList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSStringList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSStringList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSStringList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSStringList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSStringList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSStringList_Add(This,bstrVal)	\
    ( (This)->lpVtbl -> Add(This,bstrVal) ) 

#define IATSStringList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IATSStringList_Remove(This,lIndex)	\
    ( (This)->lpVtbl -> Remove(This,lIndex) ) 

#define IATSStringList_GetSize(This,plRetVal)	\
    ( (This)->lpVtbl -> GetSize(This,plRetVal) ) 

#define IATSStringList_GetItem(This,lIndex,pbstrRetVal)	\
    ( (This)->lpVtbl -> GetItem(This,lIndex,pbstrRetVal) ) 

#define IATSStringList_SetItemAt(This,lIndex,bstrNewVal)	\
    ( (This)->lpVtbl -> SetItemAt(This,lIndex,bstrNewVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSStringList_INTERFACE_DEFINED__ */


#ifndef __IATSDateTime_INTERFACE_DEFINED__
#define __IATSDateTime_INTERFACE_DEFINED__

/* interface IATSDateTime */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSDateTime;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DAB92A0C-C43C-4057-9A7B-78C205C7D2E6")
    IATSDateTime : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Year( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Year( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Month( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Month( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Day( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Day( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Hour( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Hour( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Minute( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Minute( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Second( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Second( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Nanosecond( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Nanosecond( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDate( 
            /* [in] */ SHORT Year,
            /* [in] */ SHORT Month,
            /* [in] */ SHORT Day,
            /* [in] */ SHORT Hour,
            /* [in] */ SHORT Minute,
            /* [in] */ SHORT Second,
            /* [in] */ LONG NanoSecond) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSDateTimeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSDateTime * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSDateTime * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSDateTime * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSDateTime * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSDateTime * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSDateTime * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSDateTime * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Year )( 
            IATSDateTime * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Year )( 
            IATSDateTime * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Month )( 
            IATSDateTime * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Month )( 
            IATSDateTime * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Day )( 
            IATSDateTime * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Day )( 
            IATSDateTime * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Hour )( 
            IATSDateTime * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Hour )( 
            IATSDateTime * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Minute )( 
            IATSDateTime * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Minute )( 
            IATSDateTime * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Second )( 
            IATSDateTime * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Second )( 
            IATSDateTime * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Nanosecond )( 
            IATSDateTime * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Nanosecond )( 
            IATSDateTime * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDate )( 
            IATSDateTime * This,
            /* [in] */ SHORT Year,
            /* [in] */ SHORT Month,
            /* [in] */ SHORT Day,
            /* [in] */ SHORT Hour,
            /* [in] */ SHORT Minute,
            /* [in] */ SHORT Second,
            /* [in] */ LONG NanoSecond);
        
        END_INTERFACE
    } IATSDateTimeVtbl;

    interface IATSDateTime
    {
        CONST_VTBL struct IATSDateTimeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSDateTime_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSDateTime_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSDateTime_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSDateTime_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSDateTime_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSDateTime_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSDateTime_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSDateTime_get_Year(This,pVal)	\
    ( (This)->lpVtbl -> get_Year(This,pVal) ) 

#define IATSDateTime_put_Year(This,newVal)	\
    ( (This)->lpVtbl -> put_Year(This,newVal) ) 

#define IATSDateTime_get_Month(This,pVal)	\
    ( (This)->lpVtbl -> get_Month(This,pVal) ) 

#define IATSDateTime_put_Month(This,newVal)	\
    ( (This)->lpVtbl -> put_Month(This,newVal) ) 

#define IATSDateTime_get_Day(This,pVal)	\
    ( (This)->lpVtbl -> get_Day(This,pVal) ) 

#define IATSDateTime_put_Day(This,newVal)	\
    ( (This)->lpVtbl -> put_Day(This,newVal) ) 

#define IATSDateTime_get_Hour(This,pVal)	\
    ( (This)->lpVtbl -> get_Hour(This,pVal) ) 

#define IATSDateTime_put_Hour(This,newVal)	\
    ( (This)->lpVtbl -> put_Hour(This,newVal) ) 

#define IATSDateTime_get_Minute(This,pVal)	\
    ( (This)->lpVtbl -> get_Minute(This,pVal) ) 

#define IATSDateTime_put_Minute(This,newVal)	\
    ( (This)->lpVtbl -> put_Minute(This,newVal) ) 

#define IATSDateTime_get_Second(This,pVal)	\
    ( (This)->lpVtbl -> get_Second(This,pVal) ) 

#define IATSDateTime_put_Second(This,newVal)	\
    ( (This)->lpVtbl -> put_Second(This,newVal) ) 

#define IATSDateTime_get_Nanosecond(This,pVal)	\
    ( (This)->lpVtbl -> get_Nanosecond(This,pVal) ) 

#define IATSDateTime_put_Nanosecond(This,newVal)	\
    ( (This)->lpVtbl -> put_Nanosecond(This,newVal) ) 

#define IATSDateTime_SetDate(This,Year,Month,Day,Hour,Minute,Second,NanoSecond)	\
    ( (This)->lpVtbl -> SetDate(This,Year,Month,Day,Hour,Minute,Second,NanoSecond) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSDateTime_INTERFACE_DEFINED__ */


#ifndef __IATSEvent_INTERFACE_DEFINED__
#define __IATSEvent_INTERFACE_DEFINED__

/* interface IATSEvent */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1C1E314D-182C-4ED9-9165-97CEB4E0E14F")
    IATSEvent : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EventID( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventType( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EventType( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTimeStamp( 
            /* [out][in] */ IATSDateTime **pDateTime) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Asset( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Asset( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Operator( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Operator( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Location( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Location( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SystemName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SystemName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubsystemName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SubsystemName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AlarmID( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AlarmID( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AlarmType( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AlarmType( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AlarmMMSState( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AlarmMMSState( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AlarmAvalancheType( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AlarmAvalancheType( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AlarmSeverity( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AlarmSeverity( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OperationalMaintenanceType( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OperationalMaintenanceType( 
            /* [in] */ SHORT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AlarmState( 
            /* [retval][out] */ SHORT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AlarmState( 
            /* [in] */ SHORT newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSEvent * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSEvent * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSEvent * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSEvent * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSEvent * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSEvent * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSEvent * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EventID )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EventID )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EventType )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EventType )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTimeStamp )( 
            IATSEvent * This,
            /* [out][in] */ IATSDateTime **pDateTime);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Asset )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Asset )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Description )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Operator )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Operator )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Location )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Location )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SystemName )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SystemName )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SubsystemName )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SubsystemName )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AlarmID )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AlarmID )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AlarmType )( 
            IATSEvent * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AlarmType )( 
            IATSEvent * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AlarmMMSState )( 
            IATSEvent * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AlarmMMSState )( 
            IATSEvent * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AlarmAvalancheType )( 
            IATSEvent * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AlarmAvalancheType )( 
            IATSEvent * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AlarmSeverity )( 
            IATSEvent * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AlarmSeverity )( 
            IATSEvent * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OperationalMaintenanceType )( 
            IATSEvent * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OperationalMaintenanceType )( 
            IATSEvent * This,
            /* [in] */ SHORT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AlarmState )( 
            IATSEvent * This,
            /* [retval][out] */ SHORT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AlarmState )( 
            IATSEvent * This,
            /* [in] */ SHORT newVal);
        
        END_INTERFACE
    } IATSEventVtbl;

    interface IATSEvent
    {
        CONST_VTBL struct IATSEventVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSEvent_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSEvent_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSEvent_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSEvent_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSEvent_get_EventID(This,pVal)	\
    ( (This)->lpVtbl -> get_EventID(This,pVal) ) 

#define IATSEvent_put_EventID(This,newVal)	\
    ( (This)->lpVtbl -> put_EventID(This,newVal) ) 

#define IATSEvent_get_EventType(This,pVal)	\
    ( (This)->lpVtbl -> get_EventType(This,pVal) ) 

#define IATSEvent_put_EventType(This,newVal)	\
    ( (This)->lpVtbl -> put_EventType(This,newVal) ) 

#define IATSEvent_GetTimeStamp(This,pDateTime)	\
    ( (This)->lpVtbl -> GetTimeStamp(This,pDateTime) ) 

#define IATSEvent_get_Asset(This,pVal)	\
    ( (This)->lpVtbl -> get_Asset(This,pVal) ) 

#define IATSEvent_put_Asset(This,newVal)	\
    ( (This)->lpVtbl -> put_Asset(This,newVal) ) 

#define IATSEvent_get_Description(This,pVal)	\
    ( (This)->lpVtbl -> get_Description(This,pVal) ) 

#define IATSEvent_put_Description(This,newVal)	\
    ( (This)->lpVtbl -> put_Description(This,newVal) ) 

#define IATSEvent_get_Operator(This,pVal)	\
    ( (This)->lpVtbl -> get_Operator(This,pVal) ) 

#define IATSEvent_put_Operator(This,newVal)	\
    ( (This)->lpVtbl -> put_Operator(This,newVal) ) 

#define IATSEvent_get_Location(This,pVal)	\
    ( (This)->lpVtbl -> get_Location(This,pVal) ) 

#define IATSEvent_put_Location(This,newVal)	\
    ( (This)->lpVtbl -> put_Location(This,newVal) ) 

#define IATSEvent_get_Value(This,pVal)	\
    ( (This)->lpVtbl -> get_Value(This,pVal) ) 

#define IATSEvent_put_Value(This,newVal)	\
    ( (This)->lpVtbl -> put_Value(This,newVal) ) 

#define IATSEvent_get_SystemName(This,pVal)	\
    ( (This)->lpVtbl -> get_SystemName(This,pVal) ) 

#define IATSEvent_put_SystemName(This,newVal)	\
    ( (This)->lpVtbl -> put_SystemName(This,newVal) ) 

#define IATSEvent_get_SubsystemName(This,pVal)	\
    ( (This)->lpVtbl -> get_SubsystemName(This,pVal) ) 

#define IATSEvent_put_SubsystemName(This,newVal)	\
    ( (This)->lpVtbl -> put_SubsystemName(This,newVal) ) 

#define IATSEvent_get_AlarmID(This,pVal)	\
    ( (This)->lpVtbl -> get_AlarmID(This,pVal) ) 

#define IATSEvent_put_AlarmID(This,newVal)	\
    ( (This)->lpVtbl -> put_AlarmID(This,newVal) ) 

#define IATSEvent_get_AlarmType(This,pVal)	\
    ( (This)->lpVtbl -> get_AlarmType(This,pVal) ) 

#define IATSEvent_put_AlarmType(This,newVal)	\
    ( (This)->lpVtbl -> put_AlarmType(This,newVal) ) 

#define IATSEvent_get_AlarmMMSState(This,pVal)	\
    ( (This)->lpVtbl -> get_AlarmMMSState(This,pVal) ) 

#define IATSEvent_put_AlarmMMSState(This,newVal)	\
    ( (This)->lpVtbl -> put_AlarmMMSState(This,newVal) ) 

#define IATSEvent_get_AlarmAvalancheType(This,pVal)	\
    ( (This)->lpVtbl -> get_AlarmAvalancheType(This,pVal) ) 

#define IATSEvent_put_AlarmAvalancheType(This,newVal)	\
    ( (This)->lpVtbl -> put_AlarmAvalancheType(This,newVal) ) 

#define IATSEvent_get_AlarmSeverity(This,pVal)	\
    ( (This)->lpVtbl -> get_AlarmSeverity(This,pVal) ) 

#define IATSEvent_put_AlarmSeverity(This,newVal)	\
    ( (This)->lpVtbl -> put_AlarmSeverity(This,newVal) ) 

#define IATSEvent_get_OperationalMaintenanceType(This,pVal)	\
    ( (This)->lpVtbl -> get_OperationalMaintenanceType(This,pVal) ) 

#define IATSEvent_put_OperationalMaintenanceType(This,newVal)	\
    ( (This)->lpVtbl -> put_OperationalMaintenanceType(This,newVal) ) 

#define IATSEvent_get_AlarmState(This,pVal)	\
    ( (This)->lpVtbl -> get_AlarmState(This,pVal) ) 

#define IATSEvent_put_AlarmState(This,newVal)	\
    ( (This)->lpVtbl -> put_AlarmState(This,newVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSEvent_INTERFACE_DEFINED__ */


#ifndef __IATSEventList_INTERFACE_DEFINED__
#define __IATSEventList_INTERFACE_DEFINED__

/* interface IATSEventList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSEventList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1F63D5C7-09DB-4514-A0DB-A1CE0B00F5D2")
    IATSEventList : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ IATSEvent *pATSEvent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            LONG lIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSize( 
            /* [retval][out] */ LONG *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItem( 
            /* [in] */ LONG lIndex,
            /* [out][in] */ IATSEvent **pATSEvent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetItemAt( 
            /* [in] */ LONG lIndex,
            /* [in] */ IATSEvent *pATSEvent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSEventListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSEventList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSEventList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSEventList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSEventList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSEventList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSEventList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSEventList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IATSEventList * This,
            /* [in] */ IATSEvent *pATSEvent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IATSEventList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IATSEventList * This,
            LONG lIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IATSEventList * This,
            /* [retval][out] */ LONG *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IATSEventList * This,
            /* [in] */ LONG lIndex,
            /* [out][in] */ IATSEvent **pATSEvent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetItemAt )( 
            IATSEventList * This,
            /* [in] */ LONG lIndex,
            /* [in] */ IATSEvent *pATSEvent);
        
        END_INTERFACE
    } IATSEventListVtbl;

    interface IATSEventList
    {
        CONST_VTBL struct IATSEventListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSEventList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSEventList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSEventList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSEventList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSEventList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSEventList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSEventList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSEventList_Add(This,pATSEvent)	\
    ( (This)->lpVtbl -> Add(This,pATSEvent) ) 

#define IATSEventList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IATSEventList_Remove(This,lIndex)	\
    ( (This)->lpVtbl -> Remove(This,lIndex) ) 

#define IATSEventList_GetSize(This,plRetVal)	\
    ( (This)->lpVtbl -> GetSize(This,plRetVal) ) 

#define IATSEventList_GetItem(This,lIndex,pATSEvent)	\
    ( (This)->lpVtbl -> GetItem(This,lIndex,pATSEvent) ) 

#define IATSEventList_SetItemAt(This,lIndex,pATSEvent)	\
    ( (This)->lpVtbl -> SetItemAt(This,lIndex,pATSEvent) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSEventList_INTERFACE_DEFINED__ */


#ifndef __IATSEventSystem_INTERFACE_DEFINED__
#define __IATSEventSystem_INTERFACE_DEFINED__

/* interface IATSEventSystem */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSEventSystem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ED06D7FD-69BE-4D64-801A-1C9FA0E3CB2D")
    IATSEventSystem : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubSystemList( 
            /* [out][in] */ IATSStringList **pSubSystemList) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSEventSystemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSEventSystem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSEventSystem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSEventSystem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSEventSystem * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSEventSystem * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSEventSystem * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSEventSystem * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IATSEventSystem * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IATSEventSystem * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSubSystemList )( 
            IATSEventSystem * This,
            /* [out][in] */ IATSStringList **pSubSystemList);
        
        END_INTERFACE
    } IATSEventSystemVtbl;

    interface IATSEventSystem
    {
        CONST_VTBL struct IATSEventSystemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSEventSystem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSEventSystem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSEventSystem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSEventSystem_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSEventSystem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSEventSystem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSEventSystem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSEventSystem_get_Name(This,pVal)	\
    ( (This)->lpVtbl -> get_Name(This,pVal) ) 

#define IATSEventSystem_put_Name(This,newVal)	\
    ( (This)->lpVtbl -> put_Name(This,newVal) ) 

#define IATSEventSystem_GetSubSystemList(This,pSubSystemList)	\
    ( (This)->lpVtbl -> GetSubSystemList(This,pSubSystemList) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSEventSystem_INTERFACE_DEFINED__ */


#ifndef __IATSEventSystemList_INTERFACE_DEFINED__
#define __IATSEventSystemList_INTERFACE_DEFINED__

/* interface IATSEventSystemList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSEventSystemList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BCB1B984-E91F-4210-AA2C-95686713F022")
    IATSEventSystemList : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ IATSEventSystem *pATSEventSystem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG lIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSize( 
            /* [retval][out] */ LONG *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItem( 
            LONG lIndex,
            /* [out][in] */ IATSEventSystem **pATSEventSystem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetItemAt( 
            /* [in] */ LONG lIndex,
            /* [in] */ IATSEventSystem *pATSEventSystem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSEventSystemListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSEventSystemList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSEventSystemList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSEventSystemList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSEventSystemList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSEventSystemList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSEventSystemList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSEventSystemList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IATSEventSystemList * This,
            /* [in] */ IATSEventSystem *pATSEventSystem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IATSEventSystemList * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IATSEventSystemList * This,
            /* [in] */ LONG lIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IATSEventSystemList * This,
            /* [retval][out] */ LONG *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IATSEventSystemList * This,
            LONG lIndex,
            /* [out][in] */ IATSEventSystem **pATSEventSystem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetItemAt )( 
            IATSEventSystemList * This,
            /* [in] */ LONG lIndex,
            /* [in] */ IATSEventSystem *pATSEventSystem);
        
        END_INTERFACE
    } IATSEventSystemListVtbl;

    interface IATSEventSystemList
    {
        CONST_VTBL struct IATSEventSystemListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSEventSystemList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSEventSystemList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSEventSystemList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSEventSystemList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSEventSystemList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSEventSystemList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSEventSystemList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSEventSystemList_Add(This,pATSEventSystem)	\
    ( (This)->lpVtbl -> Add(This,pATSEventSystem) ) 

#define IATSEventSystemList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IATSEventSystemList_Remove(This,lIndex)	\
    ( (This)->lpVtbl -> Remove(This,lIndex) ) 

#define IATSEventSystemList_GetSize(This,plRetVal)	\
    ( (This)->lpVtbl -> GetSize(This,plRetVal) ) 

#define IATSEventSystemList_GetItem(This,lIndex,pATSEventSystem)	\
    ( (This)->lpVtbl -> GetItem(This,lIndex,pATSEventSystem) ) 

#define IATSEventSystemList_SetItemAt(This,lIndex,pATSEventSystem)	\
    ( (This)->lpVtbl -> SetItemAt(This,lIndex,pATSEventSystem) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSEventSystemList_INTERFACE_DEFINED__ */


#ifndef __IATSFilterCriteria_INTERFACE_DEFINED__
#define __IATSFilterCriteria_INTERFACE_DEFINED__

/* interface IATSFilterCriteria */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSFilterCriteria;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("71ADC320-6B34-4D62-86D5-2B82034F88D3")
    IATSFilterCriteria : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartTimePeriodDefined( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartTimePeriodDefined( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStartTimePeriod( 
            /* [out][in] */ IATSDateTime **pDateTime) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EndTimePeriodDefined( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EndTimePeriodDefined( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEndTimePeriod( 
            /* [out][in] */ IATSDateTime **pDateTime) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEventTypeEqualOr( 
            /* [out][in] */ IATSStringList **pStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAssetEqualOr( 
            /* [out][in] */ IATSStringList **pATSStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDescriptionEqualOr( 
            /* [out][in] */ IATSStringList **pATSStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOperatorEqualOr( 
            /* [out][in] */ IATSStringList **pATSStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLocationEqualOr( 
            /* [out][in] */ IATSStringList **pATSStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetValueEqualOr( 
            /* [out][in] */ IATSStringList **pATSStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSystemNameEqualOr( 
            /* [out][in] */ IATSStringList **pATSStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubsystemNameEqualOr( 
            /* [in] */ IATSStringList **pATSStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarmIDEqualOr( 
            /* [out][in] */ IATSStringList **pATSStringList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOperationalMaintenanceTypeEqualOr( 
            /* [out][in] */ IATSIntList **pATSIntList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarmStateEqualOr( 
            /* [out][in] */ IATSIntList **pATSIntList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarmAvalancheTypeEqualOr( 
            /* [out][in] */ IATSIntList **pATSIntList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarmSeverityEqualOr( 
            /* [out][in] */ IATSIntList **pATSIntList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarmTypeEqualOr( 
            /* [out][in] */ IATSStringList **pATSStringList) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSFilterCriteriaVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSFilterCriteria * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSFilterCriteria * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSFilterCriteria * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSFilterCriteria * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSFilterCriteria * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSFilterCriteria * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSFilterCriteria * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StartTimePeriodDefined )( 
            IATSFilterCriteria * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StartTimePeriodDefined )( 
            IATSFilterCriteria * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStartTimePeriod )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSDateTime **pDateTime);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EndTimePeriodDefined )( 
            IATSFilterCriteria * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EndTimePeriodDefined )( 
            IATSFilterCriteria * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEndTimePeriod )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSDateTime **pDateTime);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEventTypeEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAssetEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pATSStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDescriptionEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pATSStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOperatorEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pATSStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLocationEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pATSStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetValueEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pATSStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSystemNameEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pATSStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSubsystemNameEqualOr )( 
            IATSFilterCriteria * This,
            /* [in] */ IATSStringList **pATSStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarmIDEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pATSStringList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOperationalMaintenanceTypeEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSIntList **pATSIntList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarmStateEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSIntList **pATSIntList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarmAvalancheTypeEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSIntList **pATSIntList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarmSeverityEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSIntList **pATSIntList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarmTypeEqualOr )( 
            IATSFilterCriteria * This,
            /* [out][in] */ IATSStringList **pATSStringList);
        
        END_INTERFACE
    } IATSFilterCriteriaVtbl;

    interface IATSFilterCriteria
    {
        CONST_VTBL struct IATSFilterCriteriaVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSFilterCriteria_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSFilterCriteria_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSFilterCriteria_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSFilterCriteria_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSFilterCriteria_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSFilterCriteria_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSFilterCriteria_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSFilterCriteria_get_StartTimePeriodDefined(This,pVal)	\
    ( (This)->lpVtbl -> get_StartTimePeriodDefined(This,pVal) ) 

#define IATSFilterCriteria_put_StartTimePeriodDefined(This,newVal)	\
    ( (This)->lpVtbl -> put_StartTimePeriodDefined(This,newVal) ) 

#define IATSFilterCriteria_GetStartTimePeriod(This,pDateTime)	\
    ( (This)->lpVtbl -> GetStartTimePeriod(This,pDateTime) ) 

#define IATSFilterCriteria_get_EndTimePeriodDefined(This,pVal)	\
    ( (This)->lpVtbl -> get_EndTimePeriodDefined(This,pVal) ) 

#define IATSFilterCriteria_put_EndTimePeriodDefined(This,newVal)	\
    ( (This)->lpVtbl -> put_EndTimePeriodDefined(This,newVal) ) 

#define IATSFilterCriteria_GetEndTimePeriod(This,pDateTime)	\
    ( (This)->lpVtbl -> GetEndTimePeriod(This,pDateTime) ) 

#define IATSFilterCriteria_GetEventTypeEqualOr(This,pStringList)	\
    ( (This)->lpVtbl -> GetEventTypeEqualOr(This,pStringList) ) 

#define IATSFilterCriteria_GetAssetEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetAssetEqualOr(This,pATSStringList) ) 

#define IATSFilterCriteria_GetDescriptionEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetDescriptionEqualOr(This,pATSStringList) ) 

#define IATSFilterCriteria_GetOperatorEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetOperatorEqualOr(This,pATSStringList) ) 

#define IATSFilterCriteria_GetLocationEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetLocationEqualOr(This,pATSStringList) ) 

#define IATSFilterCriteria_GetValueEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetValueEqualOr(This,pATSStringList) ) 

#define IATSFilterCriteria_GetSystemNameEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetSystemNameEqualOr(This,pATSStringList) ) 

#define IATSFilterCriteria_GetSubsystemNameEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetSubsystemNameEqualOr(This,pATSStringList) ) 

#define IATSFilterCriteria_GetAlarmIDEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetAlarmIDEqualOr(This,pATSStringList) ) 

#define IATSFilterCriteria_GetOperationalMaintenanceTypeEqualOr(This,pATSIntList)	\
    ( (This)->lpVtbl -> GetOperationalMaintenanceTypeEqualOr(This,pATSIntList) ) 

#define IATSFilterCriteria_GetAlarmStateEqualOr(This,pATSIntList)	\
    ( (This)->lpVtbl -> GetAlarmStateEqualOr(This,pATSIntList) ) 

#define IATSFilterCriteria_GetAlarmAvalancheTypeEqualOr(This,pATSIntList)	\
    ( (This)->lpVtbl -> GetAlarmAvalancheTypeEqualOr(This,pATSIntList) ) 

#define IATSFilterCriteria_GetAlarmSeverityEqualOr(This,pATSIntList)	\
    ( (This)->lpVtbl -> GetAlarmSeverityEqualOr(This,pATSIntList) ) 

#define IATSFilterCriteria_GetAlarmTypeEqualOr(This,pATSStringList)	\
    ( (This)->lpVtbl -> GetAlarmTypeEqualOr(This,pATSStringList) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSFilterCriteria_INTERFACE_DEFINED__ */


#ifndef __IATSBridgeUtility_INTERFACE_DEFINED__
#define __IATSBridgeUtility_INTERFACE_DEFINED__

/* interface IATSBridgeUtility */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSBridgeUtility;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CA7A7E62-9A1D-487C-B8B7-E8E24A4D0A26")
    IATSBridgeUtility : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoForwardPageQuery( 
            /* [in] */ IATSFilterCriteria *filterCriteria,
            /* [in] */ BSTR atsEventID,
            /* [in] */ LONG length,
            /* [out][in] */ IATSEventList **pAtsEvents,
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoForwardPageQueryDate( 
            /* [in] */ IATSFilterCriteria *filterCriteria,
            /* [in] */ IATSDateTime *startTime,
            LONG length,
            /* [out][in] */ IATSEventList **atsEvents,
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoPreviousPageQuery( 
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ BSTR atsEventID,
            LONG length,
            /* [out][in] */ IATSEventList **pATSEvents,
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoPreviousPageQueryDate( 
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ IATSDateTime *pEndTime,
            LONG length,
            /* [out][in] */ IATSEventList **pATSEvents,
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoStartTimeEndTimeQuery( 
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ IATSDateTime *pStartTime,
            /* [in] */ IATSDateTime *pEndTime,
            /* [out][in] */ IATSEventList **pATSEvents,
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoStatisticsQuery( 
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ IATSDateTime *pStartTime,
            /* [in] */ IATSDateTime *pEndTime,
            /* [out][in] */ LONG *pEventCount,
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoEventInfoQuery( 
            /* [out][in] */ IATSEventSystemList **pEventSystems,
            /* [out][in] */ IATSStringList **pEventTypes,
            /* [out][in] */ IATSStringList **pAlarmTypes,
            /* [retval][out] */ LONG *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoAlarmInfoQuery( 
            /* [out][in] */ IATSStringList **pAlarmTypes,
            /* [retval][out] */ LONG *plVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSBridgeUtilityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSBridgeUtility * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSBridgeUtility * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSBridgeUtility * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSBridgeUtility * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSBridgeUtility * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSBridgeUtility * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSBridgeUtility * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoForwardPageQuery )( 
            IATSBridgeUtility * This,
            /* [in] */ IATSFilterCriteria *filterCriteria,
            /* [in] */ BSTR atsEventID,
            /* [in] */ LONG length,
            /* [out][in] */ IATSEventList **pAtsEvents,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoForwardPageQueryDate )( 
            IATSBridgeUtility * This,
            /* [in] */ IATSFilterCriteria *filterCriteria,
            /* [in] */ IATSDateTime *startTime,
            LONG length,
            /* [out][in] */ IATSEventList **atsEvents,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoPreviousPageQuery )( 
            IATSBridgeUtility * This,
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ BSTR atsEventID,
            LONG length,
            /* [out][in] */ IATSEventList **pATSEvents,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoPreviousPageQueryDate )( 
            IATSBridgeUtility * This,
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ IATSDateTime *pEndTime,
            LONG length,
            /* [out][in] */ IATSEventList **pATSEvents,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoStartTimeEndTimeQuery )( 
            IATSBridgeUtility * This,
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ IATSDateTime *pStartTime,
            /* [in] */ IATSDateTime *pEndTime,
            /* [out][in] */ IATSEventList **pATSEvents,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoStatisticsQuery )( 
            IATSBridgeUtility * This,
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ IATSDateTime *pStartTime,
            /* [in] */ IATSDateTime *pEndTime,
            /* [out][in] */ LONG *pEventCount,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoEventInfoQuery )( 
            IATSBridgeUtility * This,
            /* [out][in] */ IATSEventSystemList **pEventSystems,
            /* [out][in] */ IATSStringList **pEventTypes,
            /* [out][in] */ IATSStringList **pAlarmTypes,
            /* [retval][out] */ LONG *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoAlarmInfoQuery )( 
            IATSBridgeUtility * This,
            /* [out][in] */ IATSStringList **pAlarmTypes,
            /* [retval][out] */ LONG *plVal);
        
        END_INTERFACE
    } IATSBridgeUtilityVtbl;

    interface IATSBridgeUtility
    {
        CONST_VTBL struct IATSBridgeUtilityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSBridgeUtility_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSBridgeUtility_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSBridgeUtility_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSBridgeUtility_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSBridgeUtility_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSBridgeUtility_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSBridgeUtility_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSBridgeUtility_DoForwardPageQuery(This,filterCriteria,atsEventID,length,pAtsEvents,pRetVal)	\
    ( (This)->lpVtbl -> DoForwardPageQuery(This,filterCriteria,atsEventID,length,pAtsEvents,pRetVal) ) 

#define IATSBridgeUtility_DoForwardPageQueryDate(This,filterCriteria,startTime,length,atsEvents,pRetVal)	\
    ( (This)->lpVtbl -> DoForwardPageQueryDate(This,filterCriteria,startTime,length,atsEvents,pRetVal) ) 

#define IATSBridgeUtility_DoPreviousPageQuery(This,pfilterCriteria,atsEventID,length,pATSEvents,pRetVal)	\
    ( (This)->lpVtbl -> DoPreviousPageQuery(This,pfilterCriteria,atsEventID,length,pATSEvents,pRetVal) ) 

#define IATSBridgeUtility_DoPreviousPageQueryDate(This,pfilterCriteria,pEndTime,length,pATSEvents,pRetVal)	\
    ( (This)->lpVtbl -> DoPreviousPageQueryDate(This,pfilterCriteria,pEndTime,length,pATSEvents,pRetVal) ) 

#define IATSBridgeUtility_DoStartTimeEndTimeQuery(This,pfilterCriteria,pStartTime,pEndTime,pATSEvents,pRetVal)	\
    ( (This)->lpVtbl -> DoStartTimeEndTimeQuery(This,pfilterCriteria,pStartTime,pEndTime,pATSEvents,pRetVal) ) 

#define IATSBridgeUtility_DoStatisticsQuery(This,pfilterCriteria,pStartTime,pEndTime,pEventCount,pRetVal)	\
    ( (This)->lpVtbl -> DoStatisticsQuery(This,pfilterCriteria,pStartTime,pEndTime,pEventCount,pRetVal) ) 

#define IATSBridgeUtility_DoEventInfoQuery(This,pEventSystems,pEventTypes,pAlarmTypes,plVal)	\
    ( (This)->lpVtbl -> DoEventInfoQuery(This,pEventSystems,pEventTypes,pAlarmTypes,plVal) ) 

#define IATSBridgeUtility_DoAlarmInfoQuery(This,pAlarmTypes,plVal)	\
    ( (This)->lpVtbl -> DoAlarmInfoQuery(This,pAlarmTypes,plVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSBridgeUtility_INTERFACE_DEFINED__ */


#ifndef __IATSEventSubscription_INTERFACE_DEFINED__
#define __IATSEventSubscription_INTERFACE_DEFINED__

/* interface IATSEventSubscription */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IATSEventSubscription;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("68A6F3FD-1A22-4F98-B29F-F1D8961465F7")
    IATSEventSubscription : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE startSubscription( 
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ LONG backloglength,
            /* [out][in] */ IATSEventList **pATSEvents,
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE stopSubscription( 
            /* [retval][out] */ LONG *plVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATSEventSubscriptionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IATSEventSubscription * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IATSEventSubscription * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IATSEventSubscription * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IATSEventSubscription * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IATSEventSubscription * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IATSEventSubscription * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IATSEventSubscription * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *startSubscription )( 
            IATSEventSubscription * This,
            /* [in] */ IATSFilterCriteria *pfilterCriteria,
            /* [in] */ LONG backloglength,
            /* [out][in] */ IATSEventList **pATSEvents,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *stopSubscription )( 
            IATSEventSubscription * This,
            /* [retval][out] */ LONG *plVal);
        
        END_INTERFACE
    } IATSEventSubscriptionVtbl;

    interface IATSEventSubscription
    {
        CONST_VTBL struct IATSEventSubscriptionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATSEventSubscription_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IATSEventSubscription_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IATSEventSubscription_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IATSEventSubscription_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IATSEventSubscription_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IATSEventSubscription_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IATSEventSubscription_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IATSEventSubscription_startSubscription(This,pfilterCriteria,backloglength,pATSEvents,pRetVal)	\
    ( (This)->lpVtbl -> startSubscription(This,pfilterCriteria,backloglength,pATSEvents,pRetVal) ) 

#define IATSEventSubscription_stopSubscription(This,plVal)	\
    ( (This)->lpVtbl -> stopSubscription(This,plVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IATSEventSubscription_INTERFACE_DEFINED__ */



#ifndef __ATSBridgeUtilLib_LIBRARY_DEFINED__
#define __ATSBridgeUtilLib_LIBRARY_DEFINED__

/* library ATSBridgeUtilLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ATSBridgeUtilLib;

EXTERN_C const CLSID CLSID_ATSIntList;

#ifdef __cplusplus

class DECLSPEC_UUID("78F5E6DA-3BE1-4066-911C-3E2D5EEC984F")
ATSIntList;
#endif

EXTERN_C const CLSID CLSID_ATSStringList;

#ifdef __cplusplus

class DECLSPEC_UUID("77C5CBB3-7529-4A6E-9522-2E28975FA043")
ATSStringList;
#endif

EXTERN_C const CLSID CLSID_ATSEvent;

#ifdef __cplusplus

class DECLSPEC_UUID("AA1D6690-6FDA-4B5E-986D-9387DB65B80E")
ATSEvent;
#endif

EXTERN_C const CLSID CLSID_ATSDateTime;

#ifdef __cplusplus

class DECLSPEC_UUID("8E89F9A8-DBB2-4B6B-A524-CE6ECE715EDE")
ATSDateTime;
#endif

EXTERN_C const CLSID CLSID_ATSEventList;

#ifdef __cplusplus

class DECLSPEC_UUID("C68EFDF1-91D1-4DB4-9698-E28CDECC69F5")
ATSEventList;
#endif

EXTERN_C const CLSID CLSID_ATSEventSystem;

#ifdef __cplusplus

class DECLSPEC_UUID("C988D859-E440-4797-AF37-2F32DE9840BF")
ATSEventSystem;
#endif

EXTERN_C const CLSID CLSID_ATSEventSystemList;

#ifdef __cplusplus

class DECLSPEC_UUID("65203707-C1FA-45AE-9492-E77B2CB9D8E7")
ATSEventSystemList;
#endif

EXTERN_C const CLSID CLSID_ATSFilterCriteria;

#ifdef __cplusplus

class DECLSPEC_UUID("45A9A577-30B8-4DC1-B3F0-737CCD0E6557")
ATSFilterCriteria;
#endif

EXTERN_C const CLSID CLSID_ATSBridgeUtility;

#ifdef __cplusplus

class DECLSPEC_UUID("23B16F05-40CA-47B2-8FB4-9DE37C67CADE")
ATSBridgeUtility;
#endif

#ifndef ___IATSEventSubscriptionEvents_DISPINTERFACE_DEFINED__
#define ___IATSEventSubscriptionEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IATSEventSubscriptionEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IATSEventSubscriptionEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3EC07762-AE4B-4B52-8D4F-9BE37460143E")
    _IATSEventSubscriptionEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IATSEventSubscriptionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IATSEventSubscriptionEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IATSEventSubscriptionEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IATSEventSubscriptionEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IATSEventSubscriptionEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IATSEventSubscriptionEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IATSEventSubscriptionEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IATSEventSubscriptionEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IATSEventSubscriptionEventsVtbl;

    interface _IATSEventSubscriptionEvents
    {
        CONST_VTBL struct _IATSEventSubscriptionEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IATSEventSubscriptionEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IATSEventSubscriptionEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IATSEventSubscriptionEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IATSEventSubscriptionEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IATSEventSubscriptionEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IATSEventSubscriptionEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IATSEventSubscriptionEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IATSEventSubscriptionEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ATSEventSubscription;

#ifdef __cplusplus

class DECLSPEC_UUID("DF8AE25F-804E-48FE-9C0D-400F0A25FA49")
ATSEventSubscription;
#endif
#endif /* __ATSBridgeUtilLib_LIBRARY_DEFINED__ */

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


