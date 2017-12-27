/*
 * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
 *
 * Source : $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/DssEngine.h $
 * @author  Huang Qi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2015/01/20 12:26:46 $
 * Last modified by : $Author: CM $
 *
 * 
 */

#ifndef DSSENGINE_H
#define DSSENGINE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "ace/Mutex.h"
#include "ace/Thread_Semaphore.h"

#include "core/threads/src/Thread.h"
#include "app/response_plans/plan_agent/src/IDssEngine.h"
#include "app/response_plans/plan_agent/src/DssRuleProcessor.h"

namespace TA_Base_App
{

struct TriggerRequest;

class DssEngine : public IDssEngine, public TA_Base_Core::Thread
{
public:
    DssEngine();
    virtual ~DssEngine();

    void initAndStart( const bool bEnable = true );
    void uninitAndTerminate();

    void getTypeAndTriggerKeys( TypeToTriggerKeysMap& refOutput );
    void enable( const bool bEnable = true );

    virtual void triggerUpdate( const unsigned long ulTriggerKey, const unsigned long ulTimestamp, const bool bValue, const bool bInitial );
    virtual bool isEnabled();

    virtual void run();
    virtual void terminate();

private:
	DssEngine( const DssEngine& );
	const DssEngine& operator=( const DssEngine& );

    void processTriggerUpdate();

private:
    volatile bool m_bRunning;
    volatile bool m_bEnable;
    DssRuleProcessor m_dssRuleProcessor;
    mutable ACE_Mutex m_mtxRequests;
    ACE_Thread_Semaphore m_smaWorkLock;
    std::list<TriggerRequest*> m_lstRequests;
};

} // namespace TA_Base_App

#endif // DSSENGINE_H
