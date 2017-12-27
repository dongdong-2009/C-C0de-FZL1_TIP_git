/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/LoadString.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_LOADSTRING_H__78510E34_7631_42AA_85C5_945FC5D73296__INCLUDED_)
#define AFX_LOADSTRING_H__78510E34_7631_42AA_85C5_945FC5D73296__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (_MSC_VER <= 1200) // VC6 only

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/MessageResourceException.h"

using TA_Base_Core::MessageResourceException;

#pragma warning(push)
#pragma warning(disable: 4200)

	struct STRINGRESOURCEIMAGE
	{
		WORD nLength;
		WCHAR achString[];
	};

#pragma warning(pop)	// C4200

inline const STRINGRESOURCEIMAGE* GetStringResourceImage( HINSTANCE hInstance, HRSRC hResource, UINT id ) throw()
{
	HGLOBAL hGlobal = ::LoadResource( hInstance, hResource );
	if(!hGlobal)
    {
        // The resource did not load successfull, so need to log why

        // Get the error ID.
        DWORD errorID = GetLastError();

        CString errorMessage;

        // And convert that ID into a string.
        if (
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, // Format flags - specifies the system messages should be searched
            NULL,               // The "source" for formatting. Not necessary as it will come from system resources.
            errorID,            // The numerical ID of the error retrieved from GetLastError
            0,                  // The lanugage ID. Specifying 0 will enable the system to search the default language.
            errorMessage.GetBuffer(2048),2048, // The buffer in which to place the error string, and the length of that buffer
            NULL)               // Arguments to format. Not needed.
            == 0 )
        {
            // The message formatting failed, so just use the ID.
            errorMessage.Format(_T("%d"),errorID);
        }
        
        if (errorID == ERROR_RESOURCE_NAME_NOT_FOUND)
        {
            TA_THROW(MessageResourceException(std::string(errorMessage),MessageResourceException::RESOURCE_NOT_FOUND_ERROR,id));
        }
        else if (errorID == ERROR_RESOURCE_LANG_NOT_FOUND)
        {
            TA_THROW(MessageResourceException(std::string(errorMessage),MessageResourceException::LANGUAGE_ERROR,id));
        }
        else
        {
            TA_THROW(MessageResourceException(std::string(errorMessage),MessageResourceException::GENERAL_ERROR,id));
        }

        // NickJ: 07/12/04: Removed NULL return, and replaced with TransActive exceptions
		// return( NULL );
    }

	const STRINGRESOURCEIMAGE* pImage = (const STRINGRESOURCEIMAGE*)::LockResource( hGlobal );
	if(!pImage)
    {
        TA_THROW(MessageResourceException("Could not lock the resorce.",
            MessageResourceException::GENERAL_ERROR,id));
        // NickJ: 07/12/04: Removed NULL return, and replaced with TransActive exceptions
		// return( NULL );
    }

	ULONG nResourceSize = ::SizeofResource( hInstance, hResource );
	const STRINGRESOURCEIMAGE* pImageEnd = (const STRINGRESOURCEIMAGE*)(LPBYTE( pImage )+nResourceSize);
	UINT iIndex = id&0x000f;

	while( (iIndex > 0) && (pImage < pImageEnd) )
	{
		pImage = (const STRINGRESOURCEIMAGE*)(LPBYTE( pImage )+(sizeof( STRINGRESOURCEIMAGE )+(pImage->nLength*sizeof( WCHAR ))));
		iIndex--;
	}

	if( pImage >= pImageEnd )
    {
        TA_THROW(MessageResourceException("Resoruce size was too big.",
            MessageResourceException::GENERAL_ERROR,id));
        // NickJ: 07/12/04: Removed NULL return, and replaced with TransActive exceptions
		// return( NULL );
    }
	
	if( pImage->nLength == 0 )
    {
        TA_THROW(MessageResourceException("Resource was 0 length.",
            MessageResourceException::GENERAL_ERROR,id));
        // NickJ: 07/12/04: Removed NULL return, and replaced with TransActive exceptions
		// return( NULL );
    }

	return( pImage );
}

inline const STRINGRESOURCEIMAGE* LoadStringResourceImage( HINSTANCE hInstance, UINT id, WORD wLanguage ) throw()
{
	//TD18067 marc_bugfix use FindResource so that processing would not be dependent on how resource dll was built
	//HRSRC hResource = ::FindResourceEx( hInstance, RT_STRING, MAKEINTRESOURCE( ((id>>4)+1) ), wLanguage );
	HRSRC hResource = ::FindResource(hInstance, MAKEINTRESOURCE( ((id>>4)+1) ), RT_STRING);

    
	if(!hResource)
    {
        // The resource could not be found

        // Get the error ID.
        DWORD errorID = GetLastError();

        CString errorMessage;

        // And convert that ID into a string.
        if (
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, // Format flags - specifies the system messages should be searched
            NULL,               // The "source" for formatting. Not necessary as it will come from system resources.
            errorID,            // The numerical ID of the error retrieved from GetLastError
            0,                  // The lanugage ID. Specifying 0 will enable the system to search the default language.
            errorMessage.GetBuffer(2048),2048, // The buffer in which to place the error string, and the length of that buffer
            NULL)               // Arguments to format. Not needed.
            == 0 )
        {
            // The message formatting failed, so just use the ID.
            errorMessage.Format(_T("%d"),errorID);
        }
        
        if (errorID == ERROR_RESOURCE_NAME_NOT_FOUND)
        {
            TA_THROW(MessageResourceException(std::string(errorMessage),MessageResourceException::RESOURCE_NOT_FOUND_ERROR,id));
        }
        else if (errorID == ERROR_RESOURCE_LANG_NOT_FOUND)
        {
            TA_THROW(MessageResourceException(std::string(errorMessage),MessageResourceException::LANGUAGE_ERROR,id));
        }
        else
        {
            TA_THROW(MessageResourceException(std::string(errorMessage),MessageResourceException::GENERAL_ERROR,id));
        }

        // NickJ: 07/12/04: Removed trace and NULL return, and replaced with TransActive exceptions
        //TRACE1("Failed to load resource, error = %d", ::GetLastError());
		// return( NULL );
    }
	else
    {
		return GetStringResourceImage( hInstance, hResource, id );
    }
}

bool VC6LoadString(CString& sOutput, HINSTANCE hResourceDll, UINT uiStringID, WORD wLanguage)
{
	const STRINGRESOURCEIMAGE* pImage = LoadStringResourceImage( hResourceDll, uiStringID, wLanguage);
	if(!pImage)
	{
		sOutput.Empty();
		return false;
	}
	
	try
	{
#ifdef _UNICODE
		// Allocate internal buffer
		wchar_t* pszBuffer = sOutput.GetBuffer(pImage->nLength);
		// Straight copy of image
		memcpy(pszBuffer, pImage->achString, pImage->nLength*sizeof(wchar_t));
		// Release the lock
		sOutput.ReleaseBuffer(pImage->nLength);
#else
		// get the required buffer length in chars
		int iLength = ::WideCharToMultiByte( CP_THREAD_ACP, 0, pImage->achString, pImage->nLength, NULL, 0, NULL, NULL );
		// Allocate internal buffer
		char* pszBuffer = sOutput.GetBuffer(iLength);
		// Narrow
		::WideCharToMultiByte( CP_THREAD_ACP, 0, pImage->achString, pImage->nLength, pszBuffer, iLength, NULL, NULL );
		// Release the lock
		sOutput.ReleaseBuffer(iLength);
#endif
	}
	catch(CMemoryException* e)
	{
        // Create the error message
        CString errorMessage = _T("Exception caught while attempting to load message box string for message ID %d. Exception error reason:");
        
        // and extract the message from the exception
        CString exceptionMessage;
        e->GetErrorMessage(exceptionMessage.GetBuffer(2048),2048);
		e->Delete();

        // Throw a MessageResourceException instead
        TA_THROW(TA_Base_Core::MessageResourceException(std::string(errorMessage),
            TA_Base_Core::MessageResourceException::GENERAL_ERROR,uiStringID));
	}

	return true;
}

#endif // VC6

#endif // !defined(AFX_LOADSTRING_H__78510E34_7631_42AA_85C5_945FC5D73296__INCLUDED_)