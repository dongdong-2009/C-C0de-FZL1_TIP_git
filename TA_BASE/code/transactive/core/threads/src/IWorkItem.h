/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/core/threads/src/IWorkItem.h $
  * @author:	HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  * IWorkItem interface provide callback mechanism for execution of asynchronous operation.
  * Client that wishes to use the thread pool asynchronous execution must implement this
  * interface and then register itself to the ThreadPoolSingletonManager when queuing a work
  * item for execution.
  *
  */

#ifndef IWORK_ITEM_H
#define IWORK_ITEM_H

#include "boost/shared_ptr.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace TA_Base_Core
{
	class IWorkItem
	{
	public:

		virtual ~IWorkItem() {};

		/**
		 * executeWorkItem
		 *
		* This method is used as a callback mechnism by the ThreadPoolSingleManager to notify
		* the user that it is the time to execute its asynchronous operation.
		 *
		 */
		virtual void executeWorkItem() = 0;
	};

    typedef boost::shared_ptr<IWorkItem> IWorkItemPtr;

    struct SimpleWorkItem : TA_Base_Core::IWorkItem
    {
        boost::function<void()> m_function;
        SimpleWorkItem(boost::function<void()> fun) : m_function(fun) {}
        virtual void executeWorkItem() { m_function(); }
    };
}

#endif
