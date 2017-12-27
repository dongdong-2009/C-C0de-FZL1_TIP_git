/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportDataAccessFactory.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
/// EquipReportDataAccessFactory.h: interface for the EquipReportDataAccessFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EQUIPREPORTDATAACCESSFACTORY_H__A94E0006_71FE_4EE5_B49D_95C62CFFFB5E__INCLUDED_)
#define AFX_EQUIPREPORTDATAACCESSFACTORY_H__A94E0006_71FE_4EE5_B49D_95C62CFFFB5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include "core/data_access_interface/src/EquipReportData.h"
#include "core/data_access_interface/src/ConfigEquipReportData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	class EquipReportDataAccessFactory  
	{
	public:
     
		static EquipReportDataAccessFactory& getInstance();

		
		
		// copies a particular equipment report data record to a new one
		IConfigEquipReportData* copyEquipReportData(const IConfigEquipReportData *aEquipReportDataToCopy);
		
		// copies all equipment report data records and changes the reportkey
		void copyEquipReportData(std::string& aBaseReportKey, std::string& aNewReportKey);
		
		// creates a new equipment report data record
		IConfigEquipReportData* createEquipReportData();
		
		// gets the equipment report data record based on a primary key
		IEquipReportData* getEquipReportDataByKey(std::string& aEquipReportDataKey, const bool readWrite = false);
		
		// get the equipment report data record based on the Report Key, Entity Key this record contains,
		// because entity keys are unique we do not need to specify the type of entity this is for
		IEquipReportData* getEquipReportDataKey(std::string& aReportKey, unsigned long aEntityKey);

		// get all the equipment report data records for a particular report key
		std::vector<IEquipReportData*> getAllEquipReportDatumByReportKey(std::string& aReportKey, const bool readWrite = false);

		virtual ~EquipReportDataAccessFactory() {};

	private:
		// we dont want the default constructor so make it private
		EquipReportDataAccessFactory() {};
		// we dont want the default assignment operator so make it private
		EquipReportDataAccessFactory& operator=(const EquipReportDataAccessFactory&);
		EquipReportDataAccessFactory(const EquipReportDataAccessFactory& theEquipReportDataAccessFactory) {};
		
		void getEquipReportDatumBySql(IDatabase* pDatabase,const SQLStatement& sql, std::vector<IEquipReportData*>& reportDatum, const bool readWrite);

		static EquipReportDataAccessFactory* m_instance;
	};

}
#endif // !defined(AFX_EQUIPREPORTDATAACCESSFACTORY_H__A94E0006_71FE_4EE5_B49D_95C62CFFFB5E__INCLUDED_)
