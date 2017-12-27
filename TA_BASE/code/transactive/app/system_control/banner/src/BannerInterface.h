

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Wed Apr 15 11:43:56 2015
 */
/* Compiler settings for .\src\banner.idl:
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

#ifndef __BannerInterface_h__
#define __BannerInterface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IIscsBanner_FWD_DEFINED__
#define __IIscsBanner_FWD_DEFINED__
typedef interface IIscsBanner IIscsBanner;
#endif 	/* __IIscsBanner_FWD_DEFINED__ */


#ifndef __IscsBanner_FWD_DEFINED__
#define __IscsBanner_FWD_DEFINED__

#ifdef __cplusplus
typedef class IscsBanner IscsBanner;
#else
typedef struct IscsBanner IscsBanner;
#endif /* __cplusplus */

#endif 	/* __IscsBanner_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IIscsBanner_INTERFACE_DEFINED__
#define __IIscsBanner_INTERFACE_DEFINED__

/* interface IIscsBanner */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IIscsBanner;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A8E00983-D070-433A-8320-CE1B870186C4")
    IIscsBanner : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setAtsButtonState( 
            BOOL flash) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIscsBannerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIscsBanner * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIscsBanner * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIscsBanner * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IIscsBanner * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IIscsBanner * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IIscsBanner * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IIscsBanner * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setAtsButtonState )( 
            IIscsBanner * This,
            BOOL flash);
        
        END_INTERFACE
    } IIscsBannerVtbl;

    interface IIscsBanner
    {
        CONST_VTBL struct IIscsBannerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIscsBanner_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIscsBanner_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIscsBanner_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIscsBanner_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IIscsBanner_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IIscsBanner_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IIscsBanner_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IIscsBanner_setAtsButtonState(This,flash)	\
    ( (This)->lpVtbl -> setAtsButtonState(This,flash) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIscsBanner_INTERFACE_DEFINED__ */



#ifndef __RippleBannerLib_LIBRARY_DEFINED__
#define __RippleBannerLib_LIBRARY_DEFINED__

/* library RippleBannerLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_RippleBannerLib;

EXTERN_C const CLSID CLSID_IscsBanner;

#ifdef __cplusplus

class DECLSPEC_UUID("FB4B1B7F-EBAB-40BE-9EF7-A2F77207937F")
IscsBanner;
#endif
#endif /* __RippleBannerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


