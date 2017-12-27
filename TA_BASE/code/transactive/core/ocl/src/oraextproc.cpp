/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\oraextproc.cpp
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:20 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  External procedure support
//  Last modified:      02.02.02
//////////////////////////////////////////////////

// For Extproc API
#ifndef __BORLANDC__
#define __STDC__ 1
#endif

#include "ocl.conf"
#include "oraextproc.h"

#ifdef OCL_OCI8

namespace ocl {
	
	void initExtProcSession(OCIExtProcContext* with_context, OraConnection& connection)
	{
		OCISvcCtx* hOCISvcCtx;
		char errbuf[256];
		
#ifdef OCL_DYNAMIC
		OraEnvironment::loadOCI();
#endif
		
		OraEnvironment::mOCIInited = true;
		
		sword res = OCIExtProcGetEnv(with_context, &OraEnvironment::hOCIEnv,
			&hOCISvcCtx, &OraEnvironment::hOCIError);
		
		sb4 code;
		
		if (res == OCI_ERROR) {
			OraEnvironment::hOCIEnv = 0;
			OraEnvironment::hOCIError = 0;
			OCIErrorGet(OraEnvironment::hOCIError, 1, 0, &code, (OraText*)errbuf, sizeof(errbuf), OCI_HTYPE_ERROR);
			throw OraException(code, errbuf);
		}
		
		connection.setOCISvcCtx(hOCISvcCtx);
	}
	
} // namespace ocl

#endif // OCL_OCI8