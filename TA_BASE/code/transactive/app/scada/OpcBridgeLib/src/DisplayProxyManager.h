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
// DisplayProxyManager.h: interface for the DisplayProxyManager class.
//
//////////////////////////////////////////////////////////////////////
#ifndef DISPLAY_PROXY_MANAGER_H
#define DISPLAY_PROXY_MANAGER_H

#include <set>
#include <map>
#include <string>
#include <sys/timeb.h>

#include "app/scada/OpcBridgeLib/src/DataNodeAttributes.h"
#include "app/scada/OpcBridgeLib/src/DataPointAttributes.h"
#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
    class DataPointAgentProxy;
    class DataNodeAgentProxy;
    class AbstractDataObjectAttributes;
	class TagThread;
    
    
    typedef enum
    {
        CORBA_EXCEPTION = 0,
        OBJECT_RESOLUTION_EXCEPTION = 1,
        BAD_PARAMETER_EXCEPTION = 2,
        OPERATION_MODE_EXCEPTION = 3,
        SCADA_PROXY_EXCEPTION = 4,
        UNKNOWN_EXCEPTION = 5
    } ExceptionType;

    class DisplayProxyManager 
    {
    public:
        static DisplayProxyManager* getInstance();

        static void removeInstance();
  		
		void preSetDnAttributesForAgents(std::map<unsigned long, std::vector<DataNodeAttributes*> >& dnAgents);

		void preSetDpAttributesForAgents(std::map<unsigned long, std::vector<DataPointAttributes*> >& dpAgents);

		void dynamicSetDnAttribute(DataNodeAttributes* dnAttribute);

		void dynamicSetDpAttribute(DataPointAttributes* dpAttribute);

        void startSynchronise();

        void stopSynchronise();
            
		void queueItemUnique( unsigned long itemId, AbstractDataObjectAttributes * item);

		unsigned int getUpdateCnt();

		void initTagThreads();
    private:
        
        DisplayProxyManager();
        virtual ~DisplayProxyManager();
        static DisplayProxyManager* m_instance;
        static TA_Base_Core::ReEntrantThreadLockable    m_singletonLock;
        bool m_isSynchronizing;
        
		typedef std::map<unsigned long, DataPointAgentProxy*> DpAgentMap;
        DpAgentMap								m_dpAgentMap;
        TA_Base_Core::ReEntrantThreadLockable	m_dpAgentMapLock;

		typedef std::map<unsigned long, DataNodeAgentProxy*> DnAgentMap;
        DnAgentMap								m_dnAgentMap;
        TA_Base_Core::ReEntrantThreadLockable	m_dnAgentMapLock;

		std::vector< TagThread* >						m_workers;
		unsigned int 									m_numWorkers;
		bool											m_tagWokerInited;
		TA_Base_Core::ReEntrantThreadLockable			m_tagWokerLock;
    };
}

#endif // !defined(DISPLAY_PROXY_MANAGER_H)
