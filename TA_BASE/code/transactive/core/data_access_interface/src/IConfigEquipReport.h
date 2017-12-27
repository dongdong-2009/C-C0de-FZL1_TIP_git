/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigEquipReport.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#if !defined(IConfigEquipReport_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConfigEquipReport_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IEquipReport.h"
#include "core/data_access_interface/src/IConfigItem.h"

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{

    class IConfigEquipReport : public IConfigItem, public IEquipReport
    {

    public:		

		// set Location Id of the equipment report
		virtual void setLocationId(const unsigned long locationId) = 0;
		virtual void setProfile(const unsigned long profile) = 0;
		
		// set filters
		virtual void setReportTypeFilterFlag(const bool flag) = 0;
		virtual void setReportTypeFilter(const std::string& reportFilter) = 0;
		virtual void setAssetFilter(const std::string& assetFilter) = 0;
		virtual void setAssetFilterFlag(const bool flag) = 0;
		virtual void setDescFilter(const std::string& descFilter) = 0;
		virtual void setDescFilterFlag(const bool flag) = 0;
		virtual void setDPNameFilter(const std::string& dpNameFilter) = 0;
		virtual void setDPNameFilterFlag(const bool flag) = 0;
		virtual void setDPTypeFilter(const std::string& dpTypeFilter) = 0;
		virtual void setDPTypeFilterFlag(const bool flag) = 0;
		virtual void setDPValueFilter(const std::string& dpValueFilter) = 0;
		virtual void setDPValueFilterFlag(const bool flag) = 0;

		virtual void setStatusFilterFlag(const bool flag) = 0;
		virtual void setScanInhibitFlag(const bool flag) = 0;
		virtual void setAlarmInhibitFlag(const bool flag) = 0;
		virtual void setControlInhibitFlag(const bool flag) = 0;
		virtual void setMmsInhibitFlag(const bool flag) = 0;
		virtual void setTaggedFlag(const bool flag) = 0;
		virtual void setTaggedPTWFlag(const bool flag) = 0;
		virtual void setOverriddenFlag(const bool flag) = 0;

		virtual void setStatusChangeByFilter(const std::string& statusFilter) = 0;
		virtual void setStatusChangeByFilterFlag(const bool flag) = 0;
		virtual void setTimestampFilter(const std::string& timestampFilter) = 0;
		virtual void setTimestampFilterFlag(const bool flag) = 0;
		
		// delete this report
		virtual void deleteThisEquipReport() = 0;

		virtual ~IConfigEquipReport() { };
    };

} // TA_Base_Core

#endif // !defined(IConfigEquipReport_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
