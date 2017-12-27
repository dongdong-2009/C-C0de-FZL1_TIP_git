/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaClientSourceAccessFactory.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the declaration for GenaClientSourceAccessFactory
  */

#ifndef __GENA_CLIENT_SOURCE_ACCESS_FACTORY_H_INCLUDED__
#define __GENA_CLIENT_SOURCE_ACCESS_FACTORY_H_INCLUDED__

#pragma once
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include <vector>
#include <string>

namespace TA_Base_Core
{
	// Forward delaration to help the compiler avoid unnecessary compilation
	class IGenaClientSource;
	class IDatabase;

	class GenaClientSourceAccessFactory
	{
    public:
        virtual ~GenaClientSourceAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an GenaClientSourceAccessFactory object.
         */
        static GenaClientSourceAccessFactory& getInstance();

		/**
		 * freeInstance
		 *
		 * deletes the object instance of this class.
		 *
		 * @return none.
		 */
		static void freeInstance();

        /**
         * getGenaClientSource
         *
         */
        IGenaClientSource* getGenaClientSource(const unsigned long& key);
        
		/**
         * getClientSourceForEntity
         *
         */
        std::vector<IGenaClientSource*> getClientSourceForEntity(unsigned long p_entityKey);

        /**
         * getClientSourceForLocation
         *
         */
        std::vector<IGenaClientSource*> getClientSourceForLocation(unsigned long p_locationKey);
		
        /**
         * createClientSource
         *
         * Returns a pointer to a new GenaClientSource instance.
         *
         * @return A pointer to an object conforming to the IGenaClientSource interface
         * that has not been initialised.
         */
        IGenaClientSource* createClientSource();

    private:

        /**
          * runQueryForClientSource
          *
          * This method takes an sql query and returns a vector of IAlarmData pointers. This
          * is essentially here to avoid dupilcate code in the getActiveAlarms and 
          * getActiveAlarmsForLocation methods.
          *
          * @param The query to run
          *
          * @return A vector of pointers to IAlarmData objects
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute the database query
          *
          * @exception DataException raised if the supplied location key is invalid
          */
        void runQueryForClientSource(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IGenaClientSource*>& clientSources);

        GenaClientSourceAccessFactory() {};
        GenaClientSourceAccessFactory( const GenaClientSourceAccessFactory& rhs);
        GenaClientSourceAccessFactory& operator=(const GenaClientSourceAccessFactory & rhs);
    
        static GenaClientSourceAccessFactory* m_instance;
	};
}

#endif //__GENA_CLIENT_SOURCE_ACCESS_FACTORY_H_INCLUDED__