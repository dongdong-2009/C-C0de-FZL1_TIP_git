#if !defined(ArchiveInventoryTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ArchiveInventoryTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveInventoryTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Contains the details for the AR_TABLES table.
  */

// KT, 26 Feb 04, PW# 3133: DELETION_DUE_DATE_COLUMN has been added.
// KT, 26 Feb 04, PW# 3135: START_DATE_COLUMN and END_DATE_COLUMN have been replaced with DATA_DATE_COLUMN.

#include <string>

namespace TA_Base_Core
{
    namespace TA_ArchiveManagerDAI
    {
        static const std::string ARCHIVE_INVENTORY_TABLE_NAME   = "ar_inventory";
        static const std::string INVENTORY_KEY_COLUMN           = "arinve_id";
        static const std::string DATA_DATE_COLUMN               = "data_date";
        static const std::string VOLUME_LABEL_COLUMN            = "volume_label";
        static const std::string DELETING_COLUMN                = "deleting";
        static const std::string RESTORED_DATE_COLUMN           = "restored_date";
        static const std::string RESTORED_BY_COLUMN             = "restored_by";
        static const std::string DELETION_DUE_DATE_COLUMN       = "deletion_due_date";
    }
}

#endif // ArchiveInventoryTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_
