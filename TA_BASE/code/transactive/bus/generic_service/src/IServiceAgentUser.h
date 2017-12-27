/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_service/src/IServiceAgentUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last mofified by:  $Author: CM $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#if !defined(ISERVICEAGENTUSER)
#define ISERVICEAGENTUSER

#include "bus/generic_agent/src/IGenericAgentUser.h"

namespace TA_Base_Bus
{
	class IServiceAgentUser : public virtual IGenericAgentUser
	{
    public:

        /**
          * Constructor
          */
        IServiceAgentUser(){}

        /**
          * Destructor
          */
        virtual ~IServiceAgentUser(){}

        /**
          * start
          *
          * This pure virtual method is used to start any processing
		  * after the service agent is set up
          *
          */
        virtual void start() = 0;

    private:
        // Disable copy constructor and assignment operator
        IServiceAgentUser( const IServiceAgentUser& theServiceAgentUser);
        IServiceAgentUser& operator=(const IServiceAgentUser &);
    };
}

#endif // !defined(ISERVICEAGENTUSER)
