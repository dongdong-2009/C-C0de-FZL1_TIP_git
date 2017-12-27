/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataNodeAgentProxy.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// DataNodeAgentProxy.cpp: implementation of the DataNodeAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "stdafx.h"
#include <sstream>
#include "time.h"
#include "boost/cast.hpp"

#include "app/scada/EquipmentStatusViewer/src/DataNodeAgentProxy.h"
#include "app/scada/EquipmentStatusViewer/src/EquipmentStatusViewer.h"
#include "app/scada/EquipmentStatusViewer/src/EquipmentStatusViewerDlg.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

    DataNodeAgentProxy::DataNodeAgentProxy( TA_Base_Core::CorbaNameList corbaNames, CEquipmentStatusViewerDlg* dlg )
        :
        m_toBeTerminated(false),
		//CL-20798 zhangjunsheng
		//m_pollPeriod(1000),
		//m_timerUtility(TA_Base_Core::SingletonTimerUtil::getInstance()),
        m_dlg(dlg)
    {
        std::string  dnobjectpostfix = TA_Base_Bus::DN_BULK_POLLED_INTERFACE;
        for (TA_Base_Core::CorbaNameList::iterator dnobj = corbaNames.begin() ; dnobj != corbaNames.end(); ++dnobj) 
        {
            DataNodeAgentInterface* DnagentInterface = new DataNodeAgentInterface();
			//zhangjunsheng TD CL_20798
			DnagentInterface->setObjectTimeout(20); // in secs
            //DnagentInterface->setObjectTimeout(10); // in secs
            DnagentInterface->setCorbaName ( (*dnobj).getAgentName(), (*dnobj).getAgentName() + dnobjectpostfix);
            m_agentMap.insert(std::pair<DataNodeAgentInterface *, unsigned long>(DnagentInterface, 0));
        }
    }
    
    DataNodeAgentProxy::~DataNodeAgentProxy()
    {
        terminateAndWait();
    }
    
    void DataNodeAgentProxy::terminate()
    {
        m_toBeTerminated = true;
		//CL-20798 zhangjunsheng
		//m_timerCondition.signal();
    }
	

    //CL-20798 zhangjunsheng
    //void DataNodeAgentProxy::timerExpired(long timerId, void* userData)
    //{
    //    m_timerCondition.signal();
    //}

    void DataNodeAgentProxy::run()
    {
		//CL-20798 zhangjunsheng
		// initialise the timers
        //m_timerUtility.startPeriodicTimeOutClock(this, m_pollPeriod, false);
		
        while(false == m_toBeTerminated)
        {
            std::map<DataNodeAgentInterface *, unsigned long >::iterator agentitr;
            for (agentitr = m_agentMap.begin() ; agentitr != m_agentMap.end(); ++agentitr)
            {
                TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dataNodeUpdates = NULL;
                try
                {
                    if (0 == agentitr->second )
                    {
						CORBA_CALL_RETURN( agentitr->second,
						                   (*(agentitr->first)),
						                   regiesterDisplayDataNodeClient,
						                   () );

                        if (agentitr->second > 0)
                        {
							//first time, get all update from agent
							CORBA_CALL_RETURN( dataNodeUpdates,
							                   (*(agentitr->first)),
							                   getDisplayDataNodeUpdate,
							                   ( agentitr->second, true ) );
                        }
                    }
                    else
                    {
						CORBA_CALL_RETURN( dataNodeUpdates,
						                   (*(agentitr->first)),
						                   getDisplayDataNodeUpdate,
						                   ( agentitr->second, false ) );
                       
                    }
                    // if got data point updates from the agent
                    if (NULL != dataNodeUpdates)
                    {
                        unsigned int updates = 0;
                        updates = dataNodeUpdates->length();        
                        try
                        {
                            for (unsigned int i = 0; i < updates; ++ i)
                            {
                                TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState&  dataNodeUpdate = (*dataNodeUpdates)[i];
                                TA_Base_Bus::DataNodeDisplayState toupdate;
                                TA_Base_Bus::convertDnDisplayUpdate(dataNodeUpdate, toupdate);
                                if (!toupdate.ONITP.empty())
                                {
                                    if (toupdate.ONITP.compare("N") != 0)
                                    {
                                        m_dlg->PostMessage( WM_DN_PULK_POLL_UPDATE, toupdate.entityKey, 0);
                                    }
                                }
                            }
                        }
                        catch (...) 
                        {
                            LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
                        }

                        if ( NULL != dataNodeUpdates )
                        {
                            delete dataNodeUpdates;
                            dataNodeUpdates = NULL;
                        }
                    }                
                }//end of try
                catch ( const TA_Base_Core::ObjectResolutionException & ex )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
					
					//CL-20798 zhangjunsheng
					setToBadNotConnectedQuality ();
				}
                catch ( const TA_Base_Core::ScadaProxyException & ex )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException" ,ex.what() );
                
					//CL-20798 zhangjunsheng
					setToBadNotConnectedQuality ();
				}
                catch ( const TA_Base_Core::BadParameterException & )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
                
					//CL-20798 zhangjunsheng
					setToBadNotConnectedQuality ();
				}
                catch ( const TA_Base_Core::OperationModeException & )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
                
					//CL-20798 zhangjunsheng
					// force to rebind named object next time.
					std::map<DataNodeAgentInterface *, unsigned long >::iterator agentitr;
					for (agentitr = m_agentMap.begin() ; agentitr != m_agentMap.end(); ++agentitr)
					{
						(agentitr->first)->clear();
					}
					
                    // set all data point to bad quality status and restart the polling cycle
                    // by requesting the agent for new observer ID
					//CL-20798 zhangjunsheng
					setToBadNotConnectedQuality ();
                }
                catch ( const CORBA::Exception & ex )
                {
                    LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
   
					//CL-20798 zhangjunsheng
					// force to rebind named object next time.
					std::map<DataNodeAgentInterface *, unsigned long >::iterator agentitr;
					for (agentitr = m_agentMap.begin() ; agentitr != m_agentMap.end(); ++agentitr)
					{
						(agentitr->first)->clear();
					}

					setToBadNotConnectedQuality ();
				}
                catch ( ... )
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");

					//CL-20798 zhangjunsheng
					setToBadNotConnectedQuality ();
                }
            }//end of for   
			//CL-20798 zhangjunsheng
			//m_timerCondition.wait();
			TA_Base_Core::Thread::sleep(1000);
        }

		//CL-20798 zhangjunsheng
		//m_timerUtility.stopPeriodicTimeOutClock(this);
        //Empty the agentinterface map, since it is only use for ACTIVE_PROFILE_ALL_REPORT
        std::map<DataNodeAgentInterface *, unsigned long >::iterator agentitr;
        for (agentitr = m_agentMap.begin() ; agentitr != m_agentMap.end(); ++agentitr)
        {
			//CL-20798 zhangjunsheng
			//if (agentitr->second != 0)
			if ( agentitr->second>0 )
            {
                try
                {   //unregister first
					CORBA_CALL( (*(agentitr->first)),
					            unregiesterDisplayDataNodeClient,
					            ( agentitr->second ) );
				}
                catch (...) 
                {
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", "DataNodeAgentProxy::run(), when call unregiesterDisplayDataNodeClient()" );
					LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
                }
            }
			delete agentitr->first;
        }
        m_agentMap.clear();
    }

	//CL-20798 zhangjunsheng
	void DataNodeAgentProxy::setToBadNotConnectedQuality ()
    {		
		std::map<DataNodeAgentInterface *, unsigned long >::iterator agentitr;
		for (agentitr = m_agentMap.begin() ; agentitr != m_agentMap.end(); ++agentitr)
		{
			agentitr->second = 0;
		}		
    }

	//CL-20798 zhangjunsheng
	/*std::vector<unsigned long> DataNodeAgentProxy::getCurrentEntitiesToShow()
    {
        std::vector<unsigned long> toReturn;
        std::map<DataNodeAgentInterface *, unsigned long >::iterator agentitr;
        for (agentitr = m_agentMap.begin() ; agentitr != m_agentMap.end(); ++agentitr)
        {
            TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dataNodeUpdates = NULL;
            try
            {
                if (0 == agentitr->second )
                {
					CORBA_CALL_RETURN( agentitr->second,
					                   (*(agentitr->first)),
									   regiesterDisplayDataNodeClient,
									   () );
					
                    if (agentitr->second > 0)
                    {
						//first time, get all update from agent
						CORBA_CALL_RETURN( dataNodeUpdates,
							(*(agentitr->first)),
							getDisplayDataNodeUpdate,
							( agentitr->second, true ) );
                    }
                }
                // if got data point updates from the agent
                if (NULL != dataNodeUpdates)
                {
                    unsigned int updates = 0;
                    updates = dataNodeUpdates->length();        
                    try
                    {
                        for (unsigned int i = 0; i < updates; ++ i)
                        {
                            TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState&  dataNodeUpdate = (*dataNodeUpdates)[i];
                            TA_Base_Bus::DataNodeDisplayState toupdate;
                            TA_Base_Bus::convertDnDisplayUpdate(dataNodeUpdate, toupdate);
                            if (!toupdate.ONITP.empty())
                            {
                                if (toupdate.ONITP.compare("N") != 0)
                                {
                                    toReturn.push_back(toupdate.entityKey);
                                }
                            }
                        }
                    }
                    catch (...) 
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
                    }
					
                    if ( NULL != dataNodeUpdates )
                    {
                        delete dataNodeUpdates;
                        dataNodeUpdates = NULL;
                    }
                }                
            }//end of try
            catch ( const TA_Base_Core::ObjectResolutionException & ex )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
            }
            catch ( const TA_Base_Core::ScadaProxyException & ex )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException" ,ex.what() );
            }
            catch ( const TA_Base_Core::BadParameterException & )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
            }
            catch ( const TA_Base_Core::OperationModeException & )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
				
                // set all data point to bad quality status and restart the polling cycle
                // by requesting the agent for new observer ID
            }
            catch ( const CORBA::Exception & ex )
            {
                LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");
            }
        }//end of for loop
        //start the bulkpoll process
        start();
        return toReturn;
    }*/

}