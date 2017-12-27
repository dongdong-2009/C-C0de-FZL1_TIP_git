/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManagerStructs.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef SCHEDULING_MANAGER_STRUCT_H
#define SCHEDULING_MANAGER_STRUCT_H

#include <string>
#include <vector>
#include <map>

namespace TA_Base_Core
{
    class IProfile;
    class IOperator;
    class ILocation;
}

struct NameValuePair
{
    std::string name;
    std::string value;
};

typedef std::map<unsigned long, TA_Base_Core::ILocation*> LocationMap;
typedef std::map<unsigned long, TA_Base_Core::IProfile*> ProfileMap;
typedef std::vector<TA_Base_Core::IProfile*> ProfileVector;


#endif // SCHEDULING_MANAGER_STRUCT_H