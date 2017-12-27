/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_extractor/src/ExtractEventData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(_COMBINED_EVENT_DATA_H_)
#define _COMBINED_EVENT_DATA_H_

#include "core/data_access_interface/src/ICombinedEventData.h"

namespace TA_Base_Core
{
    class IDatabase;
}

namespace TA_Base_Core
{
    class CombinedEventData : public ICombinedEventData
    {
    public:
        /**
         * Constructor
         *
         * This constructs a CombinedEventData object that has an initial state.
         * If data is requested it will NOT be loaded from the database.
         *
         * @param key The key that identifies the event.
         * @param logTime The time at which the event was logged.
         * @param alarmSeverityName The name of the severity for the alarm associated
         *                          with the event.
         * @param assetName The asset name for the equipment that generated the event.
         * @param description The description of the event.
         * @param value The value of the event.
         * @param alarmMmsState The MMS state for the alarm associated with the event.
         * @param alarmDssState The DSS state for the alarm associated with the event.
         * @param alarmAvlState The AVL state for the alarm associated with the event.
         * @param operatorName The name of the operator responsible for generating
         *                     the event.
         * @param alarmComment The comment for the alarm associated with the event.
         * @param alarmId The ID for the alarm associated with the event.
         */
        CombinedEventData( const ECombinedEventType type,
                           const unsigned long key,
                           const timeb& createTime,
                           const std::string& alarmSeverityName,
                           const std::string& assetName,
                           const std::string& description,
                           const std::string& value,
                           const std::string& alarmMmsState,
                           const std::string& alarmDssState,
                           const std::string& alarmAvlState,
                           const std::string& operatorName,
                           const std::string& alarmComment,
                           const std::string& alarmId,
						   const std::string& alarmAck,
						   const std::string& alarmStatus ) :
        m_type(type), m_key(key), m_createTime(createTime), m_alarmSeverityName(alarmSeverityName),
        m_assetName(assetName), m_description(description), m_value(value),
        m_alarmMmsState(alarmMmsState), m_alarmDssState(alarmDssState),
        m_alarmAvlState(alarmAvlState), m_operatorName(operatorName),
        m_alarmComment(alarmComment), m_alarmId(alarmId), m_alarmAck(alarmAck), m_alarmStatus(alarmStatus)
        {
        }

        //------------- Methods inherited from ICombinedEventData -------------//

        /**
         * getEventType
         *
         * Returns the type of the event, that is, whether it is an audit event or a datapoint state change
         * event.
         *
         * @return ECombinedEventType The type of the event.
         */
        virtual const ECombinedEventType getEventType();

        
        /**
         * getKey
         *
         * Returns the key for this event.
         *
         * @return unsigned long The key for the event.
         */
        virtual const unsigned long getKey();


        /**
         * getCreateTime
         *
         * Returns the creation time for this event.
         *
         * @return timeb The time at which the event was created.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const timeb getCreateTime();


        /**
         * getAlarmSeverityName
         *
         * Returns the severity name of the alarm associated with this event.
         *
         * @return std::string& The name of the severity of the alarm associated with
         *                      this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmSeverityName();


        /**
         * getAssetName
         *
         * Returns the asset name for this event.
         *
         * @return std::string& The name of the asset that generated this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAssetName();


        /**
         * getDescription
         *
         * Returns the description for this event.
         *
         * @return std::string& The description for this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getDescription();


        /**
         * getValue
         *
         * Returns the value for this event.
         *
         * @return std::string& The value for this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getValue();


        /**
         * getAlarmMmsState
         *
         * Returns the MMS state for the alarm associated with this event.
         *
         * @return std::string& The MMS state of the alarm associated with this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmMmsState();


        /**
         * getAlarmDssState
         *
         * Returns the DSS state for the alarm associated with this event.
         *
         * @return std::string& The DSS state of the alarm associated with this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmDssState();


        /**
         * getAlarmAvlState
         *
         * Returns the AVL state for the alarm associated with this event.
         *
         * @return std::string& The AVL state of the alarm associated with this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmAvlState();


        /**
         * getOperatorName
         *
         * Returns the name of the operator directly responsible for generating
         * this event.
         *
         * @return std::string& The name of the operator.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getOperatorName();


        /**
         * getAlarmComment
         *
         * Returns the comment of the alarm associated with this event.
         *
         * @return std::string& The comment.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmComment();

        /**
         * getAlarmId
         *
         * Returns the ID of the alarm associated with this event.
         *
         * @return std::string& The alarm ID.
         */
        virtual const std::string& getAlarmId();

        /**
         * getAlarmAck
         *
         * Returns the Ack status of the alarm associated with this event.
         *
         * @return std::string& The alarm ack status.
         */
        virtual const std::string& getAlarmAck();

        /**
         * getAlarmStatus
         *
         * Returns the status ie. close, open or pending of the alarm associated with this event.
         *
         * @return string& The alarm status.
         */
        virtual const std::string& getAlarmStatus();

    private:
        // Disable copy constructor and assignment operator.
        CombinedEventData( const CombinedEventData& );
        CombinedEventData& operator= ( const CombinedEventData& );

        // Variables used to store event data.
        ECombinedEventType m_type;
        unsigned long      m_key;
        timeb              m_createTime;
        std::string        m_alarmSeverityName;
        std::string        m_assetName;
        std::string        m_description;
        std::string        m_value;
        std::string        m_alarmMmsState;
        std::string        m_alarmDssState;
        std::string        m_alarmAvlState;
        std::string        m_operatorName;
        std::string        m_alarmComment;
        std::string        m_alarmId;
		std::string        m_alarmAck;
		std::string        m_alarmStatus;
    };
};

#endif // !defined(EXTRACT_EVENT_DATA_H_)