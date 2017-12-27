/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// DataNodeAgentProxy.h: interface for the DataNodeAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DATANODE_AGENT_PROXY_H
#define DATANODE_AGENT_PROXY_H

#include <set>
#include <string>
#include <sys/timeb.h>

#include "app/scada/OpcBridgeLib/src/DataNodeAttributes.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/naming/src/NamedObject.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "bus/scada/common_library/IDL/src/IDataNodeAgentCorbaDef.h"

namespace TA_Base_App
{
    class DisplayProxyManager;
    class DisplayDataNodeProxy;
    
	typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataNodeAgentCorbaDef,
		TA_Base_Bus::IDataNodeAgentCorbaDef_ptr,
		TA_Base_Bus::IDataNodeAgentCorbaDef_var >   DataNodeAgentInterface;
	typedef std::map<unsigned long, DataNodeAttributes*> DnAttributeMap;
	typedef std::map<unsigned long, TA_Base_Bus::DataNodeDisplayState> DnDisplayStateMap;

    class DataNodeAgentProxy : public TA_Base_Core::Thread
        
    {
    public:
	    DataNodeAgentProxy(unsigned long agentKey, unsigned long locationKey, DisplayProxyManager& proxyManager);

	    virtual ~DataNodeAgentProxy();

        virtual void run();
        
        virtual void terminate();
        
        void setToBadNotConnectedQuality(ExceptionType exception);

        void startSynchronise();

        void stopSynchronise();

        bool isSynchronised();

		void dynamicSetDnAttribute(unsigned long key, DataNodeAttributes* dnAttribute);
		
		void preSetDnAttribute(std::vector<DataNodeAttributes*>& dnAttributes);

		void processUpdates(TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dnUpdates);

		void prepareAgentThreadEnd();

		unsigned long getLocationKey() { return m_locationkey; }
    private:
        volatile bool	m_toBeTerminated;

        bool			m_isSynchronising;

		bool			m_isPrevSynchronising;

        unsigned long	m_observerId;

        DataNodeAgentInterface * m_agentInterface;
               
		DnAttributeMap							m_attributeMap;
		TA_Base_Core::ReEntrantThreadLockable	m_attributeMapLock;

		DnDisplayStateMap						m_displayCache;
		TA_Base_Core::ReEntrantThreadLockable	m_displayCacheLock;
		
		unsigned long   m_locationkey;
		unsigned long   m_agentKey;
        std::string		m_agentName;
        
        DisplayProxyManager& m_proxyManager;

        unsigned long	m_pollPeriod; // in millisecond

        unsigned long	m_retryPeriod; // in second

        bool			m_connected;
		volatile bool	m_ObserverResetAndResume;

        time_t			m_timeLastConnected;

		TA_Base_Core::Condition m_conditionLoop;
        
    };
}
#endif // DATANODE_AGENT_PROXY_H
