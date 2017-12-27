/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/bus/maintenance_management/mmsHelper/src/MMSAPIManager.h $
* @author: huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2011/09/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
* This class refect. 
*
*/
 
#if !defined(EA_9EBD912A_6DDA_4a42_8CAD_CD68EB57974B__INCLUDED_)
#define EA_9EBD912A_6DDA_4a42_8CAD_CD68EB57974B__INCLUDED_

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "IMMSAPI.h"

namespace TA_Base_Bus
{
	class MMSAPIManager
	{

	public:
		
		virtual ~MMSAPIManager();

		/**
		* getInstance
		* This class is a singleton
		*/
		static MMSAPIManager* getInstance();
		
		/**
		* ReleaseInstance
		* release singleton
		*/
		static void releaseInstance();
		/**
		* initMMSAPI
		*
		* get a instance of the IMMSAPI interface, the instance is used to carry out all MMSAPI functions.
		* Attention: this class is in charge of release the input instance.
		*
		* @param IMMSAPI - a instance of the IMMSAPI interface 
		*  
		*/
		void initMMSAPI(IMMSAPI* apiImplementor,TA_Base_Core::MmsAgentEntityDataPtr pData);
		 
		/**
		* submitAutoAlarmMessage
		*
		* use to submit auto alarm MMS request message to MMS
		*
		* @param AlarmDetailCorbaDef - alarm information
		* 
		* @return bool - true means submit successfully, false: submit failed.
		*                  could be determined.
		*/
		bool submitAutoAlarmMessage(const TA_Base_Core::AlarmDetailCorbaDef& alarm);


		bool submitJobRequestMessage(const TA_Base_Bus::JobRequestDetailCorbaDef& details);

		bool submitScheduledMessage(MMSScheduledMessage& msg);

		bool isMMSLive();

		void setToControlMode();

		void setToMonitorMode();
       
		 
	private:
		MMSAPIManager();

		bool isInitialized();

		static MMSAPIManager* m_instance;	 
		static TA_Base_Core::ReEntrantThreadLockable m_singletonlock;

		IMMSAPI*   m_MMSAPIImplementor;
		TA_Base_Core::ReEntrantThreadLockable m_lockImplementor;
	    //this flag is used to determine whether the MMSImplementor has been assigned or not
		bool m_isInit;
		


	};

}
#endif // !defined(EA_9EBD912A_6DDA_4a42_8CAD_CD68EB57974B__INCLUDED_)
