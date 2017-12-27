/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaBridge.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the interface declaration for GenaBridge
  */


#ifndef __IGENA_BRIDGE_H_INCLUDED__
#define __IGENA_BRIDGE_H_INCLUDED__

#include "IGENAClientStatusForwarder.h"
#include "ITopicListener.h"
#include "GENATopicDataStore.h"
#include <boost/shared_ptr.hpp>
#include "CommonDef.h"
#include <string>

namespace TA_Base_Bus
{
	// Forward declaration
	class GenaServer;
	class GenaClient;

	class GenaBridge
		: public IGENAClientStatusForwarder,
		  public IGENADataStoreObserver
	{
	public:
		/*
		* Constructor
		* @Purpose: constructor
		* @Param:	serverParams - contains server connection information.
					clientParams - contains client connection information.
		*/
		GenaBridge(const GenaServerParams& serverParams, 
			const GenaClientParams& clientParams);

		/*
		* Destructor
		* @Purpose: destroys object
		*/
		~GenaBridge();

		/*
		* startService
		* @Purpose: starts the bridge service.
		* @Param:	none
		* @return:	none
		*/
		void startService();

		/*
		* stopService
		* @Purpose: stops the bridge service.
		* @Param:	none
		* @return:	none
		*/
		void stopService();

		/*
		* destroyService
		* @Purpose: destroys internal server and client instance.
		* @Param:	none
		* @return:	none
		*/
		void destroyService();

		/*
		* getServerInstance
		* @Purpose: retrieves the server instance contained within the bridge
		* @Param:	none
		* @return:	instance of the GENA Server otherwise it returns NULL
		*/
		boost::shared_ptr<GenaServer> getServerInstance();

		/*
		* getClientInstance
		* @Purpose: retrieves the client instance contained within the bridge
		* @Param:	none
		* @return:	instance of the GENA Client otherwise it returns NULL
		*/
		boost::shared_ptr<GenaClient> getClientInstance();

	protected:
		
		/*
		* Virtual subscribe
		*/
		std::string subscribe(const std::string& topicURL, const std::string& notification);

		/*
		* Virtual unsubscribe
		*/
		virtual void unsubscribe(const std::string& topicURL);

		/*
		* Virtual subscribe
		*/
		//virtual void onNotifyUpdate(const TopicUpdate& topicUpdate);
		
		/* 
		 * OnDataChange
		 *
		 * @purpose: function called to notify observers that a data change has occurred.
		 * @Param: topicName - topic name of the data
				   topicData - data containing the update information
		 */
		virtual void onDataChange(const std::string& topicName,  GENATopicDataPtr topicData);
		
	private: // Methods
		/* 
		* disabled copy constructor
		*/ 
		GenaBridge( const GenaBridge& rhs);
		GenaBridge& operator = ( const GenaBridge& rhs);

	private: // Variables
		// Server Instance
		boost::shared_ptr<GenaServer> m_pGenaServer;
		// Client Instance
		boost::shared_ptr<GenaClient> m_pGenaClient;

		//Subscriber SID map
		typedef std::map<std::string,std::string> T_SubUrlSIDMap;
		T_SubUrlSIDMap m_subUrlSIDs;

		// Topic Data Store
		GENATopicDataStore* m_pDataStore;
	};

	typedef boost::shared_ptr<GenaBridge> GenaBridgePtr;

} //TA_Base_Bus

#endif //__IGENA_BRIDGE_H_INCLUDED__