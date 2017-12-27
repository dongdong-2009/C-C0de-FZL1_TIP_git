/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/alarm/src/Utilities.cpp $
 * @author:  Noel R. Tajanlangit
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2011/01/14 12:03:35 $
 * Last modified by:  $Author: Noel R. Tajanlangit $
 * 
 * This class provides conversion between state type to string type.
 */

#include "core/alarm/src/Utilities.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    Utilities* Utilities::s_utilities = NULL;
    TA_Base_Core::NonReEntrantThreadLockable Utilities::s_singletonLock;

    
    Utilities& Utilities::getInstance()
    {
        // protect the instance check / creation
        TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if(s_utilities == 0)
        {
            s_utilities = new Utilities();
        }
        
        return *s_utilities;
    }

	std::string Utilities::convertDSS2String(const TA_Base_Core::DecisionSupportStateType& dssType)
    {
        switch(dssType)
        {
            case(TA_Base_Core::RelatedPlanHasBeenRun):
                return "R";

            case(TA_Base_Core::RelatedPlanHasBeenPostponed):
                return "X";

            case(TA_Base_Core::RelatedAutomaticPlanNotRun):
                return "A";

            case(TA_Base_Core::RelatedSemiAutomaticPlanNotRun):
                return "S";

            default:
                return "";
        }
    }

	TA_Base_Core::DecisionSupportStateType Utilities::convertString2DDS(const std::string& strDDS)
	{
		switch(strDDS[0])
		{
		case 'R':
			return TA_Base_Core::RelatedPlanHasBeenRun;

		case 'X':
			return TA_Base_Core::RelatedPlanHasBeenPostponed;

		case 'A':
			return TA_Base_Core::RelatedAutomaticPlanNotRun;

		case 'S':
			return TA_Base_Core::RelatedSemiAutomaticPlanNotRun;

		default:
			return NoRelatedPlan;
		}
	}

    std::string Utilities::convertMMS2String(const TA_Base_Core::MmsStateType& mmsType)
    {
        switch (mmsType)
		{
			case TA_Base_Core::MMS_NONE:
				return "";

			case TA_Base_Core::MMS_SEMI:
				return "S";

			case TA_Base_Core::MMS_AUTO:
				return "A";

			case TA_Base_Core::MMS_JR_SENT:
				return "R";

			case TA_Base_Core::MMS_ALM_SENT:
				//TA_ASSERT(false, "Received MMS_ALM_SENT mmsState, which is not supported");
				return "A";

			case TA_Base_Core::MMS_INHIBIT:
				return "I";

			case TA_Base_Core::MMS_FAILED:
				return "D";

			default:
				//TA_ASSERT(false, "Received unknown mmsState. Has the MmsStateType enum changed?");
				return "";
        }
    }

	TA_Base_Core::MmsStateType Utilities::convertString2MMS(const std::string& strMMS)
	{
		switch (strMMS[0])
		{
// 		case '':
// 			return TA_Base_Core::MMS_NONE;

		case 'S':
			return TA_Base_Core::MMS_SEMI;

		case 'A':
			return TA_Base_Core::MMS_AUTO;

		case 'R':
			return TA_Base_Core::MMS_JR_SENT;

		case 'I':
			return TA_Base_Core::MMS_INHIBIT;

		case 'D':
			return TA_Base_Core::MMS_FAILED;

		default:
			return TA_Base_Core::MMS_NONE;
		}
	}

    std::string Utilities::convertAvalanche2String(const bool& bIsHead, const bool& bIsChild)
    {
        if (bIsHead)
        {
            return "H";
        }
		else if (bIsChild)
		{
			return "C";
		}

        return "";
    }

	void Utilities::convertString2Avalanche(const std::string& strAvl, bool& bIsHead, bool& bIsChild)
	{
		if (strAvl == "")
		{
			bIsHead = false;
			bIsChild = false;
		}
		else if (strAvl == "H")
		{
			bIsHead = true;
			bIsChild = false;
		}
		else if (strAvl == "C")
		{
			bIsHead = false;
			bIsChild = true;
		}
	}

} //End of TA_Base_Core

