/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of PlanExecutionEngine.
  */

#include "core/corba/src/CorbaUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageException.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/ThirdPartyLibException.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/SortablePlanId.h"
#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageQueue.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageBlock.h"
#include "app/response_plans/plan_agent/src/RunPlanMethodObject.h"
#include "app/response_plans/plan_agent/src/PlanHouseKeepingMethodObject.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/PlanExecutionEngine.h"

namespace TA_Base_App
{

    const ACE_Message_Block::Message_Flags ACE_EVENT_SET_CONTROL = ACE_Message_Block::USER_FLAGS + 0x1;

	PlanExecutionEngine::PlanExecutionEngine() :
    m_activationQueue(),
    m_mqMap(),
    m_mqMapLock(),
    m_consolidateMqLock()
	{
	}

	PlanExecutionEngine::~PlanExecutionEngine()
	{
        // MB_HANGUP is not be used any more - jian.huang
		// there should be one MB_HANGUP message left
// 		ACE_Message_Block* aceMsg;
// 		getq(aceMsg);
// 		aceMsg->release();

        if ( 0 != thr_count() )
        {
            terminateAndWait();
        }

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);

		    PlanMqMapIterator it;

            // User smart pointer to avoid object usage after deleting
		    for( it = m_mqMap.begin(); it != m_mqMap.end(); ++it )
		    {
                it->second->abandon();
// 			    delete (*it).second;
		    }

            m_mqMap.clear();
        }
	}

	int PlanExecutionEngine::open( void* )
	{
        if (( m_activationQueue.queue() != NULL ) && 
            ( m_activationQueue.queue()->state() == ACE_Message_Queue_Base::DEACTIVATED ))
        {
            m_activationQueue.queue()->activate();
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Perf-: Reactive activation queue successfully");
        }

        // PlanHouseKeeping enqueue
        m_activationQueue.enqueue( new PlanHouseKeepingMethodObject( *this ));

        // TD10997: The size of the plan execution thread pool is now configurable.
		return activate(THR_JOINABLE, PlanAgentUser::getInstance()->getActivePlanThreadPoolSize());
		//return activate(THR_JOINABLE, 1);
	}

	int PlanExecutionEngine::close( u_long flags /*=0*/ )
	{
		return 0;
	}

	int PlanExecutionEngine::svc( void )
	{
		try
		{
			while( true )
			{
				// check the activation queue
				//ACE_Time_Value aqAbsTime( ACE_OS::gettimeofday() + m_aqTimeout );
				//std::auto_ptr<ACE_Method_Request> mo(m_activationQueue.dequeue(&aqAbsTime));

                // Blocking dequeue
                std::auto_ptr<ACE_Method_Request> mo(m_activationQueue.dequeue()); // jian.huang

				// No method to execute.
				if( 0 == mo.get() )
				{
					// Check the activation queue is OK.
// 					if ( EWOULDBLOCK != ACE_OS::last_error() )
// 					{
// 						std::ostringstream what;
// 						what << "ACE_Activation_Queue::dequeue failed with errno " << ACE_OS::last_error();
// 						TA_THROW( ThirdPartyLibException( what.str() ) );
// 					}
                    
                    // jian.huang
                    // If m_activationQueue is deactivated, ACE_OS::last_error() should return ESHUTDOWN
                    // so all threads which are blocked at m_activationQueue.dequeue() should break out of the loop
                    // and this will instead of original getq method to notify idling threads to exit the loop
                    if ( ESHUTDOWN == ACE_OS::last_error() )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Idle thread receive hang up signal" );
                        break;
                    }
                    else
                    {
                        std::ostringstream what;
                        what << "ACE_Activation_Queue::dequeue failed with errno " << ACE_OS::last_error();
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, what.str().c_str() );
                        break;
                    }
					
                    // No need to get msg from message queue
// 					ACE_Message_Block* aceMsg;
// 					ACE_Time_Value mqAbsTime( ACE_OS::gettimeofday() + m_mqTimeout );
// 
// 					if( -1 == getq(aceMsg, &mqAbsTime) )
// 					{
// 						if ( EWOULDBLOCK != ACE_OS::last_error() )
// 						{
// 							std::ostringstream what;
// 							what << "ACE_Task::getq failed with errno " << ACE_OS::last_error();
// 							TA_THROW( ThirdPartyLibException( what.str() ) );
// 						}
// 					}
// 					else
// 					{
// 						if( ACE_Message_Block::MB_HANGUP == aceMsg->msg_type() )
// 						{
// 							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received MB_HANGUP message." );
// 							put( aceMsg );
// 							ActivePlanBulletinBoard::instance()->setHangupMessageReceived();
// 							break;
// 						}
// 					}
                }
				else // Method to execute
				{
					// If we get to here we've got an object on the activation queue
                    try
                    {
                        if( -1 == mo->call() )
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Method call error." );
                            break;
                        }
                    }
                    catch (...) 
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled when runPlanExecutionEngine::svc" );
                    }
				}

                // clean up work will be done in a single thread in PlanHouseKeepingMethodObject
//                 try
//                 {
//                     //as per TD 7183 
//                     ActivePlanBulletinBoard::instance()->pauseExpiredInteractivePlans ();
//                     
//                     // periodically cull stopped plan instances from
//                     // the bulletin board
//                     ActivePlanBulletinBoard::instance()->cullStoppedPlans();
//                     
//                     // Peridically clean up the message queue.
//                     consolidateMq();
//                 }
//                 catch( const std::exception& e )
//                 {
//                     LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
//                 }
//                 catch( ... )
//                 {
//                     LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled." );
//                 }
			}
		}
		catch( const std::exception& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, typeid(e).name(), e.what() );
		}
		catch( const CORBA::Exception& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() );
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unhandled." );
		}

		return 0;
	}


    ACE_Future<int> PlanExecutionEngine::run( 
        const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, 
        const int initialState, const bool manExecution, const bool bStartFromMonitorMode )
	{
		ACE_Future<int> rval;

		// First we need to reserve an instance because we have to return an instance to 
		// the caller synchronously so that they can refer to this plan in subsequent
		// messages.

		ActivePlanBulletinBoard::instance()->reserveInstance( session, pmId, plan, initialState, rval , manExecution);

		// Turn the plan into an "Active" plan. The plan is newed here and is
		// passed around as an auto_ptr so ownership is constantly passed on
		// until the final method/object that holds ownership is destroyed.

        try
        {
// 		    std::auto_ptr<ActivePlan> activePlan( new ActivePlan( plan, initialState ) );

		    // Make a message queue for this plan.
		    makeMq( plan );

		    // Put it in the list of plans to be run.
// 		    m_activationQueue.enqueue( new RunPlanMethodObject( *this, activePlan ) );
            m_activationQueue.enqueue( new RunPlanMethodObject( *this, session, pmId, plan, initialState, manExecution, bStartFromMonitorMode ));
        }
        catch ( const TA_Base_Core::PlanAgentException& ex)
        {
            ActivePlanBulletinBoard::instance()->undoReserveInstance( plan );
			TA_THROW(ex);
        }

        // Return a copy of the future to the caller so they can wait on the
        // result.
        
		return rval;
    }

    void PlanExecutionEngine::publishPlanStart( const std::string& session, const std::string& pmId, 
        const TA_Base_Core::ActivePlanId& plan, const int initialState, const bool manExecution )
    {
        // No update should be send before RunPlan or CustomisePlan is update in Monitor Agent
//        ActivePlanBulletinBoard::instance()->sendUpdateMessage( plan );

        //Send State Update message for Taking control
        TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;
        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

        TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
        psu->session = session.c_str();
        psu->pmId = pmId.c_str();//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
        psu->plan = plan;
        psu->manuExec = manExecution;

        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( plan ).path.in();
        const unsigned long ulInstanceNum = plan.instance;

        if ( TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE == initialState )
        {
            pasUpdateData->update.runPlanUpdate(psu);
            if (ActivePlanBulletinBoard::instance()->isGRCPlan(plan))
            {
                if (ActivePlanBulletinBoard::instance()->isManualPlan(plan))
                {
                    AAMessageSender::instance()->submitAuditManualGrcStarted( session, strPlanName, ulInstanceNum );	
                }
                else//automatic GRC
                {
                    AAMessageSender::instance()->submitAuditAutoGrcStarted( session, strPlanName, ulInstanceNum );
                }
            }
            else//DSS plans
            {
                AAMessageSender::instance()->submitAuditPlanExecution( session, strPlanName, ulInstanceNum );
            }
        }
        else
        {
            AAMessageSender::instance()->submitAuditPlanPending( session, strPlanName, ulInstanceNum );
        }

        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);

        // Send update to Plan Manager although useless update will send to Monitor Agent too
        ActivePlanBulletinBoard::instance()->sendUpdateMessage( plan );
    }


    ACE_Future<int> PlanExecutionEngine::spawn( 
        const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, 
        TA_Base_Core::ActivePlanId& childPlan, const int initialState, const bool bStartFromMonitorMode )
	{
		ACE_Future<int> rval;

		// First we need to reserve an instance because we have to return an instance to 
		// the caller synchronously so that they can refer to this plan in subsequent
		// messages.

		ActivePlanBulletinBoard::instance()->reserveInstance( session, pmId, parentPlan, childPlan, initialState, rval);
		// Turn the plan into an "Active" plan. The plan is newed here and is
		// passed around as an auto_ptr so ownership is constantly passed on
		// until the final method/object that holds ownership is destroyed.

        try
        {
// 		    std::auto_ptr<ActivePlan> activePlan( new ActivePlan( childPlan, initialState ) );

		    // Make a message queue for this plan.
		    makeMq( childPlan );

		    // Put it in the list of plans to be run.
// 		    m_activationQueue.enqueue( new RunPlanMethodObject( *this, activePlan ) );
            // Spawn by a parent plan, not a manual executed plan by default
            m_activationQueue.enqueue( new RunPlanMethodObject( *this, session, pmId, childPlan, initialState, false, bStartFromMonitorMode ));
        }
        catch ( const TA_Base_Core::PlanAgentException& ex)
        {
            ActivePlanBulletinBoard::instance()->undoReserveInstance( childPlan );
			TA_THROW(ex);
        }

        // Return a copy of the future to the caller so they can wait on the
        // result.
        
		return rval;
    }

	ACE_Future<int> PlanExecutionEngine::runInterlocked( std::auto_ptr<ActivePlan> activePlan )
	{
		ACE_Future<int> rval = ActivePlanBulletinBoard::instance()->getFuture( activePlan->getPlan() );
		m_activationQueue.enqueue( new RunPlanMethodObject( *this, activePlan ) );
		return rval;
	}

	void PlanExecutionEngine::runImpl( std::auto_ptr<ActivePlan> activePlan )
	{
        // User smart pointer to control instance life cycle
		ActivePlanMessageQueuePtr mq = getMq( activePlan->getPlan() );

		while( activePlan->getIsActive() )
		{
			if( NULL == mq.get() )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[P:%lu,I:%lu]: NULL message queue!", activePlan->getPlan().plan, activePlan->getPlan().instance );
                try
                {
				    ActivePlanBulletinBoard::instance()->updatePlanState( activePlan->getPlan(), TA_Base_Core::FAILED_ACTIVE_PLAN_STATE, RemarkRunPlanFailed );
                }
                catch (...)
                {
                    // Do nothing
                }
				break;
			}

            if ( ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
            {
                try
                {
                    activePlan->stop( RemarkSystemStopPlan );
                }
                catch ( const std::exception& expWhat )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when executing plan" );
                }
                break;
		    }

            try
            {
                processMessages( *activePlan, mq );
            }
            catch ( const std::exception& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
                break;
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught exception when process messages." );
                break;
            }

            if( ActivePlan::INTERLOCKED_NSEM == activePlan->executeNextStep() )
            {
                // Note: DO NOT access activePlan after calling this.
                runInterlocked( activePlan );
                break;
            }
		}
	}

	void PlanExecutionEngine::processMessages( ActivePlan& activePlan, boost::shared_ptr<ActivePlanMessageQueue> mq )
    {
        ACE_Message_Block* aceMsg = NULL;
        ActivePlanMessageBlock* msg = NULL;
        IActivePlanMessage* controlMsg = NULL;

        static const ACE_Time_Value mqTimeout( 0, 10000 );
        ACE_Time_Value absTime( ACE_OS::gettimeofday() + mqTimeout );

        if ( -1 == mq->dequeue(aceMsg, &absTime) )
		{
			if ( EWOULDBLOCK != ACE_OS::last_error() )
			{
				std::ostringstream what;
				what << "[P:" << activePlan.getPlan().plan << ",I:" << activePlan.getPlan().instance << "]:"
					 << " ACE_Message_Queue::dequeue failed with errno: " << ACE_OS::last_error() << ".";
				TA_THROW( ThirdPartyLibException( what.str() ) );
			}
		}
		else // we have a message
        {
            if ( NULL == aceMsg )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Dequeue a NULL ACE_Message_Block" );
                return;
            }

            if (( ACE_Message_Block::MB_PCEVENT == aceMsg->msg_type() ) &&
                ( ACE_EVENT_SET_CONTROL == ( aceMsg->flags() & ACE_EVENT_SET_CONTROL )))
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Msg-: Received turn to control message for Plan [%d, %d]", 
                    activePlan.getPlan().plan, activePlan.getPlan().instance );
                activePlan.setStateSyncDone( true );
                aceMsg->release();
                return;
            }

            msg = dynamic_cast<ActivePlanMessageBlock*>( aceMsg );

            if ( NULL != msg )
            {
                controlMsg = msg->getControlMessage();
            }

            if ( NULL != controlMsg )
            {
                controlMsg->execute( activePlan );
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL control message for ActivePlanMessageBlock" );
            }

            aceMsg->release();
		}
	}

	boost::shared_ptr<ActivePlanMessageQueue> PlanExecutionEngine::getMq( const TA_Base_Core::ActivePlanId& plan ) const
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
		{
			ConstPlanMqMapIterator it = m_mqMap.find(plan);

			if ( it != m_mqMap.end() )
			{
				return( (*it).second );
			}
		}

        ActivePlanMessageQueuePtr pNullPtr;
		return pNullPtr;
	}

	void PlanExecutionEngine::makeMq( const TA_Base_Core::ActivePlanId& plan )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
		{
			PlanMqMapIterator it = m_mqMap.find(plan);

			if ( it != m_mqMap.end() )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Removing duplicate message queue (%d) for [P:%lu,I:%lu]", it->second.get(), plan.plan, plan.instance );
                it->second->abandon();
// 				delete (*it).second;
// 				(*it).second = NULL;
				m_mqMap.erase( it );
			}

            ActivePlanMessageQueuePtr pNewMQ( new ActivePlanMessageQueue() );
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Create message queue (%d) for [P:%lu,I:%lu]", pNewMQ.get(), plan.plan, plan.instance );
			m_mqMap.insert( PlanMqMap::value_type(plan, pNewMQ) );
		}
	}

    int PlanExecutionEngine::put( ACE_Message_Block *mb, ACE_Time_Value *tv )
    {
        int nResult = 0;

        if ( NULL == mb )
        {
            nResult = - 1;
        }
        else if( ACE_Message_Block::MB_HANGUP == mb->msg_type() )
        {
            nResult = putq( mb, tv );
        }
        else if( ACE_Message_Block::MB_DATA == mb->msg_type() )
        {
            ActivePlanMessageBlock* apMb = dynamic_cast<ActivePlanMessageBlock*>( mb );

            if ( NULL == apMb )
            {
                mb->release();

                TA_THROW( ActivePlanMessageException( "MB_DATA message type with invalid contents discarded" ));
            }

            ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
            {
                ConstPlanMqMapIterator it = m_mqMap.find( apMb->getPlan() );

                if ( it != m_mqMap.end() )
                {
                    try
                    {
                        nResult = it->second->enqueue( mb, tv );
                    }
                    catch ( ActivePlanMessageException& )
                    {
                        mb->release();
                        nResult = -1;
                    }
                    catch (...)
                    {
                        mb->release();

                        // When consolidateMq erase the mq from m_mqMap, here may catch an unknow exception in Windows and crash in Solaris
                        std::ostringstream what;
                        what << "mq dequeue error";
                        TA_THROW( ThirdPartyLibException( what.str() ) );
                    }
                }
                else
                {
                    const unsigned long ulActiveId = apMb->getPlan().plan;
                    const unsigned long ulActiveInstance = apMb->getPlan().instance;
                    apMb->release();

                    std::ostringstream what;
                    what << "[P:" << ulActiveId << ",I:" << ulActiveInstance << "]:" << " Message for unknown plan discarded.";
                    TA_THROW( PlanNotFoundException( what.str(), TA_Base_Core::PNFE_MESSAGE_FOR_UNKNOWN_PLAN_DISCARDED, ulActiveId, ulActiveInstance) );
                }
            }
        }
        else
        {
            std::ostringstream what;
            what << " Invalid message type (" << mb->msg_type() << ") discarded.";
            mb->release();
            TA_THROW( ActivePlanMessageException( what.str() ) );
        }

        return nResult;
    }

	void PlanExecutionEngine::consolidateMq()
	{
		// Make sure only one thread tries to release the inactive message
		// queues. It would not cause a problem of more than one did but
		// it is a waste of resources having many threads taking a write
		// lock out on the message queue list.
		ACE_Guard<ACE_Mutex> guard(m_consolidateMqLock);
		{
			std::list<SortablePlanId> inactiveMqList;
			{
				ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
				{
					ConstPlanMqMapIterator it;
					for( it = m_mqMap.begin(); it != m_mqMap.end(); ++it )
                    {
                        int state = 0xFFFFFFFF;

                        try
                        {
                            state = ActivePlanBulletinBoard::instance()->getState( (*it).first.get() );
                        }
                        catch ( ... )
                        {
                            //do nothing as plan state was not found, let the processing be default
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "[P:%lu,I:%lu] ActivePlan State doesn't exist", it->first.getPlan(), it->first.getInstance() );
                        }

						switch( state )
						{
							case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
							// don't break

							case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
							// don't break

                            case TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE:
                            // don't break

							case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
							// don't break

							case TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE:								
							break;

                            //This is only used to take care of updates during Monitor Mode
                            case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
                            // don't break

                            case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
                            // don't break

                            case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
                            break;

							case TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE:
							// if the bulletin board doesn't know about it
							// but there are still some unstarted plans
							// in the activation queue we should keep the
							// messsage queue
							if( m_activationQueue.method_count() == 0 )
							{
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: message queue scheduled for deletion.", (*it).first.getPlan(), (*it).first.getInstance() );
								inactiveMqList.push_back( (*it).first.get() );
							}
							break;

							default:
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu,I:%lu]: message queue scheduled for deletion.", (*it).first.getPlan(), (*it).first.getInstance() );
								inactiveMqList.push_back( (*it).first.get() );
								break;
						}
					}
				}
			}

			std::list<SortablePlanId>::const_iterator inactiveIt;

			for( inactiveIt = inactiveMqList.begin(); inactiveIt != inactiveMqList.end(); ++inactiveIt )
			{
				ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
				{
					PlanMqMapIterator it = m_mqMap.find( (*inactiveIt).get() );

					if ( it != m_mqMap.end() )
                    {
                        // Use share pointer instead
//                         delete (*it).second;
//                         (*it).second = NULL;
                        it->second->abandon();
                        m_mqMap.erase( it );
					}
				}
			}
		}
	}

    void PlanExecutionEngine::clearAllMessageQueue()
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
        for( ConstPlanMqMapIterator itLoop = m_mqMap.begin(); itLoop != m_mqMap.end(); ++itLoop )
        {
            itLoop->second->abandon();
        }

        m_mqMap.clear();
    }

    void PlanExecutionEngine::signalAllPlansIntoControl()
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mqMapLock);
        for( ConstPlanMqMapIterator itLoop = m_mqMap.begin(); itLoop != m_mqMap.end(); ++itLoop )
        {
            ACE_Message_Block::Message_Flags flgEventType = ACE_EVENT_SET_CONTROL;
            ACE_Message_Block* pMsgBlock = new ACE_Message_Block();
            pMsgBlock->msg_type( ACE_Message_Block::MB_PCEVENT );
            pMsgBlock->set_flags( flgEventType );
            itLoop->second->enqueue( pMsgBlock );
        }
    }

    void PlanExecutionEngine::terminateAndWait()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Perf-: Start to terminate" );

        ActivePlanBulletinBoard::instance()->setHangupMessageReceived();

        ACE_Time_Value tmSleepInterval( 0, 200000 );
        while ( !ActivePlanBulletinBoard::instance()->isAllPlanStopped() )
        {
            ACE_OS::sleep( tmSleepInterval );
        }

        if (( m_activationQueue.queue() != NULL ) && ( m_activationQueue.queue()->state() == ACE_Message_Queue_Base::ACTIVATED ))
        {
            m_activationQueue.queue()->close();
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Perf-: Close m_activationQueue successfully" );
        }

        wait();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "-Perf-: Terminated successfully" );
    }
} // TA_Base_App

