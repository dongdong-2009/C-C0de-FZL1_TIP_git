/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/plugin/plugin_helper/src/IAbstractJobDetails.h $
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

#if !defined(IAbstractJobDetails_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
#define IAbstractJobDetails_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_

#include "app/scheduling/scheduling_manager/src/IJobDetailsPlugin.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerStructs.h"
#include <vector>

namespace TA_Base_Core
{
    class IDllHandler;
}

namespace TA_Base_App
{
    class IAbstractJobDetails : public IJobDetailsPlugin
    {
    public:
        /**
         * Destructor
         */
        virtual ~IAbstractJobDetails(){ };


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
        virtual void setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler, const std::string& name);


        /** 
          * setData
          *
          * Set the job specific info to be shown on the job details dialog.
          * Name and description may be left blank, they should update automatically
          * when getUpdatedData() is called.
          *
          * @param data The job details
          */
	    virtual void setData(const JobData& data);

        
        /** 
          * getData
          *
          * Retrieve the raw job details as previously set. These are unaffected
          * by any changes that may have been made via the job details dialog.
          *
          * @return the unchanged job details
          */
        virtual JobData getData() const
        {
            return m_jobData;
        }


    private:
        // The specific details associated with the job.
	    JobData m_jobData;
    };

}

#endif // !defined(IAbstractJobDetails_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
