/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/mfc_extensions/src/utilities/DateUtility.h $
 * @author:  
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/09/26 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 */

#pragma once

#include <string>


namespace TA_Base_Bus
{

    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) DateUtility
    #else
    class AFX_EXT_CLASS DateUtility
    #endif
    {
    public:

        static std::string getDefaultShortDateFormat();
        static std::string getDefaultLongDateFormat();

        static std::string convertNotationToFormattingCode( const std::string& dateNotation );

    };

} // namespace TA_Base_Bus
