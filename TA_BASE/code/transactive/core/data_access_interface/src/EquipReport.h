/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReport.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// EquipReport.h: interface for the EquipReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EQUIPREPORT_H__0C257342_E838_4A7C_A8DE_A7D16E49CE15__INCLUDED_)
#define AFX_EQUIPREPORT_H__0C257342_E838_4A7C_A8DE_A7D16E49CE15__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/src/IEquipReport.h"

namespace TA_Base_Core
{
	class IData;
	class EquipReportHelper;

	class EquipReport : public IEquipReport  
	{

	public:
        std::string getId();
		unsigned long	getKey();
		unsigned long	getLocationId();
		std::string		getName();
		unsigned long	getProfile();
		bool			getReportTypeFilterFlag();
		std::string		getReportTypeFilter();
		std::string		getAssetFilter();
		bool			getAssetFilterFlag();
		std::string		getDescFilter();
		bool			getDescFilterFlag();
		std::string		getDPNameFilter();
		bool			getDPNameFilterFlag();
		std::string		getDPTypeFilter();
		bool			getDPTypeFilterFlag();
		std::string		getDPValueFilter();
		bool			getDPValueFilterFlag();
		
		bool			getStatusFilterFlag();
		bool			getScanInhibitFlag();
		bool			getAlarmInhibitFlag();
		bool			getControlInhibitFlag();
		bool			getMmsInhibitFlag();
		bool			getTaggedFlag();
		bool			getTaggedPTWFlag();
		bool			getOverriddenFlag();

		std::string		getStatusChangeByFilter();
		bool			getStatusChangeByFilterFlag();
		std::string		getTimestampFilter();
		bool			getTimestampFilterFlag();

		void			invalidate();
		
		EquipReport(std::string& key);
		EquipReport(std::string& key, const std::string &name, const unsigned long locationId);

		/**
         * Constructor
         *
         * Construct a EquipReport class based around dataset
         */
		EquipReport(const unsigned long row, TA_Base_Core::IData& data);

		virtual ~EquipReport();

	private:
		// we do not want this constructor so make it private 
		EquipReport(const EquipReport& theEquipReport) {};
		// we do not want the assignment operator so make it private
		EquipReport& operator=(const EquipReport&) {};
		
		// pointer to our helper class
		EquipReportHelper* m_EquipReportHelper;

	};

}

#endif // !defined(AFX_EQUIPREPORT_H__0C257342_E838_4A7C_A8DE_A7D16E49CE15__INCLUDED_)
