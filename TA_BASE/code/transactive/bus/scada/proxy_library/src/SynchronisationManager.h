/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/SynchronisationManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  SynchronisationManager.h
//  Implementation of the Class SynchronisationManager
//  Created on:      28-Jun-2004 09:39:01 AM
///////////////////////////////////////////////////////////

#if !defined(SynchronisationManager_A4FBA4B3_94CE_413a_93E0_E6CE99526536__INCLUDED_)
#define SynchronisationManager_A4FBA4B3_94CE_413a_93E0_E6CE99526536__INCLUDED_

/**
 * This class will manage the synchronisation for a series of data points and
 * nodes
 * @version 1.0
 * @created 28-Jun-2004 09:39:01 AM
 */

#include <string>
#include <map>
#include "boost/shared_ptr.hpp"

#include "bus/scada/common_library/src/ThreadsafeMap.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_Bus
{
    class AgentSynchronisationInterface;
    class DataNodeManager;
    class DataNodeProxyManager;
    class DataPointManager;
    class DataPointProxyManager;
	class IEntity;

    class SynchronisationManager
    {
    public:
        /**
          * Constructor
          * @param updateCallback
          * 
          */
        SynchronisationManager();
        
        virtual ~SynchronisationManager();
    
    public:
        
        /**
          * Add a data node to be synchronised
          * @param entityKey
          * @param location
          * @param agentName
          * 
          */
        void add(TA_Base_Core::IEntityDataPtr entityData);

        /**
          * Stop synchronisation of data point
          * @param entityKey
          * 
          */
        void remove( TA_Base_Core::IEntityDataPtr entityData );
        
        void stopSynchronisation();
        
        void startSynchronisation();

        void registerLocalEntities(std::string agentName, const std::map<unsigned long, IEntity*>* entityMap);
        
        // weimin: make this as public function
        bool isLocal(unsigned long entityKey);
        IEntity* getLocalEntity(unsigned long entityKey);

		void removeAllAgentInterfaces();
    private:

        std::string getActualAgentName(std::string entityType, std::string entityAgentName);

    protected:

		boost::shared_ptr<AgentSynchronisationInterface> addAgentInterface (TA_Base_Core::IEntityDataPtr entityData);

		virtual boost::shared_ptr<AgentSynchronisationInterface> createAgentInterface(TA_Base_Core::IEntityDataPtr entityData) = 0;
               
        

    protected:

		typedef ThreadsafeMap< std::string, AgentSynchronisationInterface > AgentList;

		const std::map<unsigned long, IEntity*>*	m_localEntityMap;

		TA_Base_Core::ReEntrantThreadLockable 		m_lock;
        bool										m_isSynchronising;
		AgentList									m_agentList;
        
    private:
        static const std::string DN_AGENT_NAME_POSTFIX;	
        static const std::string DP_AGENT_NAME_POSTFIX;	

    };
};
#endif // !defined(SynchronisationManager_A4FBA4B3_94CE_413a_93E0_E6CE99526536__INCLUDED_)
