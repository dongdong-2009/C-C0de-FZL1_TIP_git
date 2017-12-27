#if !defined(AFX_TREND_PROPERTIES_DATABASE_ACCESS_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_TREND_PROPERTIES_DATABASE_ACCESS_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * TrendPropertiesDatabaseAccess.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/TrendPropertiesDatabaseAccess.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class loads and maintains a store of the the trend templates.
  *
  */

#include <memory>

#include "app/trending/trend_viewer/src/TrendPropertiesDatabaseAccess.h"
#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/globals.h"
#include "app/trending/trend_viewer/src/Utilities.h"
#include "app/trending/trend_viewer/src/TrendPropertiesStore.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/trends/src/TrendAccessFactory.h"
#include "core/data_access_interface/trends/src/ConfigTrend.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TrendViewerException.h"
#include "core/utilities/src/TAAssert.h"

#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"

#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/message/types/Configuration_MessageTypes.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace // unnamed
{
    //
    // parseItems
    //
    void parseItems(const std::string& itemsString, std::map<std::string, TA_Base_Core::ItemInfo>& items)
    {
        FUNCTION_ENTRY("parseItems");

        if(itemsString.empty())
        {
            items.clear();
            FUNCTION_EXIT;
            return;
        }

        // Format:<item name>\<display name>\<colour>\<index>,<item name>\<display name>\<colour>\<index>, ...
        
		std::string::size_type beginPos = 0;
        std::string::size_type pos = itemsString.find("\\");
        
        if(pos == std::string::npos)
        {
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_INVALID, 
                "Item property retrieved from database is invalid"));
        }

        while(pos != -1 && pos != itemsString.length())
        {
            // Retrieve item name
            int len = (pos - beginPos);
            std::string itemName = itemsString.substr(beginPos, len);

            TA_Base_Core::ItemInfo itemInfo;
            
            // Retrieve display name
            beginPos = pos + 1;
            pos = itemsString.find("\\", beginPos);
            len = (pos - beginPos);
            std::string displayName = itemsString.substr(beginPos, len);
            itemInfo.displayName = displayName;
            
            // Retrieve colour (hex digit)
            beginPos = pos + 1;
            pos = itemsString.find("\\", beginPos);
            len = (pos - beginPos);
            std::string colourStr = itemsString.substr(beginPos, len);
            
            // Convert colour to a COLORREF
            //itemInfo.colour = TA_Base_App::Utilities::convertHexStringToColorRef(colourStr);
            itemInfo.colour = colourStr;
            
            // Retrieve index
            beginPos = pos + 1;
            pos = itemsString.find(",", beginPos);

            if(pos == -1)
            {
                pos = itemsString.length();
            }
            
            len = (pos - beginPos);
            itemInfo.index = atoi(itemsString.substr(beginPos, len).c_str());
            items[itemName] = itemInfo;

            if(pos != itemsString.length())
            {
                beginPos = pos + 1;
                pos = itemsString.find("\\", beginPos);
            }
        }
        
        FUNCTION_EXIT;
    }
}

    
namespace TA_Base_App
{
    // This is the column size of the DETAILS field in the EVENT table
    const int TrendPropertiesDatabaseAccess::EVENT_DETAILS_FIELD_SIZE = 1024;

    //
    // loadTrendProperties
    //
    void TrendPropertiesDatabaseAccess::loadTrendProperties(std::map<std::string, TrendProperties>& trendTemplates)
    {
        FUNCTION_ENTRY("loadTrendProperties");

        // The following is commented out and the database is accessed directly through SimpleDb in
        // attempt to improve loading time for the Trend Viewer.

        trendTemplates.clear();

		//delete by lin
        //std::string sql("select NAME, DESCRIPTION, GRAPH_TYPE, TIME_RANGE, TO_CHAR(START_TIME, 'YYYYMMDDHH24MISS'), \
            SAMPLE_PERIOD, LIVE, ITEMS, EXTRA_INFO, TO_CHAR(DATE_MODIFIED, 'YYYYMMDDHH24MISS') from TREND_TEMPLATE");

        std::vector<std::string> columnNames;
        columnNames.push_back("Name");
        columnNames.push_back("Description");
        columnNames.push_back("GraphType");
        columnNames.push_back("TimeRange");
        columnNames.push_back("StartTime");
        columnNames.push_back("SamplePeriod");
        columnNames.push_back("Live");
        columnNames.push_back("Items");
        columnNames.push_back("ExtraInfo");
        columnNames.push_back("DateModified");
        
        // Get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Trend_Ad, TA_Base_Core::Read);

        TA_Base_Core::IData* data = NULL;
        try
        {
            //data = databaseConnection->executeQuery(sql,columnNames);
            

			TA_Base_Core::SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, TREND_TEMPLATE_SELECT_39054);
			data = databaseConnection->executeQuery(strSql, columnNames);

        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            std::string error("Database problem: ");
            error += ex.what();
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_LOADING_ERROR, error));
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::DATABASE_ERROR, "Unknown error occured while retrieving trend properties."));
        }
        
        do
        {
            try
            {
                for(unsigned int i = 0; i < data->getNumRows(); ++i)
                {
                    TrendProperties trendProperties;
                    trendProperties.name = data->getStringData(i, "Name");
                    trendProperties.description = data->getStringData(i, "Description");
                    trendProperties.graphType = TrendPropertiesStore::getGraphType(data->getIntegerData(i, "GraphType"));
                    trendProperties.startDateTime = data->getDateData(i, "StartTime");
                    trendProperties.endDateTime = trendProperties.startDateTime + data->getUnsignedLongData(i, "TimeRange");
                    trendProperties.samplePeriod = data->getUnsignedLongData(i, "SamplePeriod");
                    trendProperties.live = data->getBooleanData(i, "Live");
                    parseItems(data->getStringData(i, "Items"), trendProperties.items);
                    trendProperties.extraInfo = data->getStringData(i, "ExtraInfo");
                    
                    try
                    {
                        trendProperties.lastModified = data->getDateData(i, "DateModified");
                    }
                    catch(const TA_Base_Core::DataException& ex)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());

                        // Date modified column is empty
                        trendProperties.lastModified = 0;
                    }
                    catch(...)
                    {
						delete data; //TD17884
						data = NULL;
                        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

                        std::string error("Unknown error occured while loading templates.");
                        TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_LOADING_ERROR, error));
                    }
                    
					trendProperties.modificationsUnsaved = false;
                    trendProperties.isNew = false;
                    
                    trendTemplates[trendProperties.name] = trendProperties;
                }
            }
            catch(const TA_Base_Core::TrendViewerException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());
				delete data; //TD17884
				data = NULL;

                if(ex.getType() == TA_Base_Core::TrendViewerException::TEMPLATE_INVALID
                    || ex.getType() == TA_Base_Core::TrendViewerException::GRAPH_TYPE_INVALID)
                {
                    TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_LOADING_ERROR, ex.what()));
                }
                else
                {
                    // Should never go in here
                    TA_ASSERT(false, "Unexpected TrendViewerException exception caught");
                }
            }
            catch(const TA_Base_Core::DatabaseException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
				delete data; //TD17884
				data = NULL;
                std::string error("Database problem: ");
                error += ex.what();
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_LOADING_ERROR, error));
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
				delete data; //TD17884
                std::string error("Unknown error occured while loading templates.");
                TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEMPLATE_LOADING_ERROR, error));
            }

			delete data; //TD17884
            data = NULL;

        } while(databaseConnection->moreData(data));
        
    
        FUNCTION_EXIT;
    }


    //
    // addTrendPropertiesToDatabase
    //
    void TrendPropertiesDatabaseAccess::addTrendPropertiesToDatabase(const TrendProperties& trendProperties)
    {
        FUNCTION_ENTRY("addTrendPropertiesToDatabase"); 

        // Create the new object
        TA_Base_Core::ConfigTrend trend;
        trend.setName(trendProperties.name);
        trend.setGraphType(TrendPropertiesStore::getGraphTypeAsString(trendProperties.graphType));
        trend.setDescription(trendProperties.description);
		trend.setStartDateTime(trendProperties.startDateTime); //TD17065
        trend.setRangeSeconds(trendProperties.endDateTime - trendProperties.startDateTime);
        trend.setSampleType(trendProperties.samplePeriod);
        trend.setLive(trendProperties.live);
        trend.setItems(trendProperties.items);
        trend.setExtraInfo(trendProperties.extraInfo);
        
        // Construct the message details
        UpdateDetails updateDetails;
        updateDetails.isNewItem = trend.isNew();
        updateDetails.isDelete = false;
        updateDetails.nameOfChangedItem = trendProperties.name;
        
        // Apply the changes to the database
        trend.applyChanges();

        sendMessagesForTheChange(updateDetails);

        FUNCTION_EXIT;
    }


    //
    // removeTrendPropertiesFromDatabase
    //
    void TrendPropertiesDatabaseAccess::removeTrendPropertiesFromDatabase(const std::string& name)
    {
        FUNCTION_ENTRY("addTrendPropertiesToDatabase");

        TA_Base_Core::ITrend* iTrend = TA_Base_Core::TrendAccessFactory::getInstance().getTrend(name, true);
        TA_Base_Core::ConfigTrend* trend = dynamic_cast <TA_Base_Core::ConfigTrend*> (iTrend);

        UpdateDetails updateDetails;
        updateDetails.isDelete = true;
        updateDetails.keyOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(trend)->getKey();
        updateDetails.nameOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(trend)->getName();
    
        trend->deleteThisObject();
    
        sendMessagesForTheChange( updateDetails );
        
        FUNCTION_EXIT;
    }


    //
    // modifyTrendProperties
    //
    void TrendPropertiesDatabaseAccess::modifyTrendProperties(const std::string& name,const TrendProperties& trendProperties)
    {
        FUNCTION_ENTRY("modifyTrendProperties");

        // Create the new object
        TA_Base_Core::ITrend* iTrend = TA_Base_Core::TrendAccessFactory::getInstance().getTrend(trendProperties.name, true);
        TA_Base_Core::ConfigTrend* trend = dynamic_cast <TA_Base_Core::ConfigTrend*> (iTrend);
        
        trend->setName(trendProperties.name);
        trend->setGraphType(TrendPropertiesStore::getGraphTypeAsString(trendProperties.graphType));
        trend->setDescription(trendProperties.description);
		trend->setStartDateTime(trendProperties.startDateTime); //TD17065
        trend->setRangeSeconds(trendProperties.endDateTime - trendProperties.startDateTime);
        trend->setSampleType(trendProperties.samplePeriod);
        trend->setLive(trendProperties.live);
        trend->setItems(trendProperties.items);
        trend->setExtraInfo(trendProperties.extraInfo);
        
        // Construct the message details
        UpdateDetails updateDetails;
        updateDetails.isNewItem = trend->isNew();
        updateDetails.isDelete = false;
        updateDetails.nameOfChangedItem = trendProperties.name;

        if (!trend->isNew())
        {
            updateDetails.changes = trend->getAllItemChanges();
        }
        
        // Apply the changes to the database
        trend->applyChanges();

        sendMessagesForTheChange(updateDetails);

        FUNCTION_EXIT;
    }


    void TrendPropertiesDatabaseAccess::sendMessagesForTheChange(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendMessagesForTheChange");

        if (updateDetails.isNewItem && updateDetails.isDelete)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This is a new item being deleted so it does not need any messages to be sent");
            FUNCTION_EXIT;
            return;
        }

		//TD18017 - Trend viewer should not be sending any audit messages - GDN RevD
        //sendAuditMessages(updateDetails);
		//++TD18017
        sendOnlineUpdateMessage( updateDetails );

        FUNCTION_EXIT;
    }

    
    void TrendPropertiesDatabaseAccess::sendAuditMessages(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendAuditMessages");

        TA_Base_Core::AuditMessageSender* m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::Configuration::Context );
        TA_ASSERT(m_auditMessageSender != NULL, "The audit message sender cannot be NULL");

        // Set up the description
        TA_Base_Core::DescriptionParameters description;
        TA_Base_Core::NameValuePair component("ConfigurationItemType", "Trend");
        TA_Base_Core::NameValuePair item("Item", updateDetails.nameOfChangedItem);
        TA_Base_Core::NameValuePair attribute("Attribute", ""); // If the position of this changes in the vector then
                                                            // the code below much change. At the moment it is
                                                            // assumed to be in position 2 in the vector.

        description.push_back( &component );
        description.push_back( &item );
        description.push_back( &attribute );

        std::string details("");
        if (updateDetails.isNewItem)
        {
            // This is a new item so we need to send an ItemAdded audit message.
            m_auditMessageSender->sendAuditMessage(TA_Base_Core::Configuration::ConfigurationItemAdded,
               strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITY_KEY).c_str(), NULL, 10),
			   description,
               details,
               TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
               "","","");
        }

        if (updateDetails.isDelete)
        {
            // This is an existing item being deleted
            m_auditMessageSender->sendAuditMessage(TA_Base_Core::Configuration::ConfigurationItemDeleted,
			   strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITY_KEY).c_str(), NULL, 10),
			   description,
			   details,
               TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
               "","","");
        }

        if (!updateDetails.isNewItem && !updateDetails.isDelete)
        {
            // Since this is an update we need to record all the changes that were made. So lets
            // step through the vector of changes and generate messages for each change.

            for(TA_Base_Core::ItemChanges::const_iterator iter = updateDetails.changes.begin();
                iter != updateDetails.changes.end();
                ++iter)
            { 
                TA_Base_Core::NameValuePair attribute("Attribute", iter->first);
                // Attribute is the 3rd item added to the list
                description[2] = &attribute;

                details = iter->first;
                details += " changed from '";
                details += iter->second.oldValue;
                details += "' to '";
                details += iter->second.newValue;
                details += "'";

                // If the details field is too long to fit in the database we will just have to truncate it
                if (details.length() > EVENT_DETAILS_FIELD_SIZE)
                {
                    details = details.substr(0,EVENT_DETAILS_FIELD_SIZE);
                }

                m_auditMessageSender->sendAuditMessage(TA_Base_Core::Configuration::ConfigurationItemUpdated,
                    strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITY_KEY).c_str(), NULL, 10),
                    description,
                    details,
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
                    "","","");
            }

        }

        FUNCTION_EXIT;
    }


    void TrendPropertiesDatabaseAccess::sendOnlineUpdateMessage(const UpdateDetails& updateDetails)
    {
        FUNCTION_ENTRY("sendAuditMessage");

        TA_Base_Core::ConfigUpdateMessageSender* configMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getConfigUpdateMessageSender( TA_Base_Core::ConfigUpdate::Context );
        
        TA_ASSERT(configMessageSender != NULL, "The config online update message sender cannot be NULL");

        TA_Base_Core::EModificationType configUpdateType;

        if (updateDetails.isNewItem)
        {
            // This is a new item so we need to send config update Create message
            configUpdateType = TA_Base_Core::Create;
        }


        if (updateDetails.isDelete)
        {
            // This is an existing item being deleted
            configUpdateType = TA_Base_Core::Delete;
        }

        if (!updateDetails.isNewItem && !updateDetails.isDelete)
        {
            // This is an update of an existing item so we just need to send an update for it
            configUpdateType = TA_Base_Core::Update;
        }


        // Now we can send the online update message

        // First we iterate through and build up a list of changes. We are only interested in
        // which attributes have changed, not the values they have changed from and to.
        std::vector< std::string > changes;

        for(TA_Base_Core::ItemChanges::const_iterator iter = updateDetails.changes.begin();
            iter != updateDetails.changes.end();
            ++iter)
        {
            changes.push_back( iter->first );
        }

        configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigTrend,
            updateDetails.keyOfChangedItem,
            configUpdateType,
            changes);

        FUNCTION_EXIT;
    }
}

#endif //AFX_TREND_PROPERTIES_DATABASE_ACCESS_CPP__04E07007_870A_11D6_B248_0050BAB25370__INCLUDED_
