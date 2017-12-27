/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:     $File$
  * @author:    HoaVu
  * @version:   $Revision$
  *
  * Last modification:  $DateTime$
  * Last modified by:   $Author$
  *
  * Description:
  *
  * Object of the class TransActiveDataSource implements the interface ITransActiveDataSource and
  * performs the following functionalities:
  *
  *     1) Initialise the OPC Server (on the main thread)
  *
  *     2) Monitors callback events from the OPC Server for reading and writing of TransActive
  *        DataPoint's attributes.
  *
  *     3) Monitors callback events from the OPC Server for subscribing and unsubscribing of
  *        TransActive DataPoints's attributes.
  *
  */

// Disable identifier truncated in debug warning
#pragma warning ( disable:4786 )

#include <set>
#include "ace/OS.h"
#include "cots/WinTech/OPCServerToolKit/opc_ae.h"
#include "cots/WinTech/OPCServerToolKit/opcda.h"
#include "cots/WinTech/OPCServerToolKit/WTOPCsvrAPI.h"

#include "app/scada/OpcBridgeLib/src/TransActiveDataSource.h"
#include "app/scada/OpcBridgeLib/src/DisplayProxyManager.h"
#include "app/scada/OpcBridgeLib/src/OpcBridge.h"
#include "app/scada/OpcBridgeLib/src/AttributeMapManager.h"
#include "app/scada/OpcBridgeService/src/OpcBridgeService.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/opctag_alias/src/IOpcTagAlias.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/Timer.h"
#include <crtdbg.h>
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    /*
     * Initialise statics
     */
    OpcBridge* TransActiveDataSource::m_opcBridge = NULL;
    AttributeUpdateProcessorPool* TransActiveDataSource::m_updateProcessorPool = NULL;
    bool TransActiveDataSource::m_enableCallback = false;
    int TransActiveDataSource::m_numWorkers = 10;

    /*
     * Public member functions
     */

    TransActiveDataSource::TransActiveDataSource(OpcBridge* opcBridge)
    :
        m_initialised ( false ),
        m_isTerminating (false),
        m_updateTagPeriodically(false),
		m_bRegistered (false),
        m_updateTagInterval(500),
		m_registerExceedTime(20000),
		m_timerSemphore(0),
		m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance())
    {
        FUNCTION_ENTRY("TransActiveDataSource()");

        TA_ASSERT(m_opcBridge == NULL, "Attempt to create a second TransActiveDataSource");

        m_opcBridge = opcBridge;

        FUNCTION_EXIT;
    }   


    OpcBridge& TransActiveDataSource::getOpcBridge()
    {
        if (m_opcBridge == NULL)
        {
            TA_THROW(TA_Base_Core::ScadaCommonException("OpcBridge not available"));
        }

        return *m_opcBridge;
    }


    TransActiveDataSource::~TransActiveDataSource()
    {
        FUNCTION_ENTRY("~TransActiveDataSource()");
        
        //TA_ASSERT(false == m_initialised, "WtOPCsvr still initialised");

        // stop thread first
        terminateAndWait();

		if (m_updateProcessorPool != NULL)
		{
			delete m_updateProcessorPool;
			m_updateProcessorPool = NULL;
		}

		m_opcBridge = NULL;
       
        FUNCTION_EXIT;
    }

    
    /*
     ** Functions required for the Win-tech OPC Server Library
     */

    //
    // initialiseWtOPCsvr
    //
	void TransActiveDataSource::initialiseWtOPCsvr (const GUID& ClsIdSvr, const std::string svrName, 
		const std::string svrDesc, const std::string svrExePath, 
		const unsigned int svrRate, const std::string vendorInfo)
	{
		FUNCTION_ENTRY("initialiseWtOPCsvr()");

		TA_ASSERT (false == m_initialised, "WtOPCsvr already initialised");

		try
		{
			if (true == TA_Base_Core::RunParams::getInstance().isSet("EnableWintechDebug"))
			{
				// callback for debug logging
				EnableEventMsgs ( & EventHandler );
			}

			int svrUpdateRate = 50;
			svrUpdateRate = atoi(TA_Base_Core::RunParams::getInstance().get("SvrUpdateRate").c_str());

			if (svrUpdateRate < 50)
			{
				svrUpdateRate = 50;
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "OpcBridgeSvr update rate : %d", svrUpdateRate);

			registerOPCsvr(CLSID_SVR, SVR_NAME, SVR_DESC, SVR_EXE_PATH);

			// initialise COM/DCOM, Security, and creates the Server ClassFactory
			if ( InitWTOPCsvr ( (BYTE *) & ClsIdSvr, svrUpdateRate ) == TRUE )
			{
				//we unregisterServer again, why so trouble? there are some deep reason
				UnregisterServer ( (BYTE *) & ClsIdSvr, svrName.c_str() );

				m_initialised = true;

				unsigned long sizeHashing = 0;
				sizeHashing = atol(TA_Base_Core::RunParams::getInstance().get("OpcMapHashSize").c_str());

				if (sizeHashing > 0)
				{
					sizeHashing = SetHashing(sizeHashing);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "OpcBridgeSvr hash size : %u", sizeHashing);
				}

				OpcTagManager& opcTagManager = m_opcBridge->getOpcTagManager();
				AttributeMapManager& attributeMapManager = m_opcBridge->getAttributeMapManager();

				// create ThreadedWorker objects, based on the runtime parameter
				// "TagNotificationThreadedWorkers". Use the default number if not defined
				if ( true == TA_Base_Core::RunParams::getInstance().isSet ( "TagNotificationThreadedWorkers" ) )
				{
					int workers = atoi ( TA_Base_Core::RunParams::getInstance().get( "TagNotificationThreadedWorkers" ).c_str() );
					m_numWorkers = workers > 0 ? workers : m_numWorkers;
				}

				bool dynamicCreateTag = false;
				bool dynamicRemoveTag = false;
				if ( 0 == TA_Base_Core::RunParams::getInstance().get("DynamicCreateTag").compare("YES") )
				{
					dynamicCreateTag = true;
				}
				if ( 0 == TA_Base_Core::RunParams::getInstance().get("DynamicRemoveTag").compare("YES") )
				{
					dynamicRemoveTag = true;
				}

				if ( dynamicCreateTag || dynamicRemoveTag )
				{
					m_updateProcessorPool = new AttributeUpdateProcessorPool( m_numWorkers, *this );
					for (int i = 1; i <= m_numWorkers; ++i)
					{
						std::ostringstream tag;
						tag << OpcBridge::TRANSACTIVEDATASOUCE_UPDATEITEM_QUEUESIZE << i;

						opcTagManager.createStatisticalTag( tag.str() );
					}
				}

				// Create tags to collect statistic
				opcTagManager.createStatisticalTag( OpcBridge::ATTRIBUTES_COUNT );


				if (true == TA_Base_Core::RunParams::getInstance().isSet("UpdateTagPeriodically"))
				{
					m_updateTagPeriodically = true;
				}

				if (true == TA_Base_Core::RunParams::getInstance().isSet("UpdateTagInterval"))
				{
					unsigned interval = atoi(TA_Base_Core::RunParams::getInstance().get("UpdateTagInterval").c_str());

					// the max update interval is 1 hour
					if (interval > 500 && interval <= 1800000)
					{
						m_updateTagInterval = interval;
					}
				}
				
				std::string precreateString = TA_Base_Core::RunParams::getInstance().get("PrecreateOpctag");
				if (0 == precreateString.compare("ALIAS"))
				{
					attributeMapManager.preCreateAliasOpcTags();								
				}
				else
				{
					attributeMapManager.preCreateOpcTags( dynamicRemoveTag );				
				}
				
				//now all pre-create data are prepared, so we begin to synchronize data from agent
				//this step should do at last, or will lose update according to current logic.
				DisplayProxyManager::getInstance()->startSynchronise();

				if (dynamicCreateTag)
				{
					// callback indicating client application is subscribing a tag
					EnableUnknownItemNotification ( &UnknownTagNotificationHandler );
				}

				if (dynamicRemoveTag)
				{
					// callback indicating the tag is no longer subscribed by any client
					EnableItemRemovalNotification ( &RemoveTagNotificationHandler );
				}

				SetVendorInfo ( vendorInfo.c_str() );

				Deactivate30MinTimer( "JVRPS53R5V64226N62H4" );
				// start thread
				start();
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to initialise WtOPCSvr");
			}			
		}
		catch ( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception", "WtOPCSvr library threw an unexpected exception");
		}

		FUNCTION_EXIT;
	}

	bool TransActiveDataSource::registerOPCsvr(const GUID& ClsIdSvr, const std::string svrName, 
                                               const std::string svrDesc, const std::string svrExePath)
	{
		FUNCTION_ENTRY("registerOPCsvr");
		
		bool isSuccess = true;

		if ( UpdateRegistry ( ( BYTE * ) & ClsIdSvr,
			svrName.c_str(),
			svrDesc.c_str(),
			svrExePath.c_str() ) == TRUE )
		{
			// Register the service that will have the OPC Server
			// only if the Mode parameter has not been specified.
			if( false == TA_Base_Core::RunParams::getInstance().isSet ( RPARAM_SERVICEDEBUG ))
			{
				// Assume service mode
				AddLocalServiceKeysToRegistry( OPC_BRIDGE_SERVICE_NAME.c_str() );
			}
		}
		else
		{
			isSuccess = false;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to register WTOPCSvr");
		}

		FUNCTION_EXIT;
		
		return isSuccess;
	}
	

    void TransActiveDataSource::uninitialiseWtOPCsvr(const GUID& ClsIdSvr, const std::string svrName)
    {
        FUNCTION_ENTRY("uninitialiseWtOPCsvr()");

        if (m_initialised)
        {
            try
            {
                // remove the TransActiveOPCBridge entry from Windows Registry
                if ( m_bRegistered )					
                {
					if( UnregisterServer ( (BYTE *) & ClsIdSvr, svrName.c_str() ) == FALSE )
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to unregister WtOPCSvr");
					}
					else
					{
						// uninitializes COM/DCOM stuffs
						if ( UninitWTOPCsvr() == TRUE )
						{
							m_initialised = false;
						}
						else
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to uninitialise WtOPCSvr");
						}
					}
                    
                }
                else
                {
                    // uninitializes COM/DCOM stuffs
                    if ( UninitWTOPCsvr() == TRUE )
                    {
                        m_initialised = false;
                    }
                    else
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to uninitialise WtOPCSvr");
                    }
                }
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception", "WtOPCSvr library threw an unexpected exception");
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "WtOPCSvr not initialised");
        }

        FUNCTION_EXIT;
    }


    void CALLBACK TransActiveDataSource::UnknownTagNotificationHandler ( LPSTR path, LPSTR name )
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Unknown Tag:%s from WinTech", name);

        if ( !m_enableCallback ) return;

        std::string tagName((char*)name);
 
        OpcTagManager& opcTagManager = m_opcBridge->getOpcTagManager();
        AttributeMapManager& attributeMapManager = m_opcBridge->getAttributeMapManager();

        try
        {            
            // Check validity of tagName;
            unsigned long	  entityKey   = 0;
			OPCCacheData	  opcEntityData;
			EOPCATTRIBUTETYPE attrType;
            if ( attributeMapManager.isValidTagName(tagName, entityKey, attrType, opcEntityData) )
            {
                addUpdateProcessorPoolItem( entityKey, tagName, Add, opcEntityData, attrType);
            }
            else
            {
				//here will create a tag if it is invalid, it will prevent the second creating 
				//if the same invalid tag call back again
				opcTagManager.getOpcTag(tagName);
                LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "MarkB--Invalid tagName - %s", tagName.c_str() );
            }
        }
        catch (const TA_Base_Core::ScadaCommonException &e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"MarkB--ScadaCommonException", e.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"UnknownException", "MarkB--Unknown exception caught in UnknownTagNotificationHandler");
        }
    }


    void CALLBACK TransActiveDataSource::RemoveTagNotificationHandler ( HANDLE handle, LPSTR path, LPSTR name )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received RemoveTagNotification for %s from WinTech", name);
        
        if ( !m_enableCallback ) return;

        std::string tagName((char*)name);
        OpcBridge& opcBridge = getOpcBridge();
        AttributeMapManager& attributeMapManager = opcBridge.getAttributeMapManager();

        if (true == attributeMapManager.isPreCreatedOpcTag(tagName))
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Not removing pre-loaded OpcTag - %s", tagName.c_str() );
            return;
        }

        OpcTagManager& opcTagManager = opcBridge.getOpcTagManager();

        try
        {
            unsigned long		entityKey = 0;
			EOPCATTRIBUTETYPE	attrType;
			OPCCacheData		opcEntityData;
            if (true == attributeMapManager.isValidTagName(tagName, entityKey, attrType, opcEntityData))
            {
                attributeMapManager.remove(entityKey, attrType);
            }
			else
			{
				opcTagManager.deleteOpcTag(tagName);
			}                
        }
        catch (const TA_Base_Core::ScadaCommonException &e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo,"ScadaCommonException", e.what() );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"UnknownException", "Unknown exception caught in RemoveTagNotificationHandler");
        }
    }


    void CALLBACK TransActiveDataSource::EventHandler ( LPSTR eventMsg )
    {
        if ( NULL == eventMsg )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "WinTech try to log NULL message");
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "WinTech: %s", eventMsg);
        }

        return;
    }


    void TransActiveDataSource::addUpdateProcessorPoolItem( unsigned long entityKey,
                                                            std::string tagName,
                                                            AttributeUpdateType type,
															OPCCacheData& opcData, 
															EOPCATTRIBUTETYPE attrType,
                                                            TA_Base_Core::IOpcTagAlias* opcTagAlias)
    {
        // If the OpcBridge is not available, shouldn't be starting a thread
        if (m_opcBridge == NULL || m_updateProcessorPool == NULL)
        {
            TA_THROW(TA_Base_Core::ScadaCommonException("UpdateProcessorPool not available"));
        }

        boost::shared_ptr<AttributeUpdate> attributeUpdate(
            new AttributeUpdate(entityKey, tagName, type, attrType, opcData, opcTagAlias) );

        m_updateProcessorPool->queueItemReplace(entityKey, attributeUpdate);
    }
      
    void TransActiveDataSource::queueProcessorPoolCallback(boost::shared_ptr<AttributeUpdate> update)
    {
        FUNCTION_ENTRY("queueProcessorPoolCallback()");

        try
        {
            switch (update->m_type)
            {
            case Add:
                m_opcBridge->getAttributeMapManager().dynamicCreateTag(
					update->m_entityKey, update->m_tagName, 
					update->m_attrType, update->m_opcEntityData);
                break;
            
            case Remove:
                m_opcBridge->getAttributeMapManager().remove(update->m_entityKey, update->m_attrType);
                break;

            //case UpdateProxy:
            //    getOpcBridge().getAttributeMapManager().updateProxy(update->m_entityKey);
            //    break;
			default:
				break;
            }
        }
        catch (const TA_Base_Core::ScadaCommonException &e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"ScadaCommonException", e.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo,"UnknownException", 
                                "Unknown exception caught executing ProcessUnknownTagWorkItem");
        }

        FUNCTION_EXIT;
    }


    void TransActiveDataSource::run()
    {	
		m_timerUtility.startPeriodicTimeOutClock(this, 500, false);

		bool bDynmicCreatTag = false;
		if ( !(TA_Base_Core::RunParams::getInstance().isSet("PrecreateOpctag")) &&
			(0 == TA_Base_Core::RunParams::getInstance().get("DynamicCreateTag").compare("YES")) )
		{
			bDynmicCreatTag = true;
		}

		unsigned long reisterExceedTime = 0;
		reisterExceedTime = atol(TA_Base_Core::RunParams::getInstance().get("RegisterExceedTime").c_str());
		// 0s < m_registerExceedTime < 90s
		if (reisterExceedTime < 90000 && reisterExceedTime > 0)
		{
			m_registerExceedTime = reisterExceedTime;
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--m_registerExceedTime:%d", m_registerExceedTime);

		AttributeMapManager& manager = m_opcBridge->getAttributeMapManager();
		unsigned long entityCnt = manager.getPrecreateTagEntityCnt();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--Precreate tag entity count:%d", entityCnt);

		long timeBegin = -1;
		
        // update tag statistics periodically
        while (!m_isTerminating)
        {
			try
			{
				if ( !m_bRegistered )
				{
					// when all tag created and finished first update, we can write our OPC Server info to regedit
					// this will solve OpcBridge start too slow when there are too many tag monitored by clients
					// during the period of OpcBridge launch,our client(schematic) will light up more quick. To 
					// make sure OPC Server register in time, we will judge whether OPC Tag finished create and 
					// first update and the time, when over time(70s),we will register OPC Server despite of whether 
					// all OPC Tag finish first update, because this function(finishedFirstUpdate)do not work correctly
					// in some situation.
					
					bool bfirstUpdate = false;
					bool bExceedTime = false;				
					bfirstUpdate = (DisplayProxyManager::getInstance()->getUpdateCnt() >= entityCnt);
					if (timeBegin == -1)
					{
						timeBegin = GetTickCount();
					}
					else
					{ 
						bExceedTime = ((GetTickCount() - timeBegin) > m_registerExceedTime);
					}							

					if (bDynmicCreatTag || bfirstUpdate || bExceedTime)
					{
						m_bRegistered = registerOPCsvr(CLSID_SVR, SVR_NAME, SVR_DESC, SVR_EXE_PATH);

						m_timerUtility.stopPeriodicTimeOutClock(this);
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--registerOPCsvr dynamicCreate:%d, exceedTime:%d, bUpdate:%d, bSuccess:%d", 
									bDynmicCreatTag, bExceedTime, bfirstUpdate, m_bRegistered);

						m_timerUtility.startPeriodicTimeOutClock(this, m_updateTagInterval, false);
					}
				}
				else
				{	
					if (m_updateProcessorPool != NULL)
					{
						// update statistical pool queue size tags every 20 * m_updateTagInterval instead of update every time.
						static int updateQueueSizeCounter = 0;
						++updateQueueSizeCounter;
						if (updateQueueSizeCounter == 20)
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "++test begin updateStatisticalTag");
							OpcBridge& opcBridge = getOpcBridge();
							OpcTagManager& opcTagManager = opcBridge.getOpcTagManager();
							updateQueueSizeCounter = 0;
							std::vector<unsigned long> sizes = m_updateProcessorPool->getQueueSizes();
							std::vector<unsigned long>::iterator sizeIter;
							int count = 1;
							for (sizeIter = sizes.begin(); sizeIter != sizes.end(); sizeIter++)
							{
								std::ostringstream tag;
								tag << OpcBridge::TRANSACTIVEDATASOUCE_UPDATEITEM_QUEUESIZE << count;
								
								opcTagManager.updateStatisticalTag( tag.str(), (long)(*sizeIter) );
								
								count++;
							}
							LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "++test end updateStatisticalTag");
							
						}
					}
					
					// update statistical pool attribute count tags every 20 * m_updateTagInterval instead of update every time.				
					static int updateAttributeCounter = 0;
					updateAttributeCounter++;
					if (updateAttributeCounter == 20)
					{
						updateAttributeCounter = 0;
						
						static long total = 0;
						if(Attribute::getTotal() != total)
						{
							updateAttributeCounter = 0;
							total = Attribute::getTotal();
							OpcBridge& opcBridge = getOpcBridge();
							OpcTagManager& opcTagManager = opcBridge.getOpcTagManager();
							opcTagManager.updateStatisticalTag( OpcBridge::ATTRIBUTES_COUNT, total );
							LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "Total Attribues are %u", total);
						}
					}
					
					if (m_updateTagPeriodically)
					{
						AttributeMapManager& attributeMapManager = getOpcBridge().getAttributeMapManager();
						LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "++test begin refreshAllTags");
						attributeMapManager.refreshAllTags();
						LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "++test end refreshAllTags");
					}
				}
			}
			catch(TA_Base_Core::TransactiveException& te)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Caught exception: %s.", te.what());
			}
			catch (...) 
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Caught unknown exception.");
			}

			m_timerSemphore.wait();
		}

		m_timerUtility.stopPeriodicTimeOutClock(this);
    }

    void TransActiveDataSource::terminate()
    {
        m_isTerminating = true;
		m_timerSemphore.post();
    }

	void TransActiveDataSource::timerExpired(long timerId, void* userData)
	{
		m_timerSemphore.post();
	}

	void TransActiveDataSource::terminateAndWaitThreadPool()
	{
		if (m_updateProcessorPool != NULL)
		{
			m_updateProcessorPool->stopProcessing();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--terminate and wait update thread pool");
		}
	}
}
