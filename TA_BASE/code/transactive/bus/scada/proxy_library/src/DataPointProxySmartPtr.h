/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataPointProxySmartPtr.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  DataPointProxySmartPtr.h
//  Implementation of the Class DataPointProxySmartPtr
//  Created on:      29-Jun-2004 03:04:37 PM
///////////////////////////////////////////////////////////

#if !defined(DataPointProxySmartPtr_2B19A768_EC3E_435e_812B_3F074684CD4F__INCLUDED_)
#define DataPointProxySmartPtr_2B19A768_EC3E_435e_812B_3F074684CD4F__INCLUDED_

#include "boost/shared_ptr.hpp"
/**
 * Data Point Proxy smart pointer implementation.  It will ensure that the
 * protected IDataPointProxy pointer reference counting is maintained if copied
 * and that it is deleted when no longer required.
 * @version 1.0
 * @created 29-Jun-2004 03:04:37 PM
 */
namespace TA_Base_Bus
{
    class DataPointProxy;
    class IDataPointProxy;
    class IProxySmartPtrCallback;
	class IEntityUpdateEventProcessor;

	class DataPointProxySmartPtr
	{
		public:
			DataPointProxySmartPtr();

			virtual ~DataPointProxySmartPtr();




			// Pointer operator
            boost::shared_ptr<IDataPointProxy> operator->();

            /**
             * In case the pointer is held as a local member variable
             * but the owner doesnt want it any more, it can be reset
             * as if its not been initialised.
             * (just like a boost shared pointer)
             */
            void reset();

			void setProxyInfo(boost::shared_ptr<DataPointProxy> proxy, IEntityUpdateEventProcessor& observer, IProxySmartPtrCallback&  callback);

		private:
            DataPointProxySmartPtr(boost::shared_ptr<DataPointProxy> proxy, IEntityUpdateEventProcessor& observer, IProxySmartPtrCallback&  callback);

			DataPointProxySmartPtr& operator=(const DataPointProxySmartPtr& rhs);
			
			// Copy constructor
			DataPointProxySmartPtr(const DataPointProxySmartPtr& rhs);

            boost::shared_ptr<DataPointProxy>	m_proxy;
			IEntityUpdateEventProcessor*		m_observer;
			IProxySmartPtrCallback*	m_callback;

	};
};

#endif // !defined(DataPointProxySmartPtr_2B19A768_EC3E_435e_812B_3F074684CD4F__INCLUDED_)

