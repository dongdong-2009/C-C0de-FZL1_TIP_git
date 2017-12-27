/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/CommandLineParser.cpp $
  * @author:  B. Carbonaro
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class processes the __argc and __argv variable that 
  * contain the command line parameters for MFC applications.  
  */

#pragma warning(disable:4786)

#include "stdafx.h"
#include "CommandLineParser.hpp"

#ifdef TRANSACTIVE_BUILD
#include "core/utilities/src/RunParams.h"

namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        const char DEFAULT_PARAMETER_PREPEND = '-';


        CommandLineParser::CommandLineParser()
        {
        }


        CommandLineParser::~CommandLineParser()
        {
        }


        bool CommandLineParser::parameterExists( const char* p_szParameter )
        {
	        int i;
	        bool bFound = false;
	        CString cstrArgv;
	        CString cstrTempParameter = p_szParameter;

            // First arg will be the exe name, so don't parse it
	        for (i = 1; i < __argc; i++)
	        {
		        cstrArgv = __argv[i];
				cstrArgv.Replace("{%Blank%}"," ");		// accpt blank as input parameters, 1.Jan.2006, hongzhi
                std::string name("");
                std::string value("");
                if(extractParameterNameAndValue(cstrArgv, name, value))
                {
                    CString cstrName(name.c_str());
                    if (cstrName.CompareNoCase( cstrTempParameter ) == 0)
		            {
			            bFound = true;
			            break;
		            }
                }
            }

	        return( bFound );
        }


        std::vector<std::string> CommandLineParser::getParameterValue( const char* p_szParameter )
        {
	        std::vector<std::string> values;

	        int i;
	        char* pValue = NULL;
	        CString cstrArgv;
	        CString cstrTempParameter = p_szParameter;

            // First arg will be the exe name, so don't parse it
	        for (i = 1; i < __argc; i++)
	        {
		        cstrArgv = __argv[i];
				cstrArgv.Replace("{%Blank%}"," ");			// accpt blank as input parameters, 1.Jan.2006, hongzhi
                std::string name("");
                std::string value("");
                if(extractParameterNameAndValue(cstrArgv, name, value))
                {
                    CString cstrName(name.c_str());
                    if (cstrName.CompareNoCase( cstrTempParameter ) == 0)
		            {
                        values.push_back(value);
                    }
    		    }
	        }

	        return values;
        }


        bool CommandLineParser::extractParameterNameAndValue(const char* p_szParameterValuePair, std::string& p_szName, std::string& p_szValue)
        {
            p_szName = "";
            p_szValue = "";
            bool status = true;

            // Expect the name to start with two hyphens followed by an alpha

		    if (strncmp(p_szParameterValuePair, "--", 2) == 0)
		    {
			    p_szParameterValuePair += 2;
		    }
		    else
		    {
			    status = false;
		    }

		    if (!isalpha(*p_szParameterValuePair))
		    {
			    status = false;
		    }

		    // Extract the name. Hyphens are skipped but cause the next character
		    // to be upshifted

		    bool upshift = true;
		    while (status && *p_szParameterValuePair != '\0' && *p_szParameterValuePair != '=' && !isspace(*p_szParameterValuePair))
		    {
			    if (*p_szParameterValuePair == '-')
			    {
				    upshift = true;
			    }
			    else if (upshift)
			    {
				    p_szName += toupper(*p_szParameterValuePair);
				    upshift = false;
			    }
			    else
			    {
				    p_szName += *p_szParameterValuePair;
			    }
			    p_szParameterValuePair++;
		    }

            // If we have an "=" then extract the value

		    if (status && *p_szParameterValuePair == '=')
		    {
			    p_szParameterValuePair++;
			    while (*p_szParameterValuePair != '\0')
			    {
				    p_szValue += *p_szParameterValuePair;
				    p_szParameterValuePair++;
			    }
		    }

            return status;
        }

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
