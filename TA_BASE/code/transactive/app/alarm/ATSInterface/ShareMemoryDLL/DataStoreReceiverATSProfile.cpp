// DataStoreReceiverATSProfile.cpp: implementation of the DataStoreReceiverATSProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "DataStoreReceiverATSProfile.h"


//#include "core/utilities/src/DebugUtil.h"
//using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{

	DataStoreReceiverATSProfile* DataStoreReceiverATSProfile::m_Instance=NULL;
	//TA_Base_Core::NonReEntrantThreadLockable DataStoreReceiverATSProfile::m_singletonLock;
	DataStoreReceiverATSProfile* DataStoreReceiverATSProfile::getInstance()
	{
		if (m_Instance!=NULL)
		{
			return m_Instance;
		}
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance==NULL)
		{
			m_Instance=new DataStoreReceiverATSProfile();	
			m_Instance->initialize();
		}
		return m_Instance;
	}
	void DataStoreReceiverATSProfile::release()
	{
		//TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_Instance!=NULL)
		{
			delete m_Instance;
			m_Instance=NULL;
		}
	}

   boost::shared_ptr<ExtProfileInfo> DataStoreReceiverATSProfile::getProfile()
   {
	   std::vector<boost::shared_ptr<ExtProfileInfo>  >*  datas= getData();
	   boost::shared_ptr<ExtProfileInfo>  returnProfile (new ExtProfileInfo());
	   if  ( datas != NULL)
	   {
		   std::vector<boost::shared_ptr<ExtProfileInfo>  >::iterator iter=datas->begin();
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

	DataStoreReceiverATSProfile::DataStoreReceiverATSProfile()//:DataStoreReceiver<long, CommandCache, CommandCacheManager>("Command")
	{

	}

	DataStoreReceiverATSProfile::~DataStoreReceiverATSProfile()
	{

	}

}
