/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/AppSettings.hpp $
  * @author:  B. Carbonaro
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the header file for the AppSettings class.
  */

#ifndef APPSETTINGS_HPP
#define APPSETTINGS_HPP

#include <Windows.h>

#ifdef TRANSACTIVE_BUILD
namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        const int BUFFER_SIZE = 500;

        class AppSettings
        {
        public:
            /**
              * AppSettings
              * 
              * This is the constructor for the AppSettings class.
              *
              * @param p_szCompanyName is a string value that specifies the company
              *        name that will appear in the registry under 
              *        HKEY_CURRENT_USER/Software.
              *
              * @param p_szApplicationName is a string value that specifies the 
              *		   application name that will appear in the registry under 
              *        HKEY_CURRENT_USER/Software/<Company Name>.
              *
              */
	        AppSettings( const char* p_szCompanyName, const char* p_szApplicationName );


            /**
              * ~AppSettings
              * 
              * This is the destructor for the AppSettings class.
              *
              */
	        ~AppSettings();

            /**
              * setStringValue
              * 
              * This method sets a string value in the registry using the label
              * and value provided as parameters
              *
              * @return True if setting the value was successful otherwise it 
              *         returns false.
              *
              * @param p_szLabel is a string value that specifies the label name of
              *        the string key created in the registry.
              *
              * @param p_szValue is a string value that specifies the value 
              *        that is to be assigned to the label specified by the p_szLabel 
              *        parameter.
              *
              */
	        bool setStringValue( const char* p_szLabel, char* p_szValue );

            /**
              * getStringValue
              * 
              * This method retrieves the value of the string registry key as
              * specified by the supplied parameter.
              *
              * @return A pointer to a string that contains the value of the 
              *         registry string key retrieved. If retrieval of a value 
              *         was unsuccessful, then this pointer will be NULL.
              *
              * @param p_szLabel is a string value that specifies the label name of
              *        the string key to be retrieved.
              *
              */
            char* getStringValue( const char* p_szLabel);

            /**
              * setNumberValue
              * 
              * This method sets a number value in the registry using the label
              * and value provided as parameters
              *
              * @return True if setting the value was successful otherwise it 
              *         returns false.
              *
              * @param p_szLabel is a string value that specifies the label name of
              *        the string key created in the registry.
              *
              * @param p_lValue is a long integer value that specifies the value 
              *        that is to be assigned to the label specified by the p_szLabel 
              *        parameter.
              *
              */
	        bool setNumberValue( const char* p_szLabel, long p_lValue );

            /**
              * getNumberValue
              * 
              * This method retrieves the value of the number registry key as 
              * specified by the supplied parameter.
              *
              * @return A long integer value that contains the value of the 
              *         registry number key retrieved. 
              *
              * @param p_szLabel is a string value that specifies the label name of
              *        the string key to be retrieved.
              *
              */
	        long getNumberValue( const char* p_szLabel );

        protected:
        private:

            /**
              * Stores the registry sub key.
              */
	        char m_szSubKey[BUFFER_SIZE];

            /**
              * Used for temporary string storage.
              */
	        char m_szTemp[BUFFER_SIZE];

            /**
              * Used for the temporary storage of a long value.
              */
            long m_lTemp;

            /**
              * Stores the handle of an opned registry key.
              */
	        HKEY m_hRegKey;
        };

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD

#endif
