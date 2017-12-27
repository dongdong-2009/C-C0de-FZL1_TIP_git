/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TisConfigAccess.h $
* @author:  Bart Golab
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/09/24 11:10:12 $
* Last modified by:  $Author: grace.koh $
*
* This is a utility class. It's used to retrieve from the database
* various TIS related configuration data items.
*
*/
#ifndef STATIONZONESPROCESS_H
#define STATIONZONESPROCESS_H
#include <string>
#include <map>
#include <vector>

extern const std::string zonetypes[];
std::vector<std::string> makezones();
std::map<std::string, unsigned long> makestaionMap();

#endif