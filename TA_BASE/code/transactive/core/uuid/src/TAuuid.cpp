/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/uuid/src/TAuuid.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TA_Uuid is used to generate and compare UUID's (Universally Unique Identifier).
  */

#ifdef WIN32
	#include <objbase.h>
#else
	#include "core/uuid/src/uuid.h"
#endif

#include "core/uuid/src/TAuuid.h"

namespace TA_Base_Core
{

	/**
	  * generateUUID()
	  * 
	  * generateUUID will generate a UUID and return it as a std::string
	  *
	  * @return std::string representation of the UUID
	  */
	std::string TA_Uuid::generateUUID()
	{
	#ifdef WIN32
		// Create the new UUID
		UUID uuid;
		UuidCreate(&uuid);

		// convert the UUID to a string
		// Note, UuidToString does not free memory!
		unsigned char* buf;
		UuidToString(&uuid, &buf);

		// Copy the string into the a std::string
		std::string ret((char *)buf);

		// Free the allocated memory
		RpcStringFree(&buf);

	#else
		// Create a new UUID
		uuid_t uuid;
		uuid_generate_random(uuid);

		// Convert the UUID to a string
		char buf[40];
		uuid_unparse(uuid,buf);

		// copy the buffer
		std::string ret(buf);
	#endif

		// Return the UUID as a std::string
		return ret;
	}

	/**
	  * compareUUID()
	  * 
	  * compareUUID will compare to strings representing UUIDs.
	  *
	  * @return bool true if UUIDs are the same, false otherwise
	  */
	bool TA_Uuid::compareUUID(const std::string& subject, const std::string& test)
	{
		// Use a simple string comparison... not particularly efficient, but it works
		return ( subject.compare(test) == 0);
	}

};
