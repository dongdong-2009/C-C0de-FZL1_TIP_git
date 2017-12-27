//////////////////////////////////////////////////////////////////////
///		@file		SQLGlobalFun.h
///		@author		zhilin,ouyang
///		@date		2010-12-8 14:44:41
///
///		@brief	    to declare some useful methods used in SQL statement
///                 construct
//////////////////////////////////////////////////////////////////////
#ifndef SQLGLOBALFUN_H
#define SQLGLOBALFUN_H

#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <functional>

namespace TA_Base_Core{

/// \brief Converts anything you can insert into a C++ stream to a
/// \c std::string via \c std::ostringstream.
template <class T>
inline std::string stream2string(const T& object)
{
	std::ostringstream str;
	str << object;
	return str.str();
}

// trim from start(left)
static inline std::string& ltrim(std::string& s)
{
	s.erase(s.begin(),std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end(right)
static inline std::string& rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both end
static inline std::string& trim(std::string& s)
{
	return ltrim(rtrim(s));
}

// convert string to lower case
static inline char *strlwr(char* pszSrc)
{
	char *pszOri = pszSrc;
	for (; *pszSrc != '\0'; pszSrc++)	
		*pszSrc = tolower(*pszSrc);
	
	return pszOri;
}

static inline char* strtolwr(char* pszSrc)
{
	char *pszOri = pszSrc;
	for (; *pszSrc != '\0'; pszSrc++)
	{
		if (*pszSrc < 0x41 || *pszSrc > 0x5A)
			continue;

		*pszSrc |= 0x20;
	}

	return pszOri;
}

// convert string to upper case
static inline char *strupper(char* pszSrc)
{
	char *pszOri = pszSrc;
	for (; *pszSrc != '\0'; pszSrc++)	
		*pszSrc = toupper(*pszSrc);	

	return pszOri;
}

static inline char* strtoupper(char* pszSrc)
{
	char *pszOri = pszSrc;
	for (; *pszSrc != '\0'; pszSrc++)
	{
		if (*pszSrc < 0x61 || *pszSrc > 0x7A)
			continue;

		*pszSrc &= 0xDF;
	}

	return pszOri;
}

static inline int strnicmp(const char *string1, const char *string2, size_t count) {
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

} // end of namespace TA_Base_Core

#endif // SQLGLOBALFUN_H