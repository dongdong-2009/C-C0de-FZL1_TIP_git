/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/RealNumberContext.cpp $
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


#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/datapoint_library/src/RealNumberContext.h"


namespace TA_Base_Bus
{

	RealNumberContext::RealNumberContext()
	:
	CommonContext()
	{

	}


	RealNumberContext::RealNumberContext ( IExpressionNodeNotification * userOfExpression )
	:
	CommonContext ( userOfExpression )
	{

	}


	RealNumberContext::~RealNumberContext()
	{

	}


	double RealNumberContext::getValue ( const std::string & variableName ) const
	{
        double result = const_cast< RealNumberContext * >(this)->getDoubleValue ( variableName );

        //TD16581
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getValue(%s) = %f", variableName.c_str(), result);

        return result;
	}  

}
