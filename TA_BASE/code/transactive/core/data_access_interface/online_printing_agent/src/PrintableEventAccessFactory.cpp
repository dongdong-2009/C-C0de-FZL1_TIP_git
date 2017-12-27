/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/PrintableEventAccessFactory.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * MmSchedulingAccessFactory is a singleton that is used to retrieve 
  * IMmSchedulingData objects from the database. It is able to provide 
  * read-only access.
  */



#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>

#include "core/data_access_interface/online_printing_agent/src/PrintableEventAccessFactory.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventData.h"
#include "core/data_access_interface/online_printing_agent/src/ConfigPrintableEvent.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    PrintableEventAccessFactory* PrintableEventAccessFactory::s_instance = 0;

    const unsigned long PrintableEventAccessFactory::INVALID_SEQUENCE_NUMBER(0);
	const unsigned long PrintableEventAccessFactory::MIN_SEQUENCE_NUMBER(0);
   
	PrintableEventAccessFactory::PrintableEventAccessFactory()
    :
    m_PrintableEventLoaded(false),
    m_hasCacheChanged(false),
    m_readWriteCache(false),
    m_forceRefresh(false)
	{
	}


	PrintableEventAccessFactory::~PrintableEventAccessFactory()
	{
		try
		{
	        // Confirm we've stopped listening out for updates prior to clearing the cache
	        // (otherwise we'll be attempting to update cached objects that no longer exist!)
	        if (isListening())
	        {
	            TA_ASSERT(false, "Still registered to online updates on exit!");
	        }
		
			// Clear the cache
			clearCache();
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");		
		}
	}


    void PrintableEventAccessFactory::clearCache()
    {
        TA_Base_Core::ThreadGuard guard(m_listenerLock);        

        for( PrintableEventList::iterator iter = m_PrintableEvent.begin(); iter != m_PrintableEvent.end(); ++iter)
        {
            if (*iter != NULL)
            {
                delete *iter;
                *iter = NULL;
            }
        }

        m_PrintableEvent.clear();
        m_PrintableEventLoaded = false;
    }


    void PrintableEventAccessFactory::cleanUp()
    {
        if (isListening())
        {
            try
            {
				TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::EVENT_TYPE, *this);
            }
            catch (...)
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "from deregisterAllInterests");
            }
        }
        
        if (s_instance != NULL)
        {
            delete s_instance;
            s_instance = NULL;
        }
    }


    PrintableEventAccessFactory& PrintableEventAccessFactory::getInstance()
    {
        if( s_instance == 0 )
        {
            s_instance = new PrintableEventAccessFactory();
        }

        return *s_instance;
    }


	bool PrintableEventAccessFactory::registerInterest(TA_Base_Core::IOnlineUpdatable& callback)
	{
		TA_Base_Core::ThreadGuard guard(m_listenerLock);      

		std::vector<TA_Base_Core::IOnlineUpdatable*>::iterator itr = std::find(m_callbacks.begin(),
																	m_callbacks.end(),
																	&callback);
		if (m_callbacks.end() == itr)
		{
			// Add in new item
			m_callbacks.push_back(&callback);
			return true;
		}
		else
		{
			// Already added
			return false;
		}
	}


	bool PrintableEventAccessFactory::deregisterInterest(TA_Base_Core::IOnlineUpdatable& callback)
	{
		TA_Base_Core::ThreadGuard guard(m_listenerLock);      

		std::vector<TA_Base_Core::IOnlineUpdatable*>::iterator itr = std::find(m_callbacks.begin(),
																	m_callbacks.end(),
																	&callback);
		if (m_callbacks.end() == itr)
		{
			// Item to remove not found
			return false;
		}
		else
		{
			// Found item to remove
			m_callbacks.erase(itr);
			return true;
		}
	}


    bool PrintableEventAccessFactory::isListening()
    {
        try
        {
            return TA_Base_Core::OnlineUpdateListener::getInstance().isObjectTypeRegistered(TA_Base_Core::EVENT_TYPE, *this);
        }
        catch (...)
        {
            return false;
        }
    }


	std::vector<IPrintableEventData*> PrintableEventAccessFactory::getAllRecords( const bool readWrite /*= false*/ )
	{
		FUNCTION_ENTRY("getAllRecords");

        if (!isListening())
        {
            try
            {
                TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::EVENT_TYPE ,*this);
            }
            catch (...)
            {
                // This is handled for the most part gracefully - just means data is re-fetched
                // from database whenever user makes request.  However online updates will not be posted
                // to listeners
            	LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "from registerAllInterests");
		    }
        }        
                            
        // Lock out any online update listener responses while loading the cache
        TA_Base_Core::ThreadGuard guard(m_listenerLock);      
        
        // Determine if we need to clear the cache and re-fetch all the data
        // - if we're not currently listening out for online updates, we must
        // refetch to guarantee we have up to date data (we can't know whether it's changed or not)
        bool refetchRequired = m_forceRefresh || !isListening();

        if (refetchRequired)
        {
            // Force a refresh of cache data (performing full fetch)
            clearCache();
            m_forceRefresh = false;
        }  

        if (m_PrintableEventLoaded && 
            m_readWriteCache == readWrite)
        {
            // Cache data loaded previously (for correct r/w type), can return that data
            return m_PrintableEvent;
        }        

		// Get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Event_Ad, TA_Base_Core::Read);

        /*std::ostringstream sql;

        sql <<"select PKEY, PRINTER, ALARMFLAG, CREATETIME, ";
        sql <<" SEVERITY, ASSET, DESCRIPTION, ";
        sql <<" VALUE, MMS, DSS, AVALANCHE, ";
        sql <<" OPERATORNAME, ALARM_COMMENT, ALM, LOCATION_KEY";
        sql <<" from PRINTABLE_EVENT";*/
//		std::string strSql = databaseConnection->prepareSQLStatement( PRINTABLE_EVENT_SELECT_65001 );
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  PRINTABLE_EVENT_SELECT_65001 );

        // Set up the columnNames vector to be passed to executeQuery()
		std::string pkeyColumn = "PKEY";
        std::string printerColumn = "PRINTER";
        std::string alarmflagColumn = "ALARMFLAG";
        std::string createtimeColumn = "CREATETIME";
        std::string severityColumn = "SEVERITY";
		std::string assetColumn = "ASSET";
		std::string descriptionColumn = "DESCRIPTION";
        std::string valueColumn = "VALUE";
		std::string mmsColumn = "MMS";
		std::string dssColumn = "DSS";
		std::string avalancheColumn = "AVALANCHE";
		std::string operatornameColumn = "OPERATORNAME";
		std::string alarmcommentColumn = "ALARM_COMMENT";
		std::string almColumn = "ALM";
		std::string locationkeyColumn = "LOCATION_KEY";


		
        std::vector<std::string> columnNames;
		columnNames.push_back(pkeyColumn);
        columnNames.push_back(printerColumn);
        columnNames.push_back(alarmflagColumn);
        columnNames.push_back(createtimeColumn);
		columnNames.push_back(severityColumn);
		columnNames.push_back(assetColumn);
		columnNames.push_back(descriptionColumn);
		columnNames.push_back(valueColumn);
		columnNames.push_back(mmsColumn);
		columnNames.push_back(dssColumn);
		columnNames.push_back(avalancheColumn);
		columnNames.push_back(operatornameColumn);
		columnNames.push_back(alarmcommentColumn);
		columnNames.push_back(almColumn);
		columnNames.push_back(locationkeyColumn);


		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
                
	    // Create the vector to return
        std::vector<IPrintableEventData*> records;

 	    if (data->getNumRows() == 0) // No records found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            if (readWrite)
            {
                // Cache the data prior to exit, if requesting readWrite (to be available on next call)
                cacheConfigurableData(records, readWrite);
            }

            return records;
        }

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
				    unsigned long pkey = data->getUnsignedLongData( i, pkeyColumn );
		            std::string printer = data->getStringData( i, printerColumn );
                    std::string alarmflag = data->getStringData( i, alarmflagColumn );
	            	std::string createtime = data->getStringData( i, createtimeColumn );
	            	std::string severity = data->getStringData( i, severityColumn );
	            	std::string asset = data->getStringData( i, assetColumn );
	             	std::string description = data->getStringData( i, descriptionColumn );
	            	std::string value = data->getStringData( i, valueColumn );
            		std::string mms = data->getStringData( i, mmsColumn );
	            	std::string dss = data->getStringData( i, dssColumn );
	            	std::string avalanche = data->getStringData( i, avalancheColumn );
		            std::string operatorname = data->getStringData( i, operatornameColumn );
	              	std::string alarmcomment = data->getStringData( i, alarmcommentColumn );
	            	std::string alm = data->getStringData( i, almColumn );
	            	unsigned long locationkey = data->getUnsignedLongData( i, locationkeyColumn );



                    if (readWrite)
                    {
                        records.push_back( new ConfigPrintableEvent( pkey, printer, alarmflag, createtime, severity,
							                                         asset, description, value, mms, dss, avalanche,
																	 operatorname, alarmcomment, alm, locationkey ));
                    }
                    else
                    {
                        records.push_back( new PrintableEventData( pkey, printer, alarmflag, createtime, severity,
							                                         asset, description, value, mms, dss, avalanche,
																	 operatorname, alarmcomment, alm, locationkey ));
                    }
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
        
        if (readWrite)
        {
            // Cache the data prior to exit, if requesting readWrite (to be available on next call)
            cacheConfigurableData(records, readWrite);
        }

        return records;
	}





	std::vector<IData*> PrintableEventAccessFactory::getAllMessage(std::vector<std::string> &columnNames,
                                                unsigned long precedingMsgPkey, unsigned long maxMsgs, unsigned long lockey )
	{
        TA_ASSERT(!columnNames.empty(), "Message view column names have not been specified");
        
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
        
		std::vector<IData*> messages;

        // Build up a string of comma separated column names.
        std::string selectableColumns("");
 
        for (std::vector<std::string>::const_iterator nameIt(columnNames.begin()); nameIt != columnNames.end(); nameIt++)
        {
            if (!selectableColumns.empty())
            {
                selectableColumns += ", ";
            }

            selectableColumns += *nameIt;
        }

        // Get the required messages up to and including MAX_SEQUENCE_NUMBER.
        std::ostringstream sql;
		
        // TD17059 ++
        sql << " SELECT " << selectableColumns 
            << " FROM ( SELECT " << selectableColumns 
            << " FROM PRINTABLE_EVENT "
            << " WHERE PKEY > " << precedingMsgPkey 
            << " AND LOCATION_KEY = " << lockey
            << " ORDER BY PKEY )"
            << " WHERE ROWNUM <= " << maxMsgs;
        // ++ TD17059 
//		std::string strSql = databaseConnection->prepareSQLStatement(PRINTABLE_EVENT_SELECT_65002, selectableColumns, selectableColumns,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PRINTABLE_EVENT_SELECT_65002, selectableColumns, selectableColumns,
			precedingMsgPkey, lockey, maxMsgs);

        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        unsigned long msgCount(0);

        do
        {
            messages.push_back(data);
            msgCount += data->getNumRows();
            data = NULL;       
        }
        while (databaseConnection->moreData(data));

        return messages;
	}


    void PrintableEventAccessFactory::removePrintedEvents(unsigned long lastProcessedEvent, unsigned long lockey)
	{
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

       /* std::ostringstream sql;
        sql << " delete from PRINTABLE_EVENT " 
            << " where PKEY <= " << lastProcessedEvent 
            << " and LOCATION_KEY = " << lockey;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(PRINTABLE_EVENT_DELETE_65003, lastProcessedEvent, lockey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PRINTABLE_EVENT_DELETE_65003, lastProcessedEvent, lockey);

        databaseConnection->executeModification( strSql );
	}


    void PrintableEventAccessFactory::cacheConfigurableData(std::vector<IPrintableEventData*> data, bool readWrite)
    {
        TA_Base_Core::ThreadGuard guard(m_listenerLock);        

        clearCache();

        m_PrintableEvent         = data;
        m_PrintableEventLoaded   = true;
        m_readWriteCache    = readWrite;
    }



    bool PrintableEventAccessFactory::hasChanged()
    {
        TA_Base_Core::ThreadGuard guard(m_listenerLock);
        
        if (!isListening())
        {
            // If we're not currently listening, we can't make any assumptions on
            // whether the data has changed, so we must return true to indicate it may have changed
            return true;
        }

        return m_hasCacheChanged;
    }


    void PrintableEventAccessFactory::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");

        TA_Base_Core::ThreadGuard guard(m_listenerLock);

		try
		{
			handleUpdateNotification(updateEvent);
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "from handleUpdateNotification");
		}
        
		// Dispatch to listeners last (once invalidated internally)
		for (UpdateCallbackList::iterator itr = m_callbacks.begin(); itr != m_callbacks.end(); ++itr )
		{
			try
			{
				(*itr)->processUpdate(updateEvent);
			}
			catch (...)
			{
				// Ignore all exceptions
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "processUpdate to callback");
			}
		}

		FUNCTION_EXIT
	}


	void PrintableEventAccessFactory::handleUpdateNotification(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("handleUpdateNotification");

		TA_Base_Core::ThreadGuard guard(m_listenerLock);
	
		m_hasCacheChanged = true;
	
        // Receiving updates before managed to read from DB, may ignore
        if (!m_PrintableEventLoaded)
        {
            return;
        }

        if (updateEvent.getType() != TA_Base_Core::EVENT_TYPE)
        {
            // Only interested in MMS Scheduling updates
            FUNCTION_EXIT;
            return;
        }

        if (-1 == updateEvent.getKey())
        {
            // special case - single notification sent out for a batch of changes
            invalidateCache();
            FUNCTION_EXIT;
            return;
        }

        switch(updateEvent.getModifications())
        {
            case(TA_Base_Core::Create):
            {
                // Add a new alarm plan object to our list, provided not already there
                // (only rare cases it will already be present..)
                for( PrintableEventList::iterator iter = m_PrintableEvent.begin(); iter != m_PrintableEvent.end(); ++iter)
                {
                    if( (*iter)->getPkey() == updateEvent.getKey() )
                    {
                        // Already loaded this schedule record, nothing to do
                        return;
                    }
                }
            
                // Otherwise add record to cache - currently no easy way
                // to construct new individual object.  Because there is no way to
                // currently send a Create notification, we will never get this message at present,
                // but to handle we just clear the cache and force a complete refresh..
                invalidateCache();
                
                FUNCTION_EXIT;
                return;
            }

            case(TA_Base_Core::Update):
            {
                // Find the object being updated and invalidate it
                for( PrintableEventList::iterator iter = m_PrintableEvent.begin(); iter != m_PrintableEvent.end(); ++iter)
                {
                    if( (*iter)->getPkey() == updateEvent.getKey() )
                    {
                        (*iter)->invalidate();
                        break;
                    }
                }
                FUNCTION_EXIT;
                return;
            }

            case(TA_Base_Core::Delete):
            {
                // Find the object being deleted and remove it
                for( PrintableEventList::iterator iter = m_PrintableEvent.begin(); iter != m_PrintableEvent.end(); ++iter)
                {
                    if( (*iter)->getPkey() == updateEvent.getKey() )
                    {
                        delete *iter;
                        *iter = NULL;
                        m_PrintableEvent.erase(iter);
                        break;
                    }
                }
                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }


    void PrintableEventAccessFactory::invalidateCache()
    {
        m_forceRefresh = true;
    }
                
} // closes TA_IRS_Core
