/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundCategory.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class is an implementation of the ISoundCategory 
  * interface. It provides read-only access to an 
  * entry in the so_sound_category table.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include "SoundCategory.h"
#include "SoundCategoryHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

	SoundCategory::SoundCategory(unsigned long categoryKey)
	{
		m_helper = new SoundCategoryHelper(categoryKey);
	}


	SoundCategory::~SoundCategory()
	{
		delete m_helper;
		m_helper = NULL;
	}


	unsigned long SoundCategory::getCategoryKey()
	{
        TA_ASSERT(m_helper != NULL, "The SoundCategoryHelper pointer is null.");
        return m_helper->getCategoryKey();
	}


	std::string SoundCategory::getName()
	{
        TA_ASSERT(m_helper != NULL, "The SoundCategoryHelper pointer is null.");
        return m_helper->getName();
	}


	unsigned long SoundCategory::getPosition()
	{
        TA_ASSERT(m_helper != NULL, "The SoundCategoryHelper pointer is null.");
        return m_helper->getPosition();
	}


} // TA_Base_Core
