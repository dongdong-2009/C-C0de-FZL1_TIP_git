/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/CommandLineParser.hpp $
  * @author:  B. Carbonaro
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the header file for the CommandLineParser class.
  */

#ifndef COMMANDLINEPARSER_HPP
#define COMMANDLINEPARSER_HPP

#include <vector>

#ifdef TRANSACTIVE_BUILD
#include "core/utilities/src/RunParams.h"

namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        class CommandLineParser
        {
        public:

            /**
              * CommandLineParser
              * 
              * This is the constructor for the CommandLineParser class.
              *
              */
            CommandLineParser();

            /**
              * ~CommandLineParser
              * 
              * This is the destructor for the CommandLineParser class.
              *
              */
	        ~CommandLineParser();

            /**
              * parameterExists
              * 
              * This method checks for the existence of the command line paramater
              * as specified by the supplied parameter.
              *
              * @return True if the command line parameter exists otherwise it returns false.
              *
              * @param p_szParameter is a string value the specifies the command line 
              *        parameter whose existence is being determined.
              *
              */
	        bool parameterExists( const char* p_szParameter );

            /**
              * getParameterValue
              * 
              * This method returns the value of the of the command line
              * parameter as specified by the supplied parameter. An example
              * of a command line parameter and value pair is as follows:
              *    -<parameter1> <value>
              *
              * @return A pointer to a string that contains the value of the 
              *         command line parameter. If the command line parameter
              *         does not contain a value or it doesn't exist, then the
              *         pointer will be returned as NULL.
              *
              * @param p_szParameter is a string value the specifies the command line 
              *        parameter for which the value is being retrieved.
              *
              */
            std::vector<std::string> getParameterValue( const char* p_szParameter );

        protected:
        private:

            /**
              * Used for temporary string storage.
              */
	        CString m_cstrTemp;

            bool extractParameterNameAndValue(const char* p_szParameterValuePair, std::string& p_szName, std::string& p_szValue);
        };

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD

#endif
