/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/tis_agent/src/PidStationIdAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * PidStationIdAccessFactory is a singleton that is used to retrieve PidStationId objects either from the
  * database or newly created. All PidStationId objects returned will adhear to the IPidStationId interface.
  * This file is only used for PidsPlanStep
  */



#if !defined(PidStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D7__INCLUDED_)
#define PidStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D7__INCLUDED_

#include <string>
#include <map>


#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"


namespace TA_Base_Core
{

	class PidStationIdAccessFactory
	{


	private:

        // Disable the following methods
		PidStationIdAccessFactory() { };
		PidStationIdAccessFactory( const PidStationIdAccessFactory& thePidStationIdAccessFactory);
		PidStationIdAccessFactory& operator=(const PidStationIdAccessFactory &);


	public:

        // used to map between TA locations and station ids
		typedef std::map<std::string, unsigned long> LocationNameToStationIdMap;

      
        /** 
         * ~PidStationIdAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PidStationIdAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PidStationIdAccessFactory object.
         */
		static PidStationIdAccessFactory& getInstance();


        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();

        
        /** 
          * getLocationKeyToStationIdMap
          *
          * Gets a map of transactive location keys to station ids.
          *
          * @return A map
          *
          * @throws  DatabaseException 
          *          - if there are any problems in communicating with the database.
          *            The possilble reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @throws  TA_Base_Core::DataException 
          *          - if there is no PidStationId matching the supplied key
          *          - if there is more than one PidStationId matching the supplied key. 
          */
        LocationNameToStationIdMap getLocationNameToStationIdMap();

    private:
        static PidStationIdAccessFactory* m_instance;
	};

} // closes TA_IRS_Core

#endif // !defined(PidStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
