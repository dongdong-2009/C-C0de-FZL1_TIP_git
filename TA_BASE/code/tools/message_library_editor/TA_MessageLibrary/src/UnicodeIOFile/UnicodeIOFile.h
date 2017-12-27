// UnicodeIOFile.h: interface for the UnicodeIOFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNICODEIOFILE_H__DAA5C454_286F_4CBB_B26E_FB0A0F4F586E__INCLUDED_)
#define AFX_UNICODEIOFILE_H__DAA5C454_286F_4CBB_B26E_FB0A0F4F586E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UnicodeIOFile : public CFile  
{
	DECLARE_DYNAMIC(UnicodeIOFile);

public:
	UnicodeIOFile();
	UnicodeIOFile(LPCTSTR lpszFileName, UINT nOpenFlags);
	virtual ~UnicodeIOFile();

	UINT ProcessFlags(UINT nOpenFlags);
	WriteString(LPCTSTR lpszOutput);
};

#endif // !defined(AFX_UNICODEIOFILE_H__DAA5C454_286F_4CBB_B26E_FB0A0F4F586E__INCLUDED_)
