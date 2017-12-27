/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveManagerDaiUtilities.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Contains the constants used by multiple classes within the ArchiveManagerDai.
  * This was created as a result of the internationalisation of dates from PW #3161.
  */

#include <sstream>
#include <locale>

#include "ace/OS.h"

#include "core/data_access_interface/archive_manager_dai/src/ArchiveManagerDaiUtilities.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

namespace TA_ArchiveManagerDai
{
    std::string convertTimeTToStringDate( const time_t& date, bool includeTime /* = false */ )
    {     
        struct tm dateStruct;
        if ( 0 == ACE_OS::localtime_r( &date, &dateStruct ) )
        {
            return "";
        }

        // THIS ASSUMES THE FORMAT YYYYMMDD OR YYYYMMDD24HHMISS,
        // so it needs to be updated if the constants DATE_FORMAT or DATE_TIME_FORMAT change.

        std::stringstream dateStr( "" );
        
        // Year

        dateStr << ( dateStruct.tm_year + 1900 );      
        
        // Month

        int month = dateStruct.tm_mon + 1;
        if ( month < 10 )
        {
            dateStr << "0";
        }
        dateStr << month;
        
        // Day

        if ( dateStruct.tm_mday < 10 )
        {
            dateStr << "0";
        }
        dateStr << dateStruct.tm_mday;

        if ( includeTime )
        {
            // Hour

            if ( dateStruct.tm_hour < 10 )
            {
                dateStr << "0";
            }
            dateStr << dateStruct.tm_hour;

            // Minute

            if ( dateStruct.tm_min < 10 )
            {
                dateStr << "0";
            }
            dateStr << dateStruct.tm_min;

            // Second

            if ( dateStruct.tm_sec < 10 )
            {
                dateStr << "0";
            }
            dateStr << dateStruct.tm_sec;
        }

        // Check the formatting is correct.

        isDateInCorrectFormat( dateStr.str(), includeTime );

        return dateStr.str();
    }


    bool isDateInCorrectFormat( const std::string& date, bool includeTime /* = false */ )
    {
        // Check that the size is right.

		// TD12474 ++
		TA_Base_Core::DateFormat dateFormat; 
		std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat2); 
		// ++ TD12474

        if( includeTime )
        {
			// there are two extra characters in DATE_TIME_FORMAT
			// when set to YYYYMMDD24HHMISS to specify 24h time
			// therefore expected size is 2 less than DATE_TIME_FORMAT
			// size.
			// This will need to be updated if the constant DATE_TIME_FORMAT changes

			// TD12474 ++
			//if( ( DATE_TIME_FORMAT.size() - 2 ) != date.size() )
			if( ( dateFormatStr.size() - 2 ) != date.size() )
			// ++ TD12474
            {
                return false;
            }
        }
        else
        {
			// TD12474 ++
            //if( DATE_FORMAT.size() != date.size() )
			if( dateFormatStr.size() != date.size() )
			// ++ TD12474
            {
                return false;
            }
        }

        // Check that all the characters are digits.

        std::locale loc;
        for ( std::string::const_iterator it = date.begin(); it != date.end(); ++it )
        {
            if ( !std::isdigit( *it, loc ) )
            {
                return false;
            }
        }
        
        return true;
    }

}
