/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TrainSettingsEntityData.cpp $
  * @author Dirk McCormick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ISchedulingAgentEntityData provides read-only access to the
  * Scheduling Agent's entity data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include <iostream>

#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string TrainSettingsEntityData::ENTITY_TYPE                      = "TrainSettings";
    const std::string TrainSettingsEntityData::TimsMessageRetryLimit = "TimsMessageRetryLimit";
    const std::string TrainSettingsEntityData::AutoContinueAckLimit = "AutoContinueAckLimit";
    const std::string TrainSettingsEntityData::DefaultTimsMessageTimeout = "DefaultTimsMessageTimeout";
    const std::string TrainSettingsEntityData::TimerCctv1 = "Timer_CCTV1";
    const std::string TrainSettingsEntityData::TimerVc1 = "Timer_VC1";
    const std::string TrainSettingsEntityData::TimerVc2 = "Timer_VC2";
    const std::string TrainSettingsEntityData::TimerPec2 = "Timer_PEC2";
	const std::string TrainSettingsEntityData::TimerPec3 = "Timer_PEC3";
    const std::string TrainSettingsEntityData::TimerPec4 = "Timer_PEC4";
	const std::string TrainSettingsEntityData::TimerPec5 = "Timer_PEC5";
    const std::string TrainSettingsEntityData::TimerPa1 = "Timer_PA1";
    const std::string TrainSettingsEntityData::TimerPa2 = "Timer_PA2";
    const std::string TrainSettingsEntityData::TimerPa3 = "Timer_PA3";
    const std::string TrainSettingsEntityData::TimerPa4 = "Timer_PA4";
    const std::string TrainSettingsEntityData::TimerTis1 = "Timer_TIS1";
    const std::string TrainSettingsEntityData::TimerTis2 = "Timer_TIS2";
    const std::string TrainSettingsEntityData::TimerIscs1 = "Timer_ISCS1";
    const std::string TrainSettingsEntityData::TimerIscs2 = "Timer_ISCS2";
    const std::string TrainSettingsEntityData::Oa1Timeout = "OA1TableTimeout";
    const std::string TrainSettingsEntityData::AtcTimeout = "ATCDataTimeout";
    const std::string TrainSettingsEntityData::TotalTrainNumber = "TotalTrainNumber";
    const std::string TrainSettingsEntityData::CarOnePecOneCamera = "CarOnePecOneCamera";
    const std::string TrainSettingsEntityData::CarOnePecTwoCamera = "CarOnePecTwoCamera";
    const std::string TrainSettingsEntityData::CarOnePecThreeCamera = "CarOnePecThreeCamera";
    const std::string TrainSettingsEntityData::CarOnePecFourCamera = "CarOnePecFourCamera";
    const std::string TrainSettingsEntityData::CarTwoPecOneCamera = "CarTwoPecOneCamera";
    const std::string TrainSettingsEntityData::CarTwoPecTwoCamera = "CarTwoPecTwoCamera";
    const std::string TrainSettingsEntityData::CarTwoPecFourCamera = "CarTwoPecFourCamera";
    const std::string TrainSettingsEntityData::CarTwoPecThreeCamera = "CarTwoPecThreeCamera";
    const std::string TrainSettingsEntityData::CarThreePecOneCamera = "CarThreePecOneCamera";
    const std::string TrainSettingsEntityData::CarThreePecTwoCamera = "CarThreePecTwoCamera";
    const std::string TrainSettingsEntityData::CarThreePecThreeCamera = "CarThreePecThreeCamera";
    const std::string TrainSettingsEntityData::CarThreePecFourCamera = "CarThreePecFourCamera";
    const std::string TrainSettingsEntityData::ThreadPoolSize = "ThreadPoolSize";
	const std::string TrainSettingsEntityData::StateSyncLocPriority = "StateSyncLocPriority";
	const std::string TrainSettingsEntityData::TrainStatus12 = "TrainStatus12";

    TrainSettingsEntityData::TrainSettingsEntityData(unsigned long key)
        : EntityData(key, getType())
    {
    }

    TrainSettingsEntityData::TrainSettingsEntityData()
    {
    }
    
    TrainSettingsEntityData::~TrainSettingsEntityData()
    {
    }
	
    
    std::string TrainSettingsEntityData::getType()
    {
        return getStaticType();
    }


    const std::string TrainSettingsEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}
	
    IEntityData* TrainSettingsEntityData::clone(unsigned long key)
    {
        return new TrainSettingsEntityData(key);
    }
	
	
    void TrainSettingsEntityData::invalidate()
    {
        getHelper()->invalidate();
    }
	

    unsigned long TrainSettingsEntityData::getTimsRetryLimit()
    {
        std::string value = getHelper()->getParameter(TimsMessageRetryLimit);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimsMessageRetryLimit Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimsAutoContinueAckLimit()
    {
        std::string value = getHelper()->getParameter(AutoContinueAckLimit);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "AutoContinueAckLimit Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerVC1()
    {
        std::string value = getHelper()->getParameter(TimerVc1);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerVc1 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerVC2()
    {
        std::string value = getHelper()->getParameter(TimerVc2);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerVc2 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }

    unsigned long TrainSettingsEntityData::getTimerPEC2()
    {
        std::string value = getHelper()->getParameter(TimerPec2);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerPec2 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }

	unsigned long TrainSettingsEntityData::getTimerPEC3()
	{
		std::string value = getHelper()->getParameter(TimerPec3);
		if (!value.empty())
		{
			return EntityHelper::getUnsignedLongData(value);
		}

		TA_THROW( DataException( "TimerPec3 Not defined", DataException::NO_VALUE, "" ) );
		return 0;
	}


    unsigned long TrainSettingsEntityData::getTimerPEC4()
    {
        std::string value = getHelper()->getParameter(TimerPec4);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerPec4 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }

	unsigned long TrainSettingsEntityData::getTimerPEC5()
	{
		std::string value = getHelper()->getParameter(TimerPec5);
		if (!value.empty())
		{
			return EntityHelper::getUnsignedLongData(value);
		}

		TA_THROW( DataException( "TimerPec5 Not defined", DataException::NO_VALUE, "" ) );
		return 0;
	}

    unsigned long TrainSettingsEntityData::getTimerPA1()
    {
        std::string value = getHelper()->getParameter(TimerPa1);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerPa1 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerPA2()
    {
        std::string value = getHelper()->getParameter(TimerPa2);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerPa2 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerPA3()
    {
        std::string value = getHelper()->getParameter(TimerPa3);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerPa3 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerPA4()
    {
        std::string value = getHelper()->getParameter(TimerPa4);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerPa4 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerCCTV1()
    {
        std::string value = getHelper()->getParameter(TimerCctv1);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerCctv1 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }

    unsigned long TrainSettingsEntityData::getDefaultTimsMessageTimeout()
    {
        std::string value = getHelper()->getParameter(DefaultTimsMessageTimeout);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "DefaultTimsMessageTimeout Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }
    

    unsigned long TrainSettingsEntityData::getTimerTIS1()
    {
        std::string value = getHelper()->getParameter(TimerTis1);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerTis1 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerTIS2()
    {
        std::string value = getHelper()->getParameter(TimerTis2);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerTis2 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerISCS1()
    {
        std::string value = getHelper()->getParameter(TimerIscs1);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerIscs1 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getTimerISCS2()
    {
        std::string value = getHelper()->getParameter(TimerIscs2);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "TimerIscs2 Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getOa1Timeout()
    {
        std::string value = getHelper()->getParameter(Oa1Timeout);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "Oa1Timeout Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getAtcTimeout()
    {
        std::string value = getHelper()->getParameter(AtcTimeout);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "AtcTimeout Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getTotalTrainNumber()
    {
        std::string value = getHelper()->getParameter(TotalTrainNumber);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "TotalTrainNumber Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarOnePecOneCamera()
    {
        std::string value = getHelper()->getParameter(CarOnePecOneCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarOnePecOneCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarOnePecTwoCamera()
    {
        std::string value = getHelper()->getParameter(CarOnePecTwoCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarOnePecTwoCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarOnePecThreeCamera()
    {
        std::string value = getHelper()->getParameter(CarOnePecThreeCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarOnePecThreeCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarOnePecFourCamera()
    {
        std::string value = getHelper()->getParameter(CarOnePecFourCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarOnePecFourCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarTwoPecOneCamera()
    {
        std::string value = getHelper()->getParameter(CarTwoPecOneCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarTwoPecOneCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarTwoPecTwoCamera()
    {
        std::string value = getHelper()->getParameter(CarTwoPecTwoCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarTwoPecTwoCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarTwoPecThreeCamera()
    {
        std::string value = getHelper()->getParameter(CarTwoPecThreeCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarTwoPecThreeCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarTwoPecFourCamera()
    {
        std::string value = getHelper()->getParameter(CarTwoPecFourCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarTwoPecFourCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarThreePecOneCamera()
    {
        std::string value = getHelper()->getParameter(CarThreePecOneCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarThreePecOneCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarThreePecTwoCamera()
    {
        std::string value = getHelper()->getParameter(CarThreePecTwoCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarThreePecTwoCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarThreePecThreeCamera()
    {
        std::string value = getHelper()->getParameter(CarThreePecThreeCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarThreePecThreeCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned char TrainSettingsEntityData::getCarThreePecFourCamera()
    {
        std::string value = getHelper()->getParameter(CarThreePecFourCamera);
        if (!value.empty())
        {
            return static_cast< unsigned char > ( EntityHelper::getUnsignedLongData(value) );
        }

        TA_THROW( DataException( "CarThreePecFourCamera Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }


    unsigned long TrainSettingsEntityData::getMaxThreadPoolSize()
    {
        std::string value = getHelper()->getParameter(ThreadPoolSize);
        if (!value.empty())
        {
            return EntityHelper::getUnsignedLongData(value);
        }

        TA_THROW( DataException( "ThreadPoolSize Not defined", DataException::NO_VALUE, "" ) );
        return 0;
    }

	std::string TrainSettingsEntityData::getStateSyncLocPriority()
	{
		std::string value = getHelper()->getParameter(StateSyncLocPriority);
		if (!value.empty())
		{
			return value;
		}
		TA_THROW( DataException("DepotSyncPriority not defined", DataException::NO_VALUE, ""));

		return "";
	}

	std::string TrainSettingsEntityData::getTrainStatus12()
	{
		std::string value = getHelper()->getParameter(TrainStatus12);
		if (!value.empty())
		{
			return value;
		}
		TA_THROW( DataException("TrainStatus12 parameter not defined in the Database", DataException::NO_VALUE, ""));

		return "";
	}

} //close namespace TA_Base_Core

