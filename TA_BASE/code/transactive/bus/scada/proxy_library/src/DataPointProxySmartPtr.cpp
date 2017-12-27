/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointProxySmartPtr.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  DataPointProxySmartPtr.cpp
//  Implementation of the Class DataPointProxySmartPtr
//  Created on:      29-Jun-2004 03:04:38 PM
///////////////////////////////////////////////////////////

#include <string>

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IProxySmartPtrCallback.h"
#include "bus/scada/proxy_library/src/DataPointProxy.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
    DataPointProxySmartPtr::DataPointProxySmartPtr( boost::shared_ptr<DataPointProxy> proxy, 
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


    DataPointProxySmartPtr::DataPointProxySmartPtr()
    :
    m_proxy((DataPointProxy*)NULL),
    m_observer(NULL),
    m_callback(NULL)
    {
    }


    DataPointProxySmartPtr::DataPointProxySmartPtr(const DataPointProxySmartPtr& rhs)
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


    DataPointProxySmartPtr& DataPointProxySmartPtr::operator=(const DataPointProxySmartPtr& rhs)
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


    DataPointProxySmartPtr::~DataPointProxySmartPtr()
    {
        reset();
    }


    boost::shared_ptr<IDataPointProxy> DataPointProxySmartPtr::operator->()
    {
	    if ( NULL != m_proxy.get() )
	    {
		    return m_proxy;
	    }
	    TA_THROW(TA_Base_Core::ScadaProxyException("Smart Pointer not initialised"));
    }

    
    void DataPointProxySmartPtr::reset()
    {
        // Tell proxy factory that we have finished with this pointer
	    if( NULL != m_proxy.get() )
	    {
		    m_callback->onDeleteProxy(m_proxy.get(),*m_observer);
	    }

        m_proxy.reset();

        m_observer = NULL;
        m_callback = NULL;
    }

	//xinsong++
	void DataPointProxySmartPtr::setProxyInfo(boost::shared_ptr<DataPointProxy> proxy, IEntityUpdateEventProcessor& observer, IProxySmartPtrCallback&  callback)
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
