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

#include "AQCommonDef.h"


NS_BEGIN(TA_AQ_Lib)

/// \brief Converts anything you can insert into a C++ stream to a
/// \c std::string via \c std::ostringstream.
template <class T>
inline std::string stream2string(const T& object)
{
	std::ostringstream str;
	str << object;
	return str.str();
}

template <class MsgConT>
void g_clearContainer(MsgConT& rConT)
{
	typename MsgConT::iterator  iter = rConT.begin();

	while ( iter != rConT.end() )
	{
		DEF_DELETE(*iter);
		iter++;
	}
	rConT.clear();
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

class CUtilityFun
{
public:
	static char *strlwr(char* pszSrc);
	static char* strtolwr(char* pszSrc);
	static char *strupper(char* pszSrc);
	static char* strtoupper(char* pszSrc);
	static std::string int2str(unsigned nVal);
	static std::string u64tostr(ulong64 u64Val);
	static void int2str(int uVal, char sz[]);
	static ulong64 pow64(u_int uBase, u_int uExp);
	static ulong64 atoui64(std::string& strValue64);
	static int stricmp_(const char* pzLeft, const char* pzRight);
	static int strnicmp_(const char *pzLeft, const char *pzRight, size_t count);	

	static std::string sysDecToHex(int i);
	static int  sysHexToDec(char cLeft, char cRight);
	static std::string strRemoveUnNeedChar(const std::string& strSrc);
	static int strUUID32ToUChar16(const std::string& strSrc, u_char *pDest);
	static int uChar16TostrUUID32(u_char *pSrc, std::string& strDest);
	static char*  escapeSQLStr(char *pSrc, char *pSub, char *pRep);

};


NS_END(TA_AQ_Lib)


#endif // UTILITYFUN_H










