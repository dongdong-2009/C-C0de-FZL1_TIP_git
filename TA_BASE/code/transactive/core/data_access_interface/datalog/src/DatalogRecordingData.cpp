/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/datalog/src/DatalogRecordingData.cpp $
 * @author:  
 * @version: $Revision:  $
 *
 * Last modification: $DateTime: $
 * Last modified by:  $Author:  $
 *
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif



#include "DatalogRecordingData.h"


#include <time.h>


namespace TA_Base_Core
{

    const unsigned long DatalogRecordingData::getKey()
    {
        //TA_ASSERT(std::string("") != m_key, "A newly created alarm must be written to the database before the key can be retrieved");

        return(m_key);
    }

    const double DatalogRecordingData::getValue()
    {

        return m_value;
    }

	const tm DatalogRecordingData::getCreateTime()
	{
		return m_createTime;
	}

	const tm DatalogRecordingData::getPlanLogTime()
	{
		return m_planLogTime;
	}
	const unsigned long DatalogRecordingData::getEntityKey()
	{
		return m_entityKey;
	}

	const unsigned long DatalogRecordingData::getLocationKey()
	{
		return m_locationKey;
	}



} // closes TA_Base_Core
