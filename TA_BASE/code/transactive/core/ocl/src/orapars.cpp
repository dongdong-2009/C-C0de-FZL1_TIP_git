/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\orapars.cpp
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:21 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

/////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  Oracle parser
//  Created:            11.09.99
//  Last modified:      09.09.99
/////////////////////////////////////////////////

#include "ocl.conf"
#include "orapars.h"

namespace ocl {
	
	const char* SQLLexems[OraSymbolCount + OraKeywordCount] = {
		"*", "=", ":", ",", ".", " :=", "/",
			
			"AND",
			"AS",
			"BEGIN",
			"BODY",
			"BY",
			"CREATE",
			"DECLARE",
			"DELETE",
			"END",
			"FOR",
			"FROM",
			"FUNCTION",
			"GROUP",
			"INSERT",
			"INTO",
			"IS",
			"OR",
			"ORDER",
			"PACKAGE",
			"PROCEDURE",
			"REPLACE",
			"RETURNING",
			"SELECT",
			"TRIGGER",
			"TYPE",
			"UNION",
			"UPDATE",
			"WHERE"
	};
	
	/////////////////  OraParser  //////////////////
	
	OraParser::OraParser(const char* text) :
		CRParser(text)
		{
			mSymbolCount  = OraSymbolCount;
			mKeywordCount = OraKeywordCount;
			mArrLexem     = (char**)SQLLexems;
			
			strcpy(mInlineComment, "--");
		}
		
		bool OraParser::isAlpha(char ch) {
			
			return CRParser::isAlpha(ch) || ch == '$' || ch ==  '#';
		}
		
		bool OraParser::isStringQuote(char ch) {
			
			return ch == '\'';
		}
		
		bool OraParser::isIdentQuote(char ch) {
			
			return ch == '\"';
		}
		
} // namespace ocl
