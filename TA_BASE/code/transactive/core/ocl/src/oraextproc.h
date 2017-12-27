/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\oraextproc.h
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
//  Copyright @ 1999-2002 Core Lab. All right reserved.
//  External procedure support
//  Last modified:      02.02.02
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __ORAEXTPROC_H__
#define __ORAEXTPROC_H__

#include "oracall.h"

#ifdef OCL_OCI8

#include "ora.h"

namespace ocl {
	
	void initExtProcSession(OCIExtProcContext* with_context, OraConnection& connection);
	
}

#endif // OCL_OCI8

#endif // __ORAEXTPROC_H__
