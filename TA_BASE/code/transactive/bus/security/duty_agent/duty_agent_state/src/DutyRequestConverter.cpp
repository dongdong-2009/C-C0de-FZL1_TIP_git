/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyRequestConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Implements DutyRequestConverter.
 */

#include "DutyAgentState.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"

//[CL-21875]
#include "bus/security/duty_agent/duty_agent_state/src/DutySessionCache.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/Operator.h"
using TA_Base_Bus::DutySessionCache;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::SessionAccessFactory;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::LocationAccessFactory;

#ifdef _WIN32
	#include <yvals.h>              // warning numbers get enabled in yvals.h 
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif //_WIN32

namespace 
{
    // The following wrapper functions catch exceptions and return NULL or empty
    // values if errors occur.  This allows them to be used inline in more complicated
    // expressions.

    // TODO LPP: It makes it difficult to write neat code when so many methods in 
    // classes (such as ISession) throw exceptions rather than simply returning
    // empty strings etc.

    // NULL is returned if session 'sessionStr' can't be found.
    // NOTE that ISession methods CAN throw exceptions.
    /*gives*/ 
	//[CL-21875]
	TA_Base_Bus::SessionInfo getSession(const std::string& sessionStr)
	{
		TA_Base_Bus::SessionInfo result;
		try
		{
			return DutySessionCache::instance()->getSessionInfo(sessionStr);
		}
		catch (TA_Base_Core::TransactiveException& e)
		{
            LOG_EXCEPTION_CATCH( SourceInfo, "gGetSession(): TransactiveException", e.what() );
        }
		return result;
	}


    // Returns the location name (eg. "DBG") of the session if successful, 
    // otherwise an empty string. 'session' can be NULL. 
	//[CL-21875]
	std::string getLocationNameFromSession(TA_Base_Bus::SessionInfo session)
    {
		try
		{
			if(! session.OperatorId.empty())
			{
				return TA_Base_Bus::DutyDbCache::instance()->getLocationName(session.LocationId);
			}
		}
		catch (TA_Base_Core::TransactiveException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "gGetLocationNameFromSession(): TransactiveException", e.what() );
		}		
        return "";
    }

	// Returns the location display name (eg. "Dhoby Ghaut") of the session if successful, 
	// otherwise an empty string. 'session' can be NULL. 
	std::string getLocationDisplayNameFromSession(TA_Base_Bus::SessionInfo session)
	{
		try
		{
			if(! session.OperatorId.empty())
			{
				return TA_Base_Bus::DutyDbCache::instance()->getLocationDisplayName(session.LocationId);
			}
		}
		catch (TA_Base_Core::TransactiveException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "getLocationDisplayNameFromSession(): TransactiveException", e.what() );
		}		
		return "";
	}

    // Returns the name of the operator if successful, otherwise "".
    // For example, "JulienC".  'session' can be NULL. 
	//[CL-21875]
	std::string getOperatorNameFromSession(TA_Base_Bus::SessionInfo session)
	{
		if ( !session.OperatorId.empty() )
		{
			try
			{
				return TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(session.OperatorId[0],false)->getName();
			}
			catch (TA_Base_Core::TransactiveException& e)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "getOperatorNameFromSession(): TransactiveException", e.what() );
            }
		}
		return "";
	}

    // Returns the name of the profile if successful, otherwise "".
    // For example, "PSCO".  
	//[CL-21875]
	std::string getProfileNameFromSession(TA_Base_Bus::SessionInfo session)
	{
		if ( !session.ProfileId.empty() )
		{
			try
            {
				return TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(session.ProfileId[0],false)->getName();
            }
            catch (TA_Base_Core::TransactiveException& e)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "getProfileNameFromSession(): TransactiveException", e.what() );
            }
		}
		return "";
	}

    std::string getRegionNameFromKey(const TA_Base_Bus::DASType::DbKey& key)
    {
        try
        {
            return TA_Base_Bus::DutyDbCache::instance()->getRegionName(key);
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "getRegionNameFromKey(): TransactiveException", e.what() );
        }

        return "";
    }

	std::string getRegionDisplayNameFromKey(const TA_Base_Bus::DASType::DbKey& key)
	{
		try
		{
			return TA_Base_Bus::DutyDbCache::instance()->getRegionDisplayName(key);
		}
		catch (TA_Base_Core::TransactiveException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "getRegionDisplayNameFromKey(): TransactiveException", e.what() );
		}

		return "";
	}

    std::string getSubsystemNameFromKey(const TA_Base_Bus::DASType::DbKey& key)
    {
        try
        {
            return TA_Base_Bus::DutyDbCache::instance()->getSubsystemName(key);
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "getSubsystemNameFromKey(): TransactiveException", e.what() );
        }

        return "";
    }
}

namespace TA_Base_Bus
{
	DutyRequestConverter::DutyRequestConverter( const DutyPrimitive& requestingSession ) :
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM),
		m_requestingSession(requestingSession),
        m_auditSender(TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
			TA_Base_Core::DutyAgentAudit::Context) )
	{
	}

	void DutyRequestConverter::buildRequestMap(RequestMap& requests, DutyState& state) 
	{
		// TD19417++ Yanrong
		/*
		if (m_requestMap.empty())
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: START New request m_requestMap empty =========="); 
		
			//azenitha
			clearPendingAuditMessages(); 
			clearPendingAuditMessagesAll(); 
			//azenitha
			
			typedef std::map<TA_Base_Bus::DASType::DbKey, TA_Base_Bus::DutyState::Duty> DutyMap;
			DutyMap dutyMap = state.getDutyMap();

			DutyMap::const_iterator regionsIter;
			TA_Base_Bus::DutyState::SubDutyMap::const_iterator subsysIter;
			TA_Base_Bus::DASType::DutySet::const_iterator profileIt;

			std::string localSession = state.getInitiatingSession();
			//azenitha
			std::string temp = "TDACCESS: localSession = "; 
			temp += localSession; 
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, temp.c_str()); 
			//azenitha

			for( regionsIter = dutyMap.begin(); regionsIter != dutyMap.end(); regionsIter++ )
			{
				std::string regionName = getRegionNameFromKey( regionsIter->first );
				//azenitha
				std::string temp = "TDACCESS: regionName = "; 
				temp += regionName; 
				LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, temp.c_str()); 
				//azenitha

				for( subsysIter = regionsIter->second.m_subsystem.begin(); subsysIter != regionsIter->second.m_subsystem.end(); subsysIter++ )
				{
					std::string srcSession;
					std::string destSession;
					std::string subsystemName = getSubsystemNameFromKey(subsysIter->first);
					//azenitha
					std::string temp = "TDACCESS: subsystemName = "; 
					temp += subsystemName; 
					LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, temp.c_str()); 
					//azenitha

					for( profileIt = subsysIter->second.m_remove.begin(); profileIt != subsysIter->second.m_remove.end(); profileIt++ )
					{
						destSession = (*profileIt).m_session;

						//azenitha
						std::string temp = "TDACCESS: destSession = "; 
						temp += destSession; 
						LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, temp.c_str()); 
						//azenitha

						//azenitha
						if (localSession.compare(destSession) != 0)
						{
							if (state.getInitiatingAction() == DASConst::REQUEST_SUBSYSTEM_DUTY_TRANSFER)
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: Add to list from m_add"); 
								pendingAuditMessageToSend(localSession, destSession, subsystemName);
							} 
							else if (state.getInitiatingAction() == DASConst::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER)
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: Add to list from m_add"); 
								pendingAuditMessageToSendAll(localSession, destSession);
							}							
						}
						else
						{
							LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: localSession == destSession, ignore"); 
						}
						//azenitha
					}

					for( profileIt = subsysIter->second.m_add.begin(); profileIt != subsysIter->second.m_add.end(); profileIt++ )
					{
						// srcSession should be the same as localSession
						srcSession = (*profileIt).m_session;

						//azenitha
						std::string temp = "TDACCESS: srcSession = "; 
						temp += srcSession; 
						LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, temp.c_str()); 
						//azenitha

						//azenitha
						if (localSession.compare(srcSession) != 0)
						{
							if (state.getInitiatingAction() == DASConst::REQUEST_SUBSYSTEM_DUTY_TRANSFER)
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: Add to list from m_remove"); 
								pendingAuditMessageToSend(localSession, srcSession, subsystemName);
							} 
							else if (state.getInitiatingAction() == DASConst::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER)
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: Add to list from m_remove"); 
								pendingAuditMessageToSendAll(localSession, srcSession);
							}
						}
						else
						{
							LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: localSession == srcSession, ignore"); 
						}
						//azenitha
					}
				}
				
			}

			//azenitha
			if (state.getInitiatingAction() == DASConst::REQUEST_SUBSYSTEM_DUTY_TRANSFER)
			{
				sendPendingAuditMessages(); 
			}
			else if (state.getInitiatingAction() == DASConst::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER)
			{
				sendPendingAuditMessagesAll(); 
			}
			//azenitha
			
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: END New request m_requestMap empty =========="); 
		}
		*/
		// ++TD19417 Yanrong

		// TD19417++ Yanrong: send AuditMessage for requesting subsystems.
		LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: START buildRequestMap ==========");
		clearPendingAuditMessages(); 
		clearPendingAuditMessagesAll(); 

		SessionsMap::iterator remIt = m_removedSessions.begin();
		SessionsMap::iterator addIt;

		// if there are the same session is both added and removed,
		// this indicate that the removed session held current subsystem duty before requesting,
		// so we need to remove the session from added session.
		for( ; remIt != m_removedSessions.end(); ++remIt )
		{
			addIt = m_addedSessions.find(remIt->first);
			//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TDACCESS: erase item from m_addedSessions");
			m_addedSessions.erase(addIt);
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TDACCESS: m_addedSessions size=%d, m_removedSessions size=%d", 
			(int)m_addedSessions.size(), (int)m_removedSessions.size());

		//store all requesting sessions
		SubsystemsMap requestedSubsystems;

		// collect all requested subsystems which come from requesting session
		for( addIt = m_addedSessions.begin(); addIt != m_addedSessions.end(); ++addIt )
		{
			if( requestedSubsystems.find(addIt->second) == requestedSubsystems.end() )
				requestedSubsystems[addIt->second] = addIt->first;
			else
				requestedSubsystems[addIt->second] += (", " + addIt->first); 
		}

		// collect all requested subsystems which come form another sessions
		for( remIt = m_removedSessions.begin(); remIt != m_removedSessions.end(); ++remIt )
		{
			if( requestedSubsystems.find(remIt->second) == requestedSubsystems.end() )
				requestedSubsystems[remIt->second] = remIt->first;
			else
				requestedSubsystems[remIt->second] += (", " + remIt->first);
		}

		std::string initSession = m_requestingSession.m_session.c_str();
		SubsystemsMap::iterator iter = requestedSubsystems.begin();

		for( ; iter != requestedSubsystems.end(); ++iter )
		{
			if (m_initiatingAction == DASConst::REQUEST_SUBSYSTEM_DUTY_TRANSFER)
			{
				//LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: pending requestAudiMessage, subsystem list: %s", iter->second.c_str()); 
				pendingAuditMessageToSend(initSession, iter->first, iter->second);
			} 
			else if (m_initiatingAction == DASConst::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER)
			{
				//LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: pending requestAllAuditMessage"); 
				pendingAuditMessageToSendAll(initSession, iter->first);
			}
		}

		if (state.getInitiatingAction() == DASConst::REQUEST_SUBSYSTEM_DUTY_TRANSFER)
		{
			sendPendingAuditMessages(); 
		}
		else if (state.getInitiatingAction() == DASConst::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER)
		{
			sendPendingAuditMessagesAll(); 
		}
		// ++TD19417 Yanrong

		//LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: START New request =========="); 

		InternalRequestMap::const_iterator sesIt;

		DutyAgentTypeCorbaDef::DutyRequest newRequest;
		std::pair<RequestMap::iterator,bool> insertResult;

		DASType::SubsystemMap::const_iterator regIt;
		DASType::SubsystemSet::const_iterator subIt;

		unsigned long regIdx = 0;
		unsigned long subIdx = 0;

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"m_requestMap.size: %d ", m_requestMap.size());

		for( sesIt=m_requestMap.begin(); sesIt!=m_requestMap.end(); sesIt++ )
		{
			std::string uuid = TA_Base_Core::TA_Uuid::generateUUID();

			insertResult = requests.insert( RequestMap::value_type(uuid,newRequest) );
			insertResult.first->second.targetSession = CORBA::string_dup(sesIt->first.c_str());
			insertResult.first->second.uniqueId      = CORBA::string_dup(uuid.c_str());

			insertResult.first->second.dutyAgent     = TransactiveAgentInfo::m_responseObjRef;
			insertResult.first->second.duty.session  = CORBA::string_dup(m_requestingSession.m_session.c_str());
			insertResult.first->second.duty.profile  = m_requestingSession.m_profile;

			insertResult.first->second.duty.subsystems.length( sesIt->second.size() );

            std::string subsytemList;
			for( regIt=sesIt->second.begin(),regIdx=0; regIt!=sesIt->second.end(); regIt++,regIdx++ )
			{
				insertResult.first->second.duty.subsystems[regIdx].region = regIt->first;

				insertResult.first->second.duty.subsystems[regIdx].subsystems.length(regIt->second.size());
                
				/*
                std::string regionName = TA_Base_Bus::DutyDbCache::instance()->getRegionName( regIt->first );
                std::string subsys;
				*/
				// ++TD19417 Yanrong
				
                for( subIt=regIt->second.begin(),subIdx=0; subIt!=regIt->second.end(); subIt++,subIdx++ )
				{
					insertResult.first->second.duty.subsystems[regIdx].subsystems[subIdx] = *subIt;

					// TD19417++ Yanrong
					/*
                    std::string subsystemName = TA_Base_Bus::DutyDbCache::instance()->getSubsystemName( *subIt );
                    if (subsys.empty())
                    {
                        subsys = subsystemName;
                    }
                    else
                    {
                        subsys += ", " + subsystemName;
                    }
					*/
					// ++TD19417 Yanrong
				}
				// TD19417++ Yanrong
				/*
                subsytemList = subsytemList + " " + subsys + "@" + regionName;
				*/
				// ++TD19417 Yanrong
			}

			// TD19417++ Yanrong
			/*
            // log request duty audit message per targetSession
            try
            {
                if (m_initiatingAction == DASConst::REQUEST_SUBSYSTEM_DUTY_TRANSFER ||
                    m_initiatingAction == DASConst::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER)
                {
                    std::string initSession = m_requestingSession.m_session;

					//azenitha
					std::string temp = "TDACCESS: initSession = "; 
					temp += initSession; 
					LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, temp.c_str()); 
					//azenitha

                    std::string initProfile = TA_Base_Bus::DutyDbCache::instance()->getProfileName(m_requestingSession.m_profile);
                    std::string initLocation;
                    std::string initOperator;
                    if (!initSession.empty())
                    {
                        std::auto_ptr<TA_Base_Core::ISession> session(SessionAccessFactory::getInstance().getSession(initSession));
                        std::auto_ptr<TA_Base_Core::IEntityData> consoleEntity(EntityAccessFactory::getInstance().getEntity(session->getConsoleKey()));
                        std::auto_ptr<TA_Base_Core::ILocation> location(LocationAccessFactory::getInstance().getLocationByKey(consoleEntity->getLocation()));
                        initLocation = location->getName();
                        initOperator = session->getOperatorName();
                    }
                    std::string destSession = sesIt->first;

					//azenitha
					std::string temp2 = "TDACCESS: destSession = "; 
					temp2 += destSession; 
					LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, temp2.c_str()); 
					//azenitha

                    std::string destOperator;
                    std::string destProfile;
                    std::string destLocation;
                    if (!destSession.empty())
                    {
                        std::auto_ptr<TA_Base_Core::ISession> session(SessionAccessFactory::getInstance().getSession(destSession));
                        std::auto_ptr<TA_Base_Core::IEntityData> consoleEntity(EntityAccessFactory::getInstance().getEntity(session->getConsoleKey()));
                        std::auto_ptr<TA_Base_Core::ILocation> location(LocationAccessFactory::getInstance().getLocationByKey(consoleEntity->getLocation()));
                        destLocation = location->getName();
                        destOperator = session->getOperatorName();
                        destProfile  = TA_Base_Bus::DutyDbCache::instance()->getProfileName(session->getProfileKey());
                    }

					//azenitha
					if (initSession.compare(destSession) == 0)
					{
						LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: initSession == destSession, ignore"); 
						// TD19417++
						//break;
						continue;
						// ++TD19417
					}
					//azenitha

                    if (!initSession.empty())
			        {
                        TA_Base_Core::DescriptionParameters desc;
                        TA_Base_Core::NameValuePair param1("SrcOperatorId", initOperator + "/" + initProfile);
                        desc.push_back(&param1);
                        TA_Base_Core::NameValuePair param2("SrcLocationId", initLocation);
                        desc.push_back(&param2);
                        TA_Base_Core::NameValuePair param3("SubsystemIdList", subsytemList);
                        desc.push_back(&param3);
                        TA_Base_Core::NameValuePair param4("DstOperatorId", destOperator + "/" + destProfile);
                        desc.push_back(&param4);
                        TA_Base_Core::NameValuePair param5("DstLocationId", destLocation);
                        desc.push_back(&param5);
                        TA_Base_Core::NameValuePair param6("DataPointValue", initOperator);  // change value field
                        desc.push_back(&param6);

                        if (m_initiatingAction == DASConst::REQUEST_SUBSYSTEM_DUTY_TRANSFER)
                        {
							try
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Sent SubsystemDutyRequest");
								m_auditSender->sendAuditMessage(
									TA_Base_Core::DutyAgentAudit::SubsystemDutyRequest,
									TA_Base_Bus::TransactiveAgentInfo::m_entity,
									desc, "Subsystem duty request",
									initSession, "", "", "" );
							}
							catch (...)
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Unknown exception");
							}
                        }
                        else if (m_initiatingAction == DASConst::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER)
                        {
							try
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Sent SubsystemDutyAllRequest");
								m_auditSender->sendAuditMessage(
									TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequest,
									TA_Base_Bus::TransactiveAgentInfo::m_entity,
									desc, "Subsystem duty all request",
									initSession, "", "", "" );
							}
							catch (...)
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Unknown exception");
							}

							if (destSession.empty())
							{
								break; //so that no duplicate events
							}
							//else do not break so that SubsystemDutyAllRequestReceived will be sent
                        }
			        }

                    if (!destSession.empty())
			        {
                        TA_Base_Core::DescriptionParameters desc;
                        TA_Base_Core::NameValuePair param1("SrcOperatorId", initOperator + "/" + initProfile);
                        desc.push_back(&param1);
                        TA_Base_Core::NameValuePair param2("SrcLocationId", initLocation);
                        desc.push_back(&param2);
                        TA_Base_Core::NameValuePair param3("SubsystemIdList", subsytemList);
                        desc.push_back(&param3);
                        TA_Base_Core::NameValuePair param4("DstOperatorId", destOperator + "/" + destProfile);
                        desc.push_back(&param4);
                        TA_Base_Core::NameValuePair param5("DstLocationId", destLocation);
                        desc.push_back(&param5);
                        TA_Base_Core::NameValuePair param6("DataPointValue", destOperator);  // change value field
                        desc.push_back(&param6);

                        if (m_initiatingAction == DASConst::REQUEST_SUBSYSTEM_DUTY_TRANSFER)
                        {
							try
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Sent SubsystemDutyRequestReceived");
								m_auditSender->sendAuditMessage(
									TA_Base_Core::DutyAgentAudit::SubsystemDutyRequestReceived,
									TA_Base_Bus::TransactiveAgentInfo::m_entity,
									desc, "Subsystem duty request",
									destSession, "", "", "" );
							}
							catch (...)
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Unknown exception");
							}
                        }
                        else if (m_initiatingAction == DASConst::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER)
                        {
							try
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Sent SubsystemDutyAllRequestReceived");
								m_auditSender->sendAuditMessage(
									TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequestReceived,
									TA_Base_Bus::TransactiveAgentInfo::m_entity,
									desc, "Subsystem duty all request",
									destSession, "", "", "" );
							}
							catch (...)
							{
								LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Unknown exception");
							}

							break;
                        }
			        }
                }
            }
            catch (DatabaseException& ex)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", ex.what() );
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"unkown exception occurs");
            }
			*/
			// ++TD19417 Yanrong
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "TDACCESS: END New request =========="); 
	}		


    // Returns a string of the form "left(right)".
    std::string DutyRequestConverter::concatStr(const std::string& left, const std::string& right)
    {
		if ((!left.empty()) && (!right.empty()))
        {
            return left + "(" + right + ")";
        }
        
        return left + right;
    }

	std::string DutyRequestConverter::concatStr(const std::string& left, const std::string& right, const std::string& connector)
	{
		if ((!left.empty()) && (!right.empty()))
		{
			return left + connector + right;
		}

		return left + right;       
	}

    // If srcIsSender is true, the src session is the sender (originator) of the audit message,
    // otherwise the dst session is the sender.
    void DutyRequestConverter::sendAuditMessage(
        const TA_Base_Core::MessageType& messageType,
        const std::string& srcSessionStr,
        const std::string& dstSessionStr,
        bool srcIsSender,                         
        const std::string& subsystemIdList,
        const std::string& regionIdList,
        const std::string& msgDetailsStr )
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "begin to sendAuditMessage src=%s, dst=%s",srcSessionStr.c_str(),dstSessionStr.c_str()); 

		TA_Base_Bus::SessionInfo srcSession( getSession(srcSessionStr) );
        std::string srcOpName( getOperatorNameFromSession(srcSession) ) ;
		std::string srcOpProName( concatStr(srcOpName, getProfileNameFromSession(srcSession)) );
		std::string srcLocName( getLocationDisplayNameFromSession(srcSession) );
		std::string srcOpProWithLocName = srcOpProName;

		//Local  profile, add " @location"
		if (srcSession.ProfileId.size() > 0 && 
			srcLocName.length() > 0 &&
			TA_Base_Bus::DutyDbCache::instance()->getProfileType(srcSession.ProfileId[0]) == TA_Base_Bus::IDutyPolicy::PT_LOCAL)
		{
			srcOpProWithLocName = srcOpProWithLocName + " @" + srcLocName;
		}

        TA_Base_Bus::SessionInfo dstSession( getSession(dstSessionStr) );
        std::string dstOpName( getOperatorNameFromSession(dstSession) );
		std::string dstOpProName( concatStr(dstOpName, getProfileNameFromSession(dstSession)) );
		std::string dstLocName( getLocationDisplayNameFromSession(dstSession) );
		std::string dstOpProWithLocName = dstOpProName;

		//local profile add " @location"
		if (dstSession.ProfileId.size() > 0 && 
			dstLocName.length() > 0 &&
			TA_Base_Bus::DutyDbCache::instance()->getProfileType(dstSession.ProfileId[0]) == TA_Base_Bus::IDutyPolicy::PT_LOCAL)
		{
			dstOpProWithLocName = dstOpProWithLocName + " @" + dstLocName;
		}

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "SRC:<OpName %s>,<OpProName %s>,<LocName %s>,<OpProWithLocName %s>",srcOpName.c_str(), srcOpProName.c_str(),srcLocName.c_str(),srcOpProWithLocName.c_str());
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "DST:<OpName %s>,<OpProName %s>,<LocName %s>,<OpProWithLocName %s>",dstOpName.c_str(), dstOpProName.c_str(),dstLocName.c_str(),dstOpProWithLocName.c_str());
		
        TA_Base_Core::DescriptionParameters params;
		params.reserve(14);

		TA_Base_Core::NameValuePair param1("SrcOperatorId", srcOpProName );
		params.push_back(&param1);
		TA_Base_Core::NameValuePair param2("SrcLocationId", srcLocName );
		params.push_back(&param2);
		TA_Base_Core::NameValuePair param3("DstOperatorId", dstOpProName );
		params.push_back(&param3);
		TA_Base_Core::NameValuePair param4("DstLocationId", dstLocName );
		params.push_back(&param4);
        TA_Base_Core::NameValuePair param5("SubsystemIdList", subsystemIdList);
        params.push_back(&param5);
        TA_Base_Core::NameValuePair param6("RegionIdList", regionIdList);
        params.push_back(&param6);
        TA_Base_Core::NameValuePair param7("DataPointValue", srcIsSender ? srcOpName : dstOpName);  // "VALUE" field
        params.push_back(&param7);
		// New parameter added for JIRA DTL-1096
		TA_Base_Core::NameValuePair param8("SrcOperatorAtLocation", srcOpProWithLocName);		// [SrcOperatorId] @[SrcLocationId]
		TA_Base_Core::NameValuePair param9("FromSrcOperatorAtLocation", concatStr("from", srcOpProWithLocName, " "));		// from [SrcOperatorId] @[SrcLocationId]
		TA_Base_Core::NameValuePair param10("ToSrcOperatorAtLocation", concatStr("to", srcOpProWithLocName, " "));		// to [SrcOperatorId] @[SrcLocationId]
		TA_Base_Core::NameValuePair param11("TakenBySrcOperatorAtLocation", concatStr("taken by", srcOpProWithLocName, " "));		// taken by [SrcOperatorId] @[SrcLocationId]
		TA_Base_Core::NameValuePair param12("DstOperatorAtLocation", dstOpProWithLocName);		// [DstOperatorId] @[DstLocationId]
		TA_Base_Core::NameValuePair param13("FromDstOperatorAtLocation", concatStr("from", dstOpProWithLocName, " "));		// from [DstOperatorId] @[DstLocationId]
		TA_Base_Core::NameValuePair param14("ToDstOperatorAtLocation", concatStr("to", dstOpProWithLocName, " "));		// to [DstOperatorId] @[DstLocationId]
		
		if ( srcOpName.empty() || srcLocName.empty() )
		{
			param8.value = "";
			param9.value = "";
			param10.value = "";
			param11.value = "";
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "clear src parameter");
		}
		if ( dstOpName.empty() || dstLocName.empty() )
		{
			param12.value = "";
			param13.value = "";
			param14.value = "";
			LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "clear dst parameter");
		}

		params.push_back(&param8);
		params.push_back(&param9);
		params.push_back(&param10);
		params.push_back(&param11);
		params.push_back(&param12);
		params.push_back(&param13);
		params.push_back(&param14);

		// yanrong++: CL-19418
		std::string audiParamString;
		TA_Base_Core::gGetStringFromParameters(params, audiParamString);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[CL-19418] SendAuditMessage, messageType=%s, message parameters: %s",
			messageType.getTypeKey().c_str(), audiParamString.c_str());
		// ++yanrong

		try
		{
			m_auditSender->sendAuditMessage(
				messageType,
				TA_Base_Bus::TransactiveAgentInfo::m_entity,
				params, 
				msgDetailsStr,
				srcIsSender ? srcSessionStr : dstSessionStr, 
				"", "", "" );
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"TDACCESS: Unknown exception");
		}			
    }


	void DutyRequestConverter::setSession(const std::string& session)
	{
	}


	void DutyRequestConverter::setAction(const std::string& action)
	{
        m_initiatingAction = action;
		m_addedSessions.clear();
		m_removedSessions.clear();
	}

	void DutyRequestConverter::setRegionCount(unsigned long count)
	{
	}

	void DutyRequestConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		
		m_region = region;
	}

	void DutyRequestConverter::setSubsystemCount(unsigned long count)
	{
	}

	void DutyRequestConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem = subsystem;
	}


	void DutyRequestConverter::setRegionAddCount( unsigned long count )
	{
	}

	void DutyRequestConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void DutyRequestConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DutyRequestConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DutyRequestConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void DutyRequestConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void DutyRequestConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		std::string subsystemName = getSubsystemNameFromKey(m_subsystem);

		// store sessions who add this subsystem
		m_addedSessions[subsystemName] = session.m_session;

		std::stringstream logInfo;
		logInfo << "TDACCESS: subsystemAdd +++++ profile key="
				<< session.m_profile
				<< ". session="
				<< session.m_session
				<< ". sybsystem=" 
				<< subsystemName;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, logInfo.str().c_str());

		// ++TD19417
	}
	void DutyRequestConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		// TD19417++
		std::string subsystemName = getSubsystemNameFromKey(m_subsystem);

		std::stringstream logInfo;
		logInfo << "TDACCESS: subsystemRemove ----- profile key="
				<< session.m_profile
				<< ". session="
				<< session.m_session
				<< " subsystem="
				<< subsystemName;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, logInfo.str().c_str());

		if( m_requestingSession.m_session.compare(session.m_session.c_str()) == 0 )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "TDACCESS: ****** dest session == request session, ingnore");
			return;
		}

		// store sessions who remove this subsystem
		m_removedSessions[subsystemName] = session.m_session;
		// ++TD19417
		std::pair<InternalRequestMap::iterator,bool>    sesInsertResult;
		std::pair<DASType::SubsystemMap::iterator,bool> subInsertResult;

		sesInsertResult = m_requestMap.insert( InternalRequestMap::value_type(session.m_session,m_emptySubMap) );

		subInsertResult = sesInsertResult.first->second.insert( DASType::SubsystemMap::value_type(m_region,
			m_emptySubSet) );

		subInsertResult.first->second.insert( m_subsystem );

	}

	void DutyRequestConverter::subsystemDeny( const DutyPrimitive& session )
	{
	}

	void DutyRequestConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void DutyRequestConverter::subsystemComplete()
	{
	}

	void DutyRequestConverter::regionComplete()
	{
	}

	//azenitha
	void DutyRequestConverter::clearPendingAuditMessages()
	{
		FUNCTION_ENTRY("clearPendingAuditMessages");

		m_auditMessageDataList.clear();

		FUNCTION_EXIT;
	}

	void DutyRequestConverter::clearPendingAuditMessagesAll()
	{
		FUNCTION_ENTRY("clearPendingAuditMessagesAll");

		m_auditMessageDataAllList.clear();

		FUNCTION_EXIT;
	}

	void DutyRequestConverter::sendPendingAuditMessages()
	{
		FUNCTION_ENTRY("sendPendingAuditMessages");

		for (unsigned int i=0; i<m_auditMessageDataList.size(); i++)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Sent SubsystemDutyRequest m_requestMap empty");
			sendAuditMessage(
				TA_Base_Core::DutyAgentAudit::SubsystemDutyRequest,
				m_auditMessageDataList[i].srcSession, m_auditMessageDataList[i].destSession, true,
				m_auditMessageDataList[i].subsystemName, "" /*regionName is not used*/,
				"Subsystem duty request" );

			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Sent SubsystemDutyRequestReceived m_requestMap empty");
			sendAuditMessage(
				TA_Base_Core::DutyAgentAudit::SubsystemDutyRequestReceived,
				m_auditMessageDataList[i].srcSession, m_auditMessageDataList[i].destSession, false,
				m_auditMessageDataList[i].subsystemName, "" /*regionName is not used*/,
				"Subsystem duty request received" );
		}
		
		FUNCTION_EXIT;
	}

	void DutyRequestConverter::sendPendingAuditMessagesAll()
	{
		FUNCTION_ENTRY("sendPendingAuditMessagesAll");

		bool bSendDutyAllRequest = false;

		for (unsigned int i=0; i<m_auditMessageDataAllList.size(); i++)
		{
			if (bSendDutyAllRequest == false)
			{
				//Sent once only
				LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Sent SubsystemDutyAllRequest For ONCE");
				bSendDutyAllRequest = true;
				sendAuditMessage(
					TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequest,
					m_auditMessageDataAllList[i].srcSession, m_auditMessageDataAllList[i].destSession, true,
					"" /*subsystemName is not used*/, "" /*regionName is not used*/,
					"Subsystem duty all request" );
			}

			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Sent SubsystemDutyAllRequestReceived");
			sendAuditMessage(
				TA_Base_Core::DutyAgentAudit::SubsystemDutyAllRequestReceived,
				m_auditMessageDataAllList[i].srcSession, m_auditMessageDataAllList[i].destSession, false,
				"" /*subsystemName is not used*/, "" /*regionName is not used*/,
				"Subsystem duty all request received" );
		}
		
		FUNCTION_EXIT;
	}

	void DutyRequestConverter::pendingAuditMessageToSend(
		const std::string& srcSession,
		const std::string& destSession,
		const std::string& subsystemName)
	{
		FUNCTION_ENTRY("sendPendingAuditMessages");

		//TODO: use iterator

		// TD19417++ Yanrong
		/*
		//Add first element
		if (m_auditMessageDataList.size() == 0)
		{
			AuditMessageData auditMessageData;
			auditMessageData.srcSession = srcSession;
			auditMessageData.destSession = destSession;
			auditMessageData.subsystemName = subsystemName;
			m_auditMessageDataList.push_back(auditMessageData);
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Add first element");
		}
		else
		*/
		// ++TD19417 Yanrong
		{
			//Find if existing
			bool bFound = false;
			for (unsigned int i=0; i<m_auditMessageDataList.size(); i++)
			{
				if (((m_auditMessageDataList[i].srcSession).compare(srcSession) == 0) &&
					((m_auditMessageDataList[i].destSession).compare(destSession) == 0) &&
					((m_auditMessageDataList[i].subsystemName).compare(subsystemName) == 0))
				{
					//Duplicate, do not add
					bFound = true;
					LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Duplicate, do not add");
					break;
				}
			}

			if (!bFound)
			{
				AuditMessageData auditMessageData;
				auditMessageData.srcSession = srcSession;
				auditMessageData.destSession = destSession;
				auditMessageData.subsystemName = subsystemName;
				m_auditMessageDataList.push_back(auditMessageData);
				LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Add new element src=%s, dest=%s, subs=%s",
					srcSession.c_str(), destSession.c_str(), subsystemName.c_str());
			}
		}
	
		FUNCTION_EXIT;
	}

	void DutyRequestConverter::pendingAuditMessageToSendAll(
		const std::string& srcSession,
		const std::string& destSession)
	{
		FUNCTION_ENTRY("pendingAuditMessageToSendAll");

		//TODO: use iterator

		// TD19417++ Yanrong
		/*
		//Add first element
		if (m_auditMessageDataAllList.size() == 0)
		{
			AuditMessageDataAll auditMessageDataAll;
			auditMessageDataAll.srcSession = srcSession;
			auditMessageDataAll.destSession = destSession;
			m_auditMessageDataAllList.push_back(auditMessageDataAll);
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Add first element");
		}
		else
		*/
		// ++TD19417 Yanrong
		{
			//Find if existing
			bool bFound = false;
			for (unsigned int i=0; i<m_auditMessageDataAllList.size(); i++)
			{
				if (((m_auditMessageDataAllList[i].srcSession).compare(srcSession) == 0) &&
					((m_auditMessageDataAllList[i].destSession).compare(destSession) == 0))
				{
					//Duplicate, do not add
					bFound = true;
					LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Duplicate, do not add");
					break;
				}
			}

			if (!bFound)
			{
				AuditMessageDataAll auditMessageDataAll;
				auditMessageDataAll.srcSession = srcSession;
				auditMessageDataAll.destSession = destSession;
				m_auditMessageDataAllList.push_back(auditMessageDataAll);
				LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"TDACCESS: Add new element, src=%s, dest=%s",
					srcSession.c_str(), destSession.c_str());
			}
		}
	
		FUNCTION_EXIT;
	}
	//azenitha
}



