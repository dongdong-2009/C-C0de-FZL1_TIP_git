/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainAgentAccess.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Serves as a front end to the following Train Agent interfaces:
  *
  * 1. ITrainInformationCorbaDef:
  *
  *    Currently, the only train information provided is train IDs for inbound/outbound/all
  *    train. These are retrieved from the Train Agent only once and stored locally as it
  *    is not expected to change frequently. This locally stored data is used to service
  *    subsequent requests for train IDs.
  *
  * 2. ITISTrainCommandsCorbaDef:
  *
  *    Enables a specific train identified by its physical train ID to be queried regarding
  *    the message library version in use.
  *
  * This class handles all Train Agent exceptions and rethrows as TrainAgentAccessException.
  *
  * To avoid multiple CORBA connections (named objects) being created by Train Agent clients,
  * this class has been implemented as a singleton.
  *
  */

#if !defined(AFX_TRAINAGENTACCESS_H__EFD31578_4599_4BF7_A382_81814D9262E8__INCLUDED_)
#define AFX_TRAINAGENTACCESS_H__EFD31578_4599_4BF7_A382_81814D9262E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <map>
#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
//#include "bus/trains/TrainAgentCorbaDef/idl/src/ITrainInformationCorbaDef.h"
//#include "bus/trains/TrainAgentCorbaDef/idl/src/ITISTrainCommandsCorbaDef.h"


namespace TA_Base_App
{
    class TrainAgentAccess
    {
    // Types
    public:
        typedef unsigned char TrainId;
        typedef std::set<TrainId> TrainIdSet;
        typedef std::map<TA_Base_Bus::ITrainInformationCorbaDef::ETrack, TrainIdSet> TrainIdMap;


    // Operations
    public:
        /**
          * Returns the one and only instance of the TrainAgentAccess.
          * Throws a TrainAgentAccessException if the named object's corba name
          * cannot be initialised due to invalid Train Agent entity configuration.
          *
          */
        static TrainAgentAccess& getInstance();

       /**
         * Destructor
         *
         */
        ~TrainAgentAccess();

       /**
         * Returns the physical IDs of trains on the inbound track.
         * If the relevant train details are not stored locally, the Train Agent
         * is accessed.
         * Throws a TrainAgentAccessException if an error occurs while retrieving
         * train data from the agent.
         *
         */
       TrainIdSet getInboundTrainIds();

       /**
         * Returns the physical IDs of trains on the outbound track.
         * If the relevant train details are not stored locally, the Train Agent
         * is accessed.
         * Throws a TrainAgentAccessException if an error occurs while retrieving
         * train data from the agent.
         *
         */
       TrainIdSet getOutboundTrainIds();

       /**
         * Returns the physical IDs of all trains in the system, including those
         * that are neither inbound nor outbound.
         * If the relevant train details are not stored locally, the Train Agent
         * is accessed.
         * Throws a TrainAgentAccessException if an error occurs while retrieving
         * train data from the agent.
         *
         */
       TrainIdSet getAllTrainIds();

       /**
         * Returns the version of the TIS message library currently in use at the
         * specified train. The Train Agent is accessed every time this method is called.
         * Throws a TrainAgentAccessException if an error occurs while retrieving
         * train data from the agent.
         *
         */
       unsigned short getTisLibraryVersion(TrainId trainId);


    private:
        TrainAgentAccess();
        TrainAgentAccess(const TrainAgentAccess&);
        TrainAgentAccess& operator=(const TrainAgentAccess&);

       /**
         * Checks whether the train ID map is empty and sets it up if necessary
         * based on data retrieved from the Train Agent.
         * Throws a TrainAgentAccessException if an error occurs while retrieving
         * train data from the agent.
         *
         */
        void initialiseTrainIdMap();


    // Attributes
    private:
        static TrainAgentAccess* s_trainAgentAccess;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        // Train Agent named object for train information
        TA_Base_Core::NamedObject<TA_Base_Bus::ITrainInformationCorbaDef,
                                  TA_Base_Bus::ITrainInformationCorbaDef_ptr,
                                  TA_Base_Bus::ITrainInformationCorbaDef_var> m_trainInfo;

        // Train Agent named object for TIS related operations
        TA_Base_Core::NamedObject<TA_Base_Bus::ITISTrainCommandsCorbaDef,
                                  TA_Base_Bus::ITISTrainCommandsCorbaDef_ptr,
                                  TA_Base_Bus::ITISTrainCommandsCorbaDef_var> m_trainTisOps;

        TA_Base_Core::NonReEntrantThreadLockable m_trainIdMapLock;
        TrainIdMap m_trainIdMap; // Contains a set of train IDs for each type of track. Initialised once only.
    };

}

#endif // !defined(AFX_TRAINAGENTACCESS_H__EFD31578_4599_4BF7_A382_81814D9262E8__INCLUDED_)
