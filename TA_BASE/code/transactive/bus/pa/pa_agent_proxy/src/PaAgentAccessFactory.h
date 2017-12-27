/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/pa/pa_agent_proxy/src/PaAgentAccessFactory.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/23 16:48:56 $
  * Last modified by:  $Author: qi.huang $
  *
  * This is used to get access to the PA agent and keep a reference
  * to it.
  */

#if !defined(PA_AGENT_ACCESS_FACTORY_H)
#define PA_AGENT_ACCESS_FACTORY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#pragma warning ( disable : 4786 4503 )
#endif

#include <map>

#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

#include "bus/pa/pa_agent/IDL/src/IStationPaMethod.h"

namespace TA_Base_Bus
{
    typedef TA_Base_Core::NamedObject<TA_Base_Bus::IStationPaMethod,
        TA_Base_Bus::IStationPaMethod_ptr,
        TA_Base_Bus::IStationPaMethod_var> PaNamedObj;

    class PaAgentAccessFactory
    {
        
    public:
        /**
         * ~PaAgentAccessFactory
         *
         * Standard destructor.
         */
        virtual ~PaAgentAccessFactory();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      PaAgentAccessFactory&
         *              A reference to an instance of a PaAgentAccessFactory object.
         *
         */
		static PaAgentAccessFactory& getInstance();

        static void delInstance();

        void getOccLocationkey();

        /** 
          * getMasterPaAgentOfOcc
          *
          * Get a reference to the pa Interface held by the pa Agent.
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
          * @return a reference to the PA Interface
          */
        PaNamedObj& getMasterPaAgentOfOcc();

        /** 
          * getPaAgentByLocationKey
          *
          * Get a reference to the PA Interface held by the PA Agent.
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
          * @param location The location key in database
          * @return a reference to the Pa Interface
          */
        PaNamedObj& getPaAgentByLocationKey( unsigned long ulLocationKey );

    private:
        /**
         * PaAgentAccessFactory
         *
         * Private constructors.
         */
        PaAgentAccessFactory();
        PaAgentAccessFactory& operator=(const PaAgentAccessFactory &);
        PaAgentAccessFactory( const PaAgentAccessFactory& );

        PaNamedObj* getMasterPaAgent( unsigned long ulLocationKey );

    private:
        // class instance
        static PaAgentAccessFactory* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        typedef std::map<unsigned long, PaNamedObj*> PaNamedObjMap;

        PaNamedObjMap m_mapPaNamedObjMap;

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_pwOccLocationKey;
    };

} // end namespace TA_Base_Bus

#endif 
