// RegionMatrixSessionUpdateMO.cpp: implementation of the RegionMatrixSessionUpdateMO class.
//
//////////////////////////////////////////////////////////////////////

#include "DutyMonitorFramework.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus 
{
	RegionMatrixSessionUpdateMO::RegionMatrixSessionUpdateMO(
															RegionDutyMatrix& matrix,
															const std::string& sessionId, 
															bool deleteFlag):
		sessionId_(sessionId),deleteFlag_(deleteFlag),
		matrix_(matrix)
	{
	
	}
	
	RegionMatrixSessionUpdateMO::~RegionMatrixSessionUpdateMO()
	{
	
	}

	int RegionMatrixSessionUpdateMO::call()
	{
		try
		{
			if(deleteFlag_)	
			{
				matrix_.removeRow(sessionId_);
			}
			else
			{
				matrix_.addRow(sessionId_);
			}
		}
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException, reason=%s", ex.what() );			
        }
		catch( ... )
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Error update PeerStateChangeSubject." );			
			
		}
		
		return 0 ;
	}
}
