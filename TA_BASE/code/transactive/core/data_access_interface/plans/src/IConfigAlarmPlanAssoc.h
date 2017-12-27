/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/IConfigAlarmPlanAssoc.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 10:18:09 $
  * Last modified by:  $Author: weikun.lin $
  *
  * IConfigAlarmPlanAssoc is an interface to the AlarmPlanMap table. It provides read and write
  * access for new and existing AlarmPlanAssociations
  */

#if !defined(ICONFIGALARM_PLAN_ASSOC_H)
#define ICONFIGALARM_PLAN_ASSOC_H

#include <string>
#include "core/data_access_interface/plans/src/IAlarmPlanAssocData.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_Base_Core
{
    class IConfigAlarmPlanAssoc : public IAlarmPlanAssocData, public virtual IConfigItem
	{
	public:

		virtual ~IConfigAlarmPlanAssoc() {}


        /**
         * deleteThisAssociation
         *
         * Removes this alarm plan association from the database. 
         * The calling application MUST then delete this alarm plan association object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this alarm plan association was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This alarm plan association has not been deleted
         */
        virtual void deleteThisAssociation() =0;


        /**
         * getDateCreated
         *
         * Returns the date created for this alarm plan association.
         *
         * @return The date created for this alarm plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the alarm plan association key is invalid (and this is not a new alarm plan association).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this alarm plan association.
         *
         * @return The date modified for this alarm plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the alarm plan association key is invalid (and this is not a new alarm plan association).
         */
        virtual time_t getDateModified() =0;


        /**
         * isUsingEntity
         *
         * This returns whether the entity or entity type is being used
         *
         * @return bool - True if entity is being used, false if entity type is being used
         */
        virtual bool isUsingEntity() =0;


        /**
         * getAlarmTypeName
         *
         * @return This gets the display name for the current alarm type and returns it
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual std::string getAlarmTypeName() =0;

		/**
         * getAlarmTypeDisplayName
         *
         * @return This gets the display name for the current alarm type and returns it
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual std::string getAlarmTypeDisplayName() =0;

        
        /**
         * getEntityName
         *
         * @return This gets the display name for the current entity and returns it. If the entity is not set
         *         this returns ""
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual std::string getEntityName() =0;

        
        /**
         * getEntityTypeName
         *
         * @return This gets the display name for the current entity type and returns it. If the entity type
         *         is not set this returns ""
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual std::string getEntityTypeName() =0;


        /** 
		  * setAlarmTypeKey
		  *
		  * Sets the alarm type for this association
		  *
		  * @param std::string The alarm type
		  */
        virtual void setAlarmTypeKey(unsigned long alarmTypeKey) =0;


        /** 
		  * setAlarmType
		  *
		  * Sets the alarm type for this association. This is set using the name and must therefore look up
          * the key.
		  *
		  * @param string - An alarm type name
          * 
          * @exception DatabaseException
          * @exception DataException - This will be thrown if the name is invalid and cannot be found in 
          *                            the database. The type will be set to NO_VALUE.
		  */
        virtual void setAlarmType(const std::string& alarmTypeName) =0;

		 /** 
		  * setAlarmTypeDisplay
		  *
		  * Sets the alarm type for this association. This is set using the display name and must therefore look up
          * the key.
		  *
		  * @param string - An alarm type display name
          * 
          * @exception DatabaseException
          * @exception DataException - This will be thrown if the name is invalid and cannot be found in 
          *                            the database. The type will be set to NO_VALUE.
		  */
		virtual void setAlarmTypeByDisplayName(const std::string& alarmTypeDisplayName)=0;

        
        /** 
		  * setEntityKey
		  *
		  * Sets the entity key for this association. Set to 0 if the entity key is not used.
		  *
		  * @param unsigned long the entitykey
		  */
		virtual void setEntityKey(unsigned long entityKey) =0;


		/** 
		  * setEntity
		  *
		  * Sets the entity for this association. This is set using the name and must therefore look up
          * the key. If the name is empty then this implies the entity is not set.
		  *
		  * @param string - An entity name
          * 
          * @exception DatabaseException
          * @exception DataException - This will be thrown if the name is invalid and cannot be found in 
          *                            the database. The type will be set to NO_VALUE.
		  */
        virtual void setEntity(const std::string& entityName) =0;


        /** 
		  * setEntityTypeKey
		  *
		  * Sets the entity type key for this association. Set to 0 if the entity type key is not used.
		  *
		  * @param unsigned long the entity type key
		  */
		virtual void setEntityTypeKey(unsigned long entityTypeKey) =0;


		/** 
		  * setEntityType
		  *
		  * Sets the entity type for this association. This is set using the name and must therefore look up
          * the key. If the name is empty then this implies the entity type is not set.
		  *
		  * @param string - An entity type name
          * 
          * @exception DatabaseException
          * @exception DataException - This will be thrown if the name is invalid and cannot be found in 
          *                            the database. The type will be set to NO_VALUE.
		  */
        virtual void setEntityType(const std::string& entityTypeName) =0;

          
        /** 
		  * setPlanPath
		  *
		  * Sets the plan path for this association
		  *
		  * @param std::string the plan path
		  */
		virtual void setPlanPath(const std::string& planPath) =0;

	};

} // Closes TA_Base_Core

#endif // ICONFIGALARM_PLAN_ASSOC_H

