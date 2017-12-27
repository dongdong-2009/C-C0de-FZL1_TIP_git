/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if !defined(AFX_EQUIPREPORTDATA_H__30E577E7_FB1A_4A34_9101_5CBBEB7EC343__INCLUDED_)
#define AFX_EQUIPREPORTDATA_H__30E577E7_FB1A_4A34_9101_5CBBEB7EC343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <ctime>

#include "core/data_access_interface/src/IEquipReportData.h"

namespace TA_Base_Core
{
	class IData;
	class EquipReportDataHelper;

	class EquipReportData : public IEquipReportData  
	{

	public:

		unsigned long getKey();
        std::string   getId();
        std::string   getReportKey();
		unsigned long getDataNodeKey();
		unsigned long getDataPointKey();
		void invalidate();

        EquipReportData(std::string& key);
		EquipReportData(std::string& key, std::string& reportKey);

		/**
         * Constructor
         *
         * Construct a EquipReportData class based around dataset
         */
		EquipReportData(const unsigned long row, TA_Base_Core::IData& data);

		virtual ~EquipReportData();

	private:

		// Name field does not apply to our table so we make it private and inaccessible
		std::string getName() { std::string empty(""); return empty; }; 

		EquipReportDataHelper *m_EquipReportDataHelper;
		EquipReportData& operator=(const EquipReportData&) { };
		EquipReportData(const EquipReportData& theEquipReportData) { };
	};

}

#endif // !defined(AFX_EQUIPREPORTDATA_H__30E577E7_FB1A_4A34_9101_5CBBEB7EC343__INCLUDED_)
