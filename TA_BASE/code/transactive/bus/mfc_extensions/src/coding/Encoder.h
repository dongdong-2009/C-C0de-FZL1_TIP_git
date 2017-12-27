// CodeHelper.h: interface for the CodeHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODINGHELPER_H__086E7544_07DD_4B62_84B3_1343CBE821EF__INCLUDED_)
#define AFX_CODINGHELPER_H__086E7544_07DD_4B62_84B3_1343CBE821EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.H>
#include <memory>


namespace TA_Base_Bus
{
	class Encoder  
	{
	public:		
		virtual ~Encoder();

		static std::auto_ptr<char> Encoder::UTF8ToAnsi(const char* szU8) 
		{
#ifdef UTF8_SUPPORT
			
			//preconversion£¬calculate the memory buffer
			int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
			wchar_t* wszString = new wchar_t[wcsLen + 1];
			//conversion
			MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
			//add'\0' as ending
			wszString[wcsLen] = '\0';
			
			int ansiLen = WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
			std::auto_ptr<char> szAnsi( new char[ansiLen + 1] );
			//conversion
			//wcslen is the peer of strlen for unicode
			::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), szAnsi.get(), ansiLen, NULL, NULL);
			//'\0' for ending
			szAnsi.get()[ansiLen] = '\0';
			delete[] wszString;
			return szAnsi;
#else
			std::auto_ptr<char> szRet( new char[strlen(szU8) + 1] );
			strcpy( szRet.get(), szU8 );
			return szRet;
#endif
		}		
		
		
		static std::auto_ptr<char> Encoder::AnsiToUTF8(const char* szAnsi) 
		{
#ifdef UTF8_SUPPORT
			int wcsLen = MultiByteToWideChar(CP_ACP, NULL, szAnsi, strlen(szAnsi), NULL, 0);
			wchar_t* wszString = new wchar_t[wcsLen + 1];
			//conversion
			MultiByteToWideChar(CP_ACP, NULL, szAnsi, strlen(szAnsi), wszString, wcsLen);
			//'\0' ending
			wszString[wcsLen] = '\0';  
			int u8Len = WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
			std::auto_ptr<char> szU8(new char[u8Len + 1]);
			//conversion
			WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), szU8.get(), u8Len, NULL, NULL);
			szU8.get()[u8Len] = '\0';
			return szU8;
#else
			std::auto_ptr<char> szRet( new char[strlen(szAnsi) + 1] );
			strcpy( szRet.get(), szAnsi );
			return szRet;
#endif
		}

	private:
		Encoder();
	};
}

#endif // !defined(AFX_CODEHELPER_H__086E7544_07DD_4B62_84B3_1343CBE821EF__INCLUDED_)
