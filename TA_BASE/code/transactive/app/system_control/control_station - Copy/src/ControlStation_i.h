

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Wed Apr 15 11:32:48 2015
 */
/* Compiler settings for .\src\ControlStation.idl:
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


#ifndef __ControlStation_i_h__
#define __ControlStation_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IControlStation2_FWD_DEFINED__
#define __IControlStation2_FWD_DEFINED__
typedef interface IControlStation2 IControlStation2;
#endif 	/* __IControlStation2_FWD_DEFINED__ */


#ifndef ___IControlStation2Events_FWD_DEFINED__
#define ___IControlStation2Events_FWD_DEFINED__
typedef interface _IControlStation2Events _IControlStation2Events;
#endif 	/* ___IControlStation2Events_FWD_DEFINED__ */


#ifndef __ControlStation2_FWD_DEFINED__
#define __ControlStation2_FWD_DEFINED__

#ifdef __cplusplus
typedef class ControlStation2 ControlStation2;
#else
typedef struct ControlStation2 ControlStation2;
#endif /* __cplusplus */

#endif 	/* __ControlStation2_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_ControlStation_0000_0000 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_ControlStation_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ControlStation_0000_0000_v0_0_s_ifspec;


#ifndef __ControlStationLib_LIBRARY_DEFINED__
#define __ControlStationLib_LIBRARY_DEFINED__

/* library ControlStationLib */
/* [helpstring][version][uuid] */ 

typedef /* [public][public][helpstring] */ 
enum __MIDL___MIDL_itf_ControlStation_0000_0000_0001
    {	SCREEN_CURRENT	= 0,
	SCREEN_PREVIOUS	= ( SCREEN_CURRENT + 1 ) ,
	SCREEN_NEXT	= ( SCREEN_PREVIOUS + 1 ) ,
	SCREEN_SPECIFIC	= ( SCREEN_NEXT + 1 ) ,
	SCREEN_FIRST	= ( SCREEN_SPECIFIC + 1 ) ,
	SCREEN_LAST	= ( SCREEN_FIRST + 1 ) ,
	SCREEN_ALL	= ( SCREEN_LAST + 1 ) 
    } 	EScreen;

typedef /* [public][public][helpstring] */ 
enum __MIDL___MIDL_itf_ControlStation_0000_0000_0002
    {	AREA_SCREEN	= 0,
	AREA_BANNER	= ( AREA_SCREEN + 1 ) ,
	AREA_LAUNCHPAD	= ( AREA_BANNER + 1 ) ,
	AREA_MAINNAVIGATOR	= ( AREA_LAUNCHPAD + 1 ) ,
	AREA_SCHEMATIC	= ( AREA_MAINNAVIGATOR + 1 ) 
    } 	EArea;

typedef /* [public][helpstring] */ 
enum __MIDL___MIDL_itf_ControlStation_0000_0000_0003
    {	POS_RELATIVE	= 0x1,
	POS_BOUNDED	= 0x2,
	POS_NOOVERLAP	= 0x4,
	POS_RESIZABLE	= 0x8
    } 	EPosition;

typedef /* [public][helpstring] */ 
enum __MIDL___MIDL_itf_ControlStation_0000_0000_0004
    {	ALIGN_FIT	= 0x1,
	ALIGN_LEFT	= 0x2,
	ALIGN_VERTICAL	= 0x4,
	ALIGN_RIGHT	= 0x8,
	ALIGN_TOP	= 0x10,
	ALIGN_HORIZONTAL	= 0x20,
	ALIGN_BOTTOM	= 0x40
    } 	EAlignment;

typedef /* [public][public][helpstring] */ 
enum __MIDL___MIDL_itf_ControlStation_0000_0000_0005
    {	Permit	= 0,
	Deny	= ( Permit + 1 ) ,
	Indeterminate	= ( Deny + 1 ) ,
	TAError	= ( Indeterminate + 1 ) 
    } 	ResponseEnum;

typedef /* [public][public][helpstring] */ 
enum __MIDL___MIDL_itf_ControlStation_0000_0000_0006
    {	All	= 0,
	RadioAgent	= ( All + 1 ) ,
	ControlStation	= ( RadioAgent + 1 ) 
    } 	EntityMessageType;


EXTERN_C const IID LIBID_ControlStationLib;

#ifndef __IControlStation2_INTERFACE_DEFINED__
#define __IControlStation2_INTERFACE_DEFINED__

/* interface IControlStation2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IControlStation2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("037EDEC6-80F2-11D7-B177-0050BAB1D931")
    IControlStation2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE logout( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE giveApplicationFocus( 
            long processId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getOperator( 
            /* [retval][out] */ BSTR *__MIDL__IControlStation20000) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getSite( 
            /* [retval][out] */ BSTR *__MIDL__IControlStation20001) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getProfile( 
            /* [retval][out] */ BSTR *__MIDL__IControlStation20002) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE launchApplication( 
            long appType,
            long leftPosition,
            long topPosition,
            long width,
            long height,
            BSTR *addtionalCommands) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE launchApplicationEx( 
            long appType,
            BSTR *extraCommandLine,
            long posType,
            long alignType,
            RECT *objectDim,
            RECT *boundaryDim) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getAllApplications( 
            /* [retval][out] */ VARIANT *__MIDL__IControlStation20003) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getRunningApplications( 
            /* [retval][out] */ VARIANT *__MIDL__IControlStation20004) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getLocation( 
            /* [retval][out] */ BSTR *__MIDL__IControlStation20005) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setSelectedAlarms( 
            BSTR *asset,
            long leftPosition) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getProfileDisplay( 
            long leftPosition,
            /* [retval][out] */ BSTR *__MIDL__IControlStation20006) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE changeProfile( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE isServerContactable( 
            /* [retval][out] */ BOOL *isContactable) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE displayTaskManager( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE changeOperatorOverride( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE isOperatorOverridden( 
            /* [retval][out] */ BOOL *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE changePassword( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE launchDisplayEx( 
            BSTR display,
            BSTR asset,
            long leftPosition) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setDisplayDimension( 
            RECT *dim) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getRect( 
            EScreen targetScreen,
            EArea targetArea,
            long val,
            /* [retval][out] */ RECT *dim) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getCurrentDisplayName( 
            /* [retval][out] */ BSTR *display) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE launchDisplay( 
            BSTR display,
            long leftPosition) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE loadComplete( 
            long leftPosition) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getSessionId( 
            /* [retval][out] */ BSTR *sessionId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE launchHelp( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE launchPlan( 
            long planId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE isActionPermittedOnEntity( 
            BSTR entityName,
            long actionKey,
            /* [retval][out] */ ResponseEnum *response) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE launchPlanByName( 
            BSTR planPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE isLoginAllowed( 
            BSTR bstrOperator,
            BSTR bstrPassword,
            /* [retval][out] */ BOOL *bLogin) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE entityCommand( 
            EntityMessageType entityType,
            BSTR location,
            BSTR sourceSchematicID,
            BSTR message) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getProfileDisplayAtLocation( 
            long leftPosition,
            BSTR locationName,
            /* [retval][out] */ BSTR *__MIDL__IControlStation20007) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IControlStation2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IControlStation2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IControlStation2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IControlStation2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IControlStation2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IControlStation2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IControlStation2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IControlStation2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *logout )( 
            IControlStation2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *giveApplicationFocus )( 
            IControlStation2 * This,
            long processId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getOperator )( 
            IControlStation2 * This,
            /* [retval][out] */ BSTR *__MIDL__IControlStation20000);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getSite )( 
            IControlStation2 * This,
            /* [retval][out] */ BSTR *__MIDL__IControlStation20001);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getProfile )( 
            IControlStation2 * This,
            /* [retval][out] */ BSTR *__MIDL__IControlStation20002);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *launchApplication )( 
            IControlStation2 * This,
            long appType,
            long leftPosition,
            long topPosition,
            long width,
            long height,
            BSTR *addtionalCommands);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *launchApplicationEx )( 
            IControlStation2 * This,
            long appType,
            BSTR *extraCommandLine,
            long posType,
            long alignType,
            RECT *objectDim,
            RECT *boundaryDim);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getAllApplications )( 
            IControlStation2 * This,
            /* [retval][out] */ VARIANT *__MIDL__IControlStation20003);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getRunningApplications )( 
            IControlStation2 * This,
            /* [retval][out] */ VARIANT *__MIDL__IControlStation20004);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getLocation )( 
            IControlStation2 * This,
            /* [retval][out] */ BSTR *__MIDL__IControlStation20005);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setSelectedAlarms )( 
            IControlStation2 * This,
            BSTR *asset,
            long leftPosition);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getProfileDisplay )( 
            IControlStation2 * This,
            long leftPosition,
            /* [retval][out] */ BSTR *__MIDL__IControlStation20006);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *changeProfile )( 
            IControlStation2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *isServerContactable )( 
            IControlStation2 * This,
            /* [retval][out] */ BOOL *isContactable);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *displayTaskManager )( 
            IControlStation2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *changeOperatorOverride )( 
            IControlStation2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *isOperatorOverridden )( 
            IControlStation2 * This,
            /* [retval][out] */ BOOL *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *changePassword )( 
            IControlStation2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *launchDisplayEx )( 
            IControlStation2 * This,
            BSTR display,
            BSTR asset,
            long leftPosition);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *setDisplayDimension )( 
            IControlStation2 * This,
            RECT *dim);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getRect )( 
            IControlStation2 * This,
            EScreen targetScreen,
            EArea targetArea,
            long val,
            /* [retval][out] */ RECT *dim);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getCurrentDisplayName )( 
            IControlStation2 * This,
            /* [retval][out] */ BSTR *display);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *launchDisplay )( 
            IControlStation2 * This,
            BSTR display,
            long leftPosition);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *loadComplete )( 
            IControlStation2 * This,
            long leftPosition);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getSessionId )( 
            IControlStation2 * This,
            /* [retval][out] */ BSTR *sessionId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *launchHelp )( 
            IControlStation2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *launchPlan )( 
            IControlStation2 * This,
            long planId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *isActionPermittedOnEntity )( 
            IControlStation2 * This,
            BSTR entityName,
            long actionKey,
            /* [retval][out] */ ResponseEnum *response);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *launchPlanByName )( 
            IControlStation2 * This,
            BSTR planPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *isLoginAllowed )( 
            IControlStation2 * This,
            BSTR bstrOperator,
            BSTR bstrPassword,
            /* [retval][out] */ BOOL *bLogin);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *entityCommand )( 
            IControlStation2 * This,
            EntityMessageType entityType,
            BSTR location,
            BSTR sourceSchematicID,
            BSTR message);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getProfileDisplayAtLocation )( 
            IControlStation2 * This,
            long leftPosition,
            BSTR locationName,
            /* [retval][out] */ BSTR *__MIDL__IControlStation20007);
        
        END_INTERFACE
    } IControlStation2Vtbl;

    interface IControlStation2
    {
        CONST_VTBL struct IControlStation2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IControlStation2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IControlStation2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IControlStation2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IControlStation2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IControlStation2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IControlStation2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IControlStation2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IControlStation2_logout(This)	\
    ( (This)->lpVtbl -> logout(This) ) 

#define IControlStation2_giveApplicationFocus(This,processId)	\
    ( (This)->lpVtbl -> giveApplicationFocus(This,processId) ) 

#define IControlStation2_getOperator(This,__MIDL__IControlStation20000)	\
    ( (This)->lpVtbl -> getOperator(This,__MIDL__IControlStation20000) ) 

#define IControlStation2_getSite(This,__MIDL__IControlStation20001)	\
    ( (This)->lpVtbl -> getSite(This,__MIDL__IControlStation20001) ) 

#define IControlStation2_getProfile(This,__MIDL__IControlStation20002)	\
    ( (This)->lpVtbl -> getProfile(This,__MIDL__IControlStation20002) ) 

#define IControlStation2_launchApplication(This,appType,leftPosition,topPosition,width,height,addtionalCommands)	\
    ( (This)->lpVtbl -> launchApplication(This,appType,leftPosition,topPosition,width,height,addtionalCommands) ) 

#define IControlStation2_launchApplicationEx(This,appType,extraCommandLine,posType,alignType,objectDim,boundaryDim)	\
    ( (This)->lpVtbl -> launchApplicationEx(This,appType,extraCommandLine,posType,alignType,objectDim,boundaryDim) ) 

#define IControlStation2_getAllApplications(This,__MIDL__IControlStation20003)	\
    ( (This)->lpVtbl -> getAllApplications(This,__MIDL__IControlStation20003) ) 

#define IControlStation2_getRunningApplications(This,__MIDL__IControlStation20004)	\
    ( (This)->lpVtbl -> getRunningApplications(This,__MIDL__IControlStation20004) ) 

#define IControlStation2_getLocation(This,__MIDL__IControlStation20005)	\
    ( (This)->lpVtbl -> getLocation(This,__MIDL__IControlStation20005) ) 

#define IControlStation2_setSelectedAlarms(This,asset,leftPosition)	\
    ( (This)->lpVtbl -> setSelectedAlarms(This,asset,leftPosition) ) 

#define IControlStation2_getProfileDisplay(This,leftPosition,__MIDL__IControlStation20006)	\
    ( (This)->lpVtbl -> getProfileDisplay(This,leftPosition,__MIDL__IControlStation20006) ) 

#define IControlStation2_changeProfile(This)	\
    ( (This)->lpVtbl -> changeProfile(This) ) 

#define IControlStation2_isServerContactable(This,isContactable)	\
    ( (This)->lpVtbl -> isServerContactable(This,isContactable) ) 

#define IControlStation2_displayTaskManager(This)	\
    ( (This)->lpVtbl -> displayTaskManager(This) ) 

#define IControlStation2_changeOperatorOverride(This)	\
    ( (This)->lpVtbl -> changeOperatorOverride(This) ) 

#define IControlStation2_isOperatorOverridden(This,pRet)	\
    ( (This)->lpVtbl -> isOperatorOverridden(This,pRet) ) 

#define IControlStation2_changePassword(This)	\
    ( (This)->lpVtbl -> changePassword(This) ) 

#define IControlStation2_launchDisplayEx(This,display,asset,leftPosition)	\
    ( (This)->lpVtbl -> launchDisplayEx(This,display,asset,leftPosition) ) 

#define IControlStation2_setDisplayDimension(This,dim)	\
    ( (This)->lpVtbl -> setDisplayDimension(This,dim) ) 

#define IControlStation2_getRect(This,targetScreen,targetArea,val,dim)	\
    ( (This)->lpVtbl -> getRect(This,targetScreen,targetArea,val,dim) ) 

#define IControlStation2_getCurrentDisplayName(This,display)	\
    ( (This)->lpVtbl -> getCurrentDisplayName(This,display) ) 

#define IControlStation2_launchDisplay(This,display,leftPosition)	\
    ( (This)->lpVtbl -> launchDisplay(This,display,leftPosition) ) 

#define IControlStation2_loadComplete(This,leftPosition)	\
    ( (This)->lpVtbl -> loadComplete(This,leftPosition) ) 

#define IControlStation2_getSessionId(This,sessionId)	\
    ( (This)->lpVtbl -> getSessionId(This,sessionId) ) 

#define IControlStation2_launchHelp(This)	\
    ( (This)->lpVtbl -> launchHelp(This) ) 

#define IControlStation2_launchPlan(This,planId)	\
    ( (This)->lpVtbl -> launchPlan(This,planId) ) 

#define IControlStation2_isActionPermittedOnEntity(This,entityName,actionKey,response)	\
    ( (This)->lpVtbl -> isActionPermittedOnEntity(This,entityName,actionKey,response) ) 

#define IControlStation2_launchPlanByName(This,planPath)	\
    ( (This)->lpVtbl -> launchPlanByName(This,planPath) ) 

#define IControlStation2_isLoginAllowed(This,bstrOperator,bstrPassword,bLogin)	\
    ( (This)->lpVtbl -> isLoginAllowed(This,bstrOperator,bstrPassword,bLogin) ) 

#define IControlStation2_entityCommand(This,entityType,location,sourceSchematicID,message)	\
    ( (This)->lpVtbl -> entityCommand(This,entityType,location,sourceSchematicID,message) ) 

#define IControlStation2_getProfileDisplayAtLocation(This,leftPosition,locationName,__MIDL__IControlStation20007)	\
    ( (This)->lpVtbl -> getProfileDisplayAtLocation(This,leftPosition,locationName,__MIDL__IControlStation20007) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IControlStation2_INTERFACE_DEFINED__ */


#ifndef ___IControlStation2Events_DISPINTERFACE_DEFINED__
#define ___IControlStation2Events_DISPINTERFACE_DEFINED__

/* dispinterface _IControlStation2Events */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IControlStation2Events;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("553D8699-9A66-4CDB-9D95-77EAA2C86929")
    _IControlStation2Events : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IControlStation2EventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IControlStation2Events * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IControlStation2Events * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IControlStation2Events * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IControlStation2Events * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IControlStation2Events * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IControlStation2Events * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IControlStation2Events * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IControlStation2EventsVtbl;

    interface _IControlStation2Events
    {
        CONST_VTBL struct _IControlStation2EventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IControlStation2Events_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IControlStation2Events_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IControlStation2Events_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IControlStation2Events_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IControlStation2Events_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IControlStation2Events_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IControlStation2Events_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IControlStation2Events_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ControlStation2;

#ifdef __cplusplus

class DECLSPEC_UUID("037EDEC7-80F2-11D7-B177-0050BAB1D931")
ControlStation2;
#endif
#endif /* __ControlStationLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


