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
// DataPointAgentProxy.h: interface for the DataPointAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DATAPOINT_AGENT_PROXY_H
#define DATAPOINT_AGENT_PROXY_H

#include <set>
#include <string>
#include <sys/timeb.h>

#include "app/scada/OpcBridgeLib/src/DisplayProxyManager.h"
#include "bus/scada/common_library/IDL/src/IDataPointAgentCorbaDef.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/synchronisation/src/Condition.h"
#include "core/naming/src/NamedObject.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_App
{
    class DisplayProxyManager;
	
	typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataPointAgentCorbaDef,
		TA_Base_Bus::IDataPointAgentCorbaDef_ptr,
		TA_Base_Bus::IDataPointAgentCorbaDef_var >   DataPointAgentInterface;

	typedef std::map<unsigned long, DataPointAttributes*> DpAttributeMap;
	typedef std::map<unsigned long, TA_Base_Bus::DataPointDisplayState> DpDisplayStateMap;

    class DataPointAgentProxy : public TA_Base_Core::Thread
        
    {
    public:
	    DataPointAgentProxy(unsigned long agentKey, unsigned long locationKey, DisplayProxyManager& proxyManager);

	    virtual ~DataPointAgentProxy();

        virtual void run();
        
        virtual void terminate();
        
        void setToBadNotConnectedQuality(ExceptionType exception);
              
        void startSynchronise();

        void stopSynchronise();

        bool isSynchronised();

		void dynamicSetDpAttribute(unsigned long key, DataPointAttributes* dpAttribute);

		void preSetDpAttribute(std::vector<DataPointAttributes*>& dpAttributes);

		void processUpdates(TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dpUpdates);

		void prepareAgentThreadEnd();

		unsigned long getLocationKey() { return m_locationKey; }
    private:
        volatile bool	m_toBeTerminated;

        bool	m_isSynchronising;

		bool	m_isPrevSynchronising;

        unsigned long	m_observerId;
       
        DataPointAgentInterface *	m_agentInterface;
        		
		DpAttributeMap		m_attributeMap;
		TA_Base_Core::ReEntrantThreadLockable		m_attributeMapLock;
		DpDisplayStateMap   m_displayCache;
		TA_Base_Core::ReEntrantThreadLockable		m_displayCacheLock;
		
		unsigned long	m_locationKey;
		unsigned long   m_agentKey;
        std::string		m_agentName;
        
        DisplayProxyManager& m_proxyManager;

        unsigned long m_pollPeriod; // in millisecond

        unsigned long m_retryPeriod; // in second

        bool m_connected;
		volatile bool	m_ObserverResetAndResume;

        time_t m_timeLastConnected;

		TA_Base_Core::Condition m_conditionLoop;
    };
}
#endif // DATAPOINT_AGENT_PROXY_H
