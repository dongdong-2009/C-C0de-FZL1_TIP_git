/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/DeleteArchiveHelper.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Performs back end work for the Archive Manager deletion process.
  */

#if !defined(DELETE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_)
#define DELETE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_

#include <map>
#include <string>

#include "core/data_access_interface/archive_manager_dai/src/IArchiveInventory.h"
#include "app/data_management/archive_manager/src/BasicHelper.h"

namespace TA_Base_App
{
    class DeleteArchiveHelper
    {
    public:

        /**
          * DeleteArchiveHelper
          *
          * Standard constructor.
          */

        DeleteArchiveHelper() {};

        /**
          * ~DeleteArchiveHelper
          *
          * Standard destructor.
          */

        virtual ~DeleteArchiveHelper() {};

        // KT 11 Mar 04: PW #3133. The methods initiateDelete(), completeDelete(),
        // initiateChangeDeletionDate() and completeChangeDeletionDate() have replaced
        // the deleteRestoredArchives() method because the 
        // DeleteArchiveDlg now does both the deletion of the data and the
        // modification of the deletion due date, so the message sending
        // requirements are different.

        /** 
          * initiateDelete
          *
          * Sends the TA_Base_Core::ArchiveAudit::DeleteArchiveRequest audit message
          * and checks the user's privileges.
          *
          * @exception      ArchiveException
          *                 Thrown if:
          *                 1) The user does not have the required privileges.
          *                 2) The privileges cannot be determined.
          */

        void initiateDelete();

        /** 
          * completeDelete
          *
          * Sends the required audit message depending on how the deletion went.
          * status = IDOK       => sends TA_Base_Core::ArchiveAudit::ArchiveDeleted
          * status = IDCANCEL   => sends TA_Base_Core::ArchiveAudit::DeleteArchiveCancelled
          * status = IDABORT    => sends TA_Base_Core::ArchiveAudit::DeleteArchiveFailed
          *
          * Preconditions:  (status != IDABORT) || !abortMsg.empty()
          *                 ( IDOK == status ) || ( IDCANCEL == status ) || ( IDABORT == status )
          *
          * @param      int status
          *             IDOK, IDCANCEL or IDABORT.
          * @param      const std::string& abortMsg
          *             The description to send with the DeleteArchiveFailed audit message.
          *             
          * @exception  ArchiveException
          *             Thrown if:
          *             1) The user does not have the required privileges.
          *             2) The privileges cannot be determined.
          */

        void completeDelete( int status, const std::string& abortMsg );

        /** 
          * initiateChangeDeletionDate
          *
          * Sends the TA_Base_Core::ArchiveAudit::ChangeDeletionDateRequest audit message
          * and checks the user's privileges.
          *
          * @exception      ArchiveException
          *                 Thrown if:
          *                 1) The user does not have the required privileges.
          *                 2) The privileges cannot be determined.
          */

        void initiateChangeDeletionDate();

        /** 
          * completeChangeDeletionDate
          *
          * Sends the required audit message depending on how the deletion went.
          * status = IDOK       => sends TA_Base_Core::ArchiveAudit::DeletionDateChanged
          * status = IDCANCEL   => sends TA_Base_Core::ArchiveAudit::ChangeDeletionDateCancelled
          * status = IDABORT    => sends TA_Base_Core::ArchiveAudit::ChangeDeletionDateFailed
          *
          * Preconditions:  (status != IDABORT) || !abortMsg.empty()
          *                 ( IDOK == status ) || ( IDCANCEL == status ) || ( IDABORT == status )
          *
          * @param      int status
          *             IDOK, IDCANCEL or IDABORT.
          * @param      const std::string& abortMsg
          *             The description to send with the ChangeDeletionDateFailed audit message.
          *             
          * @exception  ArchiveException
          *             Thrown if:
          *             1) The user does not have the required privileges.
          *             2) The privileges cannot be determined.
          */

        void completeChangeDeletionDate( int status, const std::string& abortMsg );

        /**
          * getArchiveInventoryItems
          *
          * Gets all the archive inventory items from the database
          * they will be ordered by start date.
          *
          * @return     TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap
          *             The archive inventory items.
          *
          * @exception  ArchiveException
          *             Thrown if:
          *             1) There is some error with the database connection
          *             2) An SQL statement failed to execute for some reason.
          *             3) The data cannot be converted to the correct format.
          *             4) There are no archive inventory items.
          */

        TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap getArchiveInventoryItems();

        /**
          * deleteRestoredData
          *
          * Deletes the restored data for the date related to the given
          * inventory item. Once the data has been deleted, the inventory
          * item will be deleted.
          *
          * @param      unsigned long inventoryItemKey
          *             The database key of the inventory item detailing
          *             the restored data date to delete.
          * @param      const time_t& date
          *             The date of the restored data for the given archive.
          *             KT 9 Mar 04: PW #3161 Changed to remove dependence on date formatting.
          *
          * @exception  TA_Base_Core::ArchiveException
          *             Thrown if:
          *             1) The entity data for the ArchiveManager cannot be retrieved.
          *             2) The entity data for the ArchiveManager cannot be cast to ArchiveManagerEntityData.
          *             3) The inventory cannot be updated.
          *             4) The audit message cannot be sent.
          *             5) The operator does not have sufficient privileges.
          */

        void deleteRestoredData( unsigned long inventoryItemKey, const time_t& date );

    private:

        //
        // Disable copy constructor and assignment operator.
        //

		DeleteArchiveHelper( const DeleteArchiveHelper& );
		DeleteArchiveHelper& operator=( const DeleteArchiveHelper& );
    };
}

#endif // DELETE_ARCHIVE_HELPER_H__3DA15816_CA7B_43CB_A66C_A735EBE85849__INCLUDED_
