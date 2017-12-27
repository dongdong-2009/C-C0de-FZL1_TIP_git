/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeProxySmartPtr.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  DataNodeProxySmartPtr.h
//  Implementation of the Class DataNodeProxySmartPtr
//  Created on:      29-Jun-2004 03:04:37 PM
///////////////////////////////////////////////////////////

#if !defined(DataNodeProxySmartPtr_2B19A768_EC3E_435e_812B_3F074684CD4F__INCLUDED_)
#define DataNodeProxySmartPtr_2B19A768_EC3E_435e_812B_3F074684CD4F__INCLUDED_

#include "boost/shared_ptr.hpp"

/**
 * Data Point Proxy smart pointer implementation.  It will ensure that the
 * protected IDataNodeProxy pointer reference counting is maintained if copied
 * and that it is deleted when no longer required.
 * @version 1.0
 * @created 29-Jun-2004 03:04:37 PM
 */
namespace TA_Base_Bus
{
    class DataNodeProxy;
    class IDataNodeProxy;
    class IProxySmartPtrCallback;
    class IEntityUpdateEventProcessor;

    class DataNodeProxySmartPtr
    {
        public:
            DataNodeProxySmartPtr();

            virtual ~DataNodeProxySmartPtr();


            // Pointer operator
            boost::shared_ptr<IDataNodeProxy> operator->();

			//xinsong++
			void setProxyInfo(boost::shared_ptr<DataNodeProxy> proxy, IEntityUpdateEventProcessor& observer, IProxySmartPtrCallback&  callback);

        private:
            DataNodeProxySmartPtr(boost::shared_ptr<DataNodeProxy> proxy, IEntityUpdateEventProcessor& observer, IProxySmartPtrCallback&  callback);

            DataNodeProxySmartPtr& operator=(const DataNodeProxySmartPtr& rhs);
			
            // Copy constructor
            DataNodeProxySmartPtr(const DataNodeProxySmartPtr& rhs);

            boost::shared_ptr<DataNodeProxy>	m_proxy;
            IEntityUpdateEventProcessor*		m_observer;
            IProxySmartPtrCallback*		m_callback;

    };
};
#endif // !defined(DataNodeProxySmartPtr_2B19A768_EC3E_435e_812B_3F074684CD4F__INCLUDED_)
