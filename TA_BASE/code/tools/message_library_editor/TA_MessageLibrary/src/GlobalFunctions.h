#pragma once

void AppendPath(CString& sFilePath, LPCTSTR szAddition);
void WindowsError(DWORD dwErrorCode);
unsigned int hexstring_to_uword (_TCHAR* pszString);

// Window Access Functions
CView* GetActiveView();
CDocument* GetActiveDoc();
BOOL PostViewMessage(UINT uiMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);