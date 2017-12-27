//////////////////////////////////////////////////////////////////////
///		@file		UtilityFun.cpp
///		@author		zhilin,ouyang
///		@date		2010-12-8 14:44:41
///
///		@brief	    to declare some useful methods used in SQL statement
///                 construct
//////////////////////////////////////////////////////////////////////

#include "UtilityFun.h"

NS_BEGIN(TA_AQ_Lib)

char* CUtilityFun::strlwr(char* pszSrc)
{
	char *pszOri = pszSrc;
	for (; *pszSrc != '\0'; pszSrc++)	
		*pszSrc = tolower(*pszSrc);

	return pszOri;
}

char* CUtilityFun::strtolwr(char* pszSrc)
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

char* CUtilityFun::strupper(char* pszSrc)
{
	char *pszOri = pszSrc;
	for (; *pszSrc != '\0'; pszSrc++)	
		*pszSrc = toupper(*pszSrc);	

	return pszOri;
}

char* CUtilityFun::strtoupper(char* pszSrc)
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

std::string CUtilityFun::int2str(unsigned nVal)
{
	std::string strRet;
	unsigned nDigit = 0;
	while (nVal >= 10)
	{
		nDigit = nVal - (nVal / 10) * 10;
		nDigit |= 0x30;

		strRet.insert(0, 1, nDigit);
		nVal = nVal / 10;
	}
	nDigit = nVal | 0x30;
	strRet.insert(0, 1, nDigit);

	return strRet;
}

std::string CUtilityFun::u64tostr(ulong64 u64Val)
{
	std::string strRet;
	unsigned nDigit = 0;
	while (u64Val >= 10)
	{
		nDigit = u64Val - (u64Val / 10) * 10;
		nDigit |= 0x30;

		strRet.insert(0, 1, nDigit);
		u64Val = u64Val / 10;
	}
	nDigit = u64Val | 0x30;
	strRet.insert(0, 1, nDigit);

	return strRet;
}

void CUtilityFun::int2str(int uVal, char sz[])
{
	int nDigit = 0;
	int nIndex = 0;
	int nSign  = uVal;

	if ( nSign < 0 ) 
	{
		uVal = -uVal;
		sz[nIndex++] = '-';
	}

	while (uVal >= 10)
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

ulong64 CUtilityFun::pow64(u_int uBase, u_int uExp)
{
	ulong64 ulRet = 1;
	if (!uExp) return 1;

	for (u_int i = 0; i < uExp; i++)
	{
		ulRet *= uBase;
	}

	return ulRet;
}

ulong64 CUtilityFun::atoui64(std::string& strValue64)
{
	ulong64 ulRet = 0;
	size_t nBitValue = 0;
	size_t  nIndex   = 0;	

	std::string::reverse_iterator rit;
	for ( rit=strValue64.rbegin() ; rit < strValue64.rend(); rit++ )
	{
		nBitValue = (*rit) & 0xF;
		ulRet += (nBitValue * pow64(10, nIndex));

		nIndex++;
	}

	return ulRet;
}

int CUtilityFun::stricmp_(const char* pzLeft, const char* pzRight)
{
	const char* ch1 = pzLeft;
	const char* ch2 = pzRight;
	int res = 0;

	if (strlen(ch1) != strlen(ch2))
	{
		res = 1;
		return res;
	}

	while (true) {
		res = toupper(*ch1) - toupper(*ch2);
		if (res)
			break;
		else
			if (!*ch1)
				break;

		ch1++;             
		ch2++;
	}
	return res;
}

int CUtilityFun::strnicmp_(const char *pzLeft, const char *pzRight, size_t count)
{
	const char* ch1 = pzLeft;
	const char* ch2 = pzRight;
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

std::string  CUtilityFun::sysDecToHex(int i)
{
	char tmp[3] = {0}; 
	u_char t;
	for (u_long j = 0;j < 2;j++) 
	{ 
		t = (i >> (j << 2)) & 0xF;
		if (t < 10)
		{
			tmp[1 - j] = 48 + t;
		}
		else
		{
			tmp[1 - j] = 55 + t;
		}
	} 

	return std::string(tmp);
}

int  CUtilityFun::sysHexToDec(char cLeft, char cRight)
{
	int  nRetVal    = 0;
	int nRightValue = 0;
	int nLeftValue  = 0;

	if (cRight > 0x40)
		nRightValue = cRight - 55;
	else
		nRightValue = cRight & 0xF;

	if ( cLeft > 0x40)
	{
		nLeftValue = (cLeft - 55) << 4;
	}
	else
	{
		nLeftValue = (cLeft & 0xF) << 4;
	}

	nRetVal = nLeftValue + nRightValue;

	return nRetVal;
}

std::string CUtilityFun::strRemoveUnNeedChar(const std::string& strSrc)
{
	std::string   strDest;
	char		  chUnNeed = '-';

	if (strSrc.empty() || strSrc.length() > 36)
	{
		return strDest;
	}

	for (size_t nIndex = 0; nIndex < strSrc.length(); nIndex++)
	{
		if (chUnNeed != strSrc[nIndex])
		{
			strDest += strSrc[nIndex]; 
		}
	}

	return strDest;
}

int CUtilityFun::strUUID32ToUChar16(const std::string& strSrc, u_char *pDest)
{
	int          nRes = 0;
	int          nPos = 0;
	if (strSrc.empty() || strSrc.length() > 32)
	{
		nRes = -1;
		return  nRes;
	}

	nPos = 0;
	for (size_t nIndex = 0; nIndex < strSrc.length(); nIndex+=2)
	{
		pDest[nPos++] = (u_char)sysHexToDec(strSrc[nIndex], strSrc[nIndex+1]);
	}

	return  nRes;
}

int CUtilityFun::uChar16TostrUUID32(u_char *pSrc, std::string& strDest)
{
	int          nRes = 0;
	int          nPos = 0;
	if (NULL == pSrc)
	{
		nRes = -1;
		return  nRes;
	}

	strDest.clear();
	nPos = 0;
	for (int nIndex = 0; nIndex < 16; nIndex++)
	{
		strDest += sysDecToHex(pSrc[nIndex]);
	}

	return  nRes;
}



char*  CUtilityFun::escapeSQLStr(char *pSrc, char *pSub, char *pRep)
{
	char *result = NULL;
	char *pc1 = NULL;
	char *pc2 = NULL;
	char *pc3 = NULL;
	int isource = 0;
	int isub = 0;
	int irep = 0;

	isub = strlen(pSub);
	irep = strlen(pRep);
	isource = strlen(pSrc);

	if (NULL == *pSub)
	{
		return strdup(pSrc);
	}

	result = (char *)malloc(( (irep > isub) ? (float)strlen(pSrc) / isub* irep+ 1:isource ) * sizeof(char));
	pc1 = result;
	while (*pSrc != NULL)
	{

		pc2 = pSrc;
		pc3 = pSub;
		while(*pc2 == *pc3 && *pc3 != NULL && *pc2 != NULL)
		{
			pc2++;
			pc3++;
		}

		if (NULL == *pc3)
		{
			pc3 = pRep;
			while (*pc3 != NULL)
			{
				*pc1++ = *pc3++;
			}
			pc2--;
			pSrc = pc2;
		}
		else
		{
			*pc1++ = *pSrc;
		}
		pSrc++;
	}
	*pc1 = NULL;
	return result;
}

NS_END(TA_AQ_Lib)





