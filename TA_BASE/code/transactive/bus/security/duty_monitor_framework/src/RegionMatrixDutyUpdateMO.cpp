// RegionMatrixDutyUpdateMO.cpp: implementation of the RegionMatrixDutyUpdateMO class.
//
//////////////////////////////////////////////////////////////////////

#include "DutyMonitorFramework.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus 
{
	
	RegionMatrixDutyUpdateMO::RegionMatrixDutyUpdateMO(RegionDutyMatrix& matrix,
				DutyAgentTypeCorbaDef::RegionDutySequence& regionDutySeq,
				EDutyUpdateTupe updateType):
	matrix_(matrix),duty_(regionDutySeq),updateType_(updateType)
	{
	
	}
	
	RegionMatrixDutyUpdateMO::~RegionMatrixDutyUpdateMO()
	{
	
	}
	int RegionMatrixDutyUpdateMO::call()
	{
		try
		{
			matrix_.updateDuty(duty_,updateType_);
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
