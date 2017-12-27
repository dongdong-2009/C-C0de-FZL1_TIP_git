/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/history/data_recording/src/AlarmOnWriteErrorPolicy.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * Implementation of IDataWritePolicy that opens
 * an alarm when an error occurs, and closes it when
 * a successful write is done.
 *
 */

#include <sstream>
#include <vector>

#include "bus/history/data_recording/src/AlarmOnWriteErrorPolicy.h"
#include "bus/history/data_recording/src/DataCache.h"
#include "bus/history/data_recording/src/Datum.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/DataRecordingAlarms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
    AlarmOnWriteErrorPolicy::AlarmOnWriteErrorPolicy( TA_Base_Core::IEntityData& entityConfig )
    : m_currentAlarm( "" ), m_entityConfig( entityConfig )
	,m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper() )
    {
        
    }



    AlarmOnWriteErrorPolicy::~AlarmOnWriteErrorPolicy()
    {
        // Nothing to do
    }


    void AlarmOnWriteErrorPolicy::notifyDataWriteSucess()
    {
        FUNCTION_ENTRY( "notifyDataWriteSucess" );
        if ( ! m_currentAlarm.empty() )
        {
			//xinsong++ add locationkey when close alarm
            m_alarmHelper.closeAlarmAtLocation( m_currentAlarm, m_entityConfig.getLocation());
            m_currentAlarm = "";
        }
        FUNCTION_EXIT;
    }


    void AlarmOnWriteErrorPolicy::notifyDataWriteError( DataCache& unwrittenCache )
    {
        FUNCTION_ENTRY( "notifyDataWriteError" );
        if ( m_currentAlarm.empty() )
        {
            TA_Base_Core::DescriptionParameters alarmParams;
            TA_Base_Core::NameValuePair param1( "DataType", unwrittenCache.getType() );

            m_currentAlarm = m_alarmHelper.submitAlarm( TA_Base_Core::DataRecordingAlarms::DataRecordingWriteError,
                                                         m_entityConfig.getKey(), m_entityConfig.getTypeKey(),
                                                         alarmParams, m_entityConfig.getName(), m_entityConfig.getLocation(),
                                                         m_entityConfig.getSubsystem() );
        }
        FUNCTION_EXIT;
    }
}
