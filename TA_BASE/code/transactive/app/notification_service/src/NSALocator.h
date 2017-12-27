#ifndef NSALOCATOR_H
#define NSALOCATOR_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/NSALocator.h $
 * @author:  Lachlan Partington
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/12/17 15:24:44 $
 * Last modified by:  $Author: shurui.liu $
 * 
 */

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/system_controller/src/ManagedProcessAccessFactory.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <map>
#include <string>
#include <vector>


namespace TA_Base_App
{
    class NSALocator
    {
    public:

        NSALocator();
        virtual ~NSALocator();

        struct NSADetail
        {
            NSADetail(unsigned long locationKey, const std::string& hostname, unsigned short portnum, bool isBroadcast)
                : m_locationKey(locationKey),
                  m_hostname(hostname),
                  m_portnum(portnum),
                  m_isBroadcast(isBroadcast)
            {
            }

            // The 'virtual' location that this NSA is running at.
            unsigned long m_locationKey;

            // The 'serviceAddr' of this NSA is formed from "m_hostname:m_port".
            std::string m_hostname;
            unsigned short m_portnum;

            // True if the Broadcast parameter value has been set for this NSA.
            // TODO LPP: This parameter actually indicated that the NSA was centralised and
            // didn't participate in federation.  This approach is now obsolete, so it is
            // not necessary to provide broadcast NSAs now.
            bool m_isBroadcast;

            bool operator==(const NSADetail& other)
            {
                return
                    m_locationKey == other.m_locationKey &&
                    m_hostname == other.m_hostname &&
                    m_portnum == other.m_portnum &&
                    m_isBroadcast == other.m_isBroadcast;
            }
        };

        // Appends the (locationKey, hostname, port) of all NSAs at the specified location
        // to nsaDetails.  If locationKey == ANY_LOCATION, all NSAs at ALL locations
        // (including the local location) will be returned.
        // The constant LOCAL_LOCATION is not allowed.
        typedef std::vector<NSADetail> NSADetailVec;

        void findNSAsAtLocation(unsigned long locationKey, NSADetailVec& nsaDetails) const;

    private:

        void initNSALocator();

        // Populates 'entities' with all the "NotificationServiceAgent" entities running at the
        // given location, including broadcast notification services.
        // If locationKey == ANY_LOCATION, all NSA entities will be returned, including those at
        // the local location, otherwise only those NSA entities at the specific location will
        // be returned.
        // The caller is responsible for deleting the IEntityData members in 'entities'.
        void getNSAEntitiesAtLocation(unsigned long locationKey, TA_Base_Core::IEntityDataList& entities) const;

        // Returns the hostnames for all of the hosts that an NSA parent entity (which is a
        // MonitoredProcess) is running on.
        void getHostnamesForNSAParent(const unsigned long parentKey, const std::string& parentName, std::vector<std::string>& hostnames) const;

        /**
          * cleanup
          *
          * This method handles the clean up that occurs after destruct this class
          */
        void cleanup();


        // Cache the service addresses of remote services previously located by findNSAsAtLocation()
        // location key -> vector of service addresses of notification services at that location
        typedef std::map< unsigned long, NSADetailVec > LocationToNSADetailMap;
        mutable LocationToNSADetailMap m_locationToNSADetailMap;
        mutable TA_Base_Core::NonReEntrantThreadLockable m_remoteNSAsLock;  // Guard m_remoteNSAs

        // Cache the entitys previously located by findNSAsAtLocation()
        typedef std::map< unsigned long, TA_Base_Core::IEntityDataList> LocationToEntityMap;
        mutable LocationToEntityMap m_locationToEntityMap;

        // Cache the hostnames previously located by getHostnamesForNSAParent
        mutable TA_Base_Core::ProcessToHostnameMap m_processToHostnameMap;
    };

    // Allows toString() to log the vector of NSADetails, in the form (locationKey, hostname:portnum, isBroadcast).
    inline std::ostream& operator<<(std::ostream& stm, const NSALocator::NSADetail& val)
    {
        stm << "("
            << val.m_locationKey << ", "
            << TA_Base_Core::gMakeServiceAddr(val.m_hostname, val.m_portnum) << ", "
            << val.m_isBroadcast
            << ")";

        return stm;
    }

}
#endif // NSALOCATOR_H
