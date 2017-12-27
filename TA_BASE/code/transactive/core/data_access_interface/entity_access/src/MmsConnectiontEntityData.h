/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MmsConnectiontEntityData.h $
  * @author C. DeWolfe
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef MMS_CONNECTION_DATA_H_INCLUDED
#define MMS_CONNECTION_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class MmsConnectiontEntityData : public EntityData
    {

    public:
		static const std::string PING_PERIOD_SEC;
		static const std::string MSG_CHECK_PERIOD_SEC;
		static const std::string TIMEOUT_TENTH_SEC;
		static const std::string GLOBAL_MMS_INHIBIT;
		static const std::string QUEUE_OBJECT_NAME;
		static const std::string QUEUE_MANAGER_NAME;

        MmsConnectiontEntityData() {}
        MmsConnectiontEntityData(unsigned long key);

        virtual ~MmsConnectiontEntityData();

    public:



        /**
         * getPingPeriodSeconds
         *
         * @return the value of the PING_PERIOD_SEC param. This parameter indicates
		 * how often the MMS Connection should be pinged.
         */
		unsigned int getPingPeriodSeconds();

        /**
         * getMessageCheckSeconds
         *
         * @return the value of the MSG_CHECK_PERIOD_SEC param. This parameter indicates
		 * how often we should check for MMS messages on our queue.
         */
		unsigned int getMessageCheckSeconds();

        /**
         * getMmsServerTimeoutSeconds
         *
         * @return the value of the TIMEOUT_TENTH_SEC param. This is
		 * the time we should wait for the MMS Connection to respond in tenths of seconds
         */
		unsigned int getTimeoutTenthSecs();

        /**
         * getGlobalMmsInhibit
         *
         * @return the state of global mms inhibit; applied or not applied
         */
		bool getGlobalMmsInhibit();

        /**
         * setGlobalMmsInhibit
         *
         * set the state of global mms inhibit; applied or not applied
		 *
         * @throws DatabaseException if there is a database error
         *
         */
		void setGlobalMmsInhibit(bool applied);

        /**
         * getQueueObjectName
         *
         * @return the value of the MQ series object name. This is
		 * the name we will use to connect to the server
         */
        std::string getQueueObjectName();

        /**
         * getQueueManagerName
         *
         * @return the value of the MQ series queue manager name. This is
		 * the name we will use to connect to the server
         */
        std::string getQueueManagerName();

        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new MmsConnectiontEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the MmsConnectiontEntityData object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

	private:

        MmsConnectiontEntityData( const MmsConnectiontEntityData&);            
		MmsConnectiontEntityData& operator=(const MmsConnectiontEntityData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
    };
    
    typedef boost::shared_ptr<MmsConnectiontEntityData> MmsConnectiontEntityDataPtr;
} //end namespace TA_Base_Core

#endif // MMS_CONNECTION_DATA_H_INCLUDED
