/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DatapointTriggerObserver.cpp $
  * @author:  Huang Qi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */

#include "ace/Date_Time.h"

#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "app/response_plans/plan_agent/src/IDssEngine.h"
#include "app/response_plans/plan_agent/src/DatapointTriggerObserver.h"

namespace TA_Base_App
{

DatapointTriggerObserver::DatapointTriggerObserver( IDssEngine& refEngine, const unsigned long ulKey ) : 
m_refEngine( refEngine ),
m_ulTriggerKey( ulKey ),
m_ulLastestTimestamp( 0u ),
m_bIsInitial( true ),
m_bCurValue( false )
{
    init();
}

DatapointTriggerObserver::~DatapointTriggerObserver()
{
    uninit();
}

void DatapointTriggerObserver::processEntityUpdateEvent( unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType )
{
    // Only focus on value update
    if ( TA_Base_Bus::ValueStateUpdate != updateType )
    {
        return;
    }

    if ( !m_refEngine.isEnabled() )
    {
        return;
    }

    try
    {
        const unsigned long ulCurTimestamp = static_cast<unsigned long>(m_ptrDatapointProxy->getTimestamp());
        const bool bNewValue = m_ptrDatapointProxy->getValueAsBool();
        const unsigned int unQuality = m_ptrDatapointProxy->getStatus();

        if ( TA_Base_Bus::QUALITY_GOOD != ( unQuality & TA_Base_Bus::QUALITY_MASK ))
        {
            return;
        }

        if (( bNewValue != m_bCurValue ) || m_bIsInitial || ( m_ulLastestTimestamp != ulCurTimestamp ))
        {
            m_ulLastestTimestamp = ulCurTimestamp;
            m_bCurValue = bNewValue;
            m_refEngine.triggerUpdate( m_ulTriggerKey, m_ulLastestTimestamp, m_bCurValue, m_bIsInitial );
        }
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception process entity update" );
    }

    m_bIsInitial = false;
}

void DatapointTriggerObserver::init()
{
    try
    {
        TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy( m_ulTriggerKey, *this, m_ptrDatapointProxy );
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception when create DP proxy" );
    }
}

void DatapointTriggerObserver::uninit()
{
    m_ptrDatapointProxy.reset();
    TA_Base_Bus::ScadaProxyFactory::removeInstance();
}

}
