/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/IJobDetailsPlugin.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This interface must be implemented by each plugin component library. The Scheduling Manager uses
  * this interface to retrieve information from each component and also to pass other information.
  * 
  */

#if !defined(IJobDetailsPlugin_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
#define IJobDetailsPlugin_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_

#include "app/scheduling/scheduling_manager/src/SchedulingManagerStructs.h"
#include "app/scheduling/scheduling_manager/src/stdafx.h"
#include <vector>

namespace TA_Base_Core
{
    class IDllHandler;
}

namespace TA_Base_App
{
    class IJobDetailsPlugin
    {
    public:
        struct JobData
        {
            CString name;                           // auto generared job label
            CString description;                    // auto generated description of the job
            std::vector<NameValuePair> parameters;  // the name value pairs associated with the job
        };

    public:
        /**
         * Destructor
         */
        virtual ~IJobDetailsPlugin(){ };


        /** 
         * setUpSingletons
         *
         * This will set up the singletons in the DLL to match those in the main application.
         *
         * @param dllHandler This is a handler from the main application that can be used to
         *                       retrieve the details of it's singletons. This can then be used to
         *                       set up the singletons in the DLL.
         * @param name The name of the DLL
         */
        virtual void setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler, const std::string& name) =0;


        /** 
          * setData
          *
          * Set the job specific info to be shown on the job details dialog.
          * Name and description may be left blank, they should update automatically
          * when getUpdatedData() is called.
          *
          * @param data The job details
          */
	    virtual void setData(const JobData& data) =0;

        
        /** 
          * getData
          *
          * Retrieve the raw job details as previously set. These are unaffected
          * by any changes that may have been made via the job details dialog.
          *
          * @return the unchanged job details
          */
        virtual JobData getData() const =0;


        /** 
          * getUpdatedData
          *
          * Retrieve the job details, including the list of parameters the operator
          * has entered on the job details dialog. The name and description will
          * be refreshed if not already set by the operator, or if forced refresh
          * is requested.
          *
          * @param forceRefresh If this is true, the returned name and description will
          *                     be automatically updated based on the current parameters.
          *                     If false, they will be updated only if their current
          *                     value is determined not to have been manually set by
          *                     the operator.
          *
          * @return the updated job details
          */
        virtual JobData getUpdatedData(const bool forceRefresh) =0;


        /** 
          * showSelectJobDialog
          *
          * Shows the job details dialog to get operator's input.
          * 
          * @param readOnly If this is true, the dialog that is displayed will not allow its data to be modified
          */
        virtual void showSelectJobDialog(const bool readOnly) =0;

        
        /** 
          * showJobDetailsDialog
          *
          * Shows the job details dialog.  Unlike the showSelectJobDialog, the job shown
          * in this dialog is not modifiable and will not be getting input from the operator
          *
          */
        virtual void showJobDetailsDialog() =0;

    };
};
#endif // !defined(IJobDetailsPlugin_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
