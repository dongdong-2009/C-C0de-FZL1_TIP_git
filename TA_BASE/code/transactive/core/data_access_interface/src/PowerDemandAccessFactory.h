/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/PowerDemandAccessFactory.h $
  * @author:  Wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * PowerDemandAccessFactory is a singleton that is used to retrieve or set power demand data
  */



#if !defined(PowerDemandAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PowerDemandEntityAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>
#include <set>

#include "core/data_access_interface/src/PowerDemandData.h"

namespace TA_Base_Core
{
    class IPowerDemandData;

	class PowerDemandAccessFactory
	{

	public:
		PowerDemandAccessFactory() { };

        virtual ~PowerDemandAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an QuickFindEntityAccessFactory object.
         */
		static PowerDemandAccessFactory& getInstance();



        /**
         * getPowerDemandDataPointNames
         *
         * Retrieves from database and returns a set of unique names of all power demand
         * recorded in the 30-minute power demand table PWR_DMD_30MIN_TAB
         *
         * @param     Power data points' name as a set of string
		 *
		 * @ void
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no entity
         *            matching the supplied PKEY, or if there is more than one entity
         *            matching the supplied PKEY. 
         */
		 void getPowerDemandDataPointNames(std::set<std::string>& powerDemandPoints); 

		 
		 /**
         * getPowerDemandData
         *
         * Retrieves the recorded and operator-adjusted 30-minute power demand values for a specified metering 
         * data point and time interval. The powerDemandData parameter is updated with the relevant details upon 
		 * return. Where a power demand value has not been edited before, the returned operator-adjusted value is set 
		 * to be the same as the recorded value.
		 *
		 * @param     powerDemandData
         *
         * @void
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no entity
         *            matching the supplied PKEY, or if there is more than one entity
         *            matching the supplied PKEY. 
         */
         void getPowerDemandData(PowerDemandData& powerDemandData);


		 /**
         * setPowerDemandData
         *
         * Updates in database the operator-adjusted 30-minute power demand value for a specified metering data 
         * point and time interval. The updated record's MODIFIED_BY field is set to the operator's session ID 
		 * (retrieved from RunParams), while MODIFIED_DATE is set to the current system time.
		 * 
		 *
		 * @param     powerDemandData
         *
         * @void
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no entity
         *            matching the supplied PKEY, or if there is more than one entity
         *            matching the supplied PKEY. 
         */
         void setPowerDemandData(const PowerDemandData& powerDemandData);


    private:
        //
        // These are private as this method is a singleton
        //
		
		PowerDemandAccessFactory( const PowerDemandAccessFactory& thePowerDemandAccessFactory);
		PowerDemandAccessFactory& operator=(const PowerDemandAccessFactory &);

        static PowerDemandAccessFactory* s_instance;
    };

} // closes TA_Base_Core

#endif // !defined(PowerDemandAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
