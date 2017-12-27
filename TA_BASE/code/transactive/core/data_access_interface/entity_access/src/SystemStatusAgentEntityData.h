/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h $
  * @author:  Chris DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  *SystemStatusAgentEntityData proves read-only access to the agent's parameters
  *
  *
  * To add a new parameter do the following
  * 1) add a new static const std::string YOUR_NEW_PARAM_NAME;
  *	2) add a new inline class that derives from ParamHelper
  * 3) insert an instance of you new ParamHelper into the map by adding a line
  * to the SystemStatusAgentEntityData CTOR
  *
  * As of this modification unsigned int and std:string param types are supported
  * If you need a new type... have a close look at the implientation
  */


#if !defined(AFX_SYSTEMSTATUSAGENTENTITYDATA_H__DD4C1872_F0FD_44FB_B8D5_C0702DD1601A__INCLUDED_)
#define AFX_SYSTEMSTATUSAGENTENTITYDATA_H__DD4C1872_F0FD_44FB_B8D5_C0702DD1601A__INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
	class SystemStatusAgentEntityData : public EntityData  
	{
	public:
		//explicit CTOR passes 
		SystemStatusAgentEntityData(unsigned long key);
		virtual ~SystemStatusAgentEntityData();

		/**
		 * precondition: the param name must be defined in one of the 
		 * ParamHelper class implimentations and the value it holds must be integer
		 * returns the value of the param with the name specified
		 */
		int getIntParamValue(const std::string& paramName);


		/**
		 * precondition: the param name must be defined in one of the 
		 * ParamHelper class implimentations and the value it holds must be std::string
		 * returns the value of the param with the name specified
		 */
		std::string getStringParamValue(const std::string& paramName);
        
        /**
         * precondition: the param name must be defined in one of the 
    	 * ParamHelper class implimentations and the value it holds must be boolean
		 * returns the value of the param with the name specified
		 */
        bool getBoolParamValue(const std::string& paramName);

		virtual std::string getType();

		static std::string getStaticType();

		static IEntityData* clone ( unsigned long key );

		virtual void invalidate();

	private:

		//disable copy CTOR and assignment operator
		SystemStatusAgentEntityData();
		SystemStatusAgentEntityData & operator= ( const SystemStatusAgentEntityData &);
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
		static const std::string PING_RATE_PARAM;
		static const std::string PING_TIMEOUT_PARAM;
		static const std::string PING_RETRIES_PARAM;

		static const std::string TA_POLL_RATE_PARAM;

		static const std::string SYS_CON_PORT_PARAM;
		static const std::string SYS_CON_TIMEOUT_PARAM;

		static const std::string CONTROL_STATION_PORT_PARAM;
		static const std::string CONTROL_STATION_TIMEOUT_PARAM;

		static const std::string DB_SERVER_TIMEOUT_PARAM;

		static const std::string ASSET_NAME_PARAM;
        
        static const std::string IGNORE_MISSING_PING_PARAM;
	};
    
    typedef boost::shared_ptr<SystemStatusAgentEntityData> SystemStatusAgentEntityDataPtr;
} //end namespace TA_Base_Core
#endif // !defined(AFX_SYSTEMSTATUSAGENTENTITYDATA_H__DD4C1872_F0FD_44FB_B8D5_C0702DD1601A__INCLUDED_)
