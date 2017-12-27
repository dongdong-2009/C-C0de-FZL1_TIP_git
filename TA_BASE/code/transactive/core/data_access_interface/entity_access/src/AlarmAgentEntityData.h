/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmAgentEntityData.h $
  * @author Ross Tucker
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef ALARM_AGENT_ENTITY_DATA
#define ALARM_AGENT_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class AlarmAgentEntityData : public EntityData
    {

    public:
        AlarmAgentEntityData();
        AlarmAgentEntityData( unsigned long key );
        virtual ~AlarmAgentEntityData();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);

        virtual std::string getPrinterName();
        virtual std::string getPrinterHost();
        virtual std::string getPrinterPort();

        virtual unsigned int getNumberOfAlarmsToTest();

        virtual bool isPrintingEnabled();
        virtual bool isFederationEnabled();

        virtual unsigned long getUserId();
        virtual unsigned long getProfileId();
        virtual unsigned long getWorkStationId();
        virtual std::string getPassword();

		virtual std::string getAssetName();

        virtual unsigned int getSuppressionRuleLifetime(); // in seconds

		//TD16491++
		virtual std::string getMmsSubmitterType();
		virtual bool getUseSourceTime();
		//++TD16491

    protected:

    private:
        static const std::string ENTITY_TYPE;
        static const std::string PRINTER_NAME;
        static const std::string PRINTER_HOST;
        static const std::string PRINTER_PORT;
        static const std::string NUMBER_OF_ALARMS_TO_TEST;
        static const std::string USER_ID;
        static const std::string PROFILE_ID;
        static const std::string WORK_STATION_ID;
        static const std::string PASSWORD;
        static const std::string IS_PRINTING_ENABLED;
        static const std::string IS_FEDERATION_ENABLED;
        static const std::string SUPPRESSION_RULE_LIFETIME;
		static const std::string ASSET_NAME;
		//TD16491++
		static const std::string MMS_SUBMITTER_TYPE;
		static const std::string USE_SOURCE_TIME;
		//++TD16491

    };

    typedef boost::shared_ptr<AlarmAgentEntityData> AlarmAgentEntityDataPtr;
}

#endif // ALARM_AGENT_ENTITY_DATA
