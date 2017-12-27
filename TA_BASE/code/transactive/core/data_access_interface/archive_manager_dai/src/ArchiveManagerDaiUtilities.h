#if !defined(ArchiveManagerDaiUtilities_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define ArchiveManagerDaiUtilities_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveManagerDaiUtilities.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Contains the constants used by multiple classes within the ArchiveManagerDai.
  * This was created as a result of the internationalisation of dates from PW #3161.
  */

#include <string>
#include <time.h>

namespace TA_ArchiveManagerDai
{
    //
    // This is the format of the date that we use within the database.
    // It is not shown to the user.
    //
    // ******** IMPORTANT ********
    // If these are changed, the following methods need to be updated:
    // 1. convertTimeTToStringDate()
    // 2. isDateInCorrectFormat()
    //

    //const std::string DATE_TIME_FORMAT( "YYYYMMDDHH24MISS" ); //TD12474 
    //const std::string DATE_FORMAT( "YYYYMMDD" ); //TD12474 

    /**
      * convertTimeTToStringDate
      *
      * Converts the date from a time_t into a string.
      * THIS ASSUMES THE FORMAT YYYYMMDD OR YYYYMMDD24HHMISS,
      * so it needs to be updated if the constants DATE_FORMAT or DATE_TIME_FORMAT change.

      * Post-condition: isDateInCorrectFormat( <String Date>, includeTime )
      *
      * @return     std::string
      *             The date as a string format:
      *             1. DATE_TIME_FORMAT if includeTime is true, or 
      *             2. DATE_FORMAT format if includeTime is false.
      *
      * @param      const time_t& date
      *             The date as a time_t.
      * @param      bool includeTime
      *             True  => Include the date and time in the output string.
      *             False => Only include the date in the output string.
      */

    std::string convertTimeTToStringDate( const time_t& date, bool includeTime = false );
    
    /**
      * isDateInCorrectFormat
      *
      * Verifies that the date is in the correct format:
      * 1. DATE_TIME_FORMAT if includeTime is true, or 
      * 2. DATE_FORMAT format if includeTime is false.
      *
      * @return     bool
      *             True if the date is in the correct format.
      *             False otherwise.
      *
      * @param      const std::string& date
      *             The date as a string.
      * @param      bool includeTime
      *             True  => The string should be in the DATE_TIME_FORMAT format.
      *             False => The string should be in the DATE_FORMAT format.
      */

    bool isDateInCorrectFormat( const std::string& date, bool includeTime = false );
}


#endif // !defined(ArchiveManagerDaiUtilities_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
