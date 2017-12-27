/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EntityStatusData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * EntitData is an implementation of the IEntityData interface. It can be used by specific
 * to handle the actions of retrieving the standard (default) entity data from the 
 * EntityHelper. It should be sub-classed by the specific entity class.
 */

#if !defined(ENTITY_STATUS_DATA_HEADER)
#define ENTITY_STATUS_DATA_HEADER

#if defined(_MSC_VER)
#pragma warning(disable:4250 4786)
#endif // defined (_MSC_VER)

#include <string>

namespace TA_Base_Core
{
	enum EEntityStatusType
	{	
		MMSInhibitStatus = 0
	};

	enum EStatusDataType
	{
		STRING_DATA = 0,
		INTEGER_DATA = 1,
		BOOLEAN_DATA = 2,
		REAL_DATA = 3,
		ENUMERATION_DATA = 4,
		UNKNOWN_DATA = 5
	};
	

	typedef struct 
	{	
		unsigned long		entityKey;
		std::string			lastSessionId;
		time_t				lastModifyDate;
		std::string			entityStatusDataValue;
		EStatusDataType		entityStatusDataType;
		EEntityStatusType	entityStatusType;
	} EntityStatusData;
    
} //close namespace TA_Base_Core

#endif // !defined(ENTITY_STATUS_DATA_HEADER)
