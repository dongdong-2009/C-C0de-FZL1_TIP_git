/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_controller/src/ShutdownExecutor.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Implementation of the IShutdownCorbaDef
  * interface, used by the System Manager.
  *
  */

#ifndef INCLUDE_SHUTDOWN_EXECUTOR_H
#define INCLUDE_SHUTDOWN_EXECUTOR_H

#include "core/process_management/IDL/src/IShutdownCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include <map>


namespace TA_Base_App
{
    class ShutdownExecutor : public virtual POA_TA_Base_Core::IShutdownCorbaDef,
                             public virtual TA_Base_Core::ServantBase
    {
    public:

        ShutdownExecutor();
        ~ShutdownExecutor();

        void shutdown(::CORBA::Boolean notifyPeer);

    private:

        ShutdownExecutor(const ShutdownExecutor&);
        ShutdownExecutor& operator =(const ShutdownExecutor&);

        void notifyPeerToShutdown();
        void shutdownCentralDatabaseServer();
        void shutdownServer();

        std::map<std::string, TA_Base_Core::IShutdownCorbaDef_var> resolvePeerShutdownExecutor();
        void executeSystemCommand( const std::string& commandline );
        bool isNeedShutdownCentralDatabaseServer();

    private:

        unsigned int m_shutdownRetryTimes;
    };
}


#endif
