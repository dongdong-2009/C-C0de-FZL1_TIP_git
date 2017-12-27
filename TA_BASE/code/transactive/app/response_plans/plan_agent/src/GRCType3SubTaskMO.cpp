// GRCType3SubTaskMO.cpp: implementation of the GRCType3SubTaskMO class.
//
//////////////////////////////////////////////////////////////////////

#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "app/response_plans/common/src/ProjectDefinition.h"
#include "app/response_plans/plan_agent/src/ActiveDataPointSetStep.h"
#include "app/response_plans/plan_agent/src/GRCType3SubTask.h"
#include "app/response_plans/plan_agent/src/GRCType3SubTaskMO.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{

    GRCType3SubTaskMO::GRCType3SubTaskMO( GRCType3SubTask * pSubTask, ACE_Future<bool> & execResult ) : 
    m_execResult(execResult),
    m_pSubTask(pSubTask)
	{
	}

	int GRCType3SubTaskMO::call()
	{
		m_pSubTask->getDataPointSetStep()->setExecResult(m_execResult);

		try
        {
            while ( 0 == m_execResult.ready() )
            {
                m_pSubTask->doSetDataPointImpl();
                TA_Base_Core::Thread::sleep( 1000 );
            }
		}
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( const std::exception& ex )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException, reason=%s", ex.what() );
        }
		catch( ... )
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Error talking to peers." );
		}
		
		return 0 ;
	}

}
