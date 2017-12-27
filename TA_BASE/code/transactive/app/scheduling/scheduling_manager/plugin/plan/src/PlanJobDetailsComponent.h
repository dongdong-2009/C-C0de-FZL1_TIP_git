/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/plugin/plan/src/PlanJobDetailsComponent.h $
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

#if !defined(PlanJobDetailsComponent_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
#define PlanJobDetailsComponent_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_

#include "app/scheduling/scheduling_manager/plugin/plugin_helper/src/IAbstractJobDetails.h"

namespace TA_Base_App
{
    class PlanJobDetailsComponent : public IAbstractJobDetails
    {

    public:
        /**
         * Constructor
         */
        PlanJobDetailsComponent();


        /**
         * Destructor
         */
        virtual ~PlanJobDetailsComponent(){ };


        /** 
          * showSelectJobDialog
          *
          * Shows the job details dialog to get operator's input.
          * 
          * @param readOnly If this is true, the dialog that is displayed will not allow its data to be modified
          */
        virtual void showSelectJobDialog(const bool readOnly);

    
        /** 
          * showJobDetailsDialog
          *
          * Shows the job details dialog.  Unlike the showSelectJobDialog, the job shown
          * in this dialog is not modifiable and will not be getting input from the operator
          *
          */
        virtual void showJobDetailsDialog();


        /** 
          * getUpdatedData
          *
          * Retrieve the job details, including the list of parameters the operator
          * has entered on the job details dialog. The job name will be set to the 
          * full path of the selected plan. The description will be automatically set
          * if not already entered by the operator, or if forced refresh is requested.
          *
          * @param forceRefresh If this is true, the returned job description will be
          *                     automatically updated based on the current plan path.
          *                     If false, it will be updated only if last time it was
          *                     generated automatically as well.
          *
          * @return the updated job details
          */
        virtual JobData getUpdatedData(const bool forceRefresh);
   

    private:
        /** 
          * getPlanPathFromParameters
          *
          * Returns the full path of the plan associated with the job.
          * This information is extracted from job parameters.
          * Returns empty string if the parameters do not contain the plan path.
          */
        CString getPlanPathFromParameters() const;


        /** 
          * generateJobDescription
          *
          * Returns the default description of the plan job for the specified plan.
          */  
        CString generateJobDescription(const CString& planPath) const;


    private:
        // The full path of the plan currently associated with this plan job.
        CString m_planPath;
    };
};
#endif // !defined(PlanJobDetailsComponent_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
