/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/RegionDutyMatrix.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/03/10 10:15:27 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This class stores for each session the active region duties per region.
  * The duties are stored in the form of a two-dimensional matrix,
  * where rows represent sessions and columns represent regions.
  * Individual elements of the matrix contain a list of region duties.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/SessionAccessFactory.h" //15069
#include "core/exceptions/src/DataException.h" //15069

using namespace TA_Base_Bus;
using namespace TA_Base_Core;

RegionDutyMatrix::RegionDutyMatrix() :
    m_dutiesLoaded(false),
    m_dutyUpdate(NULL)	
	
{
    FUNCTION_ENTRY("RegionDutyMatrix()");

    // Register for region duty add and remove messages
// 	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
//         TA_Base_Core::DutyAgentBroadcastComms::AddLocationDuty, this, NULL);
// 
// 	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
//         TA_Base_Core::DutyAgentBroadcastComms::RemoveLocationDuty, this, NULL);
 	boost::shared_ptr<RegionDutyMatrixUpdater> tmp(new RegionDutyMatrixUpdater(*this));
 	m_updater = tmp;
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
		TA_Base_Core::DutyAgentBroadcastComms::LocationDutyChangeNotification, this,NULL);

	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
		TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification, this,NULL);

    FUNCTION_EXIT;
}


RegionDutyMatrix::~RegionDutyMatrix()
{
    FUNCTION_ENTRY("~RegionDutyMatrix()");

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);

    delete m_dutyUpdate;
    m_dutyUpdate = NULL;

    FUNCTION_EXIT;
}

void RegionDutyMatrix::addOneRowToMatrix( const std::string & sessionId, bool fromActiveSessionList )
{
//	bool exceptionCaught = false; //TD15901
//	if ( hasSessionEndedInDB( sessionId, exceptionCaught ) ) //TD15901
//	{
//		try
//		{
//			if (exceptionCaught == false) // there were no exceptions during DB access, so safe to endSession //TD15901
//			{
//				// This session has ended, so end it
//				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "RegionDutyMatrix::addOneRowToMatrix, want to end the session <%s>, should pay attention", sessionId.c_str());
//				m_authLibrary.endSession(sessionId);
//			}
//		}
//		catch (...)
//		{
//			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "RegionDutyMatrix::addOneRowToMatrix, Unknown exception caught while ending session <%s> ", sessionId.c_str());
//		}
//	}
//	else
//	{
//		if (exceptionCaught == false) // there were no exceptions during DB access, so safe to display in duty manager //TD15901
//		{
			// This session has not yet ended, so add in active sessions list
			//activeSessionIds.push_back(*sesIt);
			addRow(sessionId);
//			if ( fromActiveSessionList )
//			{
//				LOG1( SourceInfo, DebugUtil::DebugInfo, "addRow for session <%s> which come from active session list",
//						sessionId.c_str() );
//			}
//			else
//			{
//				LOG1( SourceInfo, DebugUtil::DebugInfo, "addRow for session <%s> which come from duty agent cache",
//						sessionId.c_str() );
//			}
//		}
//	}
//
//	return (!exceptionCaught);

}

void RegionDutyMatrix::populate(const RegionDutyList& regionDuties)
{
    FUNCTION_ENTRY("populate(const RegionDutyList& regionDuties)");

    if (m_dutiesLoaded)
    {
        // Already populated.
        FUNCTION_EXIT;
        return;
    }

    //DutyDataStore dutyDataStore;
    DataCache& dataCache = DutyDataStore::getDataCache();
    SessionCache& sessionCache = DutyDataStore::getSessionCache();
    bool ignoreSystemOperators(DutyDataStore::ignoreSystemOperators());

    // Get configured regions.
    RegionKeyList regionKeys(dataCache.getRegionKeys());

    // Insert the regions vertically into the matrix.
    for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
    {
        addColumn(*regionIt);
    }

	//TD15069++

	//This is a workaround
	//Need to check from DB if session has_ended
	//If has ended, need to call AuthAgent's endSession 
	//If not ended, add to the list

	SessionIdList sessionIds = sessionCache.getSessionIds();
	
	SessionAccessFactory& sessionDb =  	SessionAccessFactory::getInstance();
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RegionDutyMatrix::populate, the active sessionlist count is %d", sessionIds.size() ) ;
	for (SessionIdList::const_iterator sesIt = sessionIds.begin(); sesIt != sessionIds.end(); sesIt++)
    {
		if(sessionDb.isEndedSession(*sesIt))
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "RegionDutyMatrix::populate, session<%s> has been ended.", (*sesIt).c_str());			
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RegionDutyMatrix::populate, try to add row to matrix,  session<%s>", (*sesIt).c_str());
			addOneRowToMatrix(*sesIt, true );
		}
    }


    // Update the appropriate elements of the matrix with region duties retrieved from the agent.
 	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RegionDutyMatrix::populate, add row which session is not exist in active session list" ) ;
    for (RegionDutyList::const_iterator rgnDutyIt = regionDuties.begin(); rgnDutyIt != regionDuties.end(); rgnDutyIt++)
    {
        // Discard unwanted duties.
        if (ignoreSystemOperators && dataCache.isSystemProfile(rgnDutyIt->profileKey))
        {
            continue;
        }

        // Duty agent may have given us duties for a session that the session cache has not yet received.
        if (!rowExists(rgnDutyIt->session.in()))
        {
			//TD15069++
			addOneRowToMatrix(rgnDutyIt->session.in(), false);
        }

		// check again if it exists, might not be added due to checking if session were ended
		if (rowExists(rgnDutyIt->session.in()) && columnExists(rgnDutyIt->regionKey)) 
		{
			try
			{
				RegionDutyMatrixElement rdmElement(getElement(rgnDutyIt->session.in(), rgnDutyIt->regionKey));

				rdmElement.addDuty(*rgnDutyIt);

				setElement(rgnDutyIt->session.in(), rgnDutyIt->regionKey, rdmElement);
			}
			catch (const TA_Base_Core::MatrixException& ex)
			{
				//TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the region duty matrix.", ex.what()));
				TA_THROW(DutyMonitorFrameworkException("权限管理器中位置权限矩阵赋值失败", ex.what()));
			}  
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RegionDutyMatrix::populate, invalid (session, region) pair:  session:<%s>, region: %ul", rgnDutyIt->session.in(),rgnDutyIt->regionKey);
		}
    }

	m_updater->open(0);
    m_dutiesLoaded = true;

    FUNCTION_EXIT;
}

void RegionDutyMatrix::populateStaticData()
{
	FUNCTION_ENTRY("populateStaticData(const RegionDutyList& regionDuties)");

	if (m_dutiesLoaded)
	{
		// Already populated.
		FUNCTION_EXIT;
		return;
	}

	//DutyDataStore dutyDataStore;
	DataCache& dataCache = DutyDataStore::getDataCache();

	// Get configured regions.
	RegionKeyList regionKeys(dataCache.getRegionKeys());

	// Insert the regions vertically into the matrix.
	for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
	{
		addColumn(*regionIt);
	}

	FUNCTION_EXIT;
}

void RegionDutyMatrix::populateDynamicData(const RegionDutyList& regionDuties)
{
	FUNCTION_ENTRY("populateDynamicData(const RegionDutyList& regionDuties)");
	
	if (m_dutiesLoaded)
	{
		// Already populated.
		FUNCTION_EXIT;
		return;
	}

	//DutyDataStore dutyDataStore;
	DataCache& dataCache = DutyDataStore::getDataCache();
	SessionCache& sessionCache = DutyDataStore::getSessionCache();
	bool ignoreSystemOperators(DutyDataStore::ignoreSystemOperators());

	//TD15069++

	//This is a workaround
	//Need to check from DB if session has_ended
	//If has ended, need to call AuthAgent's endSession 
	//If not ended, add to the list

	SessionIdList sessionIds = sessionCache.getSessionIds();

	SessionAccessFactory& sessionDb =  	SessionAccessFactory::getInstance();
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RegionDutyMatrix::populate, the active sessionlist count is %d", sessionIds.size() ) ;
	for (SessionIdList::const_iterator sesIt = sessionIds.begin(); sesIt != sessionIds.end(); sesIt++)
	{
		if(sessionDb.isEndedSession(*sesIt))
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "RegionDutyMatrix::populate, session<%s> has been ended.", (*sesIt).c_str());			
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RegionDutyMatrix::populate, try to add row to matrix,  session<%s>", (*sesIt).c_str());
			addOneRowToMatrix(*sesIt, true );
		}
	}


	// Update the appropriate elements of the matrix with region duties retrieved from the agent.
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RegionDutyMatrix::populate, add row which session is not exist in active session list" ) ;
	for (RegionDutyList::const_iterator rgnDutyIt = regionDuties.begin(); rgnDutyIt != regionDuties.end(); rgnDutyIt++)
	{
		// Discard unwanted duties.
		if (ignoreSystemOperators && dataCache.isSystemProfile(rgnDutyIt->profileKey))
		{
			continue;
		}

		// Duty agent may have given us duties for a session that the session cache has not yet received.
		if (!rowExists(rgnDutyIt->session.in()))
		{
			//TD15069++
			addOneRowToMatrix(rgnDutyIt->session.in(), false);
		}

		// check again if it exists, might not be added due to checking if session were ended
		if (rowExists(rgnDutyIt->session.in()) && columnExists(rgnDutyIt->regionKey)) 
		{
			try
			{
				RegionDutyMatrixElement rdmElement(getElement(rgnDutyIt->session.in(), rgnDutyIt->regionKey));

				rdmElement.addDuty(*rgnDutyIt);

				setElement(rgnDutyIt->session.in(), rgnDutyIt->regionKey, rdmElement);
			}
			catch (const TA_Base_Core::MatrixException& ex)
			{
				//TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the region duty matrix.", ex.what()));
				TA_THROW(DutyMonitorFrameworkException("权限管理器中位置权限矩阵赋值失败", ex.what()));
			}  
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "RegionDutyMatrix::populate, invalid (session, region) pair:  session:<%s>, region: %ul", rgnDutyIt->session.in(),rgnDutyIt->regionKey);
		}
	}

	m_updater->open(0);
	m_dutiesLoaded = true;

	FUNCTION_EXIT;
}

RegionKeyList RegionDutyMatrix::getColumnIdsByDuties(const SessionId& sessionId)
{
    FUNCTION_ENTRY("getColumnIdsByDuties(const SessionId& sessionId)");

    RegionKeyList regionKeys(getColumnIds());

    //for (RegionKeyList::iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end();)
    //{
    //    if (!getElement(sessionId, *regionIt).dutiesExist())
    //    {
    //        regionIt = regionKeys.erase(regionIt);
    //        continue;
    //    }

    //    regionIt++;
    //}

    FUNCTION_EXIT;
    return regionKeys;
}


const RegionDutyUpdate* RegionDutyMatrix::getDutyUpdate()
{
    FUNCTION_ENTRY("getDutyUpdate()");

    TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);
    
    FUNCTION_EXIT;
    return m_dutyUpdate;
}


void RegionDutyMatrix::receiveSessionUpdate(const SessionUpdate& sessionUpdate)
{
    FUNCTION_ENTRY("receiveSessionUpdate(const SessionUpdate& sessionUpdate)");

    switch (sessionUpdate.updateType)
    {
        case TA_Security::SessionStart: // Operator logged in - add the session to the matrix
            //addRow(sessionUpdate.sessionId);
			m_updater->updateSession(sessionUpdate.sessionId,false);
            LOG1( SourceInfo, DebugUtil::DebugInfo, "[TD15069] addRow for session '%s'.",
                (sessionUpdate.sessionId).c_str() );
            break;

        case TA_Security::SessionEnd:   // Operator logged out - remove the session from the matrix
            //removeRow(sessionUpdate.sessionId);
			m_updater->updateSession(sessionUpdate.sessionId, true);
            break;
        
        default:
            ; // Ignore profile override events
    }
    
    FUNCTION_EXIT;
}


// TA_Base_Core::SpecialisedMessageSubscriber implementation
void RegionDutyMatrix::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)");

    EDutyUpdateTupe updateType(DUT_UNDEFINED);

    // Determine the type of duty update based on message type.
   if (TA_Base_Core::DutyAgentBroadcastComms::AddLocationDuty.getTypeKey().compare(message.messageTypeKey.in()) == 0)
   {
         updateType = DUT_ADD;         
    }
    else if (TA_Base_Core::DutyAgentBroadcastComms::RemoveLocationDuty.getTypeKey().compare(message.messageTypeKey.in()) == 0)
	{
         updateType = DUT_REMOVE;
	}
	
	else if(TA_Base_Core::DutyAgentBroadcastComms::LocationDutyChangeNotification.getTypeKey().compare(message.messageTypeKey.in()) == 0)
	{
		updateType = DUT_CHANGE;
	}
	else if(TA_Base_Core::DutyAgentBroadcastComms::StationOnLineOffLineNotification.getTypeKey().compare(message.messageTypeKey.in()) == 0)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"receiving StationOnLineOffLineNotification");
		DutyAgentTypeCorbaDef::StationStatus* stationStatus = NULL;
		if((message.messageState >>= stationStatus) )
		{
			m_updater->removeLocationDuty(stationStatus->regionKey);
		}

	}
	else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "Ignoring invalid region duty update type %s", message.messageTypeKey.in());

        FUNCTION_EXIT;
        return;            
    }

	if(DUT_ADD == updateType || DUT_REMOVE == updateType)
	{
		 DutyAgentTypeCorbaDef::RegionDutySequence* regionDutySeq = NULL;
		if((message.messageState >>= regionDutySeq) )
		{
			//updateDuty((*regionDutySeq), updateType);
			m_updater->updateDuty((*regionDutySeq),updateType);
		}
	}
	else if(updateType == DUT_CHANGE)
	{
		DutyAgentTypeCorbaDef::RegionDutyChanges* regionDutyChanges  = NULL;
		
		// Extract all region duties from the message and process one by one.
		if ((message.messageState >>= regionDutyChanges))
		{	
// 			updateDuty(regionDutyChanges->rem, DUT_REMOVE);
// 			updateDuty(regionDutyChanges->add, DUT_ADD);
			m_updater->updateDuty(regionDutyChanges->rem, DUT_REMOVE);
			m_updater->updateDuty(regionDutyChanges->add, DUT_ADD);
		}
	}
	

    FUNCTION_EXIT;
}


	//TD15069++
	bool RegionDutyMatrix::hasSessionEndedInDB(const std::string& sessionId, bool& exceptionCaught) //TD15901
    {
		std::string temp = "[TD15069] hasSessionEndedInDB for sessionId: ";
		temp += sessionId;
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, temp.c_str());

		bool hasEnded = false;
		exceptionCaught = false; //TD15901

		try
		{
			// find the session in the DB
			TA_Base_Core::ISession* session;
			session = TA_Base_Core::SessionAccessFactory::getInstance().getSession( sessionId );        

			// check if it has ended or not
			hasEnded = session->getHasEnded();
			if (hasEnded)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "[TD15069] session hasEnded in DB");
				return true; // must return now, no need to check operator and profile
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "[TD15069] session NOT hasEnded in DB");
				// check operator and profile if existing
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"TD15069: Session has not yet ended, check if operator and profile still exists in DB");

			// Comment out this checking because it was later on agreed 
			// that active operator and profiles cannot be deleted from CE
			// Please refer to TD15556

//			// check if profile is deleted in DB
//			unsigned long profileKey = session->getProfileKey();
//			std::string profileName = "";
//			try
//			{
//				profileName = TA_Base_Core::ProfileAccessFactory::getInstance().getDeletedProfileName(profileKey);
//			}
//			catch (const TA_Base_Core::DataException ex)
//            {
//				// assume error is NO_VALUE or NOT_UNIQUE, which means profile is not yet deleted, so just continue
//            }              
//			catch (...)
//            {
//                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
//					"TD15069: Forcing session ended because profile was not found due to unknown error");
//				return true;
//            }
//
//			if (!profileName.empty())
//			{
//                // If profile name has value, but is DELETED in DB
//                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
//					"TD15069: Forcing session ended because profile is not empty but is DELETED");
//				return true; //must return now, no need to check operator
//			}
//
//			// check if operator is deleted in DB
//			unsigned long operatorKey = session->getOperatorKey();
//			std::string operatorName = "";
//			try
//			{
//				operatorName = TA_Base_Core::OperatorAccessFactory::getInstance().getDeletedOperatorName(operatorKey);
//			}
//			catch (const TA_Base_Core::DataException ex)
//            {
//                // assume error is NO_VALUE or NOT_UNIQUE, which means operator is not yet deleted, so just continue
//            }              
//			catch (...)
//            {
//                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
//					"TD15069: Forcing session ended because operator was not found due to unknown error");
//				return true;
//            }
//
//			if (!operatorName.empty())
//			{
//                // If operator name has value, but is DELETED in DB
//                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
//					"TD15069: Forcing session ended because operator is not empty but is DELETED");
//				return true;
//			}
		}
		catch(const TA_Base_Core::DatabaseException& ex)
        {
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "[TD15069] DatabaseException, force hasEnded to false");
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", ex.what() );
			exceptionCaught = true; // This might be no data found, so do not allow call to endSession or display in duty manager //TD15901
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "[TD15069] Unknown exception, force hasEnded to false");
			exceptionCaught = true; // This might be no data found, so do not allow call to endSession or display in duty manager //TD15901

		}

        return hasEnded;
    }

	void RegionDutyMatrix::removeLocationDuty(unsigned long locationKey)
	{
		std::vector<SessionId> sessionIdVector		= this->getRowIds();
		std::vector<SessionId>::iterator sessionItr = sessionIdVector.begin();
		RegionDutyList tmpRegionDutyList;

		for(;sessionItr != sessionIdVector.end();++sessionItr)
		{
			unsigned long sessionLocation = SessionAccessFactory::getLocIDFromSessionId(*sessionItr);
			//valid
			//if( sessionLocation <= TA_Base_Core::SUPER_LOC_ID)
			if( sessionLocation <= 997 && sessionLocation == locationKey)
			{
				std::vector<RegionKey> regionKeys = this->getColumnIds();
				for(std::vector<RegionKey>::iterator regionItr = regionKeys.begin();regionItr!=regionKeys.end();++regionItr)
				{
					RegionDutyMatrixElement rdmElement(getElement(*sessionItr, *regionItr));
					RegionDutyList elementDuties = rdmElement.getRegionDuties();
					tmpRegionDutyList.insert(tmpRegionDutyList.end(), elementDuties.begin(),elementDuties.end());
				}
				this->removeRow(*sessionItr);
			}
			else
			{
				RegionDutyMatrixElement rdmElement(getElement(*sessionItr, locationKey));
				RegionDutyList elementDuties = rdmElement.getRegionDuties();
				tmpRegionDutyList.insert(tmpRegionDutyList.end(), elementDuties.begin(),elementDuties.end());
				rdmElement.clearDuty();
				setElement(*sessionItr, locationKey, rdmElement);
			}
		}

		if(tmpRegionDutyList.size() > 0 )
		{
			{
				TA_Base_Core::ThreadGuard dutyUpdateGuard(m_dutyUpdateLock);

				TA_ASSERT(m_dutyUpdate == NULL, "Stored duty update pointer is not NULL");

				// Create a new region duty update based on processed message details.
				m_dutyUpdate = new RegionDutyUpdate;
				m_dutyUpdate->updateType = DUT_REMOVE;
				m_dutyUpdate->duties = tmpRegionDutyList;
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


	void RegionDutyMatrix::updateDuty( DutyAgentTypeCorbaDef::RegionDutySequence& regionDutySeq, EDutyUpdateTupe updateType )
	{
		//DutyDataStore dutyDataStore;
		DataCache& dataCache = DutyDataStore::getDataCache();
		SessionCache& sessionCache = DutyDataStore::getSessionCache();
		bool ignoreSystemOperators(DutyDataStore::ignoreSystemOperators());
		
		RegionDutyList validDuties;
		
		for (unsigned long i = 0; i < regionDutySeq.length(); i++)
		{
			try
			{
				const DutyAgentTypeCorbaDef::RegionDuty& regionDuty = regionDutySeq[i];
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"Received %s region duty message for [s: %s, r: %lu]", 
					(updateType == DUT_ADD? "add" : "remove"), regionDuty.session.in(), regionDuty.regionKey);
				
				// Discard unwanted duties.
				if (ignoreSystemOperators && dataCache.isSystemProfile(regionDuty.profileKey))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "System operator duty - ignoring");
					continue;
				}
				
				// When an operator logs in, it is likely for a region duty to be received before the corresponding
				// session being update arrives via receiveSessionUpdate(). We must ensure that a new row is inserted
				// into the matrix if necessary so that region duties for the session can be successfully added to 
				// the relevant matrix element.
				if (!rowExists(regionDuty.session.in()))
				{
					if (updateType == DUT_ADD)
					{
						addRow(regionDuty.session.in());
						LOG1( SourceInfo, DebugUtil::DebugInfo, "[TD15069] addRow for session '%s'.",
							regionDuty.session.in() );
					}
					else
					{
						// A remove duty update may be received for a session that has expired and been already 
						// removed from the matrix. In this case the update is simply ignored.
						continue;
					}
				}
				
				// Retrieve the matrix element to which the given duty applies.
				RegionDutyMatrixElement rdmElement(getElement(regionDuty.session.in(), regionDuty.regionKey));
				
				// Depending on the type of update, either add or remove the duty from the element's duty list.
				// If the update is accepted by the element, the duty is OK to be passed onto observers, provided
				// that it involves a known existing session. If the duty is for a brand new session not yet in the
				// session cache, it should not be passed on as the observers could make session info queries that
				// the Authentication Agent may not be able to answer at this stage. (The duty can be extracted 
				// from the matrix later by the client when it gets a session begin update from the session cache.)
				switch (updateType)
				{
				case DUT_ADD:
					if (rdmElement.addDuty(regionDuty) && sessionCache.isKnownSession(regionDuty.session.in()))
					{
						validDuties.push_back(regionDuty);
					}
					break;
					
				case DUT_REMOVE:
					if (rdmElement.removeDuty(regionDuty) && sessionCache.isKnownSession(regionDuty.session.in()))
					{
						validDuties.push_back(regionDuty);
					}
					break;
					
				default:
					TA_ASSERT(false, "Undefined update type");
				}
				
				// Return the updated element to the matrix.
				setElement(regionDuty.session.in(), regionDuty.regionKey, rdmElement);                
			}
			catch (const DutyMonitorFrameworkException& ex)
			{
				// Possibly invalid session ID.
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
					"Ignoring region duty change (%s)", ex.what());
			}              
			catch (const TA_Base_Core::MatrixException& ex)
			{
				// There is no relevant matrix element. Although unlikely, this could occur if a new
				// region were configured and put under duty after the matrix had been initially populated.
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
					"Ignoring region duty change (%s)", ex.what());
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
				
				// Create a new region duty update based on processed message details.
				m_dutyUpdate = new RegionDutyUpdate;
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