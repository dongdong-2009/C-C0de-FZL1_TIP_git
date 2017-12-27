/********************************************************************
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\crparser.h
  * @author:  OCL,3.01
  * @version: $Revision: 
  *
  * Last modification: $DateTime: 19:1:2007 14:22 2007/01/19
  * Last modified by:  $Author: Jiang HP $
  *
  */
/*********************************************************************/

//////////////////////////////////////////////////
//  Parser
//  Copyright @ 1999-2002 Core Lab. All right reserved.
//  Last modified:      13.03.00
//////////////////////////////////////////////////
#ifndef OCL_MACRO_DEFINED
#error ( "Must include ocl.h only when using ocl" )
#endif

#ifndef __CRPARSER_H__
#define __CRPARSER_H__

#include "crdef.h"

namespace ocl {
	
	const int lcEnd     = -1;
	const int lcReserve = -100;
	const int lcSymbol  = -102;
	const int lcIdent   = -103;  // identificator
	const int lcNumber  = -105;
	const int lcString  = -106;
	const int lcBlank   = -107;
	const int lcComment = -108;
	
	class CRParser {
	public:
		CRParser(char** lexems, int lount);
		CRParser(const char* text);
		virtual ~CRParser();
		
		void setText(const char* text);
		const char* getText() const { return mText; }
		
		void setLexems(char** lexems, int count);
		
		CRString inlineComment() const;
		void setInlineComment(const CRString& value);
		
		CRString commentBegin() const;
		void setCommentBegin(const CRString& value);
		
		CRString commentEnd() const;
		void setCommentEnd(const CRString& value);
		
		bool omitBlank() const;
		void setOmitBlank(bool value);
		
		bool omitComment() const;
		void setOmitComment(bool value);
		
		void toBegin();
		
		int next(char* strLexeme);
		int next(CRString& strLexem);
		int nextCode();
		
		bool toLexeme(int lexeme);
		
		void back();
		
		int currPos();
		int prevPos();
		int prevPrevPos();
		
		int currLine();
		int prevLine();
		int currColl();
		int prevColl();
		
	protected:
		void _init();
		
		virtual bool isBlank(char ch);
		virtual bool isSymbol(char ch);
		virtual bool isAlpha(char ch);
		virtual bool isNumber(char ch);
		virtual bool isStringQuote(char ch);
		virtual bool isIdentQuote(char ch);
		
		int    mSymbolCount;
		int    mKeywordCount;
		char** mArrLexem;
		char   mCommentBegin[5];
		char   mCommentEnd[5];
		char   mInlineComment[5];
		unsigned mSize;
		
	private:
		const char* mText;
		char* mPos;
		char* mOldPos;
		char* mOldOldPos;
		int   mCurrLine;
		int   mPrevLine;
		char* mCurrBegLine;
		char* mPrevBegLine;
		bool  mOmitBlank;
		bool  mOmitComment;
		bool  mUppered;
		
	};
	
} // namespace ocl

#endif
