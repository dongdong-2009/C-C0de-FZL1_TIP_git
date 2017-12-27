/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/CallBannerPage.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/CallBannerPage.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string CallBannerPage::ENTITY_TYPE               = "CallBannerPage";
	const std::string CallBannerPage::CAR_ONE_PEC_ONE_CAMERA    = "CarOnePecOneCamera";
	const std::string CallBannerPage::CAR_ONE_PEC_TWO_CAMERA    = "CarOnePecTwoCamera";
	const std::string CallBannerPage::CAR_TWO_PEC_ONE_CAMERA    = "CarTwoPecOneCamera";
	const std::string CallBannerPage::CAR_TWO_PEC_TWO_CAMERA    = "CarTwoPecTwoCamera";
	const std::string CallBannerPage::CAR_THREE_PEC_ONE_CAMERA  = "CarThreePecOneCamera";
	const std::string CallBannerPage::CAR_THREE_PEC_TWO_CAMERA  = "CarThreePecTwoCamera";

	
    CallBannerPage::CallBannerPage() 
    {
    }


    CallBannerPage::CallBannerPage( unsigned long key )
        : EntityData(key,getStaticType()), m_isValidData(false)
    {
    }

 
    CallBannerPage::~CallBannerPage()
    {
    }


    std::string CallBannerPage::getType()
    {
        return getStaticType();
    }

    std::string CallBannerPage::getStaticType()
    {
        return ENTITY_TYPE;
    }

    TA_Base_Core::IEntityData* CallBannerPage::clone(unsigned long key)
    {
        return( new CallBannerPage(key));
    }


    void CallBannerPage::invalidate()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");

        getHelper()->invalidate();

        m_isValidData = false;
    }

  

    int CallBannerPage::getCarOnePecOneCamera()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        m_carOnePecOneCamera = 3;
		std::string value = getHelper()->getParameter(CAR_ONE_PEC_ONE_CAMERA);
		if (!value.empty())
		{
			m_carOnePecOneCamera = EntityHelper::getIntegerData(value);
		}

        return m_carOnePecOneCamera;
    }

	int CallBannerPage::getCarOnePecTwoCamera()
	{
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		m_carOnePecTwoCamera = 2;
		std::string value = getHelper()->getParameter(CAR_ONE_PEC_TWO_CAMERA);
		if (!value.empty())
		{
			m_carOnePecTwoCamera = EntityHelper::getIntegerData(value);
		}

        return m_carOnePecTwoCamera;
	}

    int CallBannerPage::getCarTwoPecOneCamera()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		m_carTwoPecOneCamera = 5;
		std::string value = getHelper()->getParameter(CAR_TWO_PEC_ONE_CAMERA);
		if (!value.empty())
		{
			m_carTwoPecOneCamera = EntityHelper::getIntegerData(value);
		}
        return m_carTwoPecOneCamera;
    }

    int CallBannerPage::getCarTwoPecTwoCamera()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		m_carTwoPecTwoCamera = 4;
		std::string value = getHelper()->getParameter(CAR_TWO_PEC_TWO_CAMERA);
		if (!value.empty())
		{
			m_carTwoPecTwoCamera = EntityHelper::getIntegerData(value);
		}
        return m_carTwoPecTwoCamera;
    }

    int CallBannerPage::getCarThreePecOneCamera()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		m_carThreePecOneCamera = 6;
		std::string value = getHelper()->getParameter(CAR_THREE_PEC_ONE_CAMERA);
		if (!value.empty())
		{
			m_carThreePecOneCamera = EntityHelper::getIntegerData(value);
		}
        return m_carThreePecOneCamera;
    }

	int CallBannerPage::getCarThreePecTwoCamera()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		m_carThreePecTwoCamera = 7;
		std::string value = getHelper()->getParameter(CAR_THREE_PEC_TWO_CAMERA);
		if (!value.empty())
		{
			m_carThreePecTwoCamera = EntityHelper::getIntegerData(value);
		}
        return m_carThreePecTwoCamera;
    }
}
