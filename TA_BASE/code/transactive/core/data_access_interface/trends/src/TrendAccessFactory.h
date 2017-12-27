/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/trends/src/TrendAccessFactory.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TrendAccessFactory is a singleton that is used to retrieve Trend objects either from the
  * database or newly created. All Trend objects returned will adhere to the ITrend interface.
  *
  */



#if !defined(TrendAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define TrendAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>
#include <map>


namespace TA_Base_Core
{
    class IConfigTrend;
    class ITrend;

	class TrendAccessFactory
	{

	public:
        virtual ~TrendAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an TrendAccessFactory object.
         */
		static TrendAccessFactory& getInstance();

        /**
         * getTrend
         *
         * Retrieves the specified profile and returns the associated data as an 
         * object conforming to the ITrend interface.
         *
         * @param name The name to the trend to retrieve
         *
         * @return A pointer to an object conforming to the ITrend interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ITrend 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no trend
         *            matching the supplied NAME, or if there is more than one trend
         *            matching the supplied NAME. 
         */
        ITrend* getTrend(const std::string& name, const bool readWrite = false);

        /**
         * getAllTrends
         *
         * Retrieves all specified trends and returns the associated data as a vector of  
         * objects conforming to the ITrend interface.
         *
         * @return A vector of pointers to objects conforming to the ITrend interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ITrend 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<ITrend*> getAllTrends(const bool readWrite = false);


        /**
         * getAllSamplePeriods
         *
         * Retrieves all the available sample periods - this is taken from Globals.h
         *
         * @return map of the sample periods and their names.
         */
        std::map<unsigned long, std::string> getAllSamplePeriods();

        /**
         * getAllSelectableItems
         *
         * Retrieves all items that can be trended. Pkeys are not retrieved because some
         * of the items will have the same pkey, eg. the live properties. Instead they are
         * just given a unique identifier which does not have any meaning.
         *
         * @return names of the items.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::map<unsigned long, std::string> getAllSelectableItems();

        /**
         * getSamplePeriodTableName
         *
         * Retrieves the name of the view from which to retrieve data for the given type
         * identifier and sample period.
         *
         * @return names of the table/view.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::string getSamplePeriodTableName(const std::string& typeIdentifier, unsigned long samplePeriod);

        /**
         * createTrend
         * 
         * Creates an empty IConfigTrend object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigTrend object
         *
         * @return A pointer to an IConfigTrend object
         *
         */
		IConfigTrend* createTrend();


        /**
         * copyTrend
         * 
         * Creates a new configuration trend object using the trend passed in
         *
         * @param IConfigTrend* The trend to copy
         *
         * @return The configuration trend object as a pointer to an IConfigTrend object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigTrend* copyTrend(const IConfigTrend* trendToCopy);

    private:
        //
        // These are private as this method is a singleton
        //
		TrendAccessFactory();
		TrendAccessFactory( const TrendAccessFactory& theTrendAccessFactory);
		TrendAccessFactory& operator=(const TrendAccessFactory &) { };
    };

} // closes TA_Base_Core

#endif // !defined(TrendAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
