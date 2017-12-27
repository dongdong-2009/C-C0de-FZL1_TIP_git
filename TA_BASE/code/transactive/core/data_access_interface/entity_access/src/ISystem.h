/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ISystem.h $
  * @author <Rob Ashcroft>
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *   
  * ISystem
  */

#if !defined(AFX_ISYSTEM_H__33D78B10_DF00_4032_B61C_0E1C4E40D7BC__INCLUDED_)
#define AFX_ISYSTEM_H__33D78B10_DF00_4032_B61C_0E1C4E40D7BC__INCLUDED_

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class ISystem : public virtual IEntityData
    {

    public:
        /**
         * blahBlahBlah
         *
         * Returns a boolean value indicating if this Camera has the Movement parameter set.
         *
         * @return true if this Camera has the Movement parameter set.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        virtual std::string getServices() = 0;

        virtual std::string getDebugLogDir() = 0;

        virtual std::string getMonitorServerList() = 0;

        virtual std::string getMonitorSlaveList() = 0;

        virtual std::string getFailover() = 0;

        virtual std::string getPreStartActiveProg() = 0;

        virtual std::string getPreStopActiveProg() = 0;

        virtual std::string getPostStopActiveProg() = 0;

        virtual std::string getPreShutdownProg() = 0;

        virtual std::string getPostShutdownProg() = 0;

        virtual std::string getPreFailoverProg() = 0;

        virtual std::string getFailoverProg() = 0;

        virtual std::string getNonFailoverProg() = 0;

        virtual std::string getPostFailoverProg() = 0;

        virtual std::string getDBFailoverProg() = 0;

        virtual std::string getPostDBFailoverProg() = 0;

        virtual std::string getDBCleanupProg() = 0;

        virtual std::string getPostDBCleanupProg() = 0;

        virtual std::string getCleanupProg() = 0;

        virtual std::string getPostCleanupProg() = 0;

        virtual std::string getSecondaryFailProg() = 0;

        virtual std::string getPostSecondaryFailProg() = 0;

        virtual std::string getDBStatusProg() = 0;

        virtual std::string getDBStatusFailProg() = 0;
    
    };

} //close namespace TA_Base_Core

#endif // !defined(AFX_ISYSTEM_H__33D78B10_DF00_4032_B61C_0E1C4E40D7BC__INCLUDED_)
