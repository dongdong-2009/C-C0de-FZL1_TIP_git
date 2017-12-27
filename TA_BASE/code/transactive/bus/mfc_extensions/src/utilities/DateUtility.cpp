/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/bus/mfc_extensions/src/utilities/DateUtility.cpp $
 * @author:  
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/09/26 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 */

#include "../stdafx.h"
#include "DateUtility.h"


namespace TA_Base_Bus
{

    std::string DateUtility::getDefaultShortDateFormat()
    {
        char dateFormat[512];
        GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, dateFormat, 512 );
        return dateFormat;
    }


    std::string DateUtility::getDefaultLongDateFormat()
    {
        char dateFormat[512];
        GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SLONGDATE, dateFormat, 512 );
        return dateFormat;
    }


    std::string DateUtility::convertNotationToFormattingCode(const std::string& dateNotation)
    {
        struct NotationFormattingCode
        {
            const char* notation;
            const char* formattingCode;
        };

        NotationFormattingCode pairs[] =
        { 
            { "yyyyy", "%Y"}, { "yyyy",  "%Y"}, { "yyy",   "%y"}, { "yy",    "%y"}, { "y",     "%y"},
            { "MMMM",  "%B"}, { "MMM",   "%b"}, { "MM",    "%m"}, { "M",     "%m"},
            { "dddd",  "%A"}, { "ddd",   "%a"}, { "dd",    "%d"}, { "d",     "%d"},
        };

        std::string dateFormattingCode = dateNotation;

        for ( size_t i = 0; i < sizeof(pairs) / sizeof(NotationFormattingCode); ++i )
        {
            size_t pos = dateFormattingCode.find( pairs[i].notation );

            if ( pos != std::string::npos )
            {
                dateFormattingCode.replace( pos, ::strlen( pairs[i].notation), pairs[i].formattingCode );
            }
        }

        return dateFormattingCode;
    }

}
