/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/FunctionEntry.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * FunctionEntry is a class used to log when functions are
  * entered and exited. Use the macro FUNCTION_ENTRY(functname)
  * at the top of a function and FUNCTION_EXIT at the bottom.
  * FunctionEntry's destructor handles exit logging, so even
  * if a function is exited due to, say, an exception, the
  * exit is still logged (but without a line number).
  *
  */

#include "FunctionEntry.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{

	FunctionEntry::FunctionEntry(const char* functName, char* file, int line)
		: m_line(-1), m_file(file), m_functName(functName)
	{
		DebugUtil::getInstance().log(file, line, DebugUtil::FunctionEntry, m_functName.c_str());
	}


    FunctionEntry::~FunctionEntry()
    {
        DebugUtil::getInstance().log(m_file, m_line, DebugUtil::FunctionExit, m_functName.c_str());
    }

} // namespace TA_Base_Core
