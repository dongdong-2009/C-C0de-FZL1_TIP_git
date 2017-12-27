/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IEquipReport.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IEquipReport is an interface to a EquipReport object. 
  * It allows the EquipReport object implementation to be changed 
  * (e.g. if necessary due to a database schema change) without changing code that uses it.
  *
  * NOTE: 
  */

#if !defined(IEquipReport_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IEquipReport_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IEquipReport : public IItem
    {

    public:
        virtual ~IEquipReport() { };
        virtual std::string getId() = 0;
		virtual unsigned long getProfile() = 0;
		virtual bool		getReportTypeFilterFlag() = 0;
		virtual std::string getReportTypeFilter() = 0;
		virtual bool		getAssetFilterFlag() = 0;
		virtual std::string getAssetFilter() = 0;
		virtual bool		getDescFilterFlag() = 0;
		virtual std::string	getDescFilter() = 0;
		virtual bool		getDPNameFilterFlag() = 0;
		virtual std::string	getDPNameFilter() = 0;
		virtual bool		getDPTypeFilterFlag() = 0;
		virtual std::string	getDPTypeFilter() = 0;
		virtual bool		getDPValueFilterFlag() = 0;
		virtual std::string getDPValueFilter() = 0;
		
		// status flags
		virtual bool		getStatusFilterFlag() = 0;		
		virtual bool		getScanInhibitFlag() = 0;
		virtual bool		getAlarmInhibitFlag() = 0;
		virtual bool		getControlInhibitFlag() = 0;
		virtual bool		getMmsInhibitFlag() = 0;
		virtual bool		getTaggedFlag() = 0;
		virtual bool		getTaggedPTWFlag() = 0;
		virtual bool		getOverriddenFlag() = 0;

		virtual bool		getStatusChangeByFilterFlag() = 0;
		virtual std::string getStatusChangeByFilter() = 0;
		virtual bool		getTimestampFilterFlag() = 0;
		virtual std::string	getTimestampFilter() = 0;
    };

} // TA_Base_Core

#endif // !defined(IEquipReport_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
