/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TelephoneManagerProfileEntityData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * TelephoneManagerProfileEntityData is an implementation of IEntityData. It proves read-only access to Radio directory data,
 * and a specific framework for Radio directory parameters.
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include <iostream>

#include "core/data_access_interface/entity_access/src/TelephoneManagerProfileEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string TelephoneManagerProfileEntityData::ENTITY_TYPE                = "TelephoneManagerProfile";
    const std::string TelephoneManagerProfileEntityData::HISTORY_LIMIT   = "HistoryLimit";
	const std::string TelephoneManagerProfileEntityData::PROFILE_BUTTONS = "ProfileButtons";
	const std::string TelephoneManagerProfileEntityData::USER_BUTTONS    = "UserButtons";
	const std::string TelephoneManagerProfileEntityData::DIALPAD_MAXNUMBER_LENGTH = "Dialpad_MaxNumberLength";
	const std::string TelephoneManagerProfileEntityData::DIALPAD_HISTORY_LENGTH   = "Dialpad_RecentlyDialledLength";
	const std::string TelephoneManagerProfileEntityData::RADIO_PATCH_NUMBER  = "RadioPatchNumber";
	const std::string TelephoneManagerProfileEntityData::EPAX_PATCH_NUMBER  = "EPAXPatchNumber";
	const std::string TelephoneManagerProfileEntityData::MFT_EXTNUMBER  = "MFT_ExtensionNumber";

    TelephoneManagerProfileEntityData::TelephoneManagerProfileEntityData(unsigned long key)
		: EntityData(key, getType())
    {
    }

    TelephoneManagerProfileEntityData::~TelephoneManagerProfileEntityData()
    {
    }


    unsigned long TelephoneManagerProfileEntityData::getHistoryLimit()
    {
		std::string value =  getHelper()->getParameter(HISTORY_LIMIT);
		unsigned long historyLimit = 1000;
		if (!value.empty())
		{
			historyLimit = EntityHelper::getUnsignedLongData(value);
		}

        return historyLimit;
    }


    unsigned short TelephoneManagerProfileEntityData::getProfileButtons()
    {
		std::string value =  getHelper()->getParameter(PROFILE_BUTTONS);
		unsigned short profileButtons = 12;
		if (!value.empty())
		{
			profileButtons = static_cast< unsigned short >(EntityHelper::getIntegerData(value));
		}
        return profileButtons;
    }


    unsigned short TelephoneManagerProfileEntityData::getUserDefinedButtons()
    {
		std::string value =  getHelper()->getParameter(USER_BUTTONS);
		unsigned short userButtons = 64;
		if (!value.empty())
		{
			userButtons = static_cast< unsigned short >(EntityHelper::getIntegerData(value));
		}

        return userButtons;
    }


    unsigned short TelephoneManagerProfileEntityData::getDialpadMaxNumber()
    {
		std::string value =  getHelper()->getParameter(DIALPAD_MAXNUMBER_LENGTH);
		unsigned short dialpadMaxNum = 16;
		if (!value.empty())
		{
			dialpadMaxNum = static_cast< unsigned short >(EntityHelper::getIntegerData(value));
		}

        return dialpadMaxNum;
    }


    unsigned short TelephoneManagerProfileEntityData::getDialpadMaxHistory()
    {
		std::string value =  getHelper()->getParameter(DIALPAD_HISTORY_LENGTH);
		unsigned short dialpadHistory = 25;
		if (!value.empty())
		{
			dialpadHistory = static_cast< unsigned short >(EntityHelper::getIntegerData(value));
		}

        return dialpadHistory;
    }


    unsigned short TelephoneManagerProfileEntityData::getRadioPatchNumber()
    {
		std::string value =  getHelper()->getParameter(RADIO_PATCH_NUMBER);
		unsigned short radioPatchNumber = 0;
		if (!value.empty())
		{
			radioPatchNumber = static_cast< unsigned short >(EntityHelper::getIntegerData(value));
		}

        return radioPatchNumber;
    }


    unsigned short TelephoneManagerProfileEntityData::getEPAXPatchNumber()
    {
		std::string value =  getHelper()->getParameter(EPAX_PATCH_NUMBER);
		unsigned short epaxPatchNumber = 0;
		if (!value.empty())
		{
			epaxPatchNumber = static_cast< unsigned short >(EntityHelper::getIntegerData(value));
		}
	
        return epaxPatchNumber;
    }


    unsigned short TelephoneManagerProfileEntityData::getMFTExtensionNumber()
    {
		std::string value =  getHelper()->getParameter(MFT_EXTNUMBER);
		unsigned short mftExtNumber = 0;
		if (!value.empty())
		{
			mftExtNumber = static_cast< unsigned short >(EntityHelper::getIntegerData(value));
		}

        return mftExtNumber;
    }


    std::string TelephoneManagerProfileEntityData::getType()
    {
        return getStaticType();
    }


	std::string TelephoneManagerProfileEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}


    IEntityData* TelephoneManagerProfileEntityData::clone(unsigned long key)
    {
        return new TelephoneManagerProfileEntityData(key);        
    }


    void TelephoneManagerProfileEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core

