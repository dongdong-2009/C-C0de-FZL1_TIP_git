// AlarmMessageQueue.cpp: implementation of the AlarmMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmMessageQueue.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/AlarmUpdate_MessageTypes.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"
//#include "SharedAlarmAccessFactory.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	AlarmMessageQueue::AlarmMessageQueue(IAlarmUpdate& callback, std::string entityName): m_callback(callback),m_entityName(entityName.c_str())
	{
	  
	}

	AlarmMessageQueue::~AlarmMessageQueue()
	{

	}

	void AlarmMessageQueue::processEvent( boost::shared_ptr<TA_Base_Core::AlarmMessageCorbaDef> newEvent )
	{
		switch ( newEvent->_d() )
		{
		case TA_Base_Core::NewAlarm:	// New Alarm
			try
			{	
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"New alarm %s notification from %s", newEvent->alarm().alarmID.in() ,m_entityName.c_str());				 
				m_callback.newAlarm(newEvent->alarm());				 
			}
			
			catch (const TA_Base_Core::TransactiveException& ex)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Alarm Store Manager %s failed to create new Alarm id %s. Caught TransactiveException: %s",m_entityName.c_str(), newEvent->alarm().alarmID.in() , ex.what());
				TA_ASSERT (false, "Transactive Exception: Failed to create new Alarm in Alarm Store Manager");
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to create new Alarm in Alarm Store Manager" );
				TA_ASSERT (false, "CORBA Exception: Failed to create new Alarm in Alarm Store Manager");
			}
			catch (...)
			{
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"Exception handling new alarm notification, alarm id is %s from agent %s", newEvent->alarm().alarmID.in() ,m_entityName.c_str());
				TA_ASSERT (false, "Failed to create new Alarm in Alarm Store Manager");
			}
			break;
			
		case TA_Base_Core::UpdateAlarm: // Updated alarm
			try
			{
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Alarm %s update notification from %s", newEvent->update().alarmID.in() ,m_entityName.c_str());
			 
                    // before updateAlarm, check if AlarmManager is busy loading
                    if (false == m_callback.shouldQueueAlarmUpdate())
                    {
                        m_callback.updateAlarm(newEvent->update());
                    }                    
				 
			}
			
			catch (const TA_Base_Core::TransactiveException& ex)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Alarm Store Manager %s failed to update Alarm id %s. Caught TransactiveException: %s",m_entityName.c_str(), newEvent->alarm().alarmID.in() , ex.what());
				TA_ASSERT (false, "Transactive Exception: Failed to update Alarm in Alarm Store Manager");
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to update Alarm in Alarm Store Manager" );
				TA_ASSERT (false, "CORBA Exception: Failed to update Alarm in Alarm Store Manager");
			}
			catch (...)
			{
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"Exception handling update alarm notification, alarm id is %s from agent %s", newEvent->alarm().alarmID.in() ,m_entityName.c_str());
				TA_ASSERT (false, "Failed to update Alarm in Alarm Store Manager");
			}
			break;			
        
		default:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"Received an unknown alarm message type");
		}

	}
}
