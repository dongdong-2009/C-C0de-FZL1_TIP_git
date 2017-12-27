/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/EquipReportAccessFactory.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if !defined(AFX_EQUIPREPORTACCESSFACTORY_H__8E45F2E4_55C1_4DC5_A1DF_3F9DA3097A7B__INCLUDED_)
#define AFX_EQUIPREPORTACCESSFACTORY_H__8E45F2E4_55C1_4DC5_A1DF_3F9DA3097A7B__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/EquipReport.h"
#include "core/data_access_interface/src/ConfigEquipReport.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	class EquipReportAccessFactory  
	{

	public:
		bool equipReportAlreadyExistsAtLocationByName(const unsigned long aLocationKey, const std::string &aEquipReportName);
     
		static EquipReportAccessFactory& getInstance();
		
		// copy an existing equipment report to a brand new one
		IConfigEquipReport* copyEquipReport(const IConfigEquipReport *aEquipReportToCopy, bool copyDependants = true);
		
		// creates a new equipment report
		IConfigEquipReport* createEquipReport();
		
		// get the equipment report based on a primary key
		IEquipReport* getEquipReportByKey(std::string& aEquipReportKey, const bool readWrite = false);

		
		// get all the equipment reports in the database
		std::vector<IEquipReport*> getAllEquipReports(const bool readWrite = false);
		
		// get all the equipment reports for a particular location
		std::vector<IEquipReport*> getAllEquipReportsByLocationKey(unsigned long aLocationKey, const bool readWrite = false);

		virtual ~EquipReportAccessFactory() {};

	private:

		// we dont want the default constructor so make it private
		EquipReportAccessFactory() {};
		// we dont want the default assignment operator so make it private
		EquipReportAccessFactory& operator=(const EquipReportAccessFactory&);
		EquipReportAccessFactory(const EquipReportAccessFactory& theEquipReportAccessFactory) {};
		
		void getEquipReportBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IEquipReport*>& reports, const bool readWrite);
		
		
		static EquipReportAccessFactory* m_instance;
	};

}
#endif // !defined(AFX_EQUIPREPORTACCESSFACTORY_H__8E45F2E4_55C1_4DC5_A1DF_3F9DA3097A7B__INCLUDED_)
