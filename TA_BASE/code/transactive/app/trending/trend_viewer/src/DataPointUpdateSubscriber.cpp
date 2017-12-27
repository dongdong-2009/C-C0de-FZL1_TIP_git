#if !defined(DATA_POINT_UPDATE_SUBSCRIBER_CPP)
#define DATA_POINT_UPDATE_SUBSCRIBER_CPP

/**
  * DataPointUpdateSubscriber.h
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/DataPointUpdateSubscriber.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class implements AbstractUpdateSubscriber and represents a subscriber that listens
  * for data point update events. When a data point value has changed, this object will notify
  * the graphed item associated with this data point so the graph can be udpated.
  */

#include "app/trending/trend_viewer/src/DataPointUpdateSubscriber.h"
#include "app/trending/trend_viewer/src/IDataUpdateObserver.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/TimeFunction.h"

#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

using namespace TA_Base_Core;

namespace TA_Base_App
{

    //
    // DataPointUpdateSubscriber
    //
    DataPointUpdateSubscriber::DataPointUpdateSubscriber(unsigned long entityKey, IDataUpdateObserver* dataUpdateObserver)
        : AbstractUpdateSubscriber(dataUpdateObserver)
          //m_entityKey(entityKey),
          //m_locationKey(0),
          //m_isSubscribed(false)
    {
        setEntityKey(entityKey);
        setLocationKey(0);
        setSubscribed(false);
        isFirstTimeSubscribed = true;
    }

	void DataPointUpdateSubscriber::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
        if (!isSubscribed()) 
        {
            return;
        } 

		if ( updateType == TA_Base_Bus::ValueStateUpdate ||
             updateType == TA_Base_Bus::AlarmSummaryUpdate || //limin++, TD20740
             updateType == TA_Base_Bus::AckSummaryUpdate ||   //limin++, TD20740
             updateType == TA_Base_Bus::ConfigAvailable )
		{
			unsigned long status = 0;
			status = m_dataPointProxy->getStatus();
			// TD17776 - ignore all overriden value updates 
			if (status == TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE)
				return;

            if(entityKey == m_dataPointProxy->getEntityKey())
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataPointUpdateSubscriber::processProxyUpdate for entity: %lu", entityKey);

                time_t timestamp = 0;                
                double value = 0;
                
                timestamp =  m_dataPointProxy->getTimestamp();	
				
				TA_Base_Core::DateFormat dateFormat;
                std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1); 
				COleDateTime timeUpdated(timestamp);
				CString timeStr = timeUpdated.Format(dateFormatStr.c_str());
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Update received: %s", timeStr);
                
			    try
			    {
				    TA_Base_Bus::DpValue dpvalue= m_dataPointProxy->getValue();
                    switch(dpvalue.getType())
                    {
                    case TA_Base_Bus::DPT_INT16_TYPE:
                    case TA_Base_Bus::DPT_INT32_TYPE:
                    case TA_Base_Bus::DPT_INT16S_TYPE:
                    case TA_Base_Bus::DPT_INT32S_TYPE:
                    case TA_Base_Bus::DPT_UINT16_TYPE:
                    case TA_Base_Bus::DPT_UINT32_TYPE:
                    case TA_Base_Bus::DPT_IEEE32_TYPE:
                        value = static_cast <double>(dpvalue.getFloat());
                    
                        break;
                    
                    case TA_Base_Bus::DPT_ENUM_TYPE:
                        value = static_cast <double>(dpvalue.getEnum());
                    
                        break;
                    
                    case TA_Base_Bus::DPT_BOOL_TYPE:
                        if(static_cast <bool> (dpvalue.getBoolean()))
                        {
                            value = 1;
                        }
                        else
                        {
                            value = 0;
                        }                    
                        break;
                    
                    case TA_Base_Bus::DPT_TEXT_TYPE:
                        TA_ASSERT(false, "Cannot trend text data points"); 
                    }
                }
                catch( const TA_Base_Core::TransactiveException & te )
                {

                    std::ostringstream desc;
                    desc << "EntityKey(" << entityKey << "); Reason: " << te.what();
                    LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", desc.str() );
                }
			    catch( ... )
			    {

                    std::ostringstream desc;
                    desc << "EntityKey(" << entityKey << "); Reason: Unknown";
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", desc.str() );
			    }

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Received DataPointStateUpdate message with entity key: %lu, timestamp: %lu and value: %lf",
                    entityKey, timestamp, value );
                
                //TD17776 - not necessary as trend viewer will only listen to 
				//           value updates with QUALITY_GOOD_NO_SPECIFIC_REASON quality
				bool goodStatus = (status == TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON) ? true : false;
				//++TD17776
                                 //TD17883 - must avoid plotting data when timestamp is zero
				if (timestamp != 0)
				{
					newDataReceived(timestamp, value, goodStatus);    
				}
            }
		}
	}

    //
    // subscribeToMessage
    //
    void DataPointUpdateSubscriber::subscribeToMessages()
    {
        FUNCTION_ENTRY("subscribeToMessage");

        // Should only have subscribed AFTER the observer has been initialised.
        TA_ASSERT(isObserverSet() != NULL, "Data Update Observer has not been set");

        // Set the location key if it hasn't been set
        if(!isLocationKeySet())
        {
            retrieveLocationKey();

            //TA_ASSERT(getLocationKey() != 0, "Location key is 0");
        }

        if(isSubscribed() == false)
        {
            //instead of subscribing to the comms message, now we use proxy to get the update
            //
            // Create the message subscriber and subscribe the subscriber with the
            // MessageSubscriptionManager by message context
            //
//            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
//                TA_Base_Core::DataPointStateUpdate::DataPointValueStatusUpdate,
//                this, 
//                getEntityKey(), 
//                0, 
//                getLocationKey(), 
//                NULL);
			//TD17935
            //if ( isFirstTimeSubscribed)
            {
                TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(getEntityKey(), *this, m_dataPointProxy);
                //isFirstTimeSubscribed = false;
            }
            setSubscribed(true);
        }

        FUNCTION_EXIT;
    }


    //
    // unsubscribeToMessage
    //
    void DataPointUpdateSubscriber::unsubscribeToMessages()
    {
        FUNCTION_ENTRY("unsubscribeToMessage");

        if(isSubscribed() == true)
        {
            setSubscribed(false);
			//TD17935 - must remove the created instance for the ScadaProxyFactory
			TA_Base_Bus::ScadaProxyFactory::removeInstance(); 
        }

        FUNCTION_EXIT;
    }


    //
    // retrieveLocationKey
    //
    void DataPointUpdateSubscriber::retrieveLocationKey()
    {
        unsigned long locationKey = 0;

        try
        {
            std::auto_ptr<TA_Base_Core::IEntityData> entityData(EntityAccessFactory::getInstance().getEntity(getEntityKey()));
            locationKey = entityData->getLocation();
        }
        catch(const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "An error occurred while retrieving location key. Setting location key to 0");
        }
        catch(const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "An error occurred while retrieving location key. Setting location key to 0");
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown exception caught while getting location key");

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "An error occurred while retrieving location key. Setting location key to 0");
        }

        setLocationKey(locationKey);
    }

}   // TA_Base_App

#endif //DATA_POINT_UPDATE_SUBSCRIBER_CPP
