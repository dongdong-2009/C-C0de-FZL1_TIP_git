#if !defined(GenericItemCpp_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_)
#define GenericItemCpp_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_

/**
  * GenericItem.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/GenericItem.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class represents an item that can be trended.
  *
  */

#include "app/trending/trend_viewer/src/GenericItem.h"
#include "app/trending/trend_viewer/src/HistoricalDataLoader.h"
#include "app/trending/trend_viewer/src/IDataUpdateObserver.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/TrendViewerException.h"

#include "core/naming/src/INamedObject.h"

#include "core/exceptions/src/ScadaProxyException.h"

#include "core/threads/src/Thread.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    //
    // GenericItem
    //
    GenericItem::GenericItem(unsigned long entityKey, const std::string& itemName, const std::string& typeIdentifier,
        double minimumLimit, double maximumLimit, const std::string& unitLabel, bool isBooleanType, const std::string& agentName,
        unsigned long locationKey, int precision, std::auto_ptr<IUpdateSubscriber> subscriber)
    : m_name(itemName),
      m_entityKey(entityKey),
      m_typeIdentifier(typeIdentifier),
      m_minimumLimit(minimumLimit),
      m_maximumLimit(maximumLimit),
      m_isBoolean(isBooleanType),
      m_updateSubscriber(subscriber),
      m_dataLoader(new HistoricalDataLoader),
      m_customAxisLabel(unitLabel),
      m_shouldDisplayLabel(false),
      m_agentName(agentName),
      m_locationKey(locationKey),
      m_labelsRetrieved(false),
      m_precision(precision)
    {
        TA_ASSERT(m_updateSubscriber.get() != NULL, "UpdateSubscriber is NULL");

        m_scaleFactor = 100.0 / (m_maximumLimit - m_minimumLimit);
    }


    //
    // ~GenericItem
    //
    GenericItem::~GenericItem()
    {
    }

    
    //
    // retrieveHistoricalData
    //
    PlotData GenericItem::retrieveHistoricalData(time_t startDateTime, time_t endDateTime, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod)
    {
        return m_dataLoader->retrieveDataFromDatabase(m_entityKey, startDateTime, endDateTime, tableName, columnName, samplePeriod);
    }


    //
    // retrieveHistoricalData
    //
    PlotData GenericItem::retrieveHistoricalData(time_t time, const std::string& tableName, const std::string& columnName, unsigned long samplePeriod)
    {
        return m_dataLoader->retrieveDataFromDatabase(m_entityKey, time, tableName, columnName, samplePeriod);
    }

    //
    // setLiveMode
    //
    void GenericItem::setLiveMode(bool isLive)
    {
        if(isLive == true)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Listening for Field Updates");

            m_updateSubscriber->subscribeToMessages();
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No longer listening for Field Updates");

            m_updateSubscriber->unsubscribeToMessages();
        }
    }


    //
    // setUpdateObserver
    //
    void GenericItem::setUpdateObserver(IDataUpdateObserver* updateObserver)
    {
        m_updateSubscriber->setObserver(updateObserver);
    }

    
    //
    // setStateLabel
    //
    void GenericItem::setStateLabel(int state, const std::string& label)
    {
        std::map<int, std::string>::iterator iter = m_stateLabelMap.find(state);
        
        if(iter != m_stateLabelMap.end())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Label for State: %d already exists and will be overwritten", state);
        }

        m_stateLabelMap[state] = label;
    }


    //
    // getStateLabels
    //
    std::map<int, std::string> GenericItem::getStateLabels()
    {
        // Don't return until we've attempted to retrieve the labels.
        // Timeout after 5 seconds and return an empty vector
        for(int i=0; i < 50; ++i)
        {
            if(!m_labelsRetrieved)
            {
                TA_Base_Core::Thread::sleep(100);
            }
            else
            {
                break;
            }
              
        }

        if(m_stateLabelMap.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve state labels for: %s", m_name.c_str());
        }
		else
		{
			//TD17605 - update the maximum limit once we get the correct label Map size
			m_maximumLimit = m_stateLabelMap.size() - 1;
			m_scaleFactor = 100.0 / (m_maximumLimit - m_minimumLimit);
		}

        return m_stateLabelMap;
    }
}

#endif // GenericItemCpp_602838B6_631C_4b97_A653_3AD5359B4C28__INCLUDED_
