/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\orascript.h
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:23 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @ 1999-2002 Core Lab. All right reserved.
//  Ora classes
//  Last modified:      22.03.00
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __ORA_SCRIPT_H__
#define __ORA_SCRIPT_H__

#include "ora.h"
#include "orapars.h"

namespace ocl {
	
	class OraScript {
	public:
		OraScript();
		OraScript(OraConnection* conn);
		virtual ~OraScript();
		
		void setConnection(OraConnection& conn);
		void setConnection(OraConnection* conn);
		OraConnection* connection() const;
		
		void setSQL(const CRString& sql);
		CRString SQL() const;
		
		void setBreakOnError(bool value);
		bool breakOnError() const;
		
		void toBegin();
		
		bool execute(bool continueExec = false);
		
		int errorOffset() const;
		
	protected:
		CRString currentSQL() const { return mCurrentSQL; }
		bool isEOF() const { return mStartPos != 0;  }
		OraCommand* getCommand() { return &mOraCmd; }
		
	private:
		void _init();
		bool executeNext();
		
		OraConnection* mConnection;
		OraParser* mParser;
		OraCommand  mOraCmd;
		CRString mScriptSQL;
		CRString mCurrentSQL;
		bool mBreakOnError;
		bool mHasErrors;
		int mStmtOffset;
		int mErrorOffset;
		int mStartPos;
		int mStartLine;
		int mEndPos;
		int mEndLine;
	};
	
} // namespace ocl;

#endif // __ORA_SCRIPT_H__

