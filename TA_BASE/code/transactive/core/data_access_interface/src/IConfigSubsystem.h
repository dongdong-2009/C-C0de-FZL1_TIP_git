/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigSubsystem.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 12:15:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * Provides a writeable interface to the Subsystem database information.
  *
  */

#ifndef ICONFIG_SUBSYSTEM_H
#define ICONFIG_SUBSYSTEM_H

#include <string>
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/IConfigItem.h"

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{

    class IConfigSubsystem : public IConfigItem, public ISubsystem
    {

    public:
        virtual ~IConfigSubsystem() { };


        /**
         * deleteThisSubsystem
         *
         * Removes this subsystem from the database. 
         * The calling application MUST then delete this subsystem object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this subsystem was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This subsystem has not been deleted
         */
        virtual void deleteThisSubsystem() =0;

        
        /**
         * canDelete
         *
         * This determines if this item can be deleted or not. Some items are reserved and can
         * therefore not be deleted
         *
         * @return bool - true if this item can be deleted, false otherwise
         */
        virtual bool canDelete() =0;


        /**
         * setExclusiveControlStatus
         * 
         * Sets whether the subsystem has exclusive control.
         *
         * @param isExclusive  true if the subsystem has exclusive control,
         *                     false otherwise.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual void setExclusiveControlStatus(bool isExclusive) =0;
		
        /**
         * setLocationExclusiveControlStatus
         * 
         * Sets whether the subsystem has location exclusive control.
         *
         * @param isExclusive  true if the subsystem has exclusive control,
         *                     false otherwise.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).	
		 */
        virtual void setLocationExclusiveControlStatus(bool isExclusive) =0;


    };
} //close namespace TA_Base_Core

#endif // ICONFIG_SUBSYSTEM_H
