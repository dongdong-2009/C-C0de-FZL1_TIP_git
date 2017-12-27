/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/ProxyObserverNotifier.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// ProxyObserverNotifier.cpp: implementation of the ProxyObserverNotifier class.
//
//////////////////////////////////////////////////////////////////////

#include "bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ProxyObserverNotifier.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////
    
    ProxyObserverNotifier::ProxyObserverNotifier()
    {
        m_observers.clear();
    }
    
    ProxyObserverNotifier::~ProxyObserverNotifier()
    {
        
    }
    
    //weimin, move observer list from observer manager to proxy
    void ProxyObserverNotifier::addObserver(IEntityUpdateEventProcessor& observer)
    {
        TA_THREADGUARD(m_observerLock);
        
        if (m_observers.find(&observer) == m_observers.end())
        {
            //not found, create new item
            m_observers[&observer] = 1;
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "proxy observer [%u] is added into observer map of entity [%u]", &observer, getEntityKey());
        }
        else
        {
            //found, reference count + 1
            m_observers[&observer] = m_observers[&observer] + 1;
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "proxy observer [%u] is added into observer map of entity [%u] for [%u] times", &observer, getEntityKey(), m_observers[&observer]);
        }
    }
    
    void ProxyObserverNotifier::removeObserver(IEntityUpdateEventProcessor& observer)
    {
        TA_THREADGUARD(m_observerLock);
        
        if (m_observers.find(&observer) == m_observers.end())
        {
            //not found, do nothing
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "proxy observer [%u] is already removed from observer map of entity [%u]", &observer, getEntityKey());
            TA_ASSERT(false, "observer should not be removed more than register times");

            return;
        }
        else
        {
            //found, reference count - 1
            m_observers[&observer] = m_observers[&observer] - 1;
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "proxy observer [%u] reference count is decreased to [%u] of entity [%u]", &observer, m_observers[&observer], getEntityKey());

            if (m_observers[&observer] < 1)
            {
                //if reference count is 0, remove this item
                m_observers.erase(&observer);
                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "proxy observer [%u] reference count is decreased to 0, observer item is removed from map of entity [%u]", &observer, getEntityKey());
            }
        }
    }
    
	void ProxyObserverNotifier::notifyObserver(IEntityUpdateEventProcessor& observer, ScadaEntityUpdateType updateType)
	{
		TA_THREADGUARD(m_observerLock);
		if (m_observers.end() == m_observers.find(&observer))
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "proxy observer [%u] is not find in observer map of entity [%u]", &observer, getEntityKey());
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Notify proxy observer [%u] of entity [%u] with update [%d]", &observer, getEntityKey(), updateType);
			observer.processEntityUpdateEvent(getEntityKey(), updateType);
		}
	}

    void ProxyObserverNotifier::notifyObservers(ScadaEntityUpdateType updateType)
    {
        TA_THREADGUARD(m_observerLock);
        
        for (ObserverMap::iterator itObserver = m_observers.begin(); itObserver != m_observers.end(); ++ itObserver)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Notify proxy observer [%u] of entity [%u] with update [%d]", itObserver->first, getEntityKey(), updateType);
            itObserver->first->processEntityUpdateEvent(getEntityKey(), updateType);
        }
    }

    bool ProxyObserverNotifier::isObserved()
    {
        TA_THREADGUARD(m_observerLock);

        if (m_observers.size() > 0)
        {
            return true;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "entity [%u] is not observed any more", getEntityKey());
        return false;
    }    
}
