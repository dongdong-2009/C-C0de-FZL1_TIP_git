/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorPasswordAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/10/27 17:38:47 $
  * Last modified by:  $Author: hui.wang $
  * 
  * Session is an implementation of ISession. It holds the data specific to an Session entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/src/IOperatorPassword.h"
#include "core/data_access_interface/src/OperatorPassword.h"
#include "core/data_access_interface/src/OperatorPasswordAccessFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/uuid/src/TAuuid.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{
	OperatorPasswordAccessFactory* OperatorPasswordAccessFactory::m_instance = 0;

	OperatorPasswordAccessFactory& OperatorPasswordAccessFactory::getInstance()
	{
		if( m_instance == 0 )
		{
			m_instance = new OperatorPasswordAccessFactory();
		}
		return *m_instance;
	}

	IOperatorPassword*  OperatorPasswordAccessFactory::getOperatorPassword(unsigned long operatorkey,bool changePassword, bool rightPassword)
 	{
		return new OperatorPassword(operatorkey, false,changePassword,rightPassword);
	}

	IOperatorPassword*  OperatorPasswordAccessFactory::CreateOperatorPassword(unsigned long operatorkey)
	{
		return new OperatorPassword(operatorkey, true,false,false);
	}
}