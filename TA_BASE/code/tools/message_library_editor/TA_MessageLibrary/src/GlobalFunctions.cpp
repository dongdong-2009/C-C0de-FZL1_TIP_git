
#include "StdAfx.h"
#include "GlobalFunctions.h"

void AppendPath(CString& sFilePath, LPCTSTR szAddition)
{
	if(!sFilePath.IsEmpty() && sFilePath[sFilePath.GetLength() - 1] != '\\')
		sFilePath += _T("\\"); //SL

	sFilePath += szAddition;
}

void WindowsError(DWORD dwErrorCode)
{
	LPVOID lpMsgBuf;
	::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION ); //SL
	
	LocalFree( lpMsgBuf );
}

unsigned int hexstring_to_uword (_TCHAR* pszString)
{
	unsigned int i = 0, j = 0;

	while (pszString && *pszString && isxdigit(*pszString)) 
	{
		i = *pszString++ - '0'; //SL
		if (9 < i)
			i -= 7;
		j <<= 4;
		j |= (i & 0x0f);
	}

	return(j);
}

CView* GetActiveView()
{
	CFrameWnd* pFrameWnd = reinterpret_cast<CFrameWnd*>(AfxGetMainWnd());
	if(pFrameWnd)
		return pFrameWnd->GetActiveView();
	else
		return NULL;
}

CDocument* GetActiveDoc()
{
	CFrameWnd* pFrameWnd = reinterpret_cast<CFrameWnd*>(AfxGetMainWnd());
	if(pFrameWnd)
		return pFrameWnd->GetActiveDocument();
	else
		return NULL;
}

BOOL PostViewMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	CFrameWnd* pFrameWnd = reinterpret_cast<CFrameWnd*>(AfxGetMainWnd());
	if(pFrameWnd)
	{
		CView* pView = pFrameWnd->GetActiveView();
		return pView->PostMessage(uiMsg, wParam, lParam);
	}
	else
		return FALSE;
}