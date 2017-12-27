/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/ManagedProcessAccessFactory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * ManagedProcessAccessFactory is used to retrieve
  * IManagedProcessData objects from the database.
  *
  */

#ifndef MANAGEDPROCESSACCESSFACTORY_H
#define MANAGEDPROCESSACCESSFACTORY_H

#include "IManagedProcessData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include <string>
#include <vector>

namespace TA_Base_Core
{
    typedef std::map< unsigned long, std::vector<std::string> > ProcessToHostnameMap;

    class ManagedProcessAccessFactory
    {
    public:
        virtual ~ManagedProcessAccessFactory() {};

        /**
          * getInstance
          *
          * Creates and returns an instance of this object.
          *
          * @return A reference to an instance of an
          *         ManagedProcessAccessFactory object.
          */
        static ManagedProcessAccessFactory& getInstance();

        /**
          * getManagedProcesses
          *
          * Returns the managed processes that need to be started by
          * the System Controller running on the given hostname.
          *
          * @return A vector of objects conforming to the IManagedProcessData
          *         interface that indicate which processes must be
          *         started by the System Controller.
          *
          * @param hostname  The hostname of the System Controller.
          *
          * @exception DataException      Data was not found, or is not unique.
          * @exception DatabaseException  Problem communicating with the database.
          */
        std::vector<IManagedProcessData*> getManagedProcesses(std::string hostname);

        /**
          * getPeersOfSystemController
          *
          * Returns the peers of the System Controller with the given
          * hostname.
          *
          * @return A vector of strings containing the peers of the System
          *         Controller.
          *
          * @param hostname  The hostname of the System Controller.
          *
          * @exception DataException      Data was not found, or is not unique.
          * @exception DatabaseException  Problem communicating with the database.
          */
        std::vector<std::string> getPeersOfSystemController(std::string hostname);

        /**
          * getPeersOfManagedProcess
          *
          * Returns the hostnames of the System Controllers that will be
          * running a peer for the given managed process.
          *
          * @return A vector of strings containing the hostnames of the
          *         System Controllers running a peer.
          *
          * @param entityName  The entity name of the managed process.
          *
          * @exception DataException      Data was not found, or is not unique.
          * @exception DatabaseException  Problem communicating with the database.
          */

        std::vector<std::string> getPeersOfManagedProcessBySql(IDatabase* databaseConnection, const SQLStatement& sql);

        std::vector<std::string> getPeersOfManagedProcess(const std::string& entityName);

        std::vector<std::string> getPeersOfManagedProcess(const unsigned long entityKey);

        void getAllPeersOfManagedProcess(ProcessToHostnameMap& processToHostnameMap);

        std::map<unsigned long, std::string> getLocation2NotifyHostsMap();

    private:

        ManagedProcessAccessFactory() {};
        ManagedProcessAccessFactory(const ManagedProcessAccessFactory&);
        ManagedProcessAccessFactory& operator=(const ManagedProcessAccessFactory &);

        static ManagedProcessAccessFactory* m_instance;
    };
} // namespace TA_Base_Core

#endif // MANAGEDPROCESSACCESSFACTORY_H
