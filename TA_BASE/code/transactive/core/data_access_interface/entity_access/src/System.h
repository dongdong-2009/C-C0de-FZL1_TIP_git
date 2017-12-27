/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/System.h $
  * @author Rob Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * System is a concrete implementation of ISystem, which is in turn an implementation of
  * IEntityData. It proves read-only access to System entity data, and a specific framework
  * for System entity parameters.
  *
  * 
  */

#if !defined(AFX_SYSTEM_H__1A7C3B36_4B08_496A_AAA4_5BF0B3F8B7D4__INCLUDED_)
#define AFX_SYSTEM_H__1A7C3B36_4B08_496A_AAA4_5BF0B3F8B7D4__INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/ISystem.h"


namespace TA_Base_Core
{
    class System : public ISystem, public EntityData
    {

    public:
        /**
         * Camera (constructor)
         *
         * Constructs a new Camera object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        System();

        /**
         * Camera (constructor)
         *
         * Constructs a new Camera object with the specified key.
         *
         * @param key The key to this Camera in the database.
         */
        System(unsigned long key);
        virtual ~System();

    public:
    
        /**
         * clone
         *
         * Returns a pointer to a new System object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the Camera object.
         */
        static IEntityData* clone(unsigned long key);

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

         std::string getServices();

         std::string getDebugLogDir();

         std::string getMonitorServerList();

         std::string getMonitorSlaveList();

         std::string getFailover();

         std::string getPreStartActiveProg();

         std::string getPreStopActiveProg();

         std::string getPostStopActiveProg();

         std::string getPreShutdownProg();

         std::string getPostShutdownProg();

         std::string getPreFailoverProg();

         std::string getFailoverProg();

         std::string getNonFailoverProg();

         std::string getPostFailoverProg();

         std::string getDBFailoverProg();

         std::string getPostDBFailoverProg();

         std::string getDBCleanupProg();

         std::string getPostDBCleanupProg();

         std::string getCleanupProg();

         std::string getPostCleanupProg();

         std::string getSecondaryFailProg();

         std::string getPostSecondaryFailProg();

         std::string getDBStatusProg();

         std::string getDBStatusFailProg();

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
		static std::string getStaticType();

    private:
        System( const System& theSystem) {};            
		System& operator=(const System &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string SERVICES;
        static const std::string DEBUGLOGDIR;
        static const std::string MONITORSERVERLIST;
        static const std::string MONITORSLAVELIST;
        static const std::string FAILOVER;
        static const std::string PRESTARTACTIVEPROG;
        static const std::string PRESTOPACTIVEPROG;
        static const std::string POSTSTOPACTIVEPROG;
        static const std::string PRESHUTDOWNPROG;
        static const std::string POSTSHUTDOWNPROG;
        static const std::string PREFAILOVERPROG;
        static const std::string FAILOVERPROG;
        static const std::string NONFAILOVERPROG;
        static const std::string POSTFAILOVERPROG;
        static const std::string DBFAILOVERPROG;
        static const std::string POSTDBFAILOVERPROG;
        static const std::string DBCLEANUPPROG;
        static const std::string POSTDBCLEANUPPROG;
        static const std::string CLEANUPPROG;
        static const std::string POSTCLEANUPPROG;
        static const std::string SECONDARYFAILPROG;
        static const std::string POSTSECONDARYFAILPROG;
        static const std::string DBSTATUSPROG;
        static const std::string DBSTATUSFAILPROG;
    };
    
    typedef boost::shared_ptr<System> SystemPtr;
} //close namespace TA_Base_Core

#endif // !defined(AFX_SYSTEM_H__1A7C3B36_4B08_496A_AAA4_5BF0B3F8B7D4__INCLUDED_)
