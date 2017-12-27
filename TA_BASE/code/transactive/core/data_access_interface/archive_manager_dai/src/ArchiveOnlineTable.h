#if !defined(ArchiveOnlineTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ArchiveOnlineTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveOnlineTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Contains the details for the AR_ONLINE table.
  */

#include <string>

namespace TA_Base_Core
{
    namespace TA_ArchiveManagerDAI
    {
        static const std::string ARCHIVE_ONLINE_TABLE_NAME      = "ar_online";
        static const std::string ARCHIVE_ONLINE_KEY_COLUMN      = "aronli_id";
        static const std::string ONLINE_PERIOD_IN_DAYS_COLUMN   = "online_period";
        static const std::string DATA_EXPIRY_IN_DAYS_COLUMN     = "data_expiry";
    }
}

#endif // ArchiveOnlineTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_
