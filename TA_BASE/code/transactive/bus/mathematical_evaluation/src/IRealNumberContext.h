#ifndef M_Eval_IRealNumberContext_H
#define M_Eval_IRealNumberContext_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mathematical_evaluation/src/IRealNumberContext.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:51:44 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Interface for resolving the values of RealNumber variables.
  */

#include <string>

namespace TA_Base_Bus
{
    class IRealNumberContext
    {
    public:
      /**
        * Destructor
        * 
        */
        virtual ~IRealNumberContext()
		{
		}

        virtual double getValue(const std::string& variableName) const = 0;
    };
}
#endif // M_Eval_IRealNumberContext_H
