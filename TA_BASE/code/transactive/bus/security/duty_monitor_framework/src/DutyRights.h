#if !defined(_DUTYRIGHTS_H__0F884B6F_C261_4DE0_A487_E548D66E605B__INCLUDED_)
#define _DUTYRIGHTS_H__0F884B6F_C261_4DE0_A487_E548D66E605B__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyRights.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Hides the singleton implementation of DutyRightsSingleton from clients.
  *
  */

namespace TA_Base_Bus
{
    class DutyRights
    {
    public:
        DutyRights();
        virtual ~DutyRights();

        /**
          * Returns true if the current session has sufficient rights to transfer region duties.
          *
          */
        bool canTransferRegions();

		//TD19218---- LiDan 27,Jun,2007
        /**
		  * @param lActionId: the action id in db
          * Returns true if the current session has sufficient rights to transfer subsystem duties.
          *
          */
        bool canTransferSubsystems(long lActionId);
		//----TD19218

        /**
          * Returns true if the current session has sufficient rights to perform system-wide duty delegation.
          *
          */
        bool canDegradeSubsystems();

    private:
        DutyRights(const DutyRights& dutyRights);
        DutyRights& operator=(const DutyRights& dutyRights);
    };

}

#endif // !defined(_DUTYRIGHTS_H__0F884B6F_C261_4DE0_A487_E548D66E605B__INCLUDED_)
