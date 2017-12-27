/********************************************************************
/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\crparser.cpp
* @author:  OCL,3.01
* @version: $Revision: 
*
* Last modification: $DateTime: 19:1:2007 14:19 2007/01/19
* Last modified by:  $Author: Jiang HP $
*
*/
/*********************************************************************/

//////////////////////////////////////////////////
//  Parser
//  Copyright @1999-2002 Core Lab. All right reserved.
//  Created:            14.05.96
//  Last modified:      13.03.00
//////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "ocl.conf"
#include "crparser.h"
#include "crdef.h"

namespace ocl {
	
	const char StringTooLarge[] = "String too large";
	
	const int CommonSymbolCount = 31;
	const int CommonKeywordCount = 3;
	
	const char* CommonLexems[CommonSymbolCount] = {
		"!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
			":", ";", "<", "=", ">", "?", "@", "[", "\\", "]", "^", "_", "`", "and", "not", "or" };
		
		/////////////////////  CRParser  /////////////////////
		
		CRParser::CRParser(char* lexems[], int count) {
			
			_init();
			setLexems(lexems, count);
		}
		
		CRParser::CRParser(const char* text) {
			
			_init();
			mText = text;
			toBegin();
		}
		
		void CRParser::_init() {
			
			strcpy(mCommentBegin, "/*");
			strcpy(mCommentEnd, "*/");
			strcpy(mInlineComment, "//");
			
			mOmitBlank   = false;
			mOmitComment = false;
			mUppered     = true;
			
			mSymbolCount  = CommonSymbolCount;
			mArrLexem     = (char**)CommonLexems;
			
			mText        = 0;
			mPos         = 0;
			mOldPos      = 0;
			mOldOldPos   = 0;
			mCurrLine    = 0;
			mPrevLine    = 0;
			mCurrBegLine = 0;
			mPrevBegLine = 0;
		}
		
		CRParser::~CRParser() {
		}
		
		void CRParser::setText(const char* text) {
			
			mText = text;
			toBegin();
		}
		
		void CRParser::setLexems(char* lexems[], int count){
			
			mSymbolCount = count;
			mArrLexem = (char**)lexems;
		}
		
		bool CRParser::isBlank(char ch) {
			
			return ch == ' ' || ch == '\n' || ch == '\r' ||
				ch == '\t' || ch == '\v';
		}
		
		bool CRParser::isSymbol(char ch) {
			
			return '!' <= ch && ch <= '/' || ':' <= ch && ch <= '@' ||
				'[' <= ch && ch <= '^' || ch == '`' || '{' <= ch && ch <= '~';
		}
		
		bool CRParser::isAlpha(char ch) {
			
			return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') ||
				128 <= (unsigned char)ch;
		}
		
		bool CRParser::isNumber(char ch) {
			
			return '0' <= ch && ch <= '9';
		}
		
		bool CRParser::isStringQuote(char ch) {
			
			return 0;
		}
		
		bool CRParser::isIdentQuote(char ch) {
			
			return 0;
		}
		
		void CRParser::toBegin(){
			
			mPos         = (char*)mText;
			mOldPos      = mPos;
			mOldOldPos   = mOldPos;
			mCurrLine    = 0;
			mPrevLine    = 0;
			mCurrBegLine = mPos;
			mPrevBegLine = mPos;
		}
		
		int CRParser::next(char* strLexem) {
			
			CRString lexeme;
			
			int code = next(lexeme);
			lexeme.copy(strLexem, lexeme.size()); // Dangerous
			strLexem[lexeme.size()] = 0;
			
			return code;
		}
		
		int CRParser::next(CRString& strLexem) {
			int Code;
			char St[200];
			char* SymbolmPos;
			int i;
			bool isComment;
			
			mOldOldPos = mOldPos;
			mOldPos = mPos;
			mPrevLine = mCurrLine;
			mPrevBegLine = mCurrBegLine;
			
			strLexem.erase();
			
			do {
				// Blanks
				while (isBlank(*mPos)) {
					strLexem += *mPos;
					if (*mPos == '\r' || *mPos == '\n')
						if ((*mPos == '\r') && (*(mPos + 1) == '\n')) {
							mCurrLine += 2;
							mCurrBegLine = mPos + 2;
						}
						else {
							mCurrLine++;
							mCurrBegLine = mPos + 1;
						}
						mPos++;
				}
				
				if (!mOmitBlank && !strLexem.empty())
					return lcBlank;
				else {
					mOldOldPos = mOldPos;
					mOldPos = mPos;
				}
				
				// End
				if (!*mPos) {
					strLexem.erase();
					return lcEnd;
				}
				
				// Comment
				isComment = false;
				if (strncmp(mInlineComment, mPos, strlen(mInlineComment)) == 0) {
					isComment = true;
					while (!(*mPos == '\r' || *mPos == '\n') && *mPos)
						mPos++;
				}
				
				if (strncmp(mCommentBegin, mPos, strlen(mCommentBegin)) == 0) {
					isComment = true;
					mPos++;
					while (*mPos && strncmp(mCommentEnd, mPos, strlen(mCommentEnd)) != 0)
						mPos++;
					
					if (*mPos)
						mPos += strlen(mCommentEnd);
				}
				
				if (!mOmitComment && isComment) {
					strLexem.assign(mOldPos, mPos - mOldPos);
					return lcComment;
				}
				else {
					mOldOldPos = mOldPos;
					mOldPos = mPos;
				}
				
			} while (isComment);
			
			if (isSymbol(*mPos)) {
				if (isStringQuote(*mPos)) {
					Code = lcString;
					mPos++;
					
					while (!isStringQuote(*mPos) && *mPos) {
						strLexem += *mPos;
						mPos++;
					}
					
					if (*mPos)
						mPos++;
				}
				else {
					if (isIdentQuote(*mPos)) {
						//Result := lcIdent;
						//StrLexem := StrLexem + Pos^;
						//repeat
						//  Inc(Pos);
						//  StrLexem := StrLexem + Pos^;
						//until IsIdentQuote(Pos^) or (Pos^ = #0);
						//if Pos^ <> #0 then
						//  Inc(Pos);
						Code = lcIdent;
						strLexem += *mPos;
						do {
							mPos++;
							strLexem += *mPos;
						} while (!isIdentQuote(*mPos) && *mPos != 0);
						if (*mPos)
							mPos++;
					}
					else {
						Code = lcSymbol;
						SymbolmPos = mPos + 1;   // WAR
						St[0] = *mPos;
						St[1] = 0;
						do {
							strLexem += *mPos;
							mPos++;
							
							i = 0;
							while (i < mSymbolCount && strLexem != mArrLexem[i])
								i++;
							
							if (i < mSymbolCount) {
								strcpy(St, strLexem.c_str());
								SymbolmPos = mPos;
								Code = i;
							}
						} while (isSymbol(*mPos));
						
						strLexem = St;
						mPos = SymbolmPos;
					}
				}
			}
			else
				if (isAlpha(*mPos) || *mPos == '_') {
					Code = lcIdent;
					do {
						strLexem += *mPos;
						mPos++;
					} while (isAlpha(*mPos) || isNumber(*mPos) || *mPos == '_');
				}
				else
					if (isNumber(*mPos)) {
						Code = lcNumber;
						do {
							strLexem += *mPos++;
						} while (isNumber(*mPos));
						if (*mPos == '.') {
							strLexem += *mPos++;
							while (isNumber(*mPos))
								strLexem += *mPos++;
						}
					}
					else
						throw CRException("Parser: Unknown character");
					
					if (Code == lcSymbol || Code == lcIdent) {
						strLexem.assign(mOldPos, mPos - mOldPos);
						strLexem.toUpper();
						i = 0;
						while (i < (mSymbolCount + mKeywordCount) && strcmp(strLexem, mArrLexem[i]))
							i++;
						if (i < (mSymbolCount + mKeywordCount)) {
							Code = i;
						}
					}
					return Code;
}


bool CRParser::toLexeme(int code) {
	
	bool isFound = false;
	CRString lexeme;
	int lxmCode = next(lexeme);
	
	while (lxmCode != lcEnd) {
		if (lxmCode == code) {
			isFound = true;
			break;
		}
		lxmCode = next(lexeme);
	}
	
	return isFound;
}


void CRParser::back() {
	
	mPos = mOldPos;
	mOldPos = mOldOldPos;
}


int CRParser::currPos() {
	
	return mPos - mText;
}


int CRParser::prevPos() {
	
	return mOldPos - mText;
}


int CRParser::prevPrevPos() {
	
	return mOldOldPos - mText;
}

int CRParser::currLine() {
	
	return mCurrLine;
}

int CRParser::prevLine() {
	
	return mPrevLine;
}

int CRParser::currColl() {
	
	return mPos - mCurrBegLine;
}

int CRParser::prevColl() {
	
	return mOldPos - mPrevBegLine;
}

CRString CRParser::inlineComment() const {
	
	return mInlineComment;
}

void CRParser::setInlineComment(const CRString& value) {
	
	if (sizeof(mInlineComment) <= value.size())
		throw CRException(StringTooLarge);
	
	strcpy(mInlineComment, value.c_str());
}

CRString CRParser::commentBegin() const {
	
	return mCommentBegin;
}

void CRParser::setCommentBegin(const CRString& value) {
	
	if (sizeof(mCommentBegin) <= value.size())
		throw CRException(StringTooLarge);
	
	strcpy(mCommentBegin, value.c_str());
}

CRString CRParser::commentEnd() const {
	
	return mCommentEnd;
}

void CRParser::setCommentEnd(const CRString& value) {
	
	if (sizeof(mCommentEnd) <= value.size())
		throw CRException(StringTooLarge);
	
	strcpy(mCommentEnd, value.c_str());
}

bool CRParser::omitBlank() const {
	
	return mOmitBlank;
}

void CRParser::setOmitBlank(bool value) {
	
	mOmitBlank = value;
}

bool CRParser::omitComment() const {
	
	return mOmitComment;
}

void CRParser::setOmitComment(bool value) {
	
	mOmitComment = value;
}

} // namespace ocl
