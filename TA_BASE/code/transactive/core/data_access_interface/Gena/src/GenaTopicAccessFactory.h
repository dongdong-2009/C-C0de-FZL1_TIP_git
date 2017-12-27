/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaTopicAccessFactory.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the declaration for GenaTopicAccessFactory
  */

#ifndef __GENA_TOPIC_ACCESS_FACTORY_H_INCLUDED__
#define __GENA_TOPIC_ACCESS_FACTORY_H_INCLUDED__

#pragma once

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include <vector>
#include <string>

namespace TA_Base_Core
{
	// Forward declaration to help the compiler avoid unnecessary compilation
	class IGenaTopic;

	class GenaTopicAccessFactory
	{
    public:
        virtual ~GenaTopicAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an GenaTopicAccessFactory object.
         */
        static GenaTopicAccessFactory& getInstance();

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
        IGenaTopic* getGenaGenaTopic(const unsigned long& key);
        
        /**
         * getGenaTopicForEntity
         *
         */
        std::vector<IGenaTopic*> getGenaTopicForEntity(unsigned long p_entityKey);
		
        /**
         * createClientSource
         *
         * Returns a pointer to a new GenaClientSource instance.
         *
         * @return A pointer to an object conforming to the IGenaClientSource interface
         * that has not been initialised.
         */
        IGenaTopic* createTopic();

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
        void runQueryForTopic(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IGenaTopic*>& genaTopics);

        GenaTopicAccessFactory() {};
        GenaTopicAccessFactory( const GenaTopicAccessFactory& rhs);
        GenaTopicAccessFactory& operator=(const GenaTopicAccessFactory & rhs);
    
        static GenaTopicAccessFactory* m_instance;
	};
}

#endif //__GENA_TOPIC_ACCESS_FACTORY_H_INCLUDED__