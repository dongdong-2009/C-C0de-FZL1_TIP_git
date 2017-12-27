/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DutyManagerEntityData.cpp $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface for Duty Manager entity configuration access.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/DutyManagerEntityData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	const unsigned long DutyManagerEntityData::USE_DEFAULT_TIMEOUT = 0;
	static const std::string USE_LOCATION_LABEL("UseLocationLabel");
	static const std::string NORMAL_DUTY_COLOUR("NormalDutyColour");
	static const std::string DELEGATED_DUTY_COLOUR("DelegatedDutyColour");
	static const std::string DEGRADED_DUTY_COLOUR("DegradedDutyColour");
	static const std::string UNALLOCATED_SUBSYSTEM_COLOUR("UnallocatedSubsystemColour");
	static const std::string CORBA_TIMEOUT_SECONDS("CorbaTimeoutSeconds");

    DutyManagerEntityData::DutyManagerEntityData() :
        EntityData()
    {
    }


    DutyManagerEntityData::DutyManagerEntityData(unsigned long key) :
        EntityData(key, getStaticType())
    {
    }


    DutyManagerEntityData::~DutyManagerEntityData()
    {
    }


    void DutyManagerEntityData::invalidate()
    {
        getHelper()->invalidate();
    }


    std::string DutyManagerEntityData::getType()
    {
		return getStaticType();
    }


    std::string DutyManagerEntityData::getStaticType()
    {
        static const std::string DUTY_MANAGER_ENTITY_TYPE("DutyManager");

        return DUTY_MANAGER_ENTITY_TYPE;
    }


    IEntityData* DutyManagerEntityData::clone(unsigned long key)
    {
        return (new DutyManagerEntityData(key));
    }


    bool DutyManagerEntityData::useLocationLabel()
    {
		std::string value = getHelper()->getParameter(USE_LOCATION_LABEL);
		bool useLocationLabel = false;
		if (!value.empty())
		{
			useLocationLabel = EntityHelper::getBooleanData(value);
		}

        return useLocationLabel;
    }


    std::string DutyManagerEntityData::getNormalDutyColour()
    {
		return getHelper()->getParameter(NORMAL_DUTY_COLOUR);
    }


    std::string DutyManagerEntityData::getDelegatedDutyColour()
    {
		return getHelper()->getParameter(DELEGATED_DUTY_COLOUR);
    }


    std::string DutyManagerEntityData::getDegradedDutyColour()
    {
		return getHelper()->getParameter(DEGRADED_DUTY_COLOUR);
    }


    std::string DutyManagerEntityData::getUnallocatedSubsystemColour()
    {
		return getHelper()->getParameter(UNALLOCATED_SUBSYSTEM_COLOUR);
    }


    unsigned long DutyManagerEntityData::getCorbaTimeoutSeconds()
    {
		std::string value = getHelper()->getParameter(CORBA_TIMEOUT_SECONDS);
		unsigned long corbaTimeoutSeconds = USE_DEFAULT_TIMEOUT;
		if (!value.empty())
		{
			corbaTimeoutSeconds = EntityHelper::getUnsignedLongData(value);
		}

        return corbaTimeoutSeconds;
    }

} // namespace TA_Base_Core
