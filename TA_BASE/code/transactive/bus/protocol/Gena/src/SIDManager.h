/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: SIDManager.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for managing subscriber ID.
  * Todo: We should create a unique SID for containing server ID + topic No. + 0000 
  */


#ifndef __SID_MANAGER_H_INCLUDED__
#define __SID_MANAGER_H_INCLUDED__
#pragma once

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <string>
namespace TA_Base_Bus
{
	class SIDManager
	{
	public:
		SIDManager(): m_sidCounter(0){};
		~SIDManager(){};

		std::string getNextSID();

		void resetSID();

	private:
		SIDManager(const SIDManager& rhs);

		unsigned long m_sidCounter;
		TA_Base_Core::NonReEntrantThreadLockable m_sidLock;
	}; //SIDManager

} //TA_Base_Bus

#endif //__SID_MANAGER_H_INCLUDED__