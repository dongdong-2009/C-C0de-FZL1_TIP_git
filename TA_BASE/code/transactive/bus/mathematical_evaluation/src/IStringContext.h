#ifndef M_Eval_IStringContext_H
#define M_Eval_IStringContext_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mathematical_evaluation/src/IStringContext.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:51:44 $
  * Last modified by:  $Author: haijun.li $
  * 
  * IStringContext is a interface to a context for evaluating a string expression. 
  * It provides a method of resolving values of string variables from their name.
  */

#include <string>

namespace TA_Base_Bus
{
    class IStringContext
    {
    public:
      /**
        * Destructor
        * 
        */
        virtual ~IStringContext()
		{
		}

        /**
         * getValue
         *
         * @exception MathematicalEvaluationException - if variable cannot be found
         */
        virtual std::string getValue(const std::string& name) const = 0;
    };
}

#endif // M_Eval_IStringContext_H
