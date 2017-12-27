/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IProcess.h $
  * @author Rob Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *   
  * IProcess is an interface to the MonitoredProcess entity
  * type.
  */

#if !defined(AFX_IPROCESS_H__04EB37B0_123F_4779_9931_DCD17A47B8A9__INCLUDED_)
#define AFX_IPROCESS_H__04EB37B0_123F_4779_9931_DCD17A47B8A9__INCLUDED_

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IProcess : public virtual IEntityData
    {
    public:

		// Methods to fetch each entity parameter
        virtual std::string getUserName() = 0;
        virtual bool isManagedProcess() = 0;
        virtual bool isServiceProcess() = 0;
        virtual unsigned int getStartLag() = 0;
        virtual unsigned int getTerminateTimeout() = 0;
        virtual unsigned int getKillTimeout() = 0;
        virtual unsigned int getStartupTimeout() = 0;
        virtual unsigned int getChangeControlTimeout() = 0;
        virtual unsigned int getChangeMonitorTimeout() = 0;
        virtual bool shouldAutoFailback() = 0;
		virtual int getPriority() = 0;
		virtual int getBoundProcessor() = 0;
		virtual std::string getPassWord() = 0;
    };

    typedef boost::shared_ptr<IProcess> IProcessPtr;

} //close namespace TA_Base_Core

#endif // !defined(AFX_IPROCESS_H__04EB37B0_123F_4779_9931_DCD17A47B8A9__INCLUDED_)
