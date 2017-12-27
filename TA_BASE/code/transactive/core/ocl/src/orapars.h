/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\orapars.h
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
//  Oracle parser
//  Last modified:      13.03.00
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __ORAPARS_H__
#define __ORAPARS_H__

#include "crparser.h"

namespace ocl {
	
	const int lxAND       = 7;
	const int lxAS        = lxAND + 1;
	const int lxBEGIN     = lxAS + 1;
	const int lxBODY      = lxBEGIN + 1;
	const int lxBY        = lxBODY + 1;
	const int lxCREATE    = lxBY + 1;
	const int lxDECLARE   = lxCREATE + 1;
	const int lxDELETE    = lxDECLARE + 1;
	const int lxEND       = lxDELETE + 1;
	const int lxFOR       = lxEND + 1;
	const int lxFROM      = lxFOR + 1;
	const int lxFUNCTION  = lxFROM + 1;
	const int lxGROUP     = lxFUNCTION + 1;
	const int lxINSERT    = lxGROUP + 1;
	const int lxINTO      = lxINSERT + 1;
	const int lxIS        = lxINTO + 1;
	const int lxOR        = lxIS + 1;
	const int lxORDER     = lxOR + 1;
	const int lxPACKAGE   = lxORDER + 1;
	const int lxPROCEDURE = lxPACKAGE + 1;
	const int lxREPLACE   = lxPROCEDURE + 1;
	const int lxRETURNING = lxREPLACE + 1;
	const int lxSELECT    = lxRETURNING + 1;
	const int lxTRIGGER   = lxSELECT + 1;
	const int lxTYPE      = lxTRIGGER + 1;
	const int lxUNION     = lxTYPE + 1;
	const int lxUPDATE    = lxUNION + 1;
	const int lxWHERE     = lxUPDATE + 1;
	
	const int OraSymbolCount = 7;
	const int OraKeywordCount = 28;
	
	class OraParser : public CRParser {
	public:
		OraParser(const char* text);
		
	protected:
		virtual bool isAlpha(char ch);
		virtual bool isStringQuote(char ch);
		virtual bool isIdentQuote(char ch);
	};
	
} // namespace ocl

#endif


