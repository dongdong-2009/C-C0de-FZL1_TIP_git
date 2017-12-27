/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/DataPointFactory.h $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  *	Creates DataPoint objects and registers them with
  * the Naming Service.
  *
  *
  */

#ifndef DATAPOINTFACTORY_H
#define DATAPOINTFACTORY_H

#include <vector>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"
//#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/common_library/IDL/src/IDataPointStateUpdateCorbaDef.h"

#include "bus/scada/common_library/src/MmsScadaUtility.h"
#include "bus/scada/datapoint_library/src/NotifyEventProcessor.h"
#include "bus/scada/datapoint_library/src/PersistProcessor.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Core
{
    class ScadaRootEntityData;
};

namespace TA_Base_Bus
{
	class DataPoint;
	class TextDataPoint;
	class BooleanDataPoint;
	class AnalogueDataPoint;
	class DataPointStateUpdateSender;
	class CommandProcessor;
	class DataPointStateUpdateServant;
	class IDataPointAgent;
    class EventDetailsFactory;

	class DataPointFactory
	{

	public:

        DataPointFactory(TA_Base_Bus::IDataPointAgent* dpAgent);
		~DataPointFactory();

		static void parseEntityAssociations( DataPoint * point,
											 const std::string & associations );

		/**
		 * DataPointFactory::createDataPoint()
		 *
		 * Constructs a single DataPoint object based on the configuration
		 * data retrieved from the DataPointEntityData object. Constructs the
		 * DataPoint with the common attributes and then calls specific routines
		 * to perform type-specific configuration.
		 *
		 * @return	pointer to the new DataPoint object
		 *
		 * @param	cfg		The configuration object
		 *
		 */
		DataPoint *	createDataPoint( TA_Base_Core::DataPointEntityDataPtr cfg );

		/**
		 * DataPointFactory::stringToDataPointType()
		 *
		 * Converts a string from the configuration database into the
		 * appropriate DataPointType enum. Returns DPT_UNKNOWN_TYPE if
		 * the string does not match those defined in CommonDefs.h
		 *
		 * @return	the DataPointType
		 *
		 * @param  	typeStr		DataPointType field value from configuration
		 *
		 */
		TA_Base_Bus::EDataPointType stringToDataPointType( const std::string & typeStr ) const;

		/**
		 * DataPointFactory::stringToDataPointDataType()
		 *
		 * Converts a string from the configuration database into the
		 * appropriate DataPointDataType enum. Returns DPT_NULL_TYPE if
		 * the string does not match those defined in CommonDefs.h
		 *
		 * @return	the DataPointDataType
		 *
		 * @param  	dataTypeStr		DataPointDataType field value from configuration
		 *
		 */
		TA_Base_Bus::EDataPointDataType stringToDataPointDataType( const std::string & dataTypeStr ) const;
	    
        /**
	     * This is used in Control Mode only to register the monitor agent. The received
	     * uuid should be compared with m_uuid. If these are different, complet update is
	     * required to sync.
	     * 
	     * @param uuid
	     */
	    void registerMonitorAgent(const std::string& uuid, TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr monitorAgentStateUpdateRef);
        TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr getDataPointStateUpdateServant();

        void setToControl();
        void setToMonitor();
		void setOperationMode(TA_Base_Core::EOperationMode mode);

		void initialize(unsigned long agentKey, std::string& agentName);

		MmsScadaUtility& getMmsScadaUtility();

		unsigned long getScadaRootKey();
    private:

		/**
		 * DataPointFactory::configureAnalogueDataPoint()
		 *
		 * Reads configuration data specific to AnalogueDataPoints from the
		 * configuration object, and performs Analogue specific configuration
		 * on the DataPoint object.
		 *
		 * @param	cfg		The configuration object
		 * @param 	point	the AnalogueDataPoint to be configured
		 *
		 */
		void configureAnalogueDataPoint( TA_Base_Core::DataPointEntityDataPtr cfg,
										 AnalogueDataPoint * point );

		/**
		 * DataPointFactory::configureTextDataPoint()
		 *
		 * Reads configuration data specific to TextDataPoints from the
		 * configuration object, and performs Text specific configuration
		 * on the DataPoint object.
		 *
		 * @param	cfg		The configuration object
		 * @param 	point	the TextDataPoint to be configured
		 *
		 */
		void configureTextDataPoint( TA_Base_Core::DataPointEntityDataPtr cfg,
									 TextDataPoint * point );


		/**
		 * DataPointFactory::configureBooleanDataPoint()
		 *
		 * Reads configuration data specific to BooleanDataPoints from the
		 * configuration object, and performs Boolean specific configuration
		 * on the DataPoint object.
		 *
		 * @param	cfg		The configuration object
		 * @param 	point	the BooleanDataPoint to be configured
		 *
		 */
		void configureBooleanDataPoint( TA_Base_Core::DataPointEntityDataPtr cfg,
										BooleanDataPoint * point );


		/**
		 * DataPointFactory::configureEntityAssociations()
		 *
		 * Parses the entity association configuration data and sets
		 * these associations in the DataPoint
		 *
		 * @param	cfg		The configuration object
		 * @param 	point	the DataPoint to be configured
		 *
		 */
		void configureEntityAssociations( TA_Base_Core::DataPointEntityDataPtr cfg,
										  DataPoint * point );


		/**
		 * DataPointFactory::configureAlarm()
		 *
		 * Parses the entity association configuration data and sets
		 * these associations in the DataPoint
		 *
		 * @param	cfg		The configuration object
		 * @param 	point	the DataPoint to be configured
		 *
		 */
		void configureAlarm( DataPoint * point,
							 const TA_Base_Bus::EDataPointAlarms & almType,
							 bool almEnabledField,
							 TA_Base_Bus::DpValue almValue,
							 const std::string & almMsgField,
							 int almDelayField,
							 int almSeverityField,
							 const std::string & engUnit,
							 const std::string & almMMSConfiguration = "",
							 double alarmThresholdConstant = 0 );


    private:
        DataPointFactory();
        DataPointFactory& operator=(const DataPointFactory& rhs);
        DataPointFactory(const DataPointFactory&);
        
        
    private:

        EventDetailsFactory*                m_eventDetailsFactory;
        ScadaPersistencePtr m_persistence;
        MmsScadaUtility m_mmsScadaUtility;
        TA_Base_Core::ScadaRootEntityData* m_scadaRootEntityData;

        NotifyEventProcessor m_notifyProcessor;
        PersistProcessor m_persistProcessor;

        TA_Base_Bus::IDataPointAgent*   m_dpAgent;
        DataPointStateUpdateSender*     m_dpStateUpdateSender;
        CommandProcessor*               m_commandProcessor;
        DataPointStateUpdateServant*    m_dpStateUpdateServant;
		volatile bool					m_initialized;
		TA_Base_Core::ReEntrantThreadLockable    m_lockInit;
	};

}

#endif
