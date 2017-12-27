#if !defined(ICallbackBurnStatus_H_)
#define ICallbackBurnStatus_H_
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/ICallbackBurnStatus.h $
  * @author:  Lincy
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Call back interface class for CD/DVD write operations.
  **/
#include "app/data_management/archive_manager/src/MediaStatus.h"

using namespace TA_Base_App;
class ICallbackBurnStatus{
public:
	virtual ~ICallbackBurnStatus(){};
	virtual void BurnStatusCallBack (long totalTime, long timeLeft, BurnStatus burnStatus) = 0;
};
#endif