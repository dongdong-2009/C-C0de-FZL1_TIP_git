/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\orascript.cpp
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:21 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  Ora classes
//  Last modified:      22.03.00
//////////////////////////////////////////////////

#include "ocl.conf"
#include "orascript.h"

namespace ocl {
	
	const char ScriptNotSet[] = "Script text was not set";
	
	OraScript::OraScript() {
		
		_init();
	}
	
	OraScript::OraScript(OraConnection* conn) {
		
		_init();
		mConnection = conn;
		mOraCmd.setConnection(conn);
	}
	
	OraScript::~OraScript() {
		
		delete mParser;
	}
	
	void OraScript::_init() {
		
		mConnection   = 0;
		mParser       = 0;
		mBreakOnError = true;
		mHasErrors    = false;
		mStmtOffset   = 0;
		mErrorOffset  = 0;
		mStartPos     = 0;
		mStartLine    = 0;
		mEndPos       = 0;
		mEndLine      = 0;
	}
	
	void OraScript::setConnection(OraConnection& conn){
		
		setConnection(&conn);
	}
	
	void OraScript::setConnection(OraConnection* conn) {
		
		if (mConnection != conn) {
			mConnection = conn;
			mOraCmd.setConnection(conn);
		}
	}
	
	OraConnection* OraScript::connection() const {
		
		return mConnection;
	}
	
	void OraScript::setSQL(const CRString& sql) {
		
		if (mScriptSQL != sql) {
			mScriptSQL = sql;
			
			if (!mParser) {
				mParser = new OraParser(mScriptSQL);
				mParser->setOmitBlank(false);
			}
			else
				mParser->setText(mScriptSQL);
		}
	}
	
	CRString OraScript::SQL() const {
		
		return mScriptSQL;
	}
	
	int OraScript::errorOffset() const {
		
		return mErrorOffset;
	}
	
	void OraScript::setBreakOnError(bool value) {
		
		mBreakOnError = value;
	}
	
	bool OraScript::breakOnError() const {
		
		return  mBreakOnError;
	}
	
	/*
	parameters:
	If continueExec == true next script
	statement after invalid statement 
	will be executed, else start executing
	from begining of the script.
	
	 return:
     If breakOnError == false return true if
     script has no errors, else return false.
     If breakOnError == true return true if
     script was complite successful, otherwise
     throws exception.
	 
	  exceptions:
	  throw CRException if script text was not set,
	  throw OraException if breakOnError is true
	  and error was occured.
	*/
	
	bool OraScript::execute(bool continueExec /*= false*/) {
		
		if (!mOraCmd.connection())
			mOraCmd.setConnection(mConnection);
		
		if (!mParser)
			throw CRException(ScriptNotSet);
		
		if (!continueExec)
			toBegin();
		
		while (!executeNext());
		
		toBegin();
		
		return !mHasErrors;
	}
	
	void OraScript::toBegin() {
		
		mStmtOffset   = 0;
		mErrorOffset  = 0;
		mStartPos     = 0;
		mStartLine    = 0;
		mEndPos       = 0;
		mEndLine      = 0;
		
		mParser->toBegin();
	}
	
	bool OraScript::executeNext() {
		
		CRString sql;
		bool inPLSQL = false;
		mErrorOffset = 0;
		int code = 0;
		int prevCode = 0;
		mStartPos = mParser->currPos();
		mStartLine = mParser->currLine();
		
		CRString st;
		
		do {
			if (code != lcBlank && code != lcComment)
				prevCode = code;
			code = mParser->next(st);
			
			if ((code == lcEnd ||
				(st == ";" && !inPLSQL) ||
				(st == "/" && mParser->currColl() == 1)) &&
				!sql.empty())
			{
				// Execution
				mEndPos = mParser->prevPos();
				mEndLine = mParser->prevLine();
				
				mCurrentSQL = sql;
				mOraCmd.setCommandText(sql);
				
				try {
					mOraCmd.execute();
				}
				catch (...) {
					mErrorOffset = mStmtOffset + mOraCmd.errorOffeset();
					if (mBreakOnError) {
						if (code == lcEnd)
							mParser->toBegin();
						else {
							code = mParser->next(st);
							if (code == lcEnd) {
								mParser->toBegin();
							}
							else
								mParser->back();
						}
						
						mStmtOffset = mParser->currPos();
						mStartPos = mParser->currPos();
						mStartLine = mParser->currLine();
						throw;
					}
					else
						mHasErrors = true;
				}
				
				mStmtOffset = mParser->currPos();
				mStartPos = mParser->currPos();
				mStartLine = mParser->currLine();
				break;
			}
			else {
				if ((code == lxBEGIN || code == lxDECLARE) ||
					((code == lxPROCEDURE || code == lxFUNCTION || code == lxPACKAGE) &&
					(prevCode == lxCREATE || prevCode == lxREPLACE)))
					inPLSQL = true;
				
				if (!(st == "/" && mParser->currColl() == 1)) {
					if (code == lcString)
						sql = sql + '\'' + st + '\'';
					else
						if (code != lcBlank || !sql.empty())
							sql += st;
						else
							mStmtOffset += strlen(st);
				}
			}
		} while (code != lcEnd);
		
		return code == lcEnd;
	}
	
} // namespace ocl;
