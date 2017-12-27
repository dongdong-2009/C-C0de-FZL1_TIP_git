#ifndef NOTIFICATIONAGENTDATA_H
#define NOTIFICATIONAGENTDATA_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/NotificationAgentData.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Implementation of configuration data accesor for the Notification Service Agent
 *
 */

#if defined(_MSC_VER)
#pragma warning(disable:4250 4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/entity_access/src/INotificationAgentData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
	class EntityHelper;

	class NotificationAgentData : public INotificationAgentData, public EntityData
	{
		
	public:

		NotificationAgentData() {}

		NotificationAgentData( unsigned long key );

		virtual ~NotificationAgentData();

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
		virtual void invalidate();


        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
		virtual std::string getType();
		static std::string getStaticType();

        /**
         * isBroadcast
         *
         * Returns true if this NotificationAgent is of type broadcast
         */
		bool isBroadcast();

        /**
         * clone
         *
         * Returns a pointer to a new NotificationAgentData object that has been initialised with the
         * specified key.
         *
         * @param key The key with which to construct the NotificationAgentData object.
         */
		static IEntityData* clone( unsigned long key );

		/**
		 * getNotifyServicePath
		 *
		 * Returns the value of the getNofityServicePath parameter
		 *
		 * @return the NotifyService parameter
		 *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
		 */
		virtual std::string getNotifyServicePath();

		virtual std::string getAssetName();

	private:

		NotificationAgentData( const NotificationAgentData& theData );
		NotificationAgentData& operator=( const NotificationAgentData& theData );

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string NOTIFY_SERVICE_PATH;
		static const std::string BROADCAST;
		static const std::string ASSET_NAME;

	};
    
    typedef boost::shared_ptr<NotificationAgentData> NotificationAgentDataPtr;
}

#endif // NOTIFICATIONAGENTDATA_H
