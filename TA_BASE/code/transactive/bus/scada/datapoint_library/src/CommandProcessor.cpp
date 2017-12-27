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

#include "core/uuid/src/TAuuid.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/CommandProcessor.h"

namespace TA_Base_Bus
{
    CommandProcessor::CommandProcessor(TA_Base_Bus::DataPointStateUpdateSender* dpStateUpdateSender, TA_Base_Bus::IDataPointAgent* dpAgent)
        :m_commandId(time(NULL)),
        m_isCompleteSyncRequired(false),
        m_uuid(""),
		m_writeQ(NULL),
		m_dpStateUpdateSender(dpStateUpdateSender),
        m_dpAgent(dpAgent),
        m_isTerminating(false),
        m_isControl(false),
        m_isJustChangedToControl(false)
    {
        // start the thread
        start();
    }

    
    CommandProcessor::~CommandProcessor()
    {
        // stop the thread
        terminateAndWait();
    }
    

    void CommandProcessor::terminate()
    {
        m_isTerminating = true;
        m_conditon.signal();
    }


    void CommandProcessor::run()
    {
        while(!m_isTerminating)
        {
            m_conditon.wait();
            
            // process the update item received from control mode agent
            processDataPointStateUpdateQueue();

            // if in control mode and just received a register request from monitor mode agent,
            // need to synchronize all the state updates to monitor mode agent.
            if (m_isControl && m_isCompleteSyncRequired)
            {
                m_isCompleteSyncRequired = false;
                std::vector<DataPointStateUpdate> stateUpdates;

                {
                    TA_THREADGUARD(m_mapDpStateUpdatesLock);
                    
                    std::map<unsigned long, DataPointStateUpdate>::iterator updateIt;
                    for (updateIt = m_mapDpStateUpdates.begin(); updateIt != m_mapDpStateUpdates.end(); ++ updateIt)
                    {
                        stateUpdates.push_back(updateIt->second);
                    }
                }
                
                sendDataPointStateUpdates(stateUpdates);
            }

            // if just changed to control, need to schedule all unfinished commands to be excuted again
            if (m_isControl && m_isJustChangedToControl)
            {
                m_isJustChangedToControl = false;

                //swap the map itmes to temp one
                std::map<unsigned long, DataPointStateUpdate> updatesMap;
                {
                    TA_THREADGUARD(m_mapDpStateUpdatesLock);
                    m_mapDpStateUpdates.swap(updatesMap);
                }

                std::map<unsigned long, DataPointStateUpdate>::iterator updateIt;
                for (updateIt = updatesMap.begin(); updateIt != updatesMap.end(); ++ updateIt)
                {
                    //recheck rights, recheck lcc and resend the control request
                    DataPoint* dp = m_dpAgent->getDataPoint(updateIt->first);

                    //ingore any exception
                    try
                    {
                        //dp->writeDataPoint(
                        //    updateIt->second.sessionId, 
                        //    updateIt->second.dpValue, 
                        //    updateIt->second.auditEventLoggingEnabled, 
                        //    updateIt->second.triggeringDpKey,
                        //    updateIt->second.triggeringDpValue);

                        // customer doesn't want to re-send any command to hardware automaticly.
                        // so, if the control has RCC check and the control status is pending, 
                        //         then to restart the count down timer to wait for the result.
                        //     else just set the control status to fail
                        if (dp->isVirtual())
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Virtual datapoint does nothing for incompleted control, commandId[%u], entityKey[%u].",
                                updateIt->second.commandId, updateIt->second.dpKey);
                            // do nothing for virtual datapoint, coz its control status will got updated by its child datapoint.
                        }
                        else if (dp->isRCCConfigured() && updateIt->second.controlState == TA_Base_Bus::pending)
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Restart the RCC timer for incompleted control, commandId[%u], entityKey[%u].",
                                updateIt->second.commandId, updateIt->second.dpKey);
                            
                            dp->setControlState(TA_Base_Bus::pending);

							if (dp->getPScadaFlag())
							{
								dp->SetAbeExpressionValue(updateIt->second.dpValue.booleanValue);
							}


                            dp->startABETimeout(updateIt->second.sessionId);
                        }
                        else
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set to fail state for incompleted control, commandId[%u], entityKey[%u].",
                                updateIt->second.commandId, updateIt->second.dpKey);
                            
                            // set the command is completed with error
                            dp->setControlState(TA_Base_Bus::failed);
                        }
                    }
                    catch (TA_Base_Core::AccessDeniedException&) 
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Checking right fails when re-schedule the command, maybe because of the command has been excuted already, commandId[%u], entityKey[%u].",
                            updateIt->second.commandId, updateIt->second.dpKey);

                        // set the command is completed with error
                        dp->setControlState(TA_Base_Bus::failed);
                    }
                    catch (TA_Base_Core::BadParameterException&)
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Fails due to bad parameter exception when re-schedule the command, maybe because of the command has been excuted already, commandId[%u], entityKey[%u].",
                            updateIt->second.commandId, updateIt->second.dpKey);

                        // set the command is completed with error
                        dp->setControlState(TA_Base_Bus::failed);
                    }
                    catch (...)
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Fails due to unknown exception when re-schedule the command, maybe because of the command has been excuted already, commandId[%u], entityKey[%u].",
                            updateIt->second.commandId, updateIt->second.dpKey);

                        // set the command is completed with error
                        dp->setControlState(TA_Base_Bus::failed);
                    }
                }
            }
        }
    }

    
    unsigned long CommandProcessor::getNextCommandId()
    {
        return (++m_commandId);
    }


    void CommandProcessor::updateDataPointStateUpdateMap(DataPointStateUpdate& dpStateUpdate)
    {
        // deal with State Update map
        TA_THREADGUARD(m_mapDpStateUpdatesLock);
        
        std::map<unsigned long, DataPointStateUpdate>::iterator updateIt;
        updateIt = m_mapDpStateUpdates.find(dpStateUpdate.dpKey);
        
        if (dpStateUpdate.controlCommandState == TA_Base_Bus::CompletedWithError ||
            dpStateUpdate.controlCommandState == TA_Base_Bus::CompletedWithSuccess)
        {
            if (updateIt != m_mapDpStateUpdates.end())
            {
                m_mapDpStateUpdates.erase(updateIt);
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "control state with commandId[%u] entityKey[%u] is removed from control update map, total [%u].",
                    dpStateUpdate.commandId, dpStateUpdate.dpKey, m_mapDpStateUpdates.size());
            }
            else
            {
                // if not found, maybe something error with synchronization
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive a final control state with commandId[%u] entityKey[%u], but its previous state is unknown.",
                    dpStateUpdate.commandId, dpStateUpdate.dpKey);
            }
        }
        else
        {
            if (updateIt != m_mapDpStateUpdates.end())
            {
                // replace
                updateIt->second = dpStateUpdate;
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "control state with commandId[%u] entityKey[%u] is updated from control update map, total [%u].",
                    dpStateUpdate.commandId, dpStateUpdate.dpKey, m_mapDpStateUpdates.size());
            }
            else
            {
                // add
                m_mapDpStateUpdates[dpStateUpdate.dpKey] = dpStateUpdate;
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "control state with commandId[%u] entityKey[%u] is added from control update map, total [%u].",
                    dpStateUpdate.commandId, dpStateUpdate.dpKey, m_mapDpStateUpdates.size());
            }
        }
    }

    
    void CommandProcessor::processDataPointStateUpdate(DataPointStateUpdate& dpWriteStateUpdate)
    {
        DataPointStateUpdate* dpStateUpdate = new DataPointStateUpdate();
        *dpStateUpdate = dpWriteStateUpdate;

        {
            TA_THREADGUARD(m_stateUpdateQLock);
            m_dpStateUpdateQ.push(dpStateUpdate);
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Monitor mode datapoint control state is queued with: commandId[%u], entityKey[%u], controlState[%u].",
            dpStateUpdate->commandId, dpStateUpdate->dpKey, dpStateUpdate->controlState);

        m_conditon.signal();
    }


    void CommandProcessor::processDataPointStateUpdateQueue()
    {
        while (true)
        {
            DataPointStateUpdate* dpStateUpdate = NULL;

            {
                TA_THREADGUARD(m_stateUpdateQLock);
                if (m_dpStateUpdateQ.empty())
                {
                    //break while loop when queue is empty
                    break;
                }
                dpStateUpdate = m_dpStateUpdateQ.front();
                m_dpStateUpdateQ.pop();
            }

            // call DataPoint to store the synchronized information in monitor mode
            if (!m_isControl)
            {
                DataPoint* dp = m_dpAgent->getDataPoint(dpStateUpdate->dpKey);
                
                // should ingore or assert to indicate something error?
                TA_ASSERT(dp != NULL, "cant find the datapoint entity.");
                dp->syncControlState(*dpStateUpdate);
                
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Monitor mode datapoint control state is synchronized with: commandId[%u], entityKey[%u], controlState[%u].",
                    dpStateUpdate->commandId, dpStateUpdate->dpKey, dpStateUpdate->controlState);
            }

            updateDataPointStateUpdateMap(*dpStateUpdate);
            
            //delete the processed item
            delete dpStateUpdate;
			dpStateUpdate = NULL;
        }
    }


    void CommandProcessor::registerMonitorAgent(const std::string& uuid, TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr monitorAgentStateUpdateRef)
    {
        m_dpStateUpdateSender->setMonitorAgentStateUpdateRef(monitorAgentStateUpdateRef);

        if (!TA_Base_Core::TA_Uuid::compareUUID(uuid, m_uuid))
        {
            m_uuid = uuid;
            setCompleteSyncRequired();
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Monitor mode agent is registered with uuid [%s]", m_uuid.c_str());

            m_conditon.signal();
        }
    }


    void CommandProcessor::deregisterMonitorAgent()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Monitor mode agent is to be deregistered with uuid [%s]", m_uuid.c_str());
        
        //clear the reference in sender
        m_dpStateUpdateSender->setMonitorAgentStateUpdateRef(NULL);
        
        //clear the registered uuid
        m_uuid = "";
    }
    
    
    void CommandProcessor::scheduleCommand(DataPointWriteRequest* dpWriteRequest)
    {
        TA_ASSERT(dpWriteRequest != NULL, "cannot schedule a command with a null write request");
        TA_ASSERT(m_isControl, "scheduleCommand must be called after setToControl is called");
        TA_ASSERT(m_writeQ != NULL, "scheduleCommand must be called after setToControl is called");

        m_writeQ->insert(dpWriteRequest);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Command: commandId[%u], entityKey[%u], value[%s] is put into write queue. Queue size [%u].", 
            dpWriteRequest->getCommandId(), dpWriteRequest->getDataPoint()->getEntityKey(), dpWriteRequest->getValue().getValueAsString().c_str(), m_writeQ->getSize() );
    }


    unsigned long CommandProcessor::sendDataPointStateUpdate(DataPointStateUpdate& dpStateUpdate, bool newCommandIdRequired)
    {
        unsigned long commandId = dpStateUpdate.commandId;

        if (newCommandIdRequired)
        {
            commandId = getNextCommandId();
            dpStateUpdate.commandId = commandId;
        }
        else
        {
            TA_ASSERT(dpStateUpdate.commandId != 0, "command id should never be 0 if newCommandId is false.");
        }

        //update the control update map
        updateDataPointStateUpdateMap(dpStateUpdate);

        //send to the monitor mode agent, any exception will cause to reset monitor agent reference
        try
        {
            m_dpStateUpdateSender->sendDataPointStateUpdate(dpStateUpdate);
        }
        catch ( const CORBA::Exception & ex )
        {
            LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
            deregisterMonitorAgent();
        }
        catch( const TA_Base_Core::TransactiveException& te )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
            deregisterMonitorAgent();
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown exception" );
            deregisterMonitorAgent();
        }
        
        return commandId;
    }

    
    void CommandProcessor::sendDataPointStateUpdates(const std::vector<DataPointStateUpdate>& stateUpdates)
    {
        //send to the monitor mode agent, any exception will cause to reset monitor agent reference
        try
        {
            m_dpStateUpdateSender->sendDataPointStateUpdates(stateUpdates);
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "send all control updates, total [%u]", 
                stateUpdates.size());
        }
        catch ( const CORBA::Exception & ex )
        {
            LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
            deregisterMonitorAgent();
        }
        catch( const TA_Base_Core::TransactiveException& te )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
            deregisterMonitorAgent();
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown exception" );
            deregisterMonitorAgent();
        }
    }


    void CommandProcessor::setCompleteSyncRequired()
    {
        m_isCompleteSyncRequired = true;
    }


    void CommandProcessor::setToControl()
    {
        m_isControl = true;

        if (m_writeQ == NULL)
        {
            m_writeQ = TA_Base_Bus::DataPointWriteQueue::getInstance();
        }

        m_isJustChangedToControl = true;
        m_conditon.signal();
    }

    
    void CommandProcessor::setToMonitor()
    {
        m_isControl = false;

        if (m_writeQ != NULL)
        {
            TA_Base_Bus::DataPointWriteQueue::removeInstance(m_writeQ);
            m_writeQ = NULL;
        }
    }

	void CommandProcessor::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if (TA_Base_Core::Control == mode)
		{
			setToControl();
		}
		else
		{
			m_isControl = false;

	        if (m_writeQ != NULL)
	        {
	            TA_Base_Bus::DataPointWriteQueue::removeInstance(m_writeQ);
	            m_writeQ = NULL;
	        }
		}
	}
	
}
