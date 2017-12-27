/**
  * String.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/TA_String.cpp $
  * @author:  A. Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  */

#include "core/utilities/src/TA_String.h"
#include "TAAssert.h"

#if defined( WIN32 )
#include <afx.h>
#endif

namespace TA_Base_Core
{
    void gTokeniseCommaSeparatedString(const std::string& str, std::vector<std::string>& tokens)
    {
		std::string::size_type strPos1 = 0;
		std::string::size_type strPos2 = 0;

		while ( strPos2 != str.npos )
		{
			strPos2 = str.find( ",", strPos1 );
            tokens.push_back(str.substr(strPos1, strPos2 - strPos1));
			strPos1 = strPos2 + 1;
		}
    }

    bool toString(va_list args, const char* formatStr, std::string& result, std::string* errorStr)
    {
        // What if formatStr is NULL or empty?

        try
		{
            #if defined( WIN32 )
                CString finalCStr("");    
                finalCStr.FormatV(formatStr, args);

                // TODO LPP: Rather than just discarding the entire message, perhaps we could return 
                // part of it.
                if(finalCStr.GetLength() > MAXMESGSIZE)
                {
                    if (errorStr != NULL)
                    {
		                std::ostringstream stm;
		                stm << "Unable to expand format string as expanded length exceeds " << MAXMESGSIZE << " characters";
                        *errorStr = stm.str();
                    }
                    return false;
                }
                else
                {
                    result = finalCStr;
                }

            #else // SOLARIS
	            char buff[MAXMESGSIZE+1];
                int ret = vsnprintf ( buff, MAXMESGSIZE, formatStr, args );

                if ( ret > MAXMESGSIZE || ret < 0 )
			    {
                    if (errorStr != NULL)
                    {
		                std::ostringstream stm;
		                stm << "Unable to expand format string as expanded length exceeds " << MAXMESGSIZE << " characters";
                        *errorStr = stm.str();
                    }
                    return false;
			    }
                else
                {
                    result = buff;
                }

            #endif

		}
		catch ( ... )
		{
            if (errorStr != NULL)
            {
		        std::ostringstream stm;
		        stm << "Unable to expand format string (was a std::string argument used?)";
                *errorStr = stm.str();
            }
            return false;
		}

        return true;
    }

}   // namespace TA_Base_Core
