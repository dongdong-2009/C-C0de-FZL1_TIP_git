/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IEquipReportData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IEquipReportData is an interface to a EquipReportData object. 
  * It allows the EquipReportData object implementation to be changed 
  * (e.g. if necessary due to a database schema change) without changing code that uses it.
  *
  * NOTE: 
  */

#if !defined(AFX_IEQUIPREPORTDATA_H__47B0DFB8_03CE_4BA0_829D_7B58B49E5B94__INCLUDED_)
#define AFX_IEQUIPREPORTDATA_H__47B0DFB8_03CE_4BA0_829D_7B58B49E5B94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/src/IItem.h"
#include <string>

namespace TA_Base_Core
{
	class IEquipReportData : public IItem
	{

	public:
		virtual ~IEquipReportData() {};
		
		// specific to the table
        virtual std::string   getId() = 0;
        virtual std::string   getReportKey() = 0;
		virtual unsigned long getDataNodeKey() = 0;
		virtual unsigned long getDataPointKey() = 0;
	};

} // TA_Base_Core

#endif // !defined(AFX_IEQUIPREPORTDATA_H__47B0DFB8_03CE_4BA0_829D_7B58B49E5B94__INCLUDED_)

