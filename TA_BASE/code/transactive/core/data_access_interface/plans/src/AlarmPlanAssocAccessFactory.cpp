/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * AlarmPlanAssocAccessFactory is a singleton that is used to retrieve 
  * IAlarmPlanAssocData objects from the database. It is able to provide 
  * read-only access.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <ctime>

#include "core/data_access_interface/plans/src/AlarmPlanAssocAccessFactory.h"
#include "core/data_access_interface/plans/src/AlarmPlanAssocData.h"
#include "core/data_access_interface/plans/src/ConfigAlarmPlanAssoc.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    AlarmPlanAssocAccessFactory* AlarmPlanAssocAccessFactory::s_instance = 0;
   
	AlarmPlanAssocAccessFactory::AlarmPlanAssocAccessFactory()
		: m_associationsLoaded( false ),m_registeredOnlineUpdates(false)
	{
	}


	AlarmPlanAssocAccessFactory::~AlarmPlanAssocAccessFactory()
	{
		// Clear the cache
        for( AssociationList::iterator iter = m_associations.begin(); iter != m_associations.end(); ++iter)
        {
            if (*iter != NULL)
            {
                delete *iter;
                *iter = NULL;
            }
        }

        m_associationsLoaded = false;
	}


    AlarmPlanAssocAccessFactory& AlarmPlanAssocAccessFactory::getInstance()
    {
        if( s_instance == 0 )
        {
            s_instance = new AlarmPlanAssocAccessFactory();
        }
        return *s_instance;
    }


    void AlarmPlanAssocAccessFactory::cleanUp()
    {
        if (m_associationsLoaded && m_registeredOnlineUpdates)
        {
            OnlineUpdateListener::getInstance().deregisterAllInterests(ALARM_PLAN_ASSOCIATION, *this);
        }

        if (s_instance != NULL)
        {
            delete s_instance;
            s_instance = NULL;
        }
    }


	bool AlarmPlanAssocAccessFactory::associationExists(unsigned long entityKey,
			unsigned long entityTypeKey,
			unsigned long alarmType, const bool registerForOnlineUpdates/* = true*/)
	{
        if(!registerForOnlineUpdates)
        {
            // reset the reload flag since associations aren't cached
            m_associationsLoaded = false;
        }

		loadAssociations(registerForOnlineUpdates);
		// Go through the loaded associations and call match. If one matches bomb out
		AssociationList::iterator it = m_associations.begin();
		for (; it != m_associations.end(); it++ )
		{
			unsigned long testEntityKey = (*it)->getEntityKey();
			unsigned long testEntityType = (*it)->getEntityTypeKey();
			unsigned long testAlarmType = (*it)->getAlarmType();
			if ( (*it)->match( entityKey, entityTypeKey, alarmType ) )
			{
				return true;
			}
		}

		return false;
	}

	
	PlanDetailsList AlarmPlanAssocAccessFactory::getAssociatedPlanDetails( 
        unsigned long entityKey,
        unsigned long entityTypeKey,
        unsigned long alarmType, 
        const bool registerForOnlineUpdates /*= true*/ )
	{
        if(!registerForOnlineUpdates)
        {
            // reset the reload flag since associations aren't cached
            m_associationsLoaded = false;
        }

		loadAssociations(registerForOnlineUpdates);

		// Set up the list we want to return
		PlanDetailsList planDetails;

		// Go through the loaded associations and call match. As each matches store the plan details
		AssociationList::iterator it = m_associations.begin();
		for ( ; it != m_associations.end(); it++ )
		{
			if ( (*it)->match( entityKey, entityTypeKey, alarmType ) )
			{
				planDetails.push_back( (*it)->getPlanPath() );
			}
		}

		return planDetails;
	}

        
    void AlarmPlanAssocAccessFactory::loadAssociations(const bool registerForOnlineUpdates/* = true*/)
	{
		if ( ! m_associationsLoaded )
		{
            m_associations.clear();

			// Load the associations
			
			// get a connection to the database
			IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

			// create the SQL string to retrieve the data of the AlarmPlanAssocData
			// based upon the key
            /*std::ostringstream sql;
            sql << "select PKEY, ENTITYTYPEKEY, ENTITYKEY, PLANPATH, ALARMTYPEKEY, ";
            sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
            sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from ALARMPLANMAPS";*/			
//			std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_Oracle_SELECT_2051);			
//			std::string sql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_SELECT_2051);			
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMPLANMAPS_SELECT_2051);
//			TA_ASSERT(sql.size() > 0, "loadAssociations: GetSQLStatement failed");

			// Set up the columnNames vector to be passed to executeQuery()
			std::string pkeyColumn = "PKEY";
			std::string entityTypeKeyColumn = "ENTITYTYPEKEY";
			std::string entityKeyColumn = "ENTITYKEY";
			std::string planPathColumn = "PLANPATH";
			std::string alarmTypeColumn = "ALARMTYPEKEY";
			std::string modifiedColumn = "DATEMODIFIED";
			std::string createdColumn = "DATECREATED";

			std::vector<std::string> columnNames;
			columnNames.push_back( pkeyColumn );
			columnNames.push_back( entityTypeKeyColumn );
			columnNames.push_back( entityKeyColumn );
			columnNames.push_back( planPathColumn );
			columnNames.push_back( alarmTypeColumn );
			columnNames.push_back( modifiedColumn );
			columnNames.push_back( createdColumn );

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			IData* data = databaseConnection->executeQuery( sql, columnNames );

			do
			{
				for ( unsigned long i = 0; i < data->getNumRows(); i++ )
				{
					try
					{
						// Create the new AlarmPlanAssociationObjects
                        IAlarmPlanAssocData* alarmPlan;

                        time_t dateModified = data->getDateData(0,modifiedColumn,0);

                        alarmPlan = new AlarmPlanAssocData( data->getUnsignedLongData( i, pkeyColumn ),
								                            data->getUnsignedLongData( i, entityKeyColumn ),
								                            data->getUnsignedLongData( i, entityTypeKeyColumn ),
								                            data->getUnsignedLongData( i, alarmTypeColumn ),
								                            data->getStringData( i, planPathColumn ),
                                                            data->getDateData(i, createdColumn ),
                                                            dateModified );

                        m_associations.push_back( alarmPlan );
					}
					catch (TA_Base_Core::DataException&)
					{
						// Clean up the data pointer
						delete data;
						data = NULL;
						throw;
					}
				}
				delete data;
				data = NULL;
			}
			while (databaseConnection->moreData(data));

            if(registerForOnlineUpdates)
            {
                OnlineUpdateListener::getInstance().registerAllInterests(ALARM_PLAN_ASSOCIATION ,*this);
                m_registeredOnlineUpdates = true;
            }

			// We have successfully loaded the associations
			m_associationsLoaded = true;
		}
	}


    void AlarmPlanAssocAccessFactory::processUpdate(const ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");

        if (updateEvent.getType() != ALARM_PLAN_ASSOCIATION)
        {
            // Only interested in alarm plan association updates
            FUNCTION_EXIT;
            return;
        }

        switch(updateEvent.getModifications())
        {
            case(Create):
            {
                // Add a new alarm plan object to our list
                IAlarmPlanAssocData* alarmPlan = new AlarmPlanAssocData(updateEvent.getKey());
                m_associations.push_back( alarmPlan );

                FUNCTION_EXIT;
                return;
            }

            case(Update):
            {
                // Find the object being updated and invalidate it
                for( AssociationList::iterator iter = m_associations.begin(); iter != m_associations.end(); ++iter)
                {
                    if( (*iter)->getKey() == updateEvent.getKey() )
                    {
                        (*iter)->invalidate();
                        break;
                    }
                }
                FUNCTION_EXIT;
                return;
            }

            case(Delete):
            {
                // Find the object being deleted and remove it
                for( AssociationList::iterator iter = m_associations.begin(); iter != m_associations.end(); ++iter)
                {
                    if( (*iter)->getKey() == updateEvent.getKey() )
                    {
                        delete *iter;
                        *iter = NULL;
                        m_associations.erase(iter);
                        break;
                    }
                }
                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }


    //
    // The following methods do not cache the alarm plan associations and do not handle online updates
    //

    std::vector<IAlarmPlanAssocData*> AlarmPlanAssocAccessFactory::getAllAlarmPlanAssociations( const bool readWrite/* = false*/ )
    {
        FUNCTION_ENTRY("getAllAlarmPlanAssociations");
		// Load the associations
		
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

		// create the SQL string to retrieve the data of the AlarmPlanAssocData
		// based upon the key
        /*std::ostringstream sql;
        sql << "select PKEY, ENTITYTYPEKEY, ENTITYKEY, PLANPATH, ALARMTYPEKEY, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from ALARMPLANMAPS";*/		
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARMPLANMAPS_Oracle_SELECT_2051);		
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARMPLANMAPS_SELECT_2051);		
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARMPLANMAPS_SELECT_2051);
//		TA_ASSERT(sql.size() > 0, "getAllAlarmPlanAssociations: GetSQLStatement failed");

		// Set up the columnNames vector to be passed to executeQuery()
		std::string pkeyColumn = "PKEY";
		std::string entityTypeKeyColumn = "ENTITYTYPEKEY";
		std::string entityKeyColumn = "ENTITYKEY";
		std::string planPathColumn = "PLANPATH";
		std::string alarmTypeColumn = "ALARMTYPEKEY";
		std::string modifiedColumn = "DATEMODIFIED";
		std::string createdColumn = "DATECREATED";

		std::vector<std::string> columnNames;
		columnNames.push_back( pkeyColumn );
		columnNames.push_back( entityTypeKeyColumn );
		columnNames.push_back( entityKeyColumn );
		columnNames.push_back( planPathColumn );
		columnNames.push_back( alarmTypeColumn );
		columnNames.push_back( modifiedColumn );
		columnNames.push_back( createdColumn );

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery( sql, columnNames );

        std::vector<IAlarmPlanAssocData*> alarmPlanAssociations;
		do
		{
			for ( unsigned long i = 0; i < data->getNumRows(); i++ )
			{
				try
				{
					// Create the new AlarmPlanAssociationObjects
                    IAlarmPlanAssocData* alarmPlan;

                    time_t dateModified = data->getDateData(i,modifiedColumn,0);

                    if (readWrite)
                    {
                        alarmPlan = new ConfigAlarmPlanAssoc( data->getUnsignedLongData( i, pkeyColumn ),
								                              data->getUnsignedLongData( i, entityKeyColumn ),
								                              data->getUnsignedLongData( i, entityTypeKeyColumn ),
								                              data->getUnsignedLongData( i, alarmTypeColumn ),
								                              data->getStringData( i, planPathColumn ),
                                                              data->getDateData(i, createdColumn ),
                                                              dateModified );
                    }
                    else
                    {
                        alarmPlan = new AlarmPlanAssocData( data->getUnsignedLongData( i, pkeyColumn ),
								                            data->getUnsignedLongData( i, entityKeyColumn ),
                                                            data->getUnsignedLongData( i, entityTypeKeyColumn ),
								                            data->getUnsignedLongData( i, alarmTypeColumn ),
								                            data->getStringData( i, planPathColumn ),
                                                            data->getDateData(i, createdColumn ),
                                                            dateModified );
                    }

                    alarmPlanAssociations.push_back( alarmPlan );
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
			}
			delete data;
			data = NULL;
		}
		while (databaseConnection->moreData(data));

        FUNCTION_EXIT;
        return alarmPlanAssociations;
    }


    IConfigAlarmPlanAssoc* AlarmPlanAssocAccessFactory::createAlarmPlanAssociation()
    {
        return new ConfigAlarmPlanAssoc();
    }


    IConfigAlarmPlanAssoc* AlarmPlanAssocAccessFactory::copyAlarmPlanAssociation(const IConfigAlarmPlanAssoc* itemToCopy)
    {
        FUNCTION_ENTRY("copyAlarmPlanAssociation");

        TA_ASSERT(itemToCopy !=  NULL, "The alarm plan association to copy was NULL");
        
        const ConfigAlarmPlanAssoc* cast = dynamic_cast<const ConfigAlarmPlanAssoc*>(itemToCopy);

        TA_ASSERT(cast != NULL, "Alarm plan Association passed could not be converted into a ConfigAlarmPlanAssoc");

        FUNCTION_EXIT;
        return new ConfigAlarmPlanAssoc(*cast);
    }


    time_t AlarmPlanAssocAccessFactory::getDateModified(time_t dateModified)
    {
        try
        {
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == dateModified) // The date was actually NULL
            {
                dateModified = 0;
            }
        }
        catch (DataException& ex)
        {//If we catch this exception, need to check if its a "NO_VALUE" type
            if (DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                dateModified = 0;
            }
        }

        return dateModified;
 }
    
} // closes TA_Base_Core
