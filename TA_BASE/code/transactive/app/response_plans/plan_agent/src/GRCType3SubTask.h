/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/GRCType3SubTask.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#if !defined(AFX_GRCTYPE3SUBTASK_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_)
#define AFX_GRCTYPE3SUBTASK_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Activation_Queue.h"

namespace TA_Base_App
{
    class ActiveDataPointSetStep;

	class GRCType3SubTask : public ACE_Task<ACE_MT_SYNCH>
	{
	public:
		GRCType3SubTask(unsigned long stepNumber, ActiveDataPointSetStep* pStep);
		virtual ~GRCType3SubTask();

	public:
		int open(void*);
		int close(u_long flags = 0 );
		int svc(void);
		ACE_Future<bool> doSetDataPoint( );
		ActiveDataPointSetStep * getDataPointSetStep();

	public:
		void doSetDataPointImpl( );

	private:
		ACE_Activation_Queue m_activationQueue;

        volatile bool m_bInSvc;
		unsigned long m_stepNumber;
		ActiveDataPointSetStep * m_pStep;
	};
}

#endif // !defined(AFX_GRCTYPE3SUBTASK_H__12F91821_8BB4_43EA_893E_E541DAED23FC__INCLUDED_)
