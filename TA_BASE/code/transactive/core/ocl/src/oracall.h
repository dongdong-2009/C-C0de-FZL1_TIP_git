/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\oracall.h
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:22 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  OCI
//  Last modified:      28.08.99
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __OCI_H__
#define __OCI_H__

#include <vector>
#include "crdef.h"

#if !defined(OCL_OCI7) && !defined(OCL_OCI8) && !defined(OCL_OCI81) && !defined(OCL_OCI90) && !defined(OCL_OCI100) && !defined(OCL_DYNAMIC)
#define OCL_DYNAMIC
#endif

#if defined(OCL_OCI81)
#define OCL_OCI8
#endif

#if defined(OCL_OCI90)
#define OCL_OCI8
#define OCL_OCI81
#endif

#if defined(OCL_OCI100)
#define OCL_OCI8
#define OCL_OCI81
#define OCL_OCI90
#endif

#if defined(OCL_OCI7) && defined(OCL_OCI8)
#error "OCL_OCI7 and OCL_OCI8 can not be defined together"
#endif

#if defined(OCL_OCI7)
extern "C" {
#include <oratypes.h>
#include <ocidfn.h>
#include <ociapr.h>
}
#endif

#if defined(OCL_OCI8)
extern "C" {
#include <oci.h>
}
#endif

#ifndef OCL_DYNAMIC
namespace ocl {
#endif
	
#ifdef OCL_DYNAMIC
	
	// Try to determine OCI version
#include "ocidfn.h"
	
#define OCL_OCI7
	
#if defined(SQLT_CLOB)         // Oracle8
#define OCL_OCI8
	
#if defined(SQLT_TIMESTAMP)  // Oracle8i
#define OCL_OCI81
	
#if defined(SQLT_PNTY)
#define OCL_OCI90        // Oracle9i
	
#if defined(SQLT_IBFLOAT)// Oracle10G  
#define OCL_OCI100
#endif
#endif
#endif
#endif
	
#ifdef OCL_OCI8
	typedef struct OCIExtProcContext OCIExtProcContext;
	
#define OCIAP_ORACLE
#define OCIAPR
#define OCIEXTP_ORACLE
	
#ifdef OCL_LINUX
#define OCI8DP_ORACLE
#endif
	
	//to support solaris, haipeng modified
#ifdef OCL_SOLARIS
#define OCI8DP_ORACLE
#endif
	//haipeng added  
	
	extern "C" {
#include "oci.h"
	}
#endif
	
	namespace ocl {
		
#ifdef OCL_OCI8
		
		//***********************OCI8 Types*****************************
		
		typedef sb4 (*OCICallbackInBind)(dvoid *ictxp, OCIBind *bindp, ub4 iter,
			ub4 index, dvoid **bufpp, ub4 *alenp,
			ub1 *piecep, dvoid **indp);
		
		typedef sb4 (*OCICallbackOutBind)(dvoid *octxp, OCIBind *bindp, ub4 iter,
			ub4 index, dvoid **bufpp, ub4 **alenp,
			ub1 *piecep, dvoid **indp,
			ub2 **rcodep);
		
		typedef sb4 (*OCICallbackDefine)(dvoid *octxp, OCIDefine *defnp, ub4 iter,
			dvoid **bufpp, ub4 **alenp, ub1 *piecep,
			dvoid **indp, ub2 **rcodep);
		
		
		typedef sword (*_OCIInitialize)(ub4 mode, dvoid *ctxp,
			dvoid *(*malocfp)(dvoid *ctxp, size_t size),
			dvoid *(*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize),
			void  (*mfreefp)(dvoid *ctxp, dvoid *memptr));
		
		typedef sword (*_OCITerminate)(ub4 mode);
		
		typedef sword (*_OCIEnvCreate)(OCIEnv **envp, ub4 mode, dvoid *ctxp,
			dvoid *(*malocfp)(dvoid *ctxp, size_t size),
			dvoid *(*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize),
			void   (*mfreefp)(dvoid *ctxp, dvoid *memptr),
			size_t xtramem_sz, dvoid **usrmempp);
		
		typedef sword (*_OCIHandleAlloc)(CONST dvoid *parenth, dvoid **hndlpp, CONST ub4 type, 
			CONST size_t xtramem_sz, dvoid **usrmempp);
		
		typedef sword (*_OCIHandleFree)(dvoid *hndlp, CONST ub4 type);
		
		typedef sword (*_OCIDescriptorAlloc)(CONST dvoid *parenth, dvoid **descpp, 
			CONST ub4 type, CONST size_t xtramem_sz, dvoid **usrmempp);
		
		typedef sword (*_OCIDescriptorFree)(dvoid *descp, CONST ub4 type);
		
		typedef sword (*_OCIEnvInit)(OCIEnv **envp, ub4 mode, size_t xtramem_sz, dvoid **usrmempp);
		
		typedef sword (*_OCIServerAttach)(OCIServer *srvhp, OCIError *errhp,
			CONST OraText *dblink, sb4 dblink_len, ub4 mode);
		
		typedef sword (*_OCIServerDetach)(OCIServer *srvhp, OCIError *errhp, ub4 mode);
		
		typedef sword (*_OCISessionBegin)(OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp,
			ub4 credt, ub4 mode);
		
		typedef sword (*_OCISessionEnd)(OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp, ub4 mode);
		
		typedef sword (*_OCIPasswordChange)(OCISvcCtx* svchp, OCIError* errhp, CONST OraText* user_name,
			ub4 usernm_len, CONST OraText* opasswd, ub4 opasswd_len, CONST OraText* npasswd,
			sb4 npasswd_len, ub4 mode);
		
		typedef sword (*_OCIServerVersion)(dvoid *hndlp, OCIError *errhp, text *bufp, ub4 bufsz, ub1 hndltype);
		
		typedef sword (*_OCISvcCtxToLda)(OCISvcCtx *srvhp, OCIError *errhp, Lda_Def *ldap);
		
		typedef sword (*_OCIStmtPrepare)(OCIStmt *stmtp, OCIError *errhp, CONST OraText *stmt,
			ub4 stmt_len, ub4 language, ub4 mode);
		
		typedef sword (*_OCIBindByName)(OCIStmt *stmtp, OCIBind **bindp, OCIError *errhp,
			CONST OraText *placeholder, sb4 placeh_len, 
			dvoid *valuep, sb4 value_sz, ub2 dty, 
			dvoid *indp, ub2 *alenp, ub2 *rcodep, 
			ub4 maxarr_len, ub4 *curelep, ub4 mode);
		
		typedef sword (*_OCIBindArrayOfStruct)(OCIBind *bindp, OCIError *errhp, 
			ub4 pvskip, ub4 indskip, ub4 alskip, ub4 rcskip);
		
		typedef sword (*_OCIBindDynamic)(OCIBind *bindp, OCIError *errhp, dvoid *ictxp,
			OCICallbackInBind icbfp, dvoid *octxp, OCICallbackOutBind ocbfp);
		
		typedef sword (*_OCIStmtGetPieceInfo)(OCIStmt *stmtp, OCIError *errhp, 
			dvoid **hndlpp, ub4 *typep, ub1 *in_outp, ub4 *iterp, ub4 *idxp, ub1 *piecep);
		
		typedef sword (*_OCIStmtSetPieceInfo)(dvoid *hndlp, ub4 type, OCIError *errhp, 
			CONST dvoid *bufp, ub4 *alenp, ub1 piece, CONST dvoid *indp, ub2 *rcodep);
		
		typedef sword (*_OCIStmtExecute)(OCISvcCtx *svchp, OCIStmt *stmtp, OCIError *errhp, 
			ub4 iters, ub4 rowoff, CONST OCISnapshot *snap_in, OCISnapshot *snap_out, ub4 mode);
		
		typedef sword (*_OCIDefineByPos)(OCIStmt *stmtp, OCIDefine **defnp, OCIError *errhp,
			ub4 position, dvoid *valuep, sb4 value_sz, ub2 dty,
			dvoid *indp, ub2 *rlenp, ub2 *rcodep, ub4 mode);
		
		typedef sword (*_OCIDefineDynamic)(OCIDefine *defnp, OCIError *errhp, dvoid *octxp,
			OCICallbackDefine ocbfp);
		
		typedef sword (*_OCIDefineArrayOfStruct)(OCIDefine *defnp, OCIError *errhp, ub4 pvskip,
			ub4 indskip, ub4 rlskip, ub4 rcskip);
		
		typedef sword (*_OCIStmtFetch)(OCIStmt *stmtp, OCIError *errhp, ub4 nrows, ub2 orientation, ub4 mode);
		
		typedef sword (*_OCIDescribeAny)(OCISvcCtx *svchp, OCIError *errhp, dvoid *objptr, 
			ub4 objnm_len, ub1 objptr_typ, ub1 info_level, ub1 objtyp, OCIDescribe *dschp);
		
		typedef sword (*_OCIParamGet)(CONST dvoid *hndlp, ub4 htype, OCIError *errhp, dvoid **parmdpp, ub4 pos);
		
		typedef sword (*_OCITransStart)(OCISvcCtx* svchp, OCIError* errhp, uword timeout, ub4 flags);
		
		typedef sword (*_OCITransPrepare)(OCISvcCtx* svchp, OCIError* errhp, ub4 flags);
		
		typedef sword (*_OCITransCommit)(OCISvcCtx *svchp, OCIError *errhp, ub4 flags);
		
		typedef sword (*_OCITransRollback)(OCISvcCtx *svchp, OCIError *errhp, ub4 flags);
		
		typedef sword (*_OCITransDetach)(OCISvcCtx* svchp, OCIError* errhp, ub4 flags);
		
		typedef sword (*_OCIErrorGet)(dvoid *hndlp, ub4 recordno, OraText *sqlstate,
			sb4 *errcodep, OraText *bufp, ub4 bufsiz, ub4 type);
		
		typedef sword (*_OCILobDisableBuffering)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp);
		
		typedef sword (*_OCILobEnableBuffering)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator  *locp);
		
		typedef sword (*_OCILobFileClose)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *filep);
		
		typedef sword (*_OCILobFileExists)(OCISvcCtx *svchp, OCIError *errhp,
			OCILobLocator *filep, boolean *flag);
		
		typedef sword (*_OCILobFileIsOpen)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *filep,
			boolean *flag);
		
		typedef sword (*_OCILobFileOpen)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *filep, ub1 mode);
		
		typedef sword (*_OCILobFileSetName)(OCIEnv *envhp, OCIError *errhp, OCILobLocator **filepp, OraText *dir_alias,
			ub2 d_length, OraText *filename, ub2 f_length);
		
		typedef sword (*_OCILobFileGetName)(OCIEnv *envhp, OCIError *errhp, CONST OCILobLocator *filep, 
			OraText *dir_alias, ub2 *d_length, OraText *filename, ub2 *f_length );
		
		typedef sword (*_OCILobGetLength)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp, ub4 *lenp);
		
		typedef sword (*_OCILobIsEqual)(OCIEnv *envhp, CONST OCILobLocator *x, CONST OCILobLocator *y, 
			boolean *is_equal);
		
		typedef sword (*_OCILobIsOpen)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp, boolean *flag);
		
		typedef sword (*_OCILobLocatorIsInit)(OCIEnv *envhp, OCIError *errhp, CONST OCILobLocator *locp,
			boolean *is_initialized);
		
		typedef sword (*_OCILobOpen)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp, ub1 mode);
		
		typedef sword (*_OCILobRead)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
			ub4 *amtp, ub4 offset, dvoid *bufp, ub4 bufl, 
			dvoid *ctxp, sb4 (*cbfp)(dvoid *ctxp, CONST dvoid *bufp, ub4 len, ub1 piece),
			ub2 csid, ub1 csfrm);
		
		typedef sword (*_OCILobTrim)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
			ub4 newlen);
		
		typedef sword (*_OCILobWrite)(OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
			ub4 *amtp, ub4 offset, dvoid *bufp, ub4 buflen, 
			ub1 piece, dvoid *ctxp, 
			sb4 (*cbfp)(dvoid *ctxp, dvoid *bufp, ub4 *len, ub1 *piece),
			ub2 csid, ub1 csfrm);
		
		typedef sword (*_OCIBreak)(dvoid *hndlp, OCIError *errhp);
		
		typedef sword (*_OCIAttrGet)(CONST dvoid *trgthndlp, ub4 trghndltyp, dvoid *attributep,
			ub4 *sizep, ub4 attrtype, OCIError *errhp);
		
		typedef sword (*_OCIAttrSet)(dvoid *trgthndlp, ub4 trghndltyp, dvoid *attributep,
			ub4 size, ub4 attrtype, OCIError *errhp);
		
		//extended procedure functions
		
		typedef sword (*_OCIExtProcGetEnv)(OCIExtProcContext *with_context, OCIEnv **envh, 
			OCISvcCtx **svch, OCIError **errh);
		
		typedef size_t (*_OCIExtProcRaiseExcpWithMsg)(OCIExtProcContext *with_context, int error_number,
			text *error_message, size_t len );
		
		typedef size_t (*_OCIExtProcRaiseExcp)(OCIExtProcContext *with_context, int error_number);
		
#ifdef OCL_OCI81
		
		// Direct Load API
		
#ifdef OCL_LINUX 
		typedef struct OCIDirPathCtx      OCIDirPathCtx;
		typedef struct OCIDirPathColArray OCIDirPathColArray;
		typedef struct OCIDirPathStream   OCIDirPathStream;
		typedef struct OCIDirPathDesc     OCIDirPathDesc;
		
#define OCI_DIRPATH_COL_COMPLETE 0
#define OCI_DIRPATH_COL_NULL     1
#endif
		
		//haipeng added
#ifdef OCL_SOLARIS
		typedef struct OCIDirPathCtx      OCIDirPathCtx;
		typedef struct OCIDirPathColArray OCIDirPathColArray;
		typedef struct OCIDirPathStream   OCIDirPathStream;
		typedef struct OCIDirPathDesc     OCIDirPathDesc;
		
#define OCI_DIRPATH_COL_COMPLETE 0
#define OCI_DIRPATH_COL_NULL     1
#endif
		//haipeng added  
		
		typedef sword (*_OCIDirPathFinish)(OCIDirPathCtx* dpctx, OCIError* errhp);
		
		typedef sword (*_OCIDirPathPrepare)(OCIDirPathCtx* dpctx, OCISvcCtx* svchp,
			OCIError* errhp);
		
		typedef sword (*_OCIDirPathLoadStream)(OCIDirPathCtx* dpctx, OCIDirPathStream* dpstr,
			OCIError* errhp);
		
		typedef sword (*_OCIDirPathColArrayEntrySet)(OCIDirPathColArray* dpca, OCIError* errhp,
			ub4 rownum, ub2 colIdx, ub1* cvalp, ub4 clen, ub1 cflg);
		
		typedef sword (*_OCIDirPathColArrayReset)(OCIDirPathColArray *dpca, OCIError *errhp);
		
		typedef sword (*_OCIDirPathColArrayToStream)(OCIDirPathColArray* dpca,
			OCIDirPathCtx* dpctx, OCIDirPathStream* dpstr, OCIError* errhp, 
			ub4 rowcnt, ub4 rowoff);
		
		typedef sword (*_OCIDirPathStreamReset)(OCIDirPathStream *dpstr, OCIError *errhp);
		
#endif // OCL_OCI81
		
#ifdef OCL_OCI90
		
		// OCIDateTime functions
		
		typedef sword (*_OCIDateTimeAssign)(dvoid* hndl, OCIError* err,
			CONST OCIDateTime* from, OCIDateTime* to);
		
		typedef sword (*_OCIDateTimeCheck)(dvoid* hndl, OCIError* err,
			CONST OCIDateTime* date, ub4* valid);
		
		typedef sword (*_OCIDateTimeCompare)(dvoid* hndl, OCIError* err,
			CONST OCIDateTime* date1, CONST OCIDateTime* date2, sword* result);
		
		typedef sword (*_OCIDateTimeConstruct)(dvoid* hndl, OCIError* err,
			OCIDateTime* datetime, sb2 yr, ub1 mnth, ub1 dy, ub1 hr, ub1 mm,
			ub1 ss, ub4 fsec, OraText* timezone, size_t timezone_length);
		
		typedef sword (*_OCIDateTimeConvert)(dvoid* hndl, OCIError* err,
			OCIDateTime* indate, OCIDateTime* outdate);
		
		typedef sword (*_OCIDateTimeFromArray)(dvoid* hndl, OCIError* err, ub1* inarray,
			ub4 len, ub1 type, OCIDateTime* datetime, CONST OCIInterval* reftz, ub1 fsprec);
		
		typedef sword (*_OCIDateTimeFromText)(dvoid* hndl, OCIError* err,
			CONST OraText* date_str, size_t dstr_length, CONST OraText* fmt,
			ub1 fmt_length, CONST OraText* lang_name, size_t lang_length, OCIDateTime* date);
		
		typedef sword (*_OCIDateTimeGetDate)(dvoid* hndl, OCIError* err,
			CONST OCIDateTime* date, sb2* yr, ub1* mnth, ub1* dy);
		
		typedef sword (*_OCIDateTimeGetTime)(dvoid* hndl, OCIError* err,
			OCIDateTime* datetime, ub1* hr, ub1* mm, ub1* ss, ub4* fsec);
		
		typedef sword (*_OCIDateTimeGetTimeZoneName)(dvoid* hndl, OCIError* err,
			CONST OCIDateTime* datetime, ub1* buf, ub4* buflen);
		
		typedef sword (*_OCIDateTimeGetTimeZoneOffset)(dvoid* hndl, OCIError* err,
			CONST OCIDateTime* datetime, sb1* hr, sb1* mm);
		
		typedef sword (*_OCIDateTimeIntervalAdd)(dvoid* hndl, OCIError* err,
			OCIDateTime* datetime, OCIInterval* inter, OCIDateTime* outdatetime);
		
		typedef sword (*_OCIDateTimeIntervalSub)(dvoid* hndl, OCIError* err,
			OCIDateTime* datetime, OCIInterval* inter, OCIDateTime* outdatetime);
		
		typedef sword (*_OCIDateTimeSubtract)(dvoid* hndl, OCIError* err,
			OCIDateTime* indate1, OCIDateTime* indate2, OCIInterval* inter);
		
		typedef sword (*_OCIDateTimeSysTimeStamp)(dvoid* hndl, OCIError* err, 
			OCIDateTime* sys_date);
		
		typedef sword (*_OCIDateTimeToArray)(dvoid* hndl, OCIError* err,
			CONST OCIDateTime* datetime, CONST OCIInterval* reftz, ub1* outarray,
			ub4* len, ub1 fsprec);
		
		typedef sword (*_OCIDateTimeToText)(dvoid* hndl, OCIError* err,
			CONST OCIDateTime* date, CONST OraText* fmt, ub1 fmt_length, ub1 fsprec, 
			CONST OraText* lang_name, size_t lang_length, ub4* buf_size, OraText* buf);
		
		// OCIInterval functions
		
		typedef sword (*_OCIIntervalAdd)(dvoid* hndl, OCIError* err, OCIInterval* addend1, 
			OCIInterval* addend2, OCIInterval* result);
		
		typedef sword (*_OCIIntervalAssign)(dvoid* hndl, OCIError* err,
			CONST OCIInterval* ininter, OCIInterval* outinter);
		
		typedef sword (*_OCIIntervalCheck)(dvoid* hndl, OCIError* err,
			CONST OCIInterval* interval, ub4* valid);
		
		typedef sword (*_OCIIntervalCompare)(dvoid* hndl, OCIError* err,
			OCIInterval* inter1, OCIInterval* inter2, sword* result);
		
		typedef sword (*_OCIIntervalDivide)(dvoid* hndl, OCIError* err,
			OCIInterval* dividend, OCINumber* divisor, OCIInterval* result);
		
		typedef sword (*_OCIIntervalFromNumber)(dvoid* hndl, OCIError* err,
			OCIInterval* inter, OCINumber* number);
		
		typedef sword (*_OCIIntervalFromText)(dvoid* hndl, OCIError* err,
			CONST OraText* inpstr, size_t str_len, OCIInterval* result);
		
		typedef sword (*_OCIIntervalFromTZ)(dvoid* hndl, OCIError* err,
			CONST oratext* inpstring, size_t str_len, OCIInterval* result);
		
		typedef sword (*_OCIIntervalGetDaySecond)(dvoid* hndl, OCIError* err,
			sb4* dy, sb4* hr, sb4* mm, sb4* ss, sb4* fsec, CONST OCIInterval* result);
		
		typedef sword (*_OCIIntervalGetYearMonth)(dvoid* hndl, OCIError* err,
			sb4* yr, sb4* mnth, CONST OCIInterval* result);
		
		typedef sword (*_OCIIntervalMultiply)(dvoid* hndl, OCIError* err,
			CONST OCIInterval* inter, OCINumber* nfactor, OCIInterval* result);
		
		typedef sword (*_OCIIntervalSetDaySecond)(dvoid* hndl, OCIError* err,
			sb4 dy, sb4 hr, sb4 mm, sb4 ss, sb4 fsec, OCIInterval* result);
		
		typedef sword (*_OCIIntervalSetYearMonth)(dvoid* hndl, OCIError* err,
			sb4 yr, sb4 mnth, OCIInterval* result);
		
		typedef sword (*_OCIIntervalSubtract)(dvoid* hndl, OCIError* err,
			OCIInterval* minuend, OCIInterval* subtrahend, OCIInterval* result);
		
		typedef sword (*_OCIIntervalToNumber)(dvoid* hndl, OCIError* err,
			CONST OCIInterval* inter, OCINumber* number);
		
		typedef sword (*_OCIIntervalToText)(dvoid* hndl, OCIError* err,
			CONST OCIInterval* inter, ub1 lfprec, ub1 fsprec, OraText* buffer,
			size_t buflen, size_t* resultlen);
		
#endif // OCL_OCI90
		
		
		extern _OCIInitialize OCIInitialize;
		extern _OCITerminate OCITerminate;
		extern _OCIEnvCreate OCIEnvCreate;
		extern _OCIHandleAlloc OCIHandleAlloc;
		extern _OCIHandleFree OCIHandleFree;
		extern _OCIDescriptorAlloc OCIDescriptorAlloc;
		extern _OCIDescriptorFree OCIDescriptorFree;
		extern _OCIEnvInit OCIEnvInit;
		extern _OCIServerAttach OCIServerAttach;
		extern _OCIServerDetach OCIServerDetach;
		extern _OCISessionBegin OCISessionBegin;
		extern _OCISessionEnd OCISessionEnd;
		extern _OCIPasswordChange OCIPasswordChange;
		extern _OCIServerVersion OCIServerVersion;
		extern _OCISvcCtxToLda OCISvcCtxToLda;
		extern _OCIStmtPrepare OCIStmtPrepare;
		extern _OCIBindByName OCIBindByName;
		extern _OCIBindDynamic OCIBindDynamic;
		extern _OCIBindArrayOfStruct OCIBindArrayOfStruct;
		extern _OCIStmtGetPieceInfo OCIStmtGetPieceInfo;
		extern _OCIStmtSetPieceInfo OCIStmtSetPieceInfo;
		extern _OCIStmtExecute OCIStmtExecute;
		extern _OCIDefineByPos OCIDefineByPos;
		extern _OCIDefineDynamic OCIDefineDynamic;
		extern _OCIDefineArrayOfStruct OCIDefineArrayOfStruct;
		extern _OCIStmtFetch OCIStmtFetch;
		extern _OCIDescribeAny OCIDescribeAny;
		extern _OCIParamGet OCIParamGet;
		extern _OCITransStart OCITransStart;
		extern _OCITransPrepare OCITransPrepare;
		extern _OCITransCommit OCITransCommit;
		extern _OCITransRollback OCITransRollback;
		extern _OCITransDetach OCITransDetach;
		extern _OCIErrorGet OCIErrorGet;
		extern _OCILobDisableBuffering OCILobDisableBuffering;
		extern _OCILobEnableBuffering OCILobEnableBuffering;
		extern _OCILobFileClose OCILobFileClose;
		extern _OCILobFileExists OCILobFileExists;
		extern _OCILobFileIsOpen OCILobFileIsOpen;
		extern _OCILobFileOpen OCILobFileOpen;
		extern _OCILobFileSetName OCILobFileSetName;
		extern _OCILobFileGetName OCILobFileGetName;
		extern _OCILobGetLength OCILobGetLength;
		extern _OCILobIsEqual OCILobIsEqual;
		extern _OCILobIsOpen OCILobIsOpen;
		extern _OCILobLocatorIsInit OCILobLocatorIsInit;
		extern _OCILobOpen OCILobOpen;
		extern _OCILobRead OCILobRead;
		extern _OCILobTrim OCILobTrim;
		extern _OCILobWrite OCILobWrite;
		extern _OCIBreak OCIBreak;
		extern _OCIAttrGet OCIAttrGet;
		extern _OCIAttrSet OCIAttrSet;
		
		//extended procedure functions
		
		extern _OCIExtProcGetEnv OCIExtProcGetEnv;
		extern _OCIExtProcRaiseExcpWithMsg OCIExtProcRaiseExcpWithMsg;
		extern _OCIExtProcRaiseExcp OCIExtProcRaiseExcp;
		
#ifdef OCL_OCI81
		
		// Direct Load API
		
		extern _OCIDirPathFinish OCIDirPathFinish;
		extern _OCIDirPathPrepare OCIDirPathPrepare;
		extern _OCIDirPathLoadStream OCIDirPathLoadStream;
		extern _OCIDirPathColArrayEntrySet OCIDirPathColArrayEntrySet;
		extern _OCIDirPathColArrayReset OCIDirPathColArrayReset;
		extern _OCIDirPathColArrayToStream OCIDirPathColArrayToStream;
		extern _OCIDirPathStreamReset OCIDirPathStreamReset;
		
#endif // OCL_OCI81
		
#ifdef OCL_OCI90
		
		// OCIDateTime functions
		
		extern _OCIDateTimeAssign OCIDateTimeAssign;
		extern _OCIDateTimeCheck OCIDateTimeCheck;
		extern _OCIDateTimeCompare OCIDateTimeCompare;
		extern _OCIDateTimeConstruct OCIDateTimeConstruct;
		extern _OCIDateTimeConvert OCIDateTimeConvert;
		extern _OCIDateTimeFromArray OCIDateTimeFromArray;
		extern _OCIDateTimeFromText OCIDateTimeFromText;
		extern _OCIDateTimeGetDate OCIDateTimeGetDate;
		extern _OCIDateTimeGetTime OCIDateTimeGetTime;
		extern _OCIDateTimeGetTimeZoneName OCIDateTimeGetTimeZoneName;
		extern _OCIDateTimeGetTimeZoneOffset OCIDateTimeGetTimeZoneOffset;
		extern _OCIDateTimeIntervalAdd OCIDateTimeIntervalAdd;
		extern _OCIDateTimeIntervalSub OCIDateTimeIntervalSub;
		extern _OCIDateTimeSubtract OCIDateTimeSubtract;
		extern _OCIDateTimeSysTimeStamp OCIDateTimeSysTimeStamp;
		extern _OCIDateTimeToArray OCIDateTimeToArray;
		extern _OCIDateTimeToText OCIDateTimeToText;
		
		// OCIInterval functions
		
		extern _OCIIntervalAdd OCIIntervalAdd;
		extern _OCIIntervalAssign OCIIntervalAssign;
		extern _OCIIntervalCheck OCIIntervalCheck;
		extern _OCIIntervalCompare OCIIntervalCompare;
		extern _OCIIntervalDivide OCIIntervalDivide;
		extern _OCIIntervalFromNumber OCIIntervalFromNumber;
		extern _OCIIntervalFromText OCIIntervalFromText;
		extern _OCIIntervalFromTZ OCIIntervalFromTZ;
		extern _OCIIntervalGetDaySecond OCIIntervalGetDaySecond;
		extern _OCIIntervalGetYearMonth OCIIntervalGetYearMonth;
		extern _OCIIntervalMultiply OCIIntervalMultiply;
		extern _OCIIntervalSetDaySecond OCIIntervalSetDaySecond;
		extern _OCIIntervalSetYearMonth OCIIntervalSetYearMonth;
		extern _OCIIntervalSubtract OCIIntervalSubtract;
		extern _OCIIntervalToNumber OCIIntervalToNumber;
		extern _OCIIntervalToText OCIIntervalToText;
		
#endif // OCL_OCI90
		
#endif // OCL_OCI8
		
		// ************************OCI7 Types*************************
		
		typedef sword (*_obindps)(struct cda_def *cursor, ub1 opcode, text *sqlvar, 
			sb4 sqlvl, ub1 *pvctx, sb4 progvl, 
			sword ftype, sword scale,
			sb2 *indp, ub2 *alen, ub2 *arcode, 
			sb4 pv_skip, sb4 ind_skip, sb4 alen_skip, sb4 rc_skip,
			ub4 maxsiz, ub4 *cursiz,
			text *fmt, sb4 fmtl, sword fmtt);
		
		typedef sword (*_obreak)(struct cda_def *lda);
		
		typedef sword (*_ocan)(struct cda_def *cursor);
		
		typedef sword (*_oclose)(struct cda_def *cursor);
		
		typedef sword (*_ocof)(struct cda_def *lda);
		
		typedef sword (*_ocom)(struct cda_def *lda);
		
		typedef sword (*_ocon)(struct cda_def *lda);
		
		/*
		* Oci DEFINe (Piecewise or with Skips) 
		*/
		typedef sword (*_odefinps)(struct cda_def *cursor, ub1 opcode, sword pos,ub1 *bufctx,
			sb4 bufl, sword ftype, sword scale, 
			sb2 *indp, text *fmt, sb4 fmtl, sword fmtt, 
			ub2 *rlen, ub2 *rcode,
			sb4 pv_skip, sb4 ind_skip, sb4 alen_skip, sb4 rc_skip);
		
		typedef sword (*_odessp)(struct cda_def *cursor, text *objnam, size_t onlen,
			ub1 *rsv1, size_t rsv1ln, ub1 *rsv2, size_t rsv2ln,
			ub2 *ovrld, ub2 *pos, ub2 *level, text **argnam,
			ub2 *arnlen, ub2 *dtype, ub1 *defsup, ub1* mode,
			ub4 *dtsiz, sb2 *prec, sb2 *scale, ub1 *radix,
			ub4 *spare, ub4 *arrsiz);
		
		typedef sword (*_odescr)(struct cda_def *cursor, sword pos, sb4 *dbsize,
			sb2 *dbtype, sb1 *cbuf, sb4 *cbufl, sb4 *dsize, sb2 *prec, sb2 *scale, sb2 *nullok);
		
		typedef sword (*_oerhms)(struct cda_def *lda, sb2 rcode, text *buf, sword bufsiz);
		
		typedef sword (*_oermsg)(sb2 rcode, text *buf);
		
		typedef sword (*_oexec)(struct cda_def *cursor);
		
		typedef sword (*_oexfet)(struct cda_def *cursor, ub4 nrows, sword cancel, sword exact);
		
		typedef sword (*_oexn)(struct cda_def *cursor, sword iters, sword rowoff);
		
		typedef sword (*_ofen)(struct cda_def *cursor, sword nrows);
		
		typedef sword (*_ofetch)(struct cda_def *cursor);
		
		typedef sword (*_oflng )(struct cda_def *cursor, sword pos, ub1 *buf,
			sb4 bufl, sword dtype, ub4 *retl, sb4 offset);
		
		typedef sword (*_ogetpi)(struct cda_def *cursor, ub1 *piecep, dvoid **ctxpp, 
			ub4 *iterp, ub4 *indexp);
		
		typedef sword (*_oopt)(struct cda_def *cursor, sword rbopt, sword waitopt);
		
		typedef sword (*_opinit)(ub4 mode);
		
		typedef sword  (*_olog)(struct cda_def *lda, ub1* hda, text *uid, sword uidl,
			text *pswd, sword pswdl, text *conn, sword connl, ub4 mode);
		
		typedef sword (*_ologof)(struct cda_def *lda);
		
		typedef sword (*_oopen)(struct cda_def *cursor, struct cda_def *lda,
			text *dbn, sword dbnl, sword arsize, text *uid, sword uidl);
		
		typedef sword (*_oparse)(struct cda_def *cursor, text *sqlstm, sb4 sqllen,
			sword defflg, ub4 lngflg);
		
		typedef sword (*_orol)(struct cda_def *lda);
		
		typedef sword (*_osetpi)(struct cda_def *cursor, ub1 piece, dvoid *bufp, ub4 *lenp);
		
		typedef void (*_sqlld2)(struct cda_def *lda, text *cname, sb4 *cnlen);
		
		typedef void (*_sqllda )(struct cda_def *lda);
		
		/* non-blocking functions */
		typedef sword (*_onbset)(struct cda_def *lda ); 
		
		typedef sword (*_onbtst)(struct cda_def *lda ); 
		
		typedef sword (*_onbclr)(struct cda_def *lda ); 
		
		typedef sword (*_ognfd)(struct cda_def *lda, dvoid *fdp);
		
		/* 
		* OBSOLETE CALLS 
		*/
		
		typedef sword (*_obndra)(struct cda_def *cursor, text *sqlvar, sword sqlvl,
			ub1 *progv, sword progvl, sword ftype, sword scale,
			sb2 *indp, ub2 *alen, ub2 *arcode, ub4 maxsiz,
			ub4 *cursiz, text *fmt, sword fmtl, sword fmtt);
		
		typedef sword (*_obndrn)(struct cda_def *cursor, sword sqlvn, ub1 *progv,
			sword progvl, sword ftype, sword scale, sb2 *indp,
			text *fmt, sword fmtl, sword fmtt);
		
		typedef sword (*_obndrv)(struct cda_def *cursor, text *sqlvar, sword sqlvl,
			ub1 *progv, sword progvl, sword ftype, sword scale,
			sb2 *indp, text *fmt, sword fmtl, sword fmtt);
		
		typedef sword  (*_odefin)(struct cda_def *cursor, sword pos, ub1 *buf,
			sword bufl, sword ftype, sword scale, sb2 *indp,
			text *fmt, sword fmtl, sword fmtt, ub2 *rlen, ub2 *rcode);
		
		/* older calls ; preferred equivalent calls above */
		
		typedef sword (*_oname)(struct cda_def *cursor, sword pos, sb1 *tbuf,
			sb2 *tbufl, sb1 *buf, sb2 *bufl);
		
		typedef sword (*_orlon)(struct cda_def *lda, ub1 *hda, text *uid, sword uidl, 
			text *pswd, sword pswdl, sword audit);
		
		typedef sword (*_olon)(struct cda_def *lda, text *uid, sword uidl,
			text *pswd, sword pswdl, sword audit);
		
		typedef sword (*_osql3)(struct cda_def *cda, text *sqlstm, sword sqllen);
		
		typedef sword (*_odsc)(struct cda_def *cursor, sword pos, sb2 *dbsize,
			sb2 *fsize, sb2 *rcode, sb2 *dtype, sb1 *buf, sb2 *bufl, sb2 *dsize);
		
		
		
		
		extern _obindps obindps;
		extern _obreak obreak;
		extern _ocan ocan;
		extern _oclose oclose;
		extern _ocof ocof;
		extern _ocom ocom;
		extern _ocon ocon;
		extern _odefinps odefinps;
		extern _odessp odessp;
		extern _odescr odescr;
		extern _oerhms oerhms;
		extern _oermsg oermsg;
		extern _oexec oexec;
		extern _oexfet oexfet;
		extern _oexn oexn;
		extern _ofen ofen;
		extern _ofetch ofetch;
		extern _oflng oflng;
		extern _ogetpi ogetpi;
		extern _oopt oopt;
		extern _opinit opinit;
		extern _olog olog;
		extern _ologof ologof;
		extern _oopen oopen;
		extern _oparse oparse;
		extern _orol orol;
		extern _osetpi osetpi;
		extern _sqlld2 sqlld2;
		extern _sqllda sqllda;
		extern _onbset onbset; 
		extern _onbtst onbtst; 
		extern _onbclr onbclr; 
		extern _ognfd ognfd;
		extern _obndra obndra;
		extern _obndrn obndrn;
		extern _obndrv obndrv;
		extern _odefin odefin;
		extern _oname oname;
		extern _orlon orlon;
		extern _olon olon;
		extern _osql3 osql3;
		extern _odsc odsc;  
		
#endif //OCL_DYNAMIC
		
#ifndef OCL_OCI8
#define OCI_DEFAULT 0
#endif
		
		// For parse
		
#define OCI_PARSE_NODEFER    0
#define OCI_PARSE_DEFER      1
#define OCI_LANG_V6          0
#define OCI_LANG_NORM        1
#define OCI_LANG_V7          2
		
		// Internal/external datatype codes
		
#define UNKNOWN_TYPE         0
#define VARCHAR2_TYPE        1
#define NUMBER_TYPE          2
#define INTEGER_TYPE         3
#define FLOAT_TYPE           4
#define STRING_TYPE          5
#define LONG_TYPE            8
#define ROWID_TYPE           11
#define DATE_TYPE            12
#define RAW_TYPE             23
#define LONGRAW_TYPE         24
#define CHAR_TYPE            96
#define CURSOR_TYPE          102
#define BOOLEAN_TYPE         252
		
		// SQL function codes
		
#define SQL_UNKNOWN          0
#define SQL_INSERT           3
#define SQL_SELECT           4
#define SQL_UPDATE           5
#define SQL_CREATE           7
#define SQL_DROP             8
#define SQL_DELETE           9
#define SQL_PLSQL            34
		
#define FC_OOPEN             14
		
		// ORACLE error codes
		
#define OCI_SUCCESS          0
#define OCI_VAR_NOT_IN_LIST  -303 // 1007
#define OCI_NO_DATA_FOUND    4    // 1403
#define OCI_NULL_VALUE_RETURNED  1405
#define OCI_BLOCKED          -3123
#define OCI_CONNECTION_BUSY  -3127
		
		// Session Errors
		
#define OCI_SESSION_KILLED      28
#define OCI_NOTLOGGEDON         1012
#define OCI_EOF_COMMUNICATION   3113
#define OCI_NOT_CONNECTED       3114
#define OCI_NO_INTERFACE        3121
		
#define OCI_STILL_IS_PIECE   -3130
#define OCI_STILL_IS_PIECE1  -3129
#define OCI_BREAKED          -1013
		
		// OCI7 specific
		
#define HDA_SIZE 256
		
		typedef Cda_Def CDA;
		typedef Lda_Def LDA;
		typedef unsigned char HDA[HDA_SIZE];
		
		struct RD {
			ub4 rcs4;   // obj num
			ub2 rcs5;   // file num
			ub1 rcs6;
		};
		
		// RowId structure
		
		struct RowId7 {
			RD rd;
			ub4 rcs7;   // block num
			ub2 rcs8;   // slot num
		};
		
		// OCI8 specific
		
		struct RowId8 {
			ub4 ridobjnum;  // data obj#--this field is unused in restricted ROWIDs
			ub2 ridfilenum;
			ub2 filler;
			ub4 ridblocknum;
			ub2 ridslotnum;
		};
		
		struct RowId81 {
			ub1 filler;
			ub4 ridobjnum; // data obj#--this field is unused in restricted ROWIDs
			ub2 ridfilenum;
			ub4 ridblocknum;
			ub2 ridslotnum;
		};
		
#ifdef OCL_OCI8
		//call back function type for piecewise fetch
		typedef sb4 (*OCICallbackDefine)(void* octxp, OCIDefine* definp, ub4 iter,
			void** bufp, ub4** alenpp, ub1* piecep, void** indpp, ub2** rcodepp);
#endif
		
#ifdef OCL_OCI8
		void check(sword errorCode);
#endif
		
		///////////////  OraEnvironment  /////////////////
		
		enum OCIVersion { None, OCI73, OCI80, OCI81, OCI90, OCI100 };
		
		class OraConnection;
		
		class OraEnvironment {
		public:
			
			static const int MaxHomes;
			
			enum InitMode {
				imDefault           = 0x00, // OCI_DEFAULT
					imThreaded          = 0x01, // OCI_THREADED
					imThreadedUnmutexed = 0x02, // OCI_THREADED | OCI_ENV_NO_MUTEX
					imObject            = 0x04, // OCI_OBJECT
					imShared            = 0x08, // OCI_SHARED
					imNoUserCallbacks   = 0x10, // OCI_NO_UCB
					imEvents            = 0x20, // OCI_EVENTS
			};
			
			static void setInitMode(InitMode mode);
			
			static void initOCI();
			static void freeOCI();
			
			static bool isThreaded();
			static void setThreaded(bool value);
			
#ifdef OCL_OCI8
			static bool isEvents();
			static void setEvents(bool value);
			
			static OCIEnv* getOCIEnv();
			static OCIError* getOCIError();
#endif
			
			static OCIVersion getOCIVersion();
			
			static OCIVersion OCICallStyle();
			static void setOCICallStyle(OCIVersion value);
			
			static bool isOCIInited();
			
#ifdef OCL_DYNAMIC
			static CRString OCILibrary();
			static void setOCILibrary(const CRString& name);
			
			static bool isOCILoaded();
#endif
			
			static int oracleHomeCount();
			static int defaultOracleHome();
			static CRString oracleHomeName(int index);
			static CRString oracleHomePath(int index);
			
			static void setOracleHome(int index);
			static void setOracleHome(const CRString& homeName);
			
			static void useDefaultOracleHome();
			
			void static resetOracleHomes();
			
			static void checkOCI80();
			static void checkOCI81();
			static void checkOCI90();
			
		private:
			OraEnvironment();
			
			// Handling errors
#ifdef OCL_OCI8
			static int  getOraError(sword errorCode, CRString& errorMsg, OCIError* errorHandle = 0);
			static void oraError(sword errorCode, OCIError* errorHandle);
#endif
			
			static void detectOCI();
			static bool AddToOracleHomes(const char *regKey);
			static OCIVersion getOCIVersion(const CRString& fileName);
			
#ifdef OCL_DYNAMIC
			static void* loadDl(const CRString& name);
			static void loadOCI();
			static void* getSymbol(void* dllHandle, const char* name);
#endif
			
#ifdef OCL_OCI8
			static OCIEnv*   hOCIEnv;
			static OCIError* hOCIError;
#endif
			
			static OCIVersion mOCICallStyle;
			static OCIVersion mOCIVersion;
			static bool mOCIInited;
			static int mDefaultOraHome;
			static int mActiveOraHome;
			static int mOraHomeCount;
			static std::vector<CRString> mOraHomePaths;
			static std::vector<CRString> mOraHomeNames;
			static unsigned mInitMode;
			static bool mOCILoaded;
#ifdef OCL_DYNAMIC
			static void* mLibHandle;
			static CRString mOCILibrary;
#endif
			
#ifdef OCL_OCI8
			friend void initExtProcSession(OCIExtProcContext* with_context,
				OraConnection& connection);
#endif
			static CRMutex mLock;
			
			friend void check(sword errorCode);
			
			friend class OraTransaction;
			friend class OraConnection;
			friend class OraCommand;
			friend class OraCursor;
			friend class OraLob;
			friend class OraFile;
			friend class OraLoader;
			friend class OraTimeStamp;
			friend class OraInterval;
};

} // namespace ocl

#endif 
