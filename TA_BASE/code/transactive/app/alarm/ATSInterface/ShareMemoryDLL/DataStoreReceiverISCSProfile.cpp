// DataStoreReceiverISCSProfile.cpp: implementation of the DataStoreReceiverISCSProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreReceiverISCSProfile.h"


//#include "core/utilities/src/DebugUtil.h"
//using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{

	DataStoreReceiverISCSProfile* DataStoreReceiverISCSProfile::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreReceiverISCSProfile::m_singletonLock;
	DataStoreReceiverISCSProfile* DataStoreReceiverISCSProfile::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreReceiverISCSProfile();	
			m_Instance->initialize();
		}
		return m_Instance;
	}
	void DataStoreReceiverISCSProfile::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			delete m_Instance;
			m_Instance=NULL;
		}
	}

   boost::shared_ptr<ISCSProfileInfo> DataStoreReceiverISCSProfile::getProfile()
   {
	   std::vector<boost::shared_ptr<ISCSProfileInfo>  >*  datas= getData();
	   boost::shared_ptr<ISCSProfileInfo>  returnProfile (new ISCSProfileInfo());
	   if  ( datas != NULL)
	   {
		   std::vector<boost::shared_ptr<ISCSProfileInfo>  >::iterator iter=datas->begin();
		   for (;iter!=datas->end();iter++)
		   {				
			   returnProfile=(*iter);
			   break;
		   }
		   datas->clear();
		   delete datas;		   
	   }
	   return returnProfile;

   }

	DataStoreReceiverISCSProfile::DataStoreReceiverISCSProfile()//:DataStoreReceiver<long, CommandCache, CommandCacheManager>("Command")
	{

	}

	DataStoreReceiverISCSProfile::~DataStoreReceiverISCSProfile()
	{

	}
	 

 


}
