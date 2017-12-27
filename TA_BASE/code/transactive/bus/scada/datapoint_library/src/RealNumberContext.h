/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/RealNumberContext.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:43:23 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *	Real Number Context...Required by the Mathematical_Evaluation library
  *
  */

#ifndef REAL_NUMBER_CONTEXT_H
#define REAL_NUMBER_CONTEXT_H

#include "bus/mathematical_evaluation/src/IRealNumberContext.h"
#include "bus/scada/datapoint_library/src/CommonContext.h"


namespace TA_Base_Bus
{

	class RealNumberContext : public TA_Base_Bus::IRealNumberContext,
							  public CommonContext
	{

	public:

		/**
		* Constructor, destructor
		* 
		*/
		RealNumberContext();
		RealNumberContext ( IExpressionNodeNotification * userOfExpression ); // for callback operation
		virtual ~RealNumberContext();


		//
		// Operations required by IBooleanContext interface
		//


		/**
		 * getValue
		 *
		 * @exception MathematicalEvaluationException - if variable cannot be found
		 */
		virtual double getValue ( const std::string & variableName ) const;

	};

}

#endif
