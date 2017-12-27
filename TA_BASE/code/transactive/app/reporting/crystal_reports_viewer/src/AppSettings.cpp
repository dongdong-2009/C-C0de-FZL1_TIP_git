/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/AppSettings.cpp $
  * @author:  B. Carbonaro
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides an API that allows strings and number to 
  * be saved and restored to and from the HKEY_CURRENT_USER section
  * of the registry.
  */

#include <stdio.h>
#include "AppSettings.hpp"

const HKEY HOME_REG_KEY = HKEY_CURRENT_USER;

#ifdef TRANSACTIVE_BUILD
namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        AppSettings::AppSettings( const char* p_szCompanyName, const char* p_szApplicationName )
        : m_hRegKey( 0 )
        {
	        strcpy( m_szSubKey, "Software\\" );
	        strcat( m_szSubKey, p_szCompanyName );
	        strcat( m_szSubKey, "\\" );
	        strcat( m_szSubKey, p_szApplicationName );

	        LONG lResult = RegCreateKey( HOME_REG_KEY,
								         m_szSubKey,
								         &m_hRegKey );
	        if (lResult == ERROR_SUCCESS)
	        {
		        // Success.
	        }
        }


        AppSettings::~AppSettings()
        {
        }


        bool AppSettings::setStringValue( const char* p_szLabel, char* p_szValue )
        {
	        bool bResult = false;

	        if ((p_szLabel) && (p_szValue))
	        {
		        LONG lResult = RegSetValueEx( m_hRegKey,
									          p_szLabel,
									          0,
									          REG_SZ,
									          (const unsigned char *)p_szValue,
									          strlen( p_szValue ));
		        if (lResult == ERROR_SUCCESS)
		        {
			        bResult = true;
		        }
	        }

	        return( bResult );
        }


        char* AppSettings::getStringValue( const char* p_szLabel)
        {
	        if (p_szLabel)
	        {
		        ULONG lNum = sizeof( m_szTemp );
		        LONG lResult = RegQueryValueEx( m_hRegKey,
										        p_szLabel,
										        0,
										        0,
										        (unsigned char*)m_szTemp,
										        &lNum);
		        if (lResult == ERROR_SUCCESS)
		        {
			        return( m_szTemp );
		        }
		        else
		        {
			        return( NULL );
		        }
	        }
	        else
	        {
		        return( NULL );
	        }
        }


        bool AppSettings::setNumberValue( const char* p_szLabel, long p_lValue )
        {
	        bool bResult = false;

	        if (p_szLabel)
	        {
		        LONG lResult = RegSetValueEx( m_hRegKey,
									          p_szLabel,
									          0,
									          REG_DWORD,
									          (const unsigned char *)&p_lValue,
									          sizeof(p_lValue));
		        if (lResult == ERROR_SUCCESS)
		        {
			        bResult = true;
		        }
	        }

	        return( bResult );
        }


        long AppSettings::getNumberValue( const char* p_szLabel)
        {
	        if (p_szLabel)
	        {
		        ULONG lNum = sizeof( m_lTemp );
		        LONG lResult = RegQueryValueEx( m_hRegKey,
										        p_szLabel,
										        0,
										        0,
										        (unsigned char*)&m_lTemp,
										        &lNum);
		        if (lResult == ERROR_SUCCESS)
		        {
			        return( m_lTemp );
		        }
		        else
		        {
			        return( 0 );
		        }
	        }
	        else
	        {
		        return( 0 );
	        }
        }

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
