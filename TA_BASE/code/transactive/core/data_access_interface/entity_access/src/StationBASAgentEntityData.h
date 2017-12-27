#ifndef STAIONBASAGENT_ENTITY_DATA_H
#define STAIONBASAGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IPMSAgentEntityData.h"

namespace TA_Base_Core
{
	class StationBASAgentEntityData : public virtual IPMSAgentEntityData, public virtual EntityData
	{

	public:

		StationBASAgentEntityData();

		StationBASAgentEntityData(unsigned long key);

		virtual ~StationBASAgentEntityData();


		/**
		* clone
		*
		* Returns a pointer to a new RTUEntityData object that has been initialised with the 
		* specified key.
		*
		* @param key The key with which to construct the object.
		*/
		static IEntityData* clone(unsigned long key);

		void invalidate();

		std::string getType();

		/**
		* getType
		*
		* Returns the entity type of this entity (i.e. "Camera").
		*
		* @return The entity type of this entity as a std::string
		*/
		static std::string getStaticType();

		virtual std::string getMyModeControlToken();
		virtual std::string getMyModeValueToken();
		virtual std::string getMyModeExecutionStatusToken();
		virtual std::string getMyModeRightStatusToken();
		virtual std::string getMyModeRightsControlToken();
        virtual std::string getMyFireModeRecoverDpTag();

		virtual std::string getCurrentDayTTPtBGN();
		virtual std::string getNextDayTTPtBGN();
		virtual std::string getTTScheduleTime();

		virtual unsigned long getFireStationMonitorSecond();//GF-268
		/************************************************************************/
		/*         Implementation of IPMSAgentEntityData                        */
		/************************************************************************/
		virtual int getScanTimeMSecs();
		virtual const std::string getStationName();
		virtual std::string getAssetName();
		virtual int getAnalogueDataPointUpdateInterval();
		virtual bool getIsRTUSwitchingDisabled();

	private:
		StationBASAgentEntityData(const StationBASAgentEntityData& obj){}
		StationBASAgentEntityData& operator=(const StationBASAgentEntityData& obj){}

		// 
		// The entity type is fixed for all instances of this class.
		// 
		static const std::string ENTITY_TYPE;

	public:
		static const std::string MY_MODE_CONTROL_SQL_TOKEN;
		static const std::string MY_MODE_VALUE_SQL_TOKEN;
		static const std::string MY_MODE_EXEC_STATUS_TOKEN;
		static const std::string MY_MODE_RIGHT_STATUS_TOKEN;
		static const std::string MY_MODE_RIGHT_CONTROL_TOKEN;
        static const std::string MY_FIRE_MODE_RECOVER_DP_TAG;

		static const std::string CURRENT_DAY_TT_PT_BGN;
		static const std::string NEXT_DAY_TT_PT_BGN;
		static const std::string TT_SCHEDULE_TIME;

		static const std::string SCANTIME;
		static const std::string STATIONNAME;
		static const std::string ASSET_NAME;
		static const std::string ANALOGUE_DP_UPDATE_INTERVAL;

		static const std::string STATION_FIRE_MONITOR_SECOND;//GF-268
		static const std::string RTU_SWITCHING_DISABLED;

	};

	typedef boost::shared_ptr<StationBASAgentEntityData> StationBASAgentEntityDataPtr;

}//close namespace TA_Base_Core

#endif
