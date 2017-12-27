/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/signs/tis_agent_access/src/TISAgentAccessFactory.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the TIS agent and keep a reference
  * to it.
  */

#if !defined(TIS_AGENT_ACCESS_FACTORY_H)
#define TIS_AGENT_ACCESS_FACTORY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4503 )

#include "bus/signs/TisManagerCorbaDef/src/ITTISManagerCorbaDef.h"
#include "bus/signs/TisManagerCorbaDef/src/ISTISManagerCorbaDef.h"
#include <map>
#include <string>
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

namespace TA_Base_Bus
{
    class TISAgentAccessFactory
    {
        
    public:

        typedef TA_Base_Core::NamedObject<TA_Base_Bus::ITTISManagerCorbaDef,
                                     TA_Base_Bus::ITTISManagerCorbaDef_ptr,
                                     TA_Base_Bus::ITTISManagerCorbaDef_var> TtisNamedObj;

        typedef TA_Base_Core::NamedObject<TA_Base_Bus::ISTISManagerCorbaDef,
                                     TA_Base_Bus::ISTISManagerCorbaDef_ptr,
                                     TA_Base_Bus::ISTISManagerCorbaDef_var> StisNamedObj;

        /**
         * ~TISAgentAccessFactory
         *
         * Standard destructor.
         */

        virtual ~TISAgentAccessFactory();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      TISAgentAccessFactory&
         *              A reference to an instance of a TISAgentAccessFactory object.
         *
         */
		static TISAgentAccessFactory& getInstance();

        /** 
          * getTTISAgent
          *
          * Get a reference to the TTIS Interface held by the TIS Agent.
          * (for this location)
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          *
          * @return a reference to the TTIS Interface
          */
        TtisNamedObj& getTTISAgent();


        /** 
          * getSTISAgent
          *
          * Get a reference to the STIS Interface held by the TIS Agent.
          * (for this location)
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          *
          * @return a reference to the TTIS Interface
          */
        StisNamedObj& getSTISAgent();

        /** 
          * getOccTTISAgent
          *
          * Get a reference to the TTIS Interface held by the TIS Agent.
          * (for the OCC)
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          *
          * @return a reference to the TTIS Interface
          */
        TtisNamedObj& getOccTTISAgent();


        /** 
          * getOccSTISAgent
          *
          * Get a reference to the STIS Interface held by the TIS Agent.
          * (for the OCC)
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          *
          * @return a reference to the TTIS Interface
          */
        StisNamedObj& getOccSTISAgent();


        /** 
          * getSTISAgentAtLocation
          *
          * Get a reference to the STIS Interface held by the TIS Agent.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          *
          * @param location The location name e.g. DBG
          * @return a reference to the TTIS Interface
          */
        StisNamedObj& getSTISAgentAtLocation(const std::string& location);


        /** 
          * getTTISAgentAtLocation
          *
          * Get a reference to the TTIS Interface held by the TIS Agent.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          *
          * @param location The location name e.g. DBG
          * @return a reference to the TTIS Interface
          */
        TtisNamedObj& getTTISAgentAtLocation(const std::string& location);

    private:
        
        /**
         * TISAgentAccessFactory
         *
         * Private constructors.
         */
        TISAgentAccessFactory();
        TISAgentAccessFactory& operator=(const TISAgentAccessFactory &) {};
        TISAgentAccessFactory( const TISAgentAccessFactory& ) {};


        /** 
          * getLocationName
          *
          * Retrieves and returns the location key.
          *
          * @pre The LocationKey runparam is set, this should be the case
          *      TA_ASSERTS if not set.
          *
          * @return The location key of this console
          */
        std::string getLocationName();


        // class instance
        static TISAgentAccessFactory* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        // location key
        std::string m_locationName;

        // TIS agent (TTIS interface)
        TtisNamedObj* m_ttisNamedObj;

        // TIS agent (STIS interface)
        StisNamedObj* m_stisNamedObj;

        // OCC TIS agent (TTIS interface)
        TtisNamedObj* m_occTtisNamedObj;

        // OCC TIS agent (STIS interface)
        StisNamedObj* m_occStisNamedObj;

        typedef std::map<std::string, StisNamedObj*> StisNamedObjMap;
        typedef std::map<std::string, TtisNamedObj*> TtisNamedObjMap;

        StisNamedObjMap m_stisNamedObjMap;
        TtisNamedObjMap m_ttisNamedObjMap;

        static const std::string OCC_LOCATION_NAME;
    };

} // end namespace TA_Base_Bus

#endif 
