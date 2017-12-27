/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointStateChangeEvent.h $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the IEvent interface. It is responsible for writing datapoint state changes
 * to the database and providing summary information that can be used for event processing.
 */

#if !defined(_DATAPOINT_STATE_CHANGE_EVENT_H_)
#define _DATAPOINT_STATE_CHANGE_EVENT_H_

#include <memory>
#include <string>


#include "core/event_processing/src/IEvent.h"

namespace TA_Base_Core
{    
    // Forward declarations.
    class IDataPointStateChangeData;
    class MessageType;
}

namespace TA_Base_Bus
{
    // Forward declarations.
    class DataPoint;

    class DataPointStateChangeEvent : public TA_Base_Core::IEvent
    {
    public:
        /**
         * Constructor
         *
         * Constructs an event based around the state change message.
         *
         * @param stateChange The state change message.
         * @param entityKey The key of the datapoint that changed state.
         * @param entityTypeKey The key of the type of the datapoint.
         */
        DataPointStateChangeEvent( boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> stateChange, DataPoint& dp,
            const TA_Base_Core::MessageType& messageType );


        /**
         * Destructor
         */
        virtual ~DataPointStateChangeEvent();

        /** 
          * getEntityKey
          *
          * Returns the key of the entity associated with the event.
          *
          * @return unsigned long The entity key.
          */
        virtual unsigned long getEntityKey();

		/** 
		  * getEventId
		  *
		  * Return the event uuid 
		  */
		virtual std::string getEventId();

        /** 
          * getEntityTypeKey
          *
          * Returns the key of the entity type associated with the event.
          *
          * @return unsigned long The entity type key.
          */
        virtual unsigned long getEntityTypeKey();


        /** 
          * getEventTypeKey
          *
          * Returns the event type key.
          *
          * @return unsigned long The event type key.
          */
        virtual unsigned long getEventTypeKey();


        /**
         * writeToDatabase
         *
         * Writes the data contained in this object to a new entry in the event table in the database.
         */
        void writeToDatabase();    

		/** 
          * getSubsystemKey
          *
          * Returns the entity subsystem key.
          *
          * @return unsigned long the entity subsystem key
          */
		virtual unsigned long getSubsystemKey();

		/** 
          * getPhysicalSubsystemKey
          *
          * Returns the entity physical subsystem key.
          *
          * @return unsigned long the entity physical subsystem key
          */
		virtual unsigned long getPhysicalSubsystemKey();

		/** 
          * getEntityLocationKey
          *
          * Returns the entity location key.
          *
          * @return unsigned long the entity location key
          */
		virtual unsigned long getEntityLocationKey();

		/** 
          * getAssetName
          *
          * Returns the asset name.
          *
          * @return string the asset name
          */
		virtual std::string getAssetName();

		/** 
          * getValue
          *
          * Returns the value of the event.
          *
          * @return string value
          */
		virtual std::string getValue();

		/** 
          * getCreateTime
          *
          * Returns the create time of the event.
          *
          * @return timeb create time
          */
		virtual timeb getCreateTime();

		/** 
          * getDescriptionParameters
          *
          * Returns the DescriptionParameters of the event.
          *
          * @return DescriptionParameters description parameters of the event
          */
		virtual TA_Base_Core::DescriptionParameters getDescriptionParameters();

    private:
		void generateDescription(boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> stateChange, 
			DataPoint& dp, std::string& alarmMessageType, unsigned long generalMessageKey);
        // Hidden Methods.
        DataPointStateChangeEvent();
        DataPointStateChangeEvent( const DataPointStateChangeEvent& );
        const DataPointStateChangeEvent& operator=( const DataPointStateChangeEvent& );

        // The keys required to summarise the audit event.
        unsigned long m_entityKey;
        unsigned long m_entityTypeKey;
        unsigned long m_eventTypeKey;
		unsigned long m_subSystemKey;
		unsigned long m_physicalSubsystemKey;
		unsigned long m_locationKey;
		std::string   m_assetName;
		std::string   m_value;
		timeb         m_createTime;
		TA_Base_Core::DescriptionParameters m_description;
        // The database object representing this message.
        std::auto_ptr<TA_Base_Core::IDataPointStateChangeData> m_stateChange;
    };
}

#endif // !defined(_DATAPOINT_STATE_CHANGE_EVENT_H_)
