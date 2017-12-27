/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MmsAgentEntityData.h $
  * @author C. DeWolfe
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef MMS_AGENT_DATA_H_INCLUDED
#define MMS_AGENT_DATA_H_INCLUDED

#include <string>
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class MmsAgentEntityData : public EntityData
    {

    public:


        MmsAgentEntityData() {}
        MmsAgentEntityData(unsigned long key);

        virtual ~MmsAgentEntityData();

    public:


	/**
	  * getServerAddress
	  * 
	  * get the server address
	  * 
	  * @return unsigned int 
	  * 
	  * @exception <exceptions> Optional
	  */
        std::string getServerAddress();


	/**
	  * getServerQueueId
	  * 
	  * get the server queue id
	  * 
	  * @return unsigned int 
	  * 
	  * @exception <exceptions> Optional
	  */
		unsigned int getServerQueueId();


	/**
	  * getServerGroupId
	  * 
	  * get the server group id
	  * 
	  * @return unsigned int 
	  * 
	  * @exception <exceptions> Optional
	  */
		unsigned int getServerGroupId();


	/**
	  * getClientQueueId
	  * 
	  * get the client queue id
	  * 
	  * @return unsigned int 
	  * 
	  * @exception <exceptions> Optional
	  */
		unsigned int getClientQueueId();


	/**
	  * getClientGroupId
	  * 
	  * get the client group id
	  * 
	  * @return unsigned int 
	  * 
	  * @exception <exceptions> Optional
	  */
		unsigned int getClientGroupId();

        std::string getAssetName();

		std::string getSFTPport();
		std::string getSFTPUserName();
		std::string getSFTPPassword();
		std::string getSFTPLocalFolder();
		std::string getSFTPDestinationFolder();
		unsigned long getSFTPPeroidInSeconds();



        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new MmsAgentEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the MmsAgentEntityData object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();


	private:

        MmsAgentEntityData( const MmsAgentEntityData&);            
		MmsAgentEntityData& operator=(const MmsAgentEntityData&);
        
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string SERVER_ADDRESS;
		static const std::string SERVER_GROUP_ID;
		static const std::string SERVER_QUEUE_ID;
		static const std::string CLIENT_GROUP_ID;
		static const std::string CLIENT_QUEUE_ID;
		static const std::string ASSET_NAME;
		static const std::string SFTP_PORT;
		static const std::string SFTP_USER_NAME;
		static const std::string SFTP_PASSWORD;
		static const std::string SFTP_LOCALFOLDER;
		static const std::string SFTP_DESTINATION_FOLDER;
		static const std::string SFTP_PEROID_IN_SECONDS;


    };
    
    typedef boost::shared_ptr<MmsAgentEntityData> MmsAgentEntityDataPtr;
} //end namespace TA_Base_Core

#endif // MMS_AGENT_DATA_H_INCLUDED
