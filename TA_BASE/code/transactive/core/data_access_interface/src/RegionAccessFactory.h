/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/RegionAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * RegionAccessFactory is a singleton that is used to retrieve Region objects either from the
  * database or newly created. All Region objects returned will adhear to the IRegion interface.
  *
  */



#if !defined(REGION_ACCESS_FACTORY_H_INCLUDED)
#define REGION_ACCESS_FACTORY_H_INCLUDED 

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IRegion.h"

namespace TA_Base_Core
{
	class RegionAccessFactory
	{
	public:
        virtual ~RegionAccessFactory() 
		{

		};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an RegionAccessFactory object.
         */
		static RegionAccessFactory& getInstance();

		
        /**
         * getAllRegions
         *
         * Retrieves all specified Regions and returns the associated data as a vector of  
         * objects conforming to the IRegion interface.
         *
         * @return A vector of pointers to objects conforming to the IRegion interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRegion 
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
        std::vector<IRegion*> getAllRegions( const bool readWrite = false );


        /** 
          * getRegionByKey
          *
          * Get a region based on the regionKey
          *
          * @return IRegion* the
          *
          * @param unsigned long regionKey
		  *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          */
		IRegion* getRegionByKey( const unsigned long regionKey, const bool readWrite = false );

        /**
         * getRegionsByOperator
         *
         * Retrieves all regions assigned to the given operator. Regions are returned as a vector
		 * of IRegion*.
         *
         * @return A vector of IRegion*.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IRegion*> getRegionsByOperator( const unsigned long operatorKey );

        /**
         * getRegionsByLocation
         *
         * Retrieves all regions assocaited with the given location as a vector of IRegion*.
         *
         * @return A vector of IRegion*.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IRegion*> getRegionsByLocation( const unsigned long locationKey );

    private:
        //
        // These are private as this method is a singleton
        //
		RegionAccessFactory() 
		{

		};
		RegionAccessFactory( const RegionAccessFactory& theRegionAccessFactory);
		RegionAccessFactory& operator=(const RegionAccessFactory &);
		
		void getRegionBySql(IDatabase* pDatabse, const SQLStatement& sql, std::vector< IRegion* >& regions, const bool readWrite=true);


		static RegionAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(REGION_ACCESS_FACTORY_H_INCLUDED)
