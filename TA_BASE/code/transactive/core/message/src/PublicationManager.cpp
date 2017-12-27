/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/PublicationManager.cpp $
  * @author:  Jade Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is the base publication Managing class. It is designed to provide some
  * logical separation between Message Publication and Alarm Publication.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/PublicationManager.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_Core
{
    // The IChannelObserver object is also the IQueueHandler<T> used by the StructuredEventSupplier,
    // so it is not necessary to delete it separately.  It is convenient to store the IChannelObserver*
    // so that we can detach the handler from the ChannelLocatorConnectionMgr.

    // channelName -> (StructuredEventSupplier*, IChannelObserver*)
    typedef std::map<std::string, std::pair<StructuredEventSupplier*, IChannelObserver*> > ChannelToSupplierMap;
    static ChannelToSupplierMap s_channelToSupplierMap;
    static TA_Base_Core::NonReEntrantThreadLockable s_lock;

    void gTerminateAndDeleteStructuredEventSuppliers()
    {
        LOG1( SourceInfo, DebugUtil::DebugDebug,
            "gTerminateAndDeleteStructuredEventSuppliers(): Destroying %d StructuredEventSuppliers",
            s_channelToSupplierMap.size() );

        TA_THREADGUARD( s_lock );

        ChannelToSupplierMap::iterator iter = s_channelToSupplierMap.begin();
        for( ; iter != s_channelToSupplierMap.end(); ++iter )
        {
            ChannelLocatorConnectionMgr::getInstance().detach(iter->second.second);
            delete iter->second.first;
        }

        s_channelToSupplierMap.clear();
    }

    StructuredEventSupplier* gGetStructuredEventSupplierForChannel( const std::string& channelName )
    {
        // Synchronise this for multi-threaded use
        TA_THREADGUARD( s_lock );

        // Do we already have a supplier for this channel?
        ChannelToSupplierMap::iterator iter = s_channelToSupplierMap.find( channelName );
        if ( iter != s_channelToSupplierMap.end() )
        {
            // Yes. Return existing supplier
            return iter->second.first;
        }
        else
        {
            // No. Need to assemble a new one, and connect it to the ChannelLocatorConnectionMgr.
            LOG0( SourceInfo, DebugUtil::DebugDebug,
                "gGetStructuredEventSupplierForChannel(): Assembling new StructuredEventSupplier" );

            IAdminProvider* adminProvider = new DefaultAdminProvider;
            SESQueueHandler* handler = new SESQueueHandler(channelName, gGetLocalLocationKey(), /*takes*/ adminProvider, NULL);
            StructuredEventSupplier* supplier = new StructuredEventSupplier(/*takes*/ handler);

            ChannelLocatorConnectionMgr::getInstance().attach(handler);

            // We need to put the handler (which ISA IChannelObserver) into the map so
            // that we can later detach it from the ChannelLocatorConnectionMgr.
            s_channelToSupplierMap.insert( ChannelToSupplierMap::value_type(channelName, std::make_pair(supplier, handler)) );

            return supplier;
        }
    }

}; // TA_Base_Core

