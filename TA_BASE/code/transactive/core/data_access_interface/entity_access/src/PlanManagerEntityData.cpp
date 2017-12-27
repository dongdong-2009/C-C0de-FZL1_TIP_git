/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PlanManagerEntityData.cpp $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of PlanManagerEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/PlanManagerEntityData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const char* const ENTITY_TYPE = "PlanManager";
    const char* const FLOWCHART_DIR = "FlowchartDir";
    const char* const REPORT_TEMPLATE = "ReportTemplate";
    const char* const EXPORT_DIRECTORY = "ExportDirectory";
    const char* const MAX_CATEGORY_DEPTH = "MaxCategoryDepth";

    const char* const DEFAULT_MAX_CATEGORY_DEPTH = "5";

    PlanManagerEntityData::PlanManagerEntityData() : EntityData()
    {
    }

    PlanManagerEntityData::PlanManagerEntityData(unsigned long key) : EntityData(key, getStaticType())
    {
    }

    PlanManagerEntityData::~PlanManagerEntityData()
    {
    }

    std::string PlanManagerEntityData::getFlowchartDirectory()
    {
		return getHelper()->getParameter(FLOWCHART_DIR);
   }

    std::string PlanManagerEntityData::getReportTemplate()
    {
		return getHelper()->getParameter(REPORT_TEMPLATE);
    }

	std::string PlanManagerEntityData::getExportDirectory()
	{
		return getHelper()->getParameter(EXPORT_DIRECTORY);
    }

    std::string PlanManagerEntityData::getMaxCategoryDepth()
    {
        std::string strDepth = getHelper()->getParameter(MAX_CATEGORY_DEPTH);
        if ( strDepth.empty() )
        {
            strDepth = DEFAULT_MAX_CATEGORY_DEPTH;
        }

        return strDepth.c_str();
    }

    void PlanManagerEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

    std::string PlanManagerEntityData::getType()
    {
		return getStaticType();
    }
    
    std::string PlanManagerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    IEntityData* PlanManagerEntityData::clone(unsigned long key)
    {
        return (new PlanManagerEntityData(key));
    }
} // namespace TA_Base_Core
