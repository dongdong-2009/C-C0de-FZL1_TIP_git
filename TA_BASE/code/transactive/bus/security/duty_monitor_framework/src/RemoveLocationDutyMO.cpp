#include "DutyMonitorFramework.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus 
{
	
	RemoveLocationDutyMO::RemoveLocationDutyMO(RegionDutyMatrix& matrix,unsigned long locationKey)
	:matrix_(matrix),
	 m_locationKey(locationKey)
	{
	
	}
	
	RemoveLocationDutyMO::~RemoveLocationDutyMO()
	{
	
	}
	int RemoveLocationDutyMO::call()
	{
		try
		{
			matrix_.removeLocationDuty(m_locationKey);
		}
        // TD9899: Added logging to allow peer communication to be debugged.
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException, reason=%s", ex.what() );			
        }
		catch( ... )
		{
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Error RemoveLocationDutyMO::call()" );			
			
		}
		
		return 0 ;
	}
}