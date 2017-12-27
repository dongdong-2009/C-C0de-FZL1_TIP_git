// UnicodeIOFile.cpp: implementation of the UnicodeIOFile class.
//
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "UnicodeIOFile.h"
#include "..\TA_MessageLibrary.h"

static const wchar_t BOM = wchar_t(0xFEFF); // UTF16 Little Endian

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(UnicodeIOFile, CFile)

UnicodeIOFile::UnicodeIOFile()
	:CFile()
{

}

UnicodeIOFile::~UnicodeIOFile()
{

}

UnicodeIOFile::UnicodeIOFile(LPCTSTR lpszFileName, UINT nOpenFlags)
	:CFile(lpszFileName, ProcessFlags(nOpenFlags))
{
	if(!GetApp()->m_bNarrowOutput)
	{
		Write((const void*)&BOM, sizeof(BOM));
		Flush();
	}
}

UnicodeIOFile::WriteString(LPCTSTR lpszOutput)
{
	CString sOutput(lpszOutput);
	sOutput.Replace(_T("\n"), _T("\r\n"));

	if(GetApp()->m_bNarrowOutput)
	{
		// Convert it to char and output
		char* pszNarrow = new char[sOutput.GetLength() + 1];
		wcstombs(pszNarrow, sOutput, sOutput.GetLength());
		Write((const void*)pszNarrow, sOutput.GetLength());
		delete [] pszNarrow;
	}
	else
		Write((const void*)(LPCTSTR)sOutput, sOutput.GetLength() * sizeof(TCHAR));
}

UINT UnicodeIOFile::ProcessFlags(UINT nOpenFlags)
{
	UINT uiNewFlags = nOpenFlags;

	if(!(uiNewFlags & CFile::typeBinary))
		uiNewFlags |= CFile::typeBinary;

	return uiNewFlags;
}



