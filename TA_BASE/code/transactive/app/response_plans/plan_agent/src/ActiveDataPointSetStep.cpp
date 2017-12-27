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
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_DATA_POINT_SET )

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/threads/src/Thread.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveDataPointSetStep.h"
#include "app/response_plans/common/src/StringConstants.h"


namespace TA_Base_App
{
    const int DP_CONTROL_STATE_DELAY_TIME = 1500; // millisecond

    enum DataPointControlType
    {
        eNoOperation = 0,
        eDataPointExecute,
        eDataPointSelect,
    };

    ActiveDataPointSetStep::ActiveDataPointSetStep( const TA_Base_Core::StepDetail& stepDetail,
        const TA_Base_Core::DataPointSetParameters& params, IActivePlanFlow& flowControl, bool bSubStep ) :
	ActiveStep(stepDetail, flowControl),
	m_params(params),
	m_semaphore(0),
	m_agentConnectedSem(0),
    m_pDataPoint(),
    m_bRccSucceed(false),
	m_bSelectSucceed(false),
	m_noControlReceived(false),
    m_nControlType(eNoOperation)
	{
        m_bSubStep = bSubStep;
	}

    ActiveDataPointSetStep::~ActiveDataPointSetStep()
    {
         m_pDataPoint.reset();
    }

	void ActiveDataPointSetStep::processEntityUpdateEvent(unsigned long entityKey , TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		if (TA_Base_Bus::AgentConnected != updateType && TA_Base_Bus::ControlStateUpdate != updateType)
		{
			return;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received entity %lu update %d when %d control, this:%x", 
			entityKey, updateType, m_nControlType, this);


		if( TA_Base_Bus::AgentConnected == updateType )	
		{
			m_agentConnectedSem.post();
            return;
		}

		
		TA_Base_Bus::EDataPointControlState eState = m_pDataPoint->getControlState();
		if (eState == TA_Base_Bus::noControl)
		{
			m_noControlReceived = true;
			return;
		}

		// Discard control state change if no TA_Base_Bus::noControl received before or
		// step is not in control state
        if( eNoOperation == m_nControlType || false == m_noControlReceived)
        {
            return;
        }
		
		switch ( eState )
        {
        case TA_Base_Bus::conditionalSuccess:
        case TA_Base_Bus::success:
            {
				if(eDataPointExecute == m_nControlType)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint %lu Set response: success", entityKey);

					m_bRccSucceed = true;

					m_semaphore.post();

					const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail(m_flowControl.getPlan()).path.in();
					const unsigned long ulInstanceNum = m_flowControl.getPlan().instance;
					AAMessageSender::instance()->submitAuditManualGrcSucceed(strPlanName, ulInstanceNum);

					break;
				}
            }
        case TA_Base_Bus::LCC_failed:
            {
				if (eDataPointSelect == m_nControlType)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint %lu Set response: LCC_failed", entityKey);

					m_semaphore.post();

					const std::string strEquipDesc = m_pDataPoint->getDescription();
					const std::string strLcc = m_pDataPoint->getLaunchingConditionResultString();
					AAMessageSender::instance()->submitAuditManualGrcBadLaunch( strEquipDesc, strLcc );

					break;
				}
            }
        case TA_Base_Bus::RCC_failed:
            {
				if (eDataPointExecute == m_nControlType)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint %lu Set response: RCC_failed", entityKey);

					m_semaphore.post();

					const std::string strEquipDesc = m_pDataPoint->getDescription();
					const std::string strRcc = m_pDataPoint->getLaunchingConditionResultString();
					AAMessageSender::instance()->submitAuditManualGrcBadReturn( strEquipDesc, strRcc );

					break;
				}
            }
        case TA_Base_Bus::failed:
        case TA_Base_Bus::comms_failed:
		case TA_Base_Bus::deviceControlException:
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Datapoint %lu Set response: failed / comms_failed / deviceControlException", entityKey);

                m_semaphore.post();

                const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail(m_flowControl.getPlan()).path.in();
                const unsigned long ulInstanceNum = m_flowControl.getPlan().instance;
                AAMessageSender::instance()->submitAuditManualGrcFailed( "", strPlanName, "", ulInstanceNum, m_detail.position );

                break;
            }
        case TA_Base_Bus::execCmdConfirmTimeout:
        case TA_Base_Bus::execCmdSuccessTimeout:
		case TA_Base_Bus::execCmdCfmNegative:
            {
                if (eDataPointExecute == m_nControlType)
                {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"DataPoint %lu Set response: execCmdConfirmTimeout / execCmdSuccessTimeout / execCmdCfmNegative", entityKey);

                    m_semaphore.post();

                    const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail(m_flowControl.getPlan()).path.in();
                    const unsigned long ulInstanceNum = m_flowControl.getPlan().instance;
                    AAMessageSender::instance()->submitAuditManualGrcFailed( "", strPlanName, "", ulInstanceNum, m_detail.position );
                }

                break;
            }
        case TA_Base_Bus::selectCmdConfirmTimeout:
		case TA_Base_Bus::selectCmdCfmNegative:
            {
                if (eDataPointSelect == m_nControlType)
                {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DataPoint %lu Set response: selectCmdConfirmTimeout / selectCmdCfmNegative", entityKey);

                    m_semaphore.post();

                    const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail(m_flowControl.getPlan()).path.in();
                    const unsigned long ulInstanceNum = m_flowControl.getPlan().instance;
                    AAMessageSender::instance()->submitAuditManualGrcFailed( "", strPlanName, "", ulInstanceNum, m_detail.position );
                }

                break;
            }
        case TA_Base_Bus::selectCmdConfirm:
            {
                if (eDataPointSelect == m_nControlType)
                {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DataPoint %lu Set response: selectCmdConfirm", entityKey);
                    m_bSelectSucceed = true;
                    m_semaphore.post();
                }

                break;
            }
        default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unhandled control state:%d for DataPoint %lu",eState, entityKey);
			break;
        }
    }

    bool ActiveDataPointSetStep::initialize(std::string& strSession, std::string &strValue, int& waitMillSec)
    {
        m_remark = "";

        std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(NULL);
        try
        {
            dpEntity.reset(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_params.dataPoint.in()));
        }
        catch (...) 
        {
            m_remark = RemarkInvalidDatapoint;

            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );

            return false;
        }

        strSession = ActivePlanBulletinBoard::instance()->getSession(m_flowControl.getPlan());

        std::string reason = "";
        if ( !ActivePlanBulletinBoard::instance()->isActionPermitted(strSession, dpEntity->getKey(), TA_Base_Bus::aca_DATA_POINT_WRITE, reason) )
        {
            m_remark = RemarkAccessDataPointDenied;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "%s Write Access to Data Point %s not allowed.", dumpContext().c_str(), m_params.dataPoint.in() );
            return false;
        }

        try
        {
            boost::shared_ptr<TA_Base_Core::DataPointEntityData> ptrEntity( dynamic_cast<TA_Base_Core::DataPointEntityData*>(dpEntity.release()) );
            waitMillSec = ptrEntity->getABETimeoutPeriodSeconds() * 1000;
			// if ABETimeout is zero, we need define a minimum timeout 10 second
			// other wise, PScada control sequence will not correct
			if(0 == waitMillSec)
			{
				waitMillSec = 10000;
			}

            TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy( ptrEntity,*this, m_pDataPoint );

            if(!m_agentConnectedSem.timedWait(5000))
            {
                m_remark = RemarkInvalidDatapoint;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "DataPoint is not config valid");
                return false;
            }

            if(m_pDataPoint->isDerived())
            {
                strValue = m_pDataPoint->getDerivedStateAsString( static_cast<unsigned long>(m_params.newValue.numericValue().value) );
            }
            else if(m_pDataPoint->isText())
            {
                strValue = m_params.newValue.textValue();
            }
            else if(m_pDataPoint->isNumber())
            {
                strValue = TA_Base_Core::DataConversion::toString( m_params.newValue.numericValue().value );
            }
            else if(m_pDataPoint->isBoolean())
            {
                strValue = m_pDataPoint->getBooleanLabel(m_params.newValue.booleanValue());
            }
            else
            {
                m_remark = RemarkInvalidDatapoint;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknow DataPoint Type");
                return false;
            }
        }
        catch( std::exception& expWhat )
        {
            m_remark = RemarkInvalidDatapoint;

            std::ostringstream msg;
            msg << dumpContext() << ": " << expWhat.what() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), msg.str().c_str() );

            return false;
        }
        catch (...) 
        {
            m_remark = RemarkInvalidDatapoint;

            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );

            return false;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "DataPoint <%s> setValue: value=%s, sessionId=%s, semaphore timeout = %d",
            m_params.dataPoint.in(), strValue.c_str(), strSession.c_str(), waitMillSec );

        return true;
    }

    bool ActiveDataPointSetStep::sendControl(const std::string& strSession, const std::string &strValue, const int waitMillSec)
    {
        bool bSuccess = false;

        // Clear all signal of m_semaphore, only wait for m_pDataPoint->setValue response.
        while ( m_semaphore.tryWait() )
        {
            // Do nothing
        }

        m_bRccSucceed = false;
        m_nControlType = eDataPointExecute;

        long tryCount = 0;

        try
        {
            while( tryCount++ < 100 )
            {
                try
                {
                    m_pDataPoint->setValue(strValue, strSession);
                    bSuccess = true;
                    break;
                }
                catch (TA_Base_Core::ScadaProxyException &e)
                {
                    if ( tryCount >= 100 )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ScadaProxyException", e.what() );

                        break;
                    }

                    TA_Base_Core::Thread::sleep( 500 );
                }
            }
        }
        catch( std::exception& expWhat )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << expWhat.what() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), msg.str().c_str() );
        }
        catch( TA_Base_Core::BadParameterException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", msg.str().c_str() );
        }
        catch( TA_Base_Core::OperationModeException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", msg.str().c_str() );
        }
        catch( TA_Base_Bus::ScadaCorbaTypes::NotWriteable& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ScadaCorbaTypes::NotWriteable", msg.str().c_str() );
        }
        catch( TA_Base_Core::NotAvailableException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::NotAvailableException", msg.str().c_str() );
        }
        catch( TA_Base_Core::AccessDeniedException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", msg.str().c_str() );
        }
        catch( CORBA::Exception& e )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", msg.str().c_str() );
        }
        catch( ... )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );
        }

        //only wait when there are RCC checking
        if (( 0 != waitMillSec ) && bSuccess )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Waiting for Entity Update Event: millsec=%d", waitMillSec );
            if( !m_semaphore.timedWait(waitMillSec + DP_CONTROL_STATE_DELAY_TIME))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Waiting for Entity Update Event: time out");
                bSuccess = false;
            }
            else if( !m_bRccSucceed )
            {
                bSuccess = false;
            }
        }

        m_nControlType = eNoOperation;

        return bSuccess;
    }

	bool ActiveDataPointSetStep::sendPScadaControl(const std::string& strSession, const std::string &strValue, const int waitMillSec, TA_Base_Bus::EControlOperation controlOperation)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Enter executePScadaControl type: %d, session: %s, value: %s ", controlOperation, strSession.c_str(), strValue.c_str() );
		
		m_nControlType = ( controlOperation == TA_Base_Bus::CONTROL_EXECUTE ) ? eDataPointExecute : eDataPointSelect;

        long tryCount = 0;
		bool bSuccess = false;
        try
        {
            while( tryCount++ < 100 )
            {
                try
                {
					if (eDataPointExecute == m_nControlType )
					{
						m_bRccSucceed = false;
					}
					else if (eDataPointSelect == m_nControlType)
					{
						m_bSelectSucceed = false;
					}

					// Clear all signal of m_semaphore, only wait for m_pDataPoint->setValue response.
					while ( m_semaphore.tryWait() )
					{
						// Do nothing
					}

                    m_pDataPoint->setControlValue(strValue, strSession, controlOperation);
                    bSuccess = true;
                    break;
                }
                catch (TA_Base_Core::ScadaProxyException &e)
                {
                    if ( tryCount >= 100 )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ScadaProxyException", e.what() );

                        break;
                    }

                    TA_Base_Core::Thread::sleep( 500 );
                }
            }
        }
        catch( std::exception& expWhat )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << expWhat.what() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), msg.str().c_str() );
        }
        catch( TA_Base_Core::BadParameterException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", msg.str().c_str() );
        }
        catch( TA_Base_Core::OperationModeException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", msg.str().c_str() );
        }
        catch( TA_Base_Bus::ScadaCorbaTypes::NotWriteable& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ScadaCorbaTypes::NotWriteable", msg.str().c_str() );
        }
        catch( TA_Base_Core::NotAvailableException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::NotAvailableException", msg.str().c_str() );
        }
        catch( TA_Base_Core::AccessDeniedException& e)
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.reason.in() << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", msg.str().c_str() );
        }
        catch( CORBA::Exception& e )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", msg.str().c_str() );
        }
        catch( ... )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << " for data point " << m_params.dataPoint.in() << ".";
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );
        }

		//only wait when there are RCC checking
		if (( 0 != waitMillSec ) && bSuccess )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Waiting for Entity Update Event: millsec=%d", waitMillSec );

            if( !m_semaphore.timedWait( waitMillSec + DP_CONTROL_STATE_DELAY_TIME ))
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Data Point get response time out");
				// In some special situation, noControl state is never received
				// so we will discard control state base on current logic.
				// so we need double check current control state if semaphore timeout
				if (eDataPointSelect == m_nControlType && TA_Base_Bus::selectCmdConfirm == m_pDataPoint->getControlState())
				{
					m_noControlReceived = true;
				}
				else
				{
					bSuccess = false;
				}                
            }
			else if (eDataPointExecute == m_nControlType && !m_bRccSucceed)
			{
				bSuccess = false;
			}
            else if(eDataPointSelect == m_nControlType && !m_bSelectSucceed)
            {
                bSuccess = false;
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s operation %s", 
                ( controlOperation == TA_Base_Bus::CONTROL_SELECT ? "Select" : "Execution" ), ( bSuccess ? "succeed" : "failed" ) );
        }

        m_nControlType = eNoOperation;

		return bSuccess;
    }

	TA_Base_Core::EActiveStepState ActiveDataPointSetStep::executeImpl( void )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ActiveDataPointSetStep %lu", getStepNumber() );

        bool bSuccess = false;
        int abeWaitMillSec = 0;
        std::string strValue = "";
        std::string strSession = "";

        if ( initialize( strSession, strValue, abeWaitMillSec ))
        {
		    if (!m_pDataPoint->getPScadaFlag())
		    {
			    bSuccess = sendControl( strSession, strValue, abeWaitMillSec );
		    }
		    else
		    {
				//selectWaitTime is 10s here because selectCmdConfirmTimeOut is 5s
				int selectWaitTime = 10000; // 10s
			    if ( sendPScadaControl( strSession, strValue, selectWaitTime, TA_Base_Bus::CONTROL_SELECT ))
			    {
				    bSuccess = sendPScadaControl(strSession, strValue, abeWaitMillSec, TA_Base_Bus::CONTROL_EXECUTE);
			    }
		    }
        }

        if( bSuccess )
        {
            m_remark = RemarkStepExecutionSucceed;
        }

        if ( m_bSubStep )
        {
            m_execResult.set( bSuccess );
        }
        else if ( bSuccess )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DataPoint [%s] setValue successfully", m_params.dataPoint.in() );
            m_flowControl.setNextStep();
        }

        return ( bSuccess ? TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE : TA_Base_Core::FAILED_ACTIVE_STEP_STATE );
	}

    void ActiveDataPointSetStep::setExecResult( ACE_Future<bool> & execResult )
    {
        m_execResult = execResult;
    }

} // TA_Base_App


#endif //  STEPTYPE_DATA_POINT_SET
