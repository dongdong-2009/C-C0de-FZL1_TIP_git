/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/plugin/plan/src/PlanJobDetailsComponent.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Inherits from the IJobDetailsComponent interface.  This class is responsible for showing a dialog for getting
  * a plan Id from the operator.
  * 
  */

#include <sstream>
#include "app/scheduling/scheduling_manager/plugin/plan/src/StdAfx.h"
#include "app/scheduling/scheduling_manager/plugin/plan/src/PlanDialog.h"
#include "app/scheduling/scheduling_manager/plugin/plan/src/PlanJobDetailsComponent.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/response_plans/plan_tree/src/PlanTreeCtrl.h"
#include "bus/application_types/src/apptypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
    
namespace TA_Base_App
{
    static const std::string PLAN_PATH_PARAMETER("PlanPath");


    PlanJobDetailsComponent::PlanJobDetailsComponent() :
        m_planPath("")
	{
		TA_Base_Bus::CPlanTreeCtrl::RootItemName = "ËùÓÐÔ¤°¸";
    }

    
    void PlanJobDetailsComponent::showSelectJobDialog(const bool readOnly)
    {
        PlanDialog dlg(readOnly, getPlanPathFromParameters());

        if (dlg.DoModal() == IDOK)
        {
            m_planPath = dlg.getSelectedPlanPath();
        }
    }


    void PlanJobDetailsComponent::showJobDetailsDialog()
    {
        CString planPath = getPlanPathFromParameters();
        TA_ASSERT(!planPath.IsEmpty(), 
                  "Should only show the Plan Manager when there is a plan associated with the schedule");

        std::ostringstream command;
        command << "--" << RPARAM_VIEWPLANID << "=" << static_cast<LPCTSTR>(planPath);

        CPoint pt(GetMessagePos());
        using namespace TA_Base_Bus::TA_GenericGui;
        RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT, AREA_SCHEMATIC, pt.x);
        TA_Base_Bus::AppLauncher::getInstance().launchApplication(PLAN_MANAGER_GUI_APPTYPE,
                                                                  command.str().c_str(),
                                                                  POS_BOUNDED,
                                                                  ALIGN_FIT,
                                                                  NULL,
                                                                  &boundary);
    }


    IJobDetailsPlugin::JobData PlanJobDetailsComponent::getUpdatedData(const bool forceRefresh)
    {
        JobData updatedData;
        CString oldPlanPath(getPlanPathFromParameters());
        CString newPlanPath(m_planPath.IsEmpty()? oldPlanPath : m_planPath); // revert to previous plan 
                                                                             // if none has been picked

        // Check we have a valid plan ID.
        if (newPlanPath.IsEmpty())
        {
            return updatedData;
        }

        // Set the job name to contain the current plan path. 
        updatedData.name = newPlanPath;

        // Generate a description for the current plan path.
        CString newAutoDescription(generateJobDescription(newPlanPath));

        // Use the automatically generated description if directly asked to do so.
        if (forceRefresh)
        {
            updatedData.description = newAutoDescription;
        }
        else
        {
            // Generate a description for the previous plan path.
            CString oldAutoDescription(oldPlanPath.IsEmpty()? "" : generateJobDescription(oldPlanPath) );

            // Set the job description to reflect the plan to be executed, but only if the previous 
            // description was also automatically generated. Otherwise reuse the previous job description.
            CString oldDescription(getData().description);
            updatedData.description = (oldDescription == oldAutoDescription? newAutoDescription : oldDescription);
        }

        // Generate updated parameter list.
        NameValuePair planPathParam;
        planPathParam.name = PLAN_PATH_PARAMETER;
        planPathParam.value = static_cast<LPCTSTR>(newPlanPath);

        updatedData.parameters.push_back(planPathParam);

        return updatedData;
    }


    CString PlanJobDetailsComponent::getPlanPathFromParameters() const
    {
        std::vector<NameValuePair> parameters(getData().parameters);

        for (unsigned int i = 0; i < parameters.size(); i++)
        {
            if (parameters[i].name == PLAN_PATH_PARAMETER)
            {
                return parameters[i].value.c_str();
            }
        }

        return "";
    }


    CString PlanJobDetailsComponent::generateJobDescription(const CString& planPath) const
    {
        CString description(_T(""));
    #ifdef AUTO_DESCRIPTION
        AfxFormatString1(description, IDS_DESCRIPTION_FORMAT, planPath); 
    #endif
        return description;
    }
};
