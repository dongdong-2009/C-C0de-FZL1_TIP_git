/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/bus/maintenance_management/mmsHelper/src/MMSSubmitManager.h $
* @author: huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2011/09/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
* This class provides factory method to get a instance of IMMSSubmitter
*  
*
*/

#if !defined(EA_4B468D30_E29F_4a4c_8170_E3D5208D05C2__INCLUDED_)
#define EA_4B468D30_E29F_4a4c_8170_E3D5208D05C2__INCLUDED_

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "IMMSSubmitter.h"

namespace TA_Base_Bus
{
	class MMSSubmitManager
	{

	public:
		
		virtual ~MMSSubmitManager();

		/**
		* getInstance
		* This class is a singleton
		*/
		static MMSSubmitManager* getInstance();

		IMMSSubmitter*  getSubmitter();

	private:
		MMSSubmitManager();

		static MMSSubmitManager* m_instance;

		//provides a thread lock
		static TA_Base_Core::ReEntrantThreadLockable m_singletonlock;

		IMMSSubmitter*   m_submitterBase;


		TA_Base_Core::ReEntrantThreadLockable m_lockSubmitter;



	};

}
#endif // !defined(EA_4B468D30_E29F_4a4c_8170_E3D5208D05C2__INCLUDED_)
