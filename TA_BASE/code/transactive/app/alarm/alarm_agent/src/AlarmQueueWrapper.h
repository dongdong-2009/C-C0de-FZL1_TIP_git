/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmQueueWrapper.h $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  *
  * This interface should be implemented by all configured rule types.
  */
#if !defined(ALARMQUEUEWRAPPER_H)
#define ALARMQUEUEWRAPPER_H

#include <string>
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"

namespace TA_Base_Core
{
    struct AlarmAgentQueueItemCorbaDef;
    struct AlarmDetailCorbaDef;
}

namespace TA_Base_App
{
const unsigned long CLOSE_FOR_ACK=1;
const unsigned long CLOSE_FOR_NOMALIZED=2;

class AlarmQueueWrapper
{
public:
    /**
      * Constructor
      */ 
		AlarmQueueWrapper( TA_Base_Core::AlarmAgentQueueItemCorbaDef* pAlarmQueueItem,
			const TA_Base_Core::AlarmDetailCorbaDef& refDetail);

    /** 
      * Destructor
      */
    virtual ~AlarmQueueWrapper();

	const std::string getAlarmDescritpion();
	const std::string getAlarmValue();
	const std::string getAlarmComments();
	const std::string getAlarmMMSState();
	const std::string getAlarmDSSState();
	const std::string getAlarmAVLstate();
	const std::string getAlarmACKedBy();
	const std::string getAlarmStatus();
	const std::string getOMFlag();
    const unsigned long getEventLelOrSeverityKey();
    const unsigned long getAlarmTypeKey();
    TA_Base_Core::AlarmAgentQueueItemCorbaDef* getAlarmAgentQueueItem();
	void setIsSendCloseEvent(unsigned long shouldSendCloseEvent)
	{
		m_shouldSendCloseEvent = shouldSendCloseEvent;
	}
	unsigned long getIsSendCloseEvent()
	{
		return m_shouldSendCloseEvent;
	}



		
		/**
		  * planStateToString
		  *
		  * This method converts a TA_Base_Core::DecisionSupportStateType to a string 
		  * (for insertion into the database)
		  *
		  * @param The TA_Base_Core::DecisionSupportStateType
		  *
		  * @return A string representation of the TA_Base_Core::DecisionSupportStateType
		  */
		static std::string convertPlanState( const TA_Base_Core::DecisionSupportStateType planState );

		/**
		  * This is to convert the AVLState form AlarmDetail to DB updatable IAlarmData format
		  */
		static TA_Base_Core::IAlarmData::EAlarmAVLStatus convertAVLStatus( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );
                            
		/**
		  * This is to convert the MMS State type form AlarmDetail to DB updatable IAlarmData format
		  */
		static TA_Base_Core::IAlarmData::EMmsState convertMmsStateType ( const TA_Base_Core::MmsStateType mmsState);

		/**
		  * This is to convert the Alarm State type form AlarmDetail to DB updatable IAlarmData format
		  */
        static TA_Base_Core::IAlarmData::EAlarmState convertAlarmStatus( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );


		static std::string getMMSStateDBString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail );
		static std::string getAVLStateDBString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail );
		static std::string getAlarmStateDBString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail );
		static std::string getPlanStateDBString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail );
		static std::string getOMFlagString( const TA_Base_Core::AlarmDetailCorbaDef& refDetail );
private:
    AlarmQueueWrapper();
    const AlarmQueueWrapper& operator =( const AlarmQueueWrapper& refSrc );
    AlarmQueueWrapper( const AlarmQueueWrapper& refSrc );

		//std::string initDSSState( const TA_Base_Core::AlarmDetailCorbaDef& refDetail );
	
	//std::string initOMFlag( const TA_Base_Core::AlarmDetailCorbaDef& refDetail );

protected:
    TA_Base_Core::AlarmAgentQueueItemCorbaDef* m_pAlarmQueueItem;
    std::string m_alarmDescription;
    std::string m_alarmValue;
    std::string m_alarmComments;
    std::string m_alarmMMSState;
    std::string m_alarmDSSState;
    std::string m_alarmAVLstate;
    std::string m_alarmACKedBy;
    std::string m_alarmStatus;
	std::string m_OMFlag;
    unsigned long m_eventLelOrSeverityKey;
    unsigned long m_alarmTypeKey;
	unsigned long m_shouldSendCloseEvent;
};  
    
}

#endif // !defined(ALARMQUEUEWRAPPER_H)
