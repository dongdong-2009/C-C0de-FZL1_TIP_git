/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/SubsystemDutyMatrix.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/29 17:51:30 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This class stores the active subsystem duties per region.
  * The duties are stored in the form of a two-dimensional matrix,
  * where rows represent physical subsystems and columns represent regions.
  * Individual elements of the matrix contain a list of subsystem duties, 
  * as well as a boolean value indicating whether the corresponding subsystem 
  * is part of the corresponding region.
  * Where this is set to false, the duty list is always empty.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


SubsystemDutyMatrix::SubsystemDutyMatrix() :
    m_dutiesLoaded(false),
    m_dutyUpdate(NULL)
{
    FUNCTION_ENTRY("SubsystemDutyMatrix()");
// 
//     // Register for subsystem duty add and remove messages
// 	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
//         TA_Base_Core::DutyAgentBroadcastComms::AddSubsystemDuty, this, NULL);
// 
// 	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
//         TA_Base_Core::DutyAgentBroadcastComms::RemoveSubsystemDuty, this, NULL);

	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
		TA_Base_Core::DutyAgentBroadcastComms::SubsubsystemDutyChangeNotification, this, NULL);


    // Register for subsystem state change messages
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::DutyAgentBroadcastComms::SubsystemStateChangeNotification, this, NULL );

	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification, this, NULL );

    FUNCTION_EXIT;
}


SubsystemDutyMatrix::~SubsystemDutyMatrix()
{
    FUNCTION_ENTRY("~SubsystemDutyMatrix()");

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    delete m_dutyUpdate;
    m_dutyUpdate = NULL;

    FUNCTION_EXIT;
}


void SubsystemDutyMatrix::populate(const SubsystemStateList& subsystemStates, const SubsystemDutyList& subsystemDuties)
{
    FUNCTION_ENTRY("populate(const SubsystemDutyList& subsystemDuties)");

    if (m_dutiesLoaded)
    {
        // Already populated.
        FUNCTION_EXIT;
        return;
    }

    //DutyDataStore dutyDataStore;
    DataCache& dataCache = DutyDataStore::getDataCache();
    bool ignoreSystemOperators(DutyDataStore::ignoreSystemOperators());
    bool ignoreAppSubsystems(DutyDataStore::ignoreAppSubsystems());

    // Get configured subsystems.
    SubsystemKeyList subsystemKeys(dataCache.getSubsystemKeys());
    
    // Insert the subsystems horizontally into the matrix.
    for (SubsystemKeyList::const_iterator subsystemIt = subsystemKeys.begin(); subsystemIt != subsystemKeys.end(); subsystemIt++)
    {
        addRow(*subsystemIt);
    }


    ///////////////////////////////////////////////////////////////////////
	//jinhua++
	SubsystemTreeMap allSubsystemKeyMap( dataCache.getAllAssociatedSubsystemkeys() );
	SubsystemTreeMap::iterator regioniter = allSubsystemKeyMap.begin();
	for ( ; regioniter != allSubsystemKeyMap.end(); ++regioniter )
	{
		addColumn( regioniter->first );

		SubsystemKeyList::iterator subsystemiter  = regioniter->second.begin();
		for ( ; subsystemiter != regioniter->second.end(); ++subsystemiter )
		{
			try
			{
				SubsystemDutyMatrixElement sdmElement( getElement( *subsystemiter, regioniter->first ) );
				sdmElement.setSubsystemInRegion( true );
				setElement( *subsystemiter, regioniter->first, sdmElement );
			}
            catch (const TA_Base_Core::MatrixException& ex)
            {
					//TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the subsystem duty matrix.", ex.what()));
					//here we should log the exception and continue for other subsystem duty matrix element
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to set an element of the subsystem duty matrix. %s", ex.what());
            }

		}
	}

    // Get configured regions.
// 	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "jinhua start subsystempopulate3");
//     RegionKeyList regionKeys(dataCache.getRegionKeys());
// 
//     // Insert the regions vertically into the matrix.
// 	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "jinhua start subsystempopulate4");
//     for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
//     {
//         addColumn(*regionIt);
// 
//         // For the given region get allocated subsystems.
//         SubsystemKeyList assignedSubsystemKeys(dataCache.getAssociatedSubsystemKeys(*regionIt));
// 
//         // Update the appropriate elements of the matrix to indicate which regions contain which subsystems.
//         for (SubsystemKeyList::const_iterator assignedSubIt = assignedSubsystemKeys.begin();
//              assignedSubIt != assignedSubsystemKeys.end(); assignedSubIt++)
//         {
//             try
//             {
//                 SubsystemDutyMatrixElement sdmElement(getElement(*assignedSubIt, *regionIt));
// 
//                 sdmElement.setSubsystemInRegion(true);
// 
//                 setElement(*assignedSubIt, *regionIt, sdmElement);
//             }
//             catch (const TA_Base_Core::MatrixException& ex)
//             {
//                 TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the subsystem duty matrix.", ex.what()));
//             }
// 
//         }
//     }
	// ++jinhua
	//////////////////////////////////////////////////////////////////////////////////////////////////////

    // Update the appropriate elements of the matrix with subsystem duties retrieved from the agent.
    for (SubsystemDutyList::const_iterator subDutyIt = subsystemDuties.begin(); subDutyIt != subsystemDuties.end(); subDutyIt++)
    {
        // Discard unwanted duties.
        if (ignoreSystemOperators && dataCache.isSystemProfile(subDutyIt->profileKey) ||
            ignoreAppSubsystems && dataCache.isAppSubsystem(subDutyIt->subsystemKey))
        {
            continue;
        }

        try
        {
            SubsystemDutyMatrixElement sdmElement(getElement(subDutyIt->subsystemKey, subDutyIt->regionKey));

            sdmElement.addDuty(*subDutyIt);

            setElement(subDutyIt->subsystemKey, subDutyIt->regionKey, sdmElement);
        }
        catch (const TA_Base_Core::MatrixException& ex)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the subsystem duty matrix.", ex.what()));
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to set an element of the subsystem duty matrix. %s", ex.what());
        }           
    }

    // TD12449: Now update the subsystem states.
    for (SubsystemStateList::const_iterator subStateIt = subsystemStates.begin(); subStateIt != subsystemStates.end(); subStateIt++)
    {
        try
        {
			if(rowExists(subStateIt->subsystemKey) && columnExists(subStateIt->regionKey))
			{
				SubsystemDutyMatrixElement sdmElement( getElement( subStateIt->subsystemKey, subStateIt->regionKey ) );

				// ESubsystemState is defined to be the same as TA_Base_Bus::SubsystemState::State which is used
				// inside the duty agent and from which the 'prevState' and 'currState' numbers where cast from. It
				// is therefore alright to cast it back to out enumerated type provided the two remain in sync!
				sdmElement.setSubsystemState( static_cast<ESubsystemState>(subStateIt->currState) );

				setElement( subStateIt->subsystemKey, subStateIt->regionKey, sdmElement );
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
					"Ignoring invalid physical subsystem %d ---- region %d pair: ", subStateIt->subsystemKey,subStateIt->regionKey );
			}
        }
        catch (const TA_Base_Core::MatrixException& ex)
        {
            //TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the subsystem duty matrix.", ex.what()));
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to set an element of the subsystem duty matrix. %s", ex.what());
        }
    }

    m_dutiesLoaded = true;

    FUNCTION_EXIT;
}

void SubsystemDutyMatrix::populateStaticData()
{
	FUNCTION_ENTRY("populateStaticData()");
	
	if (m_dutiesLoaded)
	{
		FUNCTION_EXIT;
		return;
	}
	DataCache& dataCache = DutyDataStore::getDataCache();
	SubsystemKeyList subsystemKeys(dataCache.getSubsystemKeys());

	for (SubsystemKeyList::const_iterator subsystemIt = subsystemKeys.begin(); subsystemIt != subsystemKeys.end(); subsystemIt++)
	{
		addRow(*subsystemIt);
	}

	///////////////////////////////////////////////////////////////////////
	//jinhua++
	SubsystemTreeMap allSubsystemKeyMap( dataCache.getAllAssociatedSubsystemkeys() );
	SubsystemTreeMap::iterator regioniter = allSubsystemKeyMap.begin();
	for ( ; regioniter != allSubsystemKeyMap.end(); ++regioniter )
	{
		addColumn( regioniter->first );

		SubsystemKeyList::iterator subsystemiter  = regioniter->second.begin();
		for ( ; subsystemiter != regioniter->second.end(); ++subsystemiter )
		{
			if(rowExists(*subsystemiter) && columnExists(regioniter->first))
			{
				try
				{
					SubsystemDutyMatrixElement sdmElement( getElement( *subsystemiter, regioniter->first ) );
					sdmElement.setSubsystemInRegion( true );
					setElement( *subsystemiter, regioniter->first, sdmElement );
				}
				catch (const TA_Base_Core::MatrixException& ex)
				{
					//TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the subsystem duty matrix.", ex.what()));
					//here we should log the exception and continue for other subsystem duty matrix element
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to set an element of the subsystem duty matrix. %s", ex.what());
				}

			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "SubsystemDutyMatrix::populate Invalid pair of subsystem: %u, region:%u",*subsystemiter, regioniter->first);
			}

		}
	}

	FUNCTION_EXIT;
}

void SubsystemDutyMatrix::populateDynamicData(const SubsystemStateList& subsystemStates, const SubsystemDutyList& subsystemDuties)
{
	FUNCTION_ENTRY("populateDynamicData(const SubsystemDutyList& subsystemDuties)");

	if (m_dutiesLoaded)
	{
		// Already populated.
		FUNCTION_EXIT;
		return;
	}

	//DutyDataStore dutyDataStore;
	DataCache& dataCache = DutyDataStore::getDataCache();
	bool ignoreSystemOperators(DutyDataStore::ignoreSystemOperators());
	bool ignoreAppSubsystems(DutyDataStore::ignoreAppSubsystems());

	for (SubsystemDutyList::const_iterator subDutyIt = subsystemDuties.begin(); subDutyIt != subsystemDuties.end(); subDutyIt++)
	{
		// Discard unwanted duties.
		if (ignoreSystemOperators && dataCache.isSystemProfile(subDutyIt->profileKey) ||
			ignoreAppSubsystems && dataCache.isAppSubsystem(subDutyIt->subsystemKey))
		{
			continue;
		}

		try
		{
			if(rowExists(subDutyIt->subsystemKey) && columnExists(subDutyIt->regionKey))			 
			{
				SubsystemDutyMatrixElement sdmElement(getElement(subDutyIt->subsystemKey, subDutyIt->regionKey));

				sdmElement.addDuty(*subDutyIt);

				setElement(subDutyIt->subsystemKey, subDutyIt->regionKey, sdmElement);
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SubsystemDutyMatrix::populate invalid (subsystem, region) pair, subsystem: %u, region:%u",subDutyIt->subsystemKey,subDutyIt->regionKey);
			}
		}
		catch (const TA_Base_Core::MatrixException& ex)
		{
			//TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the subsystem duty matrix.", ex.what()));
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to set an element of the subsystem duty matrix. %s", ex.what());
		}           
	}

	// TD12449: Now update the subsystem states.
	for (SubsystemStateList::const_iterator subStateIt = subsystemStates.begin(); subStateIt != subsystemStates.end(); subStateIt++)
	{
		try
		{
			SubsystemDutyMatrixElement sdmElement( getElement( subStateIt->subsystemKey, subStateIt->regionKey ) );

			// ESubsystemState is defined to be the same as TA_Base_Bus::SubsystemState::State which is used
			// inside the duty agent and from which the 'prevState' and 'currState' numbers where cast from. It
			// is therefore alright to cast it back to out enumerated type provided the two remain in sync!
			sdmElement.setSubsystemState( static_cast<ESubsystemState>(subStateIt->currState) );

			setElement( subStateIt->subsystemKey, subStateIt->regionKey, sdmElement );
		}
		catch (const TA_Base_Core::MatrixException& ex)
		{
			//TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the subsystem duty matrix.", ex.what()));
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failed to set an element of the subsystem duty matrix. %s", ex.what());
		}
	}

	m_dutiesLoaded = true;

	FUNCTION_EXIT;
}

SubsystemKeyList SubsystemDutyMatrix::getRowIdsByRights(const ProfileKeyList& profileKeys)
{
    FUNCTION_ENTRY("getRowIdsByRights(const ProfileKeyList& profileKeys)");

    typedef std::set<SubsystemKey> SubsystemKeyMap;
    SubsystemKeyMap subsystemKeyMap;
    DataCache& dataCache = DutyDataStore::getDataCache();

    // Collate the accessible subsystems for each profile specified. Use a set to eliminate duplicates.
    for (ProfileKeyList::const_iterator profileIt = profileKeys.begin(); profileIt != profileKeys.end(); profileIt++)
    {
        SubsystemKeyList subsystemKeysForProfile(dataCache.getAccessibleSubsystemKeys(*profileIt));

        subsystemKeyMap.insert(subsystemKeysForProfile.begin(), subsystemKeysForProfile.end());
    }

    SubsystemKeyList subsystemKeys;
    subsystemKeys.reserve(subsystemKeyMap.size());

    // Throw away any subsystem keys that do not denote valid matrix rows.
    for (SubsystemKeyMap::const_iterator subKeyIt = subsystemKeyMap.begin(); subKeyIt != subsystemKeyMap.end(); subKeyIt++)
    {
        if (rowExists(*subKeyIt))
        {
            subsystemKeys.push_back(*subKeyIt);
        } 
    } 

    FUNCTION_EXIT;
    return subsystemKeys;
}


const SubsystemDutyUpdate* SubsystemDutyMatrix::getDutyUpdate()
{
    FUNCTION_ENTRY("getDutyUpdate()");

    TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);
    
    FUNCTION_EXIT;
    return m_dutyUpdate;
}


// TA_Base_Core::SpecialisedMessageSubscriber implementation
void SubsystemDutyMatrix::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)");

    EDutyUpdateTupe updateType(DUT_UNDEFINED);

    // Determine the type of duty update based on message type.
    if (TA_Base_Core::DutyAgentBroadcastComms::AddSubsystemDuty.getTypeKey().compare(message.messageTypeKey.in()) == 0)
    {
        updateType = DUT_ADD;         
    }
    else if (TA_Base_Core::DutyAgentBroadcastComms::RemoveSubsystemDuty.getTypeKey().compare(message.messageTypeKey.in()) == 0)
    {
        updateType = DUT_REMOVE;
    }
	else if (TA_Base_Core::DutyAgentBroadcastComms::SubsubsystemDutyChangeNotification.getTypeKey().compare(message.messageTypeKey.in()) == 0)
    {
        updateType = DUT_CHANGE;
    }
    else if (TA_Base_Core::DutyAgentBroadcastComms::SubsystemStateChangeNotification.getTypeKey().compare(message.messageTypeKey.in()) == 0)
    {
        updateType = DUT_STATE;
    }
	else if (TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification.getTypeKey().compare(message.messageTypeKey.in()) == 0)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "receiving StationOnLineOffLineNotification");
		DutyAgentTypeCorbaDef::StationStatus* stationStatus = NULL;
		if((message.messageState >>= stationStatus) )
		{
			this->removeLocationDuty(stationStatus->regionKey);
		}
		this->notify(*this);
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "Ignoring invalid subsystem duty update type %s", message.messageTypeKey.in());

        FUNCTION_EXIT;
        return;            
    }

    if ( ( DUT_ADD == updateType ) || ( DUT_REMOVE == updateType ) )
    {
        DutyAgentTypeCorbaDef::SubsystemDutySequence* subsystemDutySeq = NULL;
    
        // Extract all subsystem duties from the message and process one by one.
        if ((message.messageState >>= subsystemDutySeq))
        {
            updateSubystemDuty((*subsystemDutySeq), updateType);
        }
    }
	else if( DUT_CHANGE == updateType)
	{
		DutyAgentTypeCorbaDef::SubsystemDutyChanges* changes = NULL;
		if ((message.messageState >>= changes))
		{
			updateSubystemDuty(changes->remove,DUT_REMOVE);
			updateSubystemDuty(changes->add,DUT_ADD);			
		}
	}
    else if ( DUT_STATE == updateType )
    {
        DutyAgentTypeCorbaDef::SubsystemStateSequence* subsystemStateSeq = NULL;
    
        // Extract all subsystem states from the message and process one by one.
        if ((message.messageState >>= subsystemStateSeq))
        {
            SubsystemStateList validStates;

            for ( unsigned long i=0; i<subsystemStateSeq->length(); i++ )
            {
                try
                {
                    const DutyAgentTypeCorbaDef::SubsystemState& subsystemState = (*subsystemStateSeq)[i];

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                                "Received subsystem state change message for [s: %lu, r: %lu, c: %lu]", 
                                subsystemState.subsystemKey, subsystemState.regionKey, subsystemState.currState);

					// yanrong++: CL-21120
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"[CL-21120] Received subsystem state update for[s: %lu, r: %lu, prevS: %lu, currS: %lu]",
								subsystemState.subsystemKey, subsystemState.regionKey, subsystemState.prevState,subsystemState.currState);
					// ++yanrong

                    // Retrieve the matrix element to which the given duty applies.
                    SubsystemDutyMatrixElement sdmElement( getElement( subsystemState.subsystemKey, subsystemState.regionKey ) );

                    // Add it to the list of valid states.
                    validStates.push_back( subsystemState );

                    // Update the state in the matrix.
                    sdmElement.setSubsystemState( static_cast<ESubsystemState>(subsystemState.currState) );

                    // Finally return the updated element to the matrix.
                    setElement( subsystemState.subsystemKey, subsystemState.regionKey, sdmElement );
                }
                catch (const DutyMonitorFrameworkException& ex)
                {
                    // Possibly invalid session ID or subsystem key.
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                                "Ignoring subsystem duty change (%s)", ex.what());
                }              
                catch (const TA_Base_Core::MatrixException& ex)
                {
                    // There is no relevant matrix element. Although unlikely, this could occur if a new
                    // subsystem were configured and put under duty after the matrix had been initially populated.
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                                "Ignoring subsystem duty change (%s)", ex.what());
                }              
                catch (...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Ignoring subsystem duty change");
                }
            }

            if ( !validStates.empty() )
            {
                {
                    TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);
                      
                    TA_ASSERT(m_dutyUpdate == NULL, "Stored duty update pointer is not NULL");

                    // Create a new subsystem duty update based on processed message details.
                    m_dutyUpdate = new SubsystemDutyUpdate;
                    m_dutyUpdate->updateType = updateType;
                    m_dutyUpdate->states = validStates;
                }

                // Let observers know there's a new update waiting to be picked up.
                notify(*this);

                {
                    TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);
                         
                    // All observers should now have processed the update.
                    delete m_dutyUpdate;
                    m_dutyUpdate = NULL;
                }
            }
        }
    }

    FUNCTION_EXIT;
}

void SubsystemDutyMatrix::updateSubystemDuty( DutyAgentTypeCorbaDef::SubsystemDutySequence& subsystemDutySeq, EDutyUpdateTupe updateType )
{
	//DutyDataStore dutyDataStore;
	DataCache& dataCache = DutyDataStore::getDataCache();
	bool ignoreSystemOperators(DutyDataStore::ignoreSystemOperators());
	bool ignoreAppSubsystems(DutyDataStore::ignoreAppSubsystems());
	
	SubsystemDutyList validDuties;
	
	for (unsigned long i = 0; i < subsystemDutySeq.length(); i++)
	{
		try
		{
			const DutyAgentTypeCorbaDef::SubsystemDuty& subsystemDuty = subsystemDutySeq[i];
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Received %s subsystem duty message for [s: %lu, r: %lu]", 
				(updateType == DUT_ADD? "add" : "remove"),
				subsystemDuty.subsystemKey, subsystemDuty.regionKey);
			
			// Discard unwanted duties.
			if (ignoreSystemOperators && dataCache.isSystemProfile(subsystemDuty.profileKey) ||
				ignoreAppSubsystems && dataCache.isAppSubsystem(subsystemDuty.subsystemKey))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "System operator or app subsystem duty - ignoring");
				continue;
			}
			
			// Retrieve the matrix element to which the given duty applies.
			SubsystemDutyMatrixElement sdmElement(getElement(subsystemDuty.subsystemKey, subsystemDuty.regionKey));
			
			// Depending on the type of update, either add to or remove the duty from the element's duty list.
			switch (updateType)
			{
			case DUT_ADD:
				if (sdmElement.addDuty(subsystemDuty))
				{
					validDuties.push_back(subsystemDuty);
				}
				break;
				
			case DUT_REMOVE:
				if (sdmElement.removeDuty(subsystemDuty))
				{
					validDuties.push_back(subsystemDuty);
				}
				break;
				
			default:
				TA_ASSERT(false, "Undefined update type");
			}
			
			// Finally return the updated element to the matrix.
			setElement(subsystemDuty.subsystemKey, subsystemDuty.regionKey, sdmElement);                
		}
		catch (const DutyMonitorFrameworkException& ex)
		{
			// Possibly invalid session ID or subsystem key.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"Ignoring subsystem duty change (%s)", ex.what());
		}              
		catch (const TA_Base_Core::MatrixException& ex)
		{
			// There is no relevant matrix element. Although unlikely, this could occur if a new
			// subsystem were configured and put under duty after the matrix had been initially populated.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				"Ignoring subsystem duty change (%s)", ex.what());
		}              
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Ignoring subsystem duty change");
		}
	}
	
	if (!validDuties.empty())
	{
		{
			TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);
			
			TA_ASSERT(m_dutyUpdate == NULL, "Stored duty update pointer is not NULL");
			
			// Create a new subsystem duty update based on processed message details.
			m_dutyUpdate = new SubsystemDutyUpdate;
			m_dutyUpdate->updateType = updateType;
			m_dutyUpdate->duties = validDuties;
		}
		
		// Let observers know there's a new update waiting to be picked up.
		notify(*this);
		
		{
			TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);
			
			// All observers should now have processed the update.
			delete m_dutyUpdate;
			m_dutyUpdate = NULL;
		}
	}
}


void SubsystemDutyMatrix::removeLocationDuty(unsigned long locationKey)
{
	std::vector<SubsystemKey> subsystemKeys    = this->getRowIds();
	std::vector<SubsystemKey>::iterator subItr = subsystemKeys.begin();
	SubsystemDutyList	tmpDutyList;

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "remove location");

	for(;subItr!=subsystemKeys.end();++subItr)
	{
		SubsystemDutyMatrixElement sdmElement(getElement(*subItr, locationKey));
		SubsystemDutyList elementDuties = sdmElement.getSubsystemDuties();
		tmpDutyList.insert(tmpDutyList.end(), elementDuties.begin(), elementDuties.end());
		sdmElement.clearDuty();
		setElement(*subItr, locationKey, sdmElement);
	}

	if (!tmpDutyList.empty())
	{
		{
			TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);

			TA_ASSERT(m_dutyUpdate == NULL, "Stored duty update pointer is not NULL");

			// Create a new subsystem duty update based on processed message details.
			m_dutyUpdate = new SubsystemDutyUpdate;
			m_dutyUpdate->updateType = DUT_REMOVE;
			m_dutyUpdate->duties = tmpDutyList;
		}

		// Let observers know there's a new update waiting to be picked up.
		notify(*this);

		{
			TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);

			// All observers should now have processed the update.
			delete m_dutyUpdate;
			m_dutyUpdate = NULL;
		}
	}
}

