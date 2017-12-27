/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/DataLogAgentEntityData.h $
  * @author Ross Tucker
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by: $Author: grace.koh $
  * 
  * Implement the IEntityData interface.
  */

#ifndef DATALOG_AGENT_ENTITY_DATA
#define DATALOG_AGENT_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class DataLogAgentEntityData : public EntityData
    {

    public:
        DataLogAgentEntityData();
        DataLogAgentEntityData( unsigned long key );
        virtual ~DataLogAgentEntityData();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);        

		 
        virtual int getDataMaxTimeout();

		virtual int getTimeoutDelay();
		virtual int getMaxDelayTimes();

		virtual int getBatchUpdateSize();
         
		 
		 
    protected:

    private:
        static const std::string ENTITY_TYPE;
		static const std::string DATA_MAX_TIMEOUT;
		static const std::string TIMEOUT_DELAY;
		static const std::string BATCH_UPDATE_SIZE;
		static const std::string MAX_DELAY_TIMES;

    };

    typedef boost::shared_ptr<DataLogAgentEntityData> DataLogAgentEntityDataPtr;
}

#endif // DATALOG_AGENT_ENTITY_DATA
