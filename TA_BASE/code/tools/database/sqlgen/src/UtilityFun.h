//////////////////////////////////////////////////////////////////////
///		@file		SQLGlobalFun.h
///		@author		zhilin,ouyang
///		@date		2010-12-8 14:44:41
///
///		@brief	    to declare some useful methods used in SQL statement
///                 construct
//////////////////////////////////////////////////////////////////////
#ifndef UTILITYFUN_H
#define UTILITYFUN_H

#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <functional>

#include "CommonDef.h"


//NS_BEGIN(TA_DB_Sync)

// namespace TA_DB_Sync
// {

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
inline std::string& ltrim(std::string& s)
{
	s.erase(s.begin(),std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end(right)
inline std::string& rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both end
inline std::string& trim(std::string& s)
{
	return ltrim(rtrim(s));
}

// convert string to lower case
inline char *strlwr(char* pszSrc)
{
	char *pszOri = pszSrc;
	for (; *pszSrc != '\0'; pszSrc++)	
		*pszSrc = tolower(*pszSrc);
	
	return pszOri;
}

inline char* strtolwr(char* pszSrc)
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
inline char *strupper(char* pszSrc)
{
	char *pszOri = pszSrc;
	for (; *pszSrc != '\0'; pszSrc++)	
		*pszSrc = toupper(*pszSrc);	

	return pszOri;
}

inline char* strtoupper(char* pszSrc)
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

inline void int2str(int uVal, char sz[])
{
	int nDigit = 0;
	int nIndex = 0;
	int nSign  = uVal;

	if ( nSign < 0 ) 
	{
		uVal = -uVal;
		sz[nIndex++] = '-';
	}

	while (uVal > 10)
	{
		nDigit = uVal % 10;
		nDigit |= 0x30;
		sz[nIndex++] = nDigit;
		uVal /= 10;
	}
	nDigit = uVal | 0x30;
	sz[nIndex++] = nDigit;

	sz[nIndex] = 0;
	int nSwap = 0;
	int i = 1;
	while ( i <= nIndex / 2 )
	{
		nSwap = sz[i];
		sz[i] = sz[nIndex - i];
		sz[nIndex - i] = nSwap;
		i++;
	}
}


//NS_END(TA_DB_Sync)


#endif // UTILITYFUN_H















