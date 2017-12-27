/********************************************************************
/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: e:\01-p4\code_ta_base_ddb\transactive\core\ocl\src\crdef.cpp
* @author:  OCL,3.01
* @version: $Revision: 
*
* Last modification: $DateTime: 19:1:2007 14:19 2007/01/19
* Last modified by:  $Author: Jiang HP $
*
*/
/*********************************************************************/

/////////////////////////////////////////////////
//  Oracle Class Library
//  Copyright @1999-2002 Core Lab. All right reserved.
//  OCL definations
//  Created:            28.08.98
//  Last modified:      28.03.00
/////////////////////////////////////////////////

#include <exception>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include "ocl.conf"
#include "crdef.h"

namespace ocl {
	
	const char SInvalidDate[] = "Invalid date";
	
#ifdef OCL_SCL
	
	char* strupr(char* s) {
		char* ch = s;
		
		while (*ch) {
			*ch = (char)toupper(*ch);
			ch++;
		}
		return s;
	}
	
	int stricmp(const char* string1, const char* string2) {
		const char* ch1 = string1;
		const char* ch2 = string2;
		int res;
		
		while (true) {
			res = toupper(*ch1) - toupper(*ch2);
			if (res)
				return res;
			else
				if (!*ch1)
					return 0;
				
				ch1++;
				ch2++;
		}
	}
	
	int strnicmp(const char *string1, const char *string2, size_t count) {
		const char* ch1 = string1;
		const char* ch2 = string2;
		int res;
		
		while (count > 0) {
			res = toupper(*ch1) - toupper(*ch2);
			if (res)
				return res;
			else
				if (!*ch1)
					return 0;
				
				ch1++;             
				ch2++;
				count--;
		}
		return 0;
	}
	
#endif
	
	//////////////////  CRString  ///////////////////
	
	CRString& CRString::toUpper() {
		for (size_type i = 0; i < size(); i++)
			at(i) = toupper(at(i));
		
		return *this;
	}
	
	CRString& CRString::toLower() {
		for (size_type i = 0; i < size(); i++)
			at(i) = tolower(at(i));
		
		return *this;
	}
	
	CRString& CRString::trim() {
		
		size_type pos1 = find_first_not_of(' ');
		pos1 = (pos1 == npos) ? 0 : pos1;
		
		if (pos1 != 0)
			erase(0, pos1);
		
		size_type pos2 = find_last_not_of(' ');  
		
		if (pos2 != size())
			erase(pos2 + 1, size() - pos2);
		
		return *this;
	}
	
	CRString& CRString::trimLeft() {
		
		size_type pos = find_first_not_of(' ');
		
		if (pos == npos) {
			erase();
			return *this;
		}
		
		erase(0, pos);
		
		return *this;
	}
	
	CRString& CRString::trimRight() {
		
		size_type pos = find_last_not_of(' ');
		
		if (pos == npos) {
			erase();
			return *this;
		}
		else
			pos = pos + 1;
		
		erase(pos, size() - pos);
		
		return *this;
	}
	
	/////////////////  CRException  /////////////////
	
#ifdef OCL_SCL
	
	CRException::CRException(const CRString& msg) {
		
		mMsg = msg;
	}
	
	CRException::~CRException() throw()
	{
	}
	
	CRString CRException::message() {
		
		return mMsg;
	}
	
	void CRException::setMessage(const CRString& msg) {
		
		mMsg = msg;
	}
	
	const char* CRException::what() const throw(){
		
		return mMsg.c_str();
	}
	
#endif // OCL_SCL
	
#ifdef OCL_MFC
	
	CRException::CRException(const CRString& msg) {
		
		mMsg = msg;
	}
	
	CRString CRException::message() {
		
		return mMsg;
	}
	
	void CRException::setMessage(const CRString& msg) {
		
		mMsg = msg;
	}
	
	BOOL CRException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
		PUINT pnHelpContext) {
		
		strncpy(lpszError, mMsg.c_str(), nMaxError);
		return true;
	}
	
#endif // OCL_MFC
	
#ifdef OCL_VCL
	
	CRException::CRException(const CRString& msg)
		:Exception(msg.c_str())
	{
	}
	
	CRString& CRException::message() {
		
		return Exception::Message.c_str();
	}
	
	void CRException::setMessage(const CRString& msg) {
		
		Exception::Message = msg;
	}
	
#endif // OCL_VCL
	
	//////////////////  CRDate  /////////////////////
	
	CRString CRDate::mDateFormat;
	
	CRDate::CRDate() {
		
		mYear = 1;
		mMonth = 1;
		mDay = 1;
		mHour = 0;
		mMinute = 0;
		mSecond = 0;
	}
	
	CRDate::CRDate(unsigned year, unsigned month, unsigned day, unsigned hour,
		unsigned minute, unsigned second/*, unsigned Millesecond*/)
	{
		setYear(year);
		setMonth(month);
		setDay(day);
		setHour(hour);
		setMinute(minute);
		setSecond(second);
	}
	
	void CRDate::setYear(unsigned year) {
		
		mYear = (word)year;
	}
	
	void CRDate::setMonth(unsigned month) {
		
		if (month < 1 || month > 12)
			throwError(SInvalidDate);
		mMonth = (byte)month;
	}
	
	void CRDate::setDay(unsigned day) {
		
		if (day < 1 || day > 31)
			throwError(SInvalidDate);
		mDay = (byte)day;
	}
	
	void CRDate::setHour(unsigned hour) {
		
		if (hour > 23)
			throwError(SInvalidDate);
		mHour = (byte)hour;
	}
	
	void CRDate::setMinute(unsigned minute) {
		
		if (minute > 59)
			throwError(SInvalidDate);
		mMinute = (byte)minute;
	}
	
	void CRDate::setSecond(unsigned second) {
		
		if (second > 59)
			throwError(SInvalidDate);
		mSecond = (byte)second;
	}
	
	bool CRDate::operator==(const CRDate& value) const {
		return mYear   == value.mYear   &&
			mMonth  == value.mMonth  &&
			mDay    == value.mDay    &&
			mHour   == value.mHour   &&
			mMinute == value.mMinute &&
			mSecond == value.mSecond;
	}
	
	CRString CRDate::format(const char* format) const {
		char buf[10];
		CRString st;
		const char* ch;
		const char* firstCh;
		int val;
		const char* fmt;
		
		if (!format || !*format)
			format = dateFormat().c_str();
		
		ch = format;
		firstCh = ch;
		do {
			ch++;
			if (*ch != *firstCh) {
				if (ch - firstCh == 1)
					fmt = "%i";
				else
					fmt = "%02i";
				
				switch (*firstCh) {
				case 'y':
					if (ch - firstCh <= 2) {
						val = mYear % 100;
					}
					else {
						val = mYear;
						fmt = "%04i";
					}
					break;
				case 'm': case 'M':
					val = mMonth;
					break;
				case 'd':
					val = mDay;
					break;
				case 'h':
					val = mHour;
					break;
				case 'n':
					val = mMinute;
					break;
				case 's':
					val = mSecond;
					break;
				default:
					fmt = NULL;
					for (;firstCh < ch; firstCh++)
						st += *firstCh;
				}
				if (fmt) {
					sprintf(buf, fmt, val);
					st += buf;
				}
				firstCh = ch;
			}
		} while (*ch);
		
		return st;
	}
	
	void CRDate::now() {
		time_t timer;
		struct tm *T;
		
		timer = time(NULL);
		T = localtime(&timer);
		mYear = (word)(T->tm_year + 1900);
		mMonth = (byte)T->tm_mon + 1;
		mDay = (byte)T->tm_mday;
		mHour = (byte)T->tm_hour;
		mMinute = (byte)T->tm_min;
		mSecond = (byte)T->tm_sec;
	}
	
	unsigned CRDate::dayOfWeek() {
		time_t Timer;
		struct tm T;
		struct tm* T1;
		
		T.tm_year = mYear - 1900;
		// mMonth is one-based, tm_mon is zero-based
		T.tm_mon = mMonth - 1;
		T.tm_mday = mDay;
		T.tm_hour = 0;
		T.tm_min = 0;
		T.tm_sec = 0;
		
		Timer = mktime(&T);
		T1 = localtime(&Timer);
		return T1->tm_wday;
	}
	
	CRString CRDate::dateFormat() {
		
		if (mDateFormat.size() == 0) {
			
			char buffer[40];  
			
#ifdef OCL_WIN32  
			GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,
				LOCALE_SSHORTDATE, buffer, sizeof(buffer));
#else
			strcpy(buffer, "dd.mm.yyyy");
#endif
			
			mDateFormat = buffer;
		}
		
		return mDateFormat;
	}
	
	void CRDate::setDateFormat(const CRString& value) {
		if (value.size() != 0)
			mDateFormat = value;
	}
	
	// Misc
	
	void throwError(const char* format, ...) {
		va_list argList;
		char msg[1000];
		
		va_start(argList, format);
		vsprintf(msg, format, argList);
		va_end(argList);
		
		throw CRException(msg);
	}
	
} // namespace ocl
