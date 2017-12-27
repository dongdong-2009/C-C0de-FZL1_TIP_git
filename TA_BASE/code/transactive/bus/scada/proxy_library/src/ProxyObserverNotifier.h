/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/ProxyObserverNotifier.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// ProxyObserverNotifier.h: interface for the ProxyObserverNotifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROXYOBSERVERNOTIFIER_H__C449CAF3_E239_48D5_8D9C_AF52DCF2B6B3__INCLUDED_)
#define AFX_PROXYOBSERVERNOTIFIER_H__C449CAF3_E239_48D5_8D9C_AF52DCF2B6B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.h"

namespace TA_Base_Bus
{
    class ObserverUpdateQueueProcessor;
    class IEntityUpdateEventProcessor;
    
    class IProxyObserverNotifier
    {
    public:
        virtual ~IProxyObserverNotifier() {};
        virtual void addObserver(IEntityUpdateEventProcessor& observer) = 0;
        virtual void removeObserver(IEntityUpdateEventProcessor& observer) = 0;
        virtual void notifyObserver(IEntityUpdateEventProcessor& observer, ScadaEntityUpdateType updateType) = 0;
		virtual void notifyObservers(ScadaEntityUpdateType updateType) = 0;
        virtual bool isObserved() = 0;
    };
    
    class ProxyObserverNotifier : public IProxyObserverNotifier  
    {
    public:
        ProxyObserverNotifier();
        virtual ~ProxyObserverNotifier();
        
        // implement IProxyObserverNotifier functions
        virtual void addObserver(IEntityUpdateEventProcessor& observer);
        virtual void removeObserver(IEntityUpdateEventProcessor& observer);
		virtual void notifyObserver(IEntityUpdateEventProcessor& observer, ScadaEntityUpdateType updateType);
        virtual void notifyObservers(ScadaEntityUpdateType updateType);
        virtual bool isObserved();
        virtual unsigned long getEntityKey() const = 0;
        
    private:

        typedef std::map<IEntityUpdateEventProcessor*, unsigned long> ObserverMap;//<observer pointer, observer reference count>
        
        ObserverMap m_observers; 
        TA_Base_Core::ReEntrantThreadLockable		m_observerLock;
    };
}

#endif // !defined(AFX_PROXYOBSERVERNOTIFIER_H__C449CAF3_E239_48D5_8D9C_AF52DCF2B6B3__INCLUDED_)
