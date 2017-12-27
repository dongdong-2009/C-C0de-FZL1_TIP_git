/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: $
 * @author:  
 * @version: $Revision: $
 *
 * Last modification: $Date: $
 * Last modified by:  $Author: $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datapoint_library/src/DataPointStateUpdateServant.h"
#include "bus/scada/datapoint_library/src/CommandProcessor.h"

namespace TA_Base_Bus
{
    DataPointStateUpdateServant::DataPointStateUpdateServant(const std::string& servantName, CommandProcessor* commandProcessor)
        :m_servantName(servantName),
        m_commandProcessor(commandProcessor),
        m_isControl(false),
        m_isActivated(false)
    {
        if (!m_isActivated)
        {
            m_isActivated = true;
            activateServantWithName(m_servantName);

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataPointStateUpdateServant was activated with name [%s].", m_servantName.c_str());
        }
    }

    DataPointStateUpdateServant::~DataPointStateUpdateServant()
    {
        if (m_isActivated)
        {
            m_isActivated = false;
            deactivateServant();
        }
    }
    
    TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr DataPointStateUpdateServant::getDataPointStateUpdateServant()
    {
        return _this();
    }

    void DataPointStateUpdateServant::sendDataPointStateUpdate(const TA_Base_Bus::DataPointCorbaTypes::DataPointWriteStateUpdate& dpWriteStateUpdate)
    {
        if (m_isControl)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ControlMode Agent shoudl not received a datapoint state update, commandId[%u], entityKey[%u].",
            dpWriteStateUpdate.commandId, dpWriteStateUpdate.dpKey);
        }
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "received a datapoint state update from control mode agent, commandId[%u], entityKey[%u].",
            dpWriteStateUpdate.commandId, dpWriteStateUpdate.dpKey);

        TA_Base_Bus::DataPointStateUpdate dpStateUpdate;
        TA_Base_Bus::convertDataPointStateUpdate(dpWriteStateUpdate, dpStateUpdate);

        m_commandProcessor->processDataPointStateUpdate(dpStateUpdate);
    }

    void DataPointStateUpdateServant::sendDataPointStateUpdates(const TA_Base_Bus::DataPointCorbaTypes::DataPointWriteStateUpdateSequence& dpWriteStateUpdates)
    {
        if (m_isControl)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ControlMode Agent should not received a datapoint state updates from control mode agent, total[%u].",
                dpWriteStateUpdates.length());
        }
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "received datapoint state updates from control mode agent, total[%u].",
            dpWriteStateUpdates.length());
        
        TA_Base_Bus::DataPointStateUpdate dpStateUpdate;

        for (unsigned long i = 0; i < dpWriteStateUpdates.length(); ++ i)
        {
            TA_Base_Bus::convertDataPointStateUpdate(dpWriteStateUpdates[i], dpStateUpdate);
            m_commandProcessor->processDataPointStateUpdate(dpStateUpdate);
        }
    }

    void DataPointStateUpdateServant::setToControl()
    {
        FUNCTION_ENTRY("setToControl");
        m_isControl = true;

        // TODO: keep servant active for boht Monitor & Control, except Control mode should not accept any calls
//        if (m_isActivated)
//        {
//            m_isActivated = false;
//            deactivateServant();
//
//            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataPointStateUpdateServant was deactivated.");
//        }
		FUNCTION_EXIT;
    }

    void DataPointStateUpdateServant::setToMonitor()
    {
        FUNCTION_ENTRY("setToMonitor");
        m_isControl = false;

        // TODO: keep servant active for boht Monitor & Control, except Control mode should not accept any calls
//        if (!m_isActivated)
//        {
//            m_isActivated = true;
//            //activateServantWithName(m_servantName);
//
//            //to set 2 seconds corba timeout
//            
//            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DataPointStateUpdateServant was activated with name [%s].", m_servantName.c_str());
//        }
		FUNCTION_EXIT;
    }

	void DataPointStateUpdateServant::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if (TA_Base_Core::Control == mode)
		{
			m_isControl = true;
		}
		else
		{
			m_isControl = false;
		}
	}
	
}
