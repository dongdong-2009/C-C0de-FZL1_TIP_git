/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeProxySmartPtr.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  DataNodeProxySmartPtr.cpp
//  Implementation of the Class DataNodeProxySmartPtr
//  Created on:      29-Jun-2004 03:04:38 PM
///////////////////////////////////////////////////////////

#include <string>

#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IProxySmartPtrCallback.h"
#include "bus/scada/proxy_library/src/DataNodeProxy.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
    DataNodeProxySmartPtr::DataNodeProxySmartPtr( boost::shared_ptr<DataNodeProxy> proxy, 
											      IEntityUpdateEventProcessor& observer, 
											      IProxySmartPtrCallback& callback )
    :
    m_proxy(proxy),
    m_observer(&observer),
    m_callback(&callback)
    {
        if( NULL != m_callback)
	    {
		    // Let the observer manager know so it can check if the constructed proxy 
		    // is actually a copy - this allows a ConfigAvailable update to be sent if 
		    // required
		    m_callback->onCreateProxy(m_proxy.get(),*m_observer);
	    }
    }


    DataNodeProxySmartPtr::DataNodeProxySmartPtr()
    :
    m_proxy((DataNodeProxy*)NULL),
    m_observer(NULL),
    m_callback(NULL)
    {
    }


    DataNodeProxySmartPtr::DataNodeProxySmartPtr(const DataNodeProxySmartPtr& rhs)
	:
    m_proxy(rhs.m_proxy),
	m_observer(rhs.m_observer),
    m_callback(rhs.m_callback)
    {
	    if( NULL != m_callback )
	    {
		    // Let factory know about new observer
		    m_callback->onCopyProxy(m_proxy.get(),*m_observer);
	    }
    }


    DataNodeProxySmartPtr& DataNodeProxySmartPtr::operator=(const DataNodeProxySmartPtr& rhs)
    {
	    if ( NULL != m_proxy.get() )
	    {
		    m_callback->onDeleteProxy(m_proxy.get(),*m_observer);
	    }

	    m_proxy = rhs.m_proxy;
	    m_observer = rhs.m_observer;
        m_callback = rhs.m_callback;

	    if( NULL != m_callback )
	    {
		    // Let factory know about new observer
		    m_callback->onCopyProxy(m_proxy.get(),*m_observer);
	    }
	    return *this;	
    }


    DataNodeProxySmartPtr::~DataNodeProxySmartPtr()
    {
	    // Tell proxy factory that we have finished with this pointer
	    if( NULL != m_proxy.get() )
	    {
		    m_callback->onDeleteProxy(m_proxy.get(),*m_observer);
	    }
    }


    boost::shared_ptr<IDataNodeProxy> DataNodeProxySmartPtr::operator->()
    {
	    if ( NULL != m_proxy.get() )
	    {
		    return m_proxy;
	    }
	    TA_THROW(TA_Base_Core::ScadaProxyException("Smart Pointer not initialised"));
    }

	//xinsong++
	void DataNodeProxySmartPtr::setProxyInfo(boost::shared_ptr<DataNodeProxy> proxy, IEntityUpdateEventProcessor& observer, IProxySmartPtrCallback&  callback)
    {
		if( NULL != m_proxy.get() )
		{
			TA_ASSERT(m_observer != NULL, "observer should not be NULL");
			m_callback->onDeleteProxy(m_proxy.get(),*m_observer);
		}

		m_proxy = proxy;
		m_observer = &observer;
		m_callback = &callback;
        if( NULL != m_callback)
		{
			// Let the observer manager know so it can check if the constructed proxy 
			// is actually a copy - this allows a ConfigAvailable update to be sent if 
			// required
			m_callback->onCreateProxy(m_proxy.get(),*m_observer);
		}
    }
}
