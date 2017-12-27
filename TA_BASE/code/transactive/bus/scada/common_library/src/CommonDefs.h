/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/common_library/src/CommonDefs.h $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  * Common defines for SCADA
  *
  */

#ifndef COMMONDEFS_H
#define COMMONDEFS_H

#include <string>
#include <vector>
#include <map>
#include <sys/timeb.h>
#include "boost/shared_ptr.hpp"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"


/*//#include "app/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

// Valid alarm severities
#define ALARM_SEV_SYSTEM_PKEY	1
#define ALARM_SEV_MINOR_PKEY   	2
#define ALARM_SEV_MAJOR_PKEY    3
#define ALARM_SEV_EXTREME_PKEY	4

//
// Typedefs to shorten these long type names
//
//typedef TA_App::DataPointCorbaDef::DataPointAlarmTypes 		  DPDmIfAlarmType;
//typedef TA_App::DataPointCorbaDef::DataPointAlarms 	  		  DPDmIfAlarms;
*/

//
// Enumerated values for the possible service states of
// the DataPoint
//
namespace TA_Base_Bus
{
    //************************************
    //
    //  Datapoint DataNode bulk poll 
	//  and access interface constant
    //
	const std::string DP_ACCESS_INTERFACE = "DpAccessInterface";
	const std::string DN_ACCESS_INTERFACE = "DnAccessInterface";
	const std::string DP_BULK_POLLED_INTERFACE = "DpBulkPolledIf";
	const std::string DN_BULK_POLLED_INTERFACE = "DnBulkPolledIf";	
    const std::string DP_STATE_UPDATE_INTERFACE = "DpStateUpdateInterface";


    //************************************
    //
    //  Datapoint properties Related Enum
    //

    //
    // Implement OPC Data Quality Word as follows:
    // 00000000 QQSSSS 00
    // where QQ are Quality BitField
    //		 SSSS are Substatus BitField
    //
    // Possible values are:
    enum EQualityStatus
    {
	    QUALITY_BAD_NO_SPECIFIC_REASON = 0,				// 00000000 00000000
	    QUALITY_BAD_CONFIGURATION_ERROR = 4,			// 00000000 00000100
	    QUALITY_BAD_NOT_CONNECTED = 8,					// 00000000 00001000
	    QUALITY_BAD_DEVICE_FAILURE = 12,				// 00000000 00001100
	    QUALITY_BAD_SENSOR_FAILURE = 16,				// 00000000 00010000
	    QUALITY_BAD_LAST_KNOWN_VALUE = 20,				// 00000000 00010100
	    QUALITY_BAD_COMM_FAILURE = 24,					// 00000000 00011000
	    QUALITY_BAD_OUT_OF_SERVICE = 28,				// 00000000 00011100

	    QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON = 64,	// 00000000 01000000
	    QUALITY_UNCERTAINTY_LAST_USABLE_VALUE = 68,		// 00000000 01000100
	    QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE = 80,	// 00000000 01010000
	    QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED = 84,		// 00000000 01010100
	    QUALITY_UNCERTAINTY_SUB_NORMAL = 88,			// 00000000 01011000

	    QUALITY_GOOD_NO_SPECIFIC_REASON = 192,			// 00000000 11000000
	    QUALITY_GOOD_LOCAL_OVERRIDE = 216				// 00000000 11011000
    };

    // The type of the data point
    enum EDataPointType
    {
	    DPT_UNKNOWN_TYPE,
	    DPT_ANALOGUE,			// Analogue DataPoint
	    DPT_BOOLEAN,			// Boolean DataPoint
	    DPT_TEXT,				// Text DataPoint
	    DPT_DERIVED,			// Derived DataPoint
	    DPT_COMBINED_OUTPUT,	// Combined Output DataPoint
	    DPT_SAFETY_OUTPUT,		// Safety Output DataPoint
	    DPT_METERED,			// Metered DataPoint
	    DPT_CALCULATED,			// Calculated DataPoint
	    DPT_ENUM,				// Enumerated DataPoint
		DPT_MULTIBIT			// Mutibit DataPoit
    };

	// IEC 104 Protocol Control Operation
	enum EControlOperation
	{
		CONTROL_NO_OPERATION,	// No Control Operation
		CONTROL_SELECT,			// Control Select Operation
		CONTROL_EXECUTE,		// Control Execute Operation
		CONTROL_CANCEL,			// Control Cancel Operation
	};

    enum EDataPointDataType
    {
	    DPT_NULL_TYPE,

	    // 16 and 32 bit integer using 2's Complement format for signed value
	    DPT_INT16_TYPE,		// associated with DataPointValueType::dpvFloat
	    DPT_INT32_TYPE,		// associated with DataPointValueType::dpvFloat

	    // 16 and 32 bit integer using the most significant bit as the signed bit for signed value
	    DPT_INT16S_TYPE,	// associated with DataPointValueType::dpvFloat
	    DPT_INT32S_TYPE,	// associated with DataPointValueType::dpvFloat

	    // 16 and 32 bit unsigned integer
	    DPT_UINT16_TYPE,		// associated with DataPointValueType::dpvFloat
	    DPT_UINT32_TYPE,		// associated with DataPointValueType::dpvFloat

	    // 32 bit floating point number using IEEE 32 Bit Floating Point conversion algorithm
	    DPT_IEEE32_TYPE,	// associated with DataPointValueType::dpvFloat

	    DPT_ENUM_TYPE,		// associated with DataPointValueType::dpvEnum

	    DPT_BOOL_TYPE,		// associated with DataPointValueType::dpvBoolean

	    DPT_TEXT_TYPE		// associated with DataPointValueType::dpvText
    };

    //
    //  Alarm Related Enum
    // 
    //this is use for both alarm type and alarm state
	//the different types of alarms
	enum EDataPointAlarms
	{
        DptNoAlarm,
		DptOverScaleHi,	// OverScale High alarm type
		DptOverScaleLo,	// OverScale Low alarm type
		DptHiHi,			// High High alarm type
		DptHi,				// Low Low alarm type
		DptLo,				// Low alarm type
		DptLoLo,			// Low Low alarm type
		DptBoolTrue,		// True alarm type
		DptBoolFalse,		// False alarm type
		DptGeneral			// A generic alarm type, i.e none of the above types
							//for instance, alarm raised by
						    // a derived state of the Derived Datapoint
	};

    enum EScadaAlarmAckState
    {
        ScadaAlarmNack = 0,
        ScadaAlarmAck = 1,
        ScadaAlarmNotPresent = 2,
        ScadaAlarmUnknown = 3
    };

    //
    //  Datapoint control relatead Enum
    //

    //
    // Enumerated values for the possible status values of a
    // DataPoint
    enum  EDataPointServiceState
    {
        IN_SERVICE,
        OUT_OF_SERVICE
    };

    // The state of the output command
    enum  EDataPointControlState
    {
	    noControl,			// no value is written
	    written,			// the value is written to device
	    pending,			// the output is waiting for confirmation
	    conditionalSuccess,	// the output is conditionally successfull
	    success,			// the output is confirmed
	    failed,				// the output has failed...don't know why....
	    LCC_failed,			// the output has failed because of LCC
	    RCC_failed,			// the output has failed because of RCC
	    comms_failed,		// the output has failed because of comms error

		//add for IEC 104 Protocol control process
		selectCmdWritten,			//the select command output is written to PSCADA Controller
		selectCmdConfirm,			//the select command output is confirmed by PSCADA Controller
		selectCmdConfirmTimeout,	//the select command output is not response by PSCADA Controller
		operateCmdTimeout,			//Operator don't send the next command in the defined timeout
		execCmdWritten,				//the execute command output is written to PSCADA Controller
		execCmdConfirm,				//the execute command output is confirmed by PSCADA Controller
		execCmdConfirmTimeout,		//the execute command output is not response by PSCADA Controller
		execCmdSuccess,				//the execute command success is confirmed by PSCADA Controller
		execCmdSuccessTimeout,		//the execute command success is not received by PSCADA Controller
		cancelCmdWritten,			//the cancel command output is written to PSCADA Controller
		cancelCmdConfirm,			//the cancel command output is confirmed by PSCADA Controller
		cancelCmdConfirmTimeout,	//the cancel command output is not response by PSCADA Controller
		selectCmdCfmNegative,		//the select command output is negative confirmed by PSCADA Controller
		execCmdCfmNegative,			//the execute command output is negative confirmed by PSCADA Controller
		cancelCmdCfmNegative,		//the cancel command output is negative confirmed by PSCADA Controller
		deviceControlException,		//reseived unexpected data from PSCADA Controller
    };

    //************************************
    //
    //  Datapoint control relatead Struct
    //
    typedef std::vector<unsigned char> ByteSequence;
	
    struct DataPointValue
    {
        //indicate the datapoint type;
        EDataPointDataType  dpType;
		EQualityStatus		quality;
		timeb				timeStamp;
		EControlOperation	controlOperation; //add for 104 Library Control Operation

        double				floatValue;//float datapoint value
        unsigned long		enumValue;//enum datapoint value
        bool				booleanValue;//boolean datapoint
        ByteSequence		stringValue;//text datapoint
    };

    //Datapont Control update struct
	struct DataPointControl
	{
		EDataPointControlState  controlState;
		unsigned long	        triggerringDpKey;
        std::string			    triggerringDpValue;
        std::string			    lccResult;
		bool    			    rccResult;

	};

    //engineering units for analogue data point
	struct DataPointEngineeringUnits
	{
		bool EngineeringUnitsEnabled; // Have engineering units been specified for this datapoint
		double	LowLimit; // The minimum allowable value for display or writing to the PLC, this is already expressedc in EU
		double	HighLimit; // The maximum allowable value for display or writing to the PLC
        std::string	Units; // The units that are used for this datapoint (KM/H, RPM, ...)
	};

    //************************************
    //
    //  Datapoint and datanode related constant definition
    //
    
    // Mask and values for GOOD, BAD and UNCERTAIN
    const unsigned int QUALITY_MASK			= 0xC0;		// 00000000 11000000
    const unsigned int QUALITY_BAD			= 0x00;		// 00000000 00000000
    const unsigned int QUALITY_UNCERTAIN	= 0x40;		// 00000000 01000000
    const unsigned int QUALITY_GOOD			= 0xC0;		// 00000000 11000000

    //
    // Maximum supported length for a TextDataPoint
    //
    const int DPT_MAX_TEXT_LENGTH = 64;

    //
    // DataNode Keepalive timer - in seconds
    //
    const int DATANODE_KEEPALIVE_SEC = (5 * 60);
    const long DATANODE_KEEPALIVE_MILLI = (DATANODE_KEEPALIVE_SEC * 1000);

    //
    // String representations of quality
    //
    const char * const QUALITY_BAD_NO_SPECIFIC_REASON_STRING			= "Bad";
    const char * const QUALITY_BAD_CONFIGURATION_ERROR_STRING 			= "Bad - Configuration Error";
    const char * const QUALITY_BAD_NOT_CONNECTED_STRING 				= "Bad - Not Connected";
    const char * const QUALITY_BAD_DEVICE_FAILURE_STRING 				= "Bad - Device Failure";
    const char * const QUALITY_BAD_SENSOR_FAILURE_STRING 				= "Bad - Sensor Failure";
    const char * const QUALITY_BAD_LAST_KNOWN_VALUE_STRING 				= "Bad - Last Known Value";
    const char * const QUALITY_BAD_COMM_FAILURE_STRING 					= "Bad - Comm Failure";
    const char * const QUALITY_BAD_OUT_OF_SERVICE_STRING 				= "Bad - Out of Service";

    const char * const QUALITY_BAD_VALUE_UNKNOWN_STRING					= "#####";

    const char * const QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON_STRING 	= "Uncertain";
    const char * const QUALITY_UNCERTAINTY_LAST_USABLE_VALUE_STRING 	= "Uncertain - Last Usable Value";
    const char * const QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE_STRING 	= "Uncertain - Sensor not Accurate";
    const char * const QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED_STRING 	= "Uncertain - Engineering Unit Exceeded";
    const char * const QUALITY_UNCERTAINTY_SUB_NORMAL_STRING 			= "Uncertain - Sub-normal";

    const char * const QUALITY_GOOD_NO_SPECIFIC_REASON_STRING			= "Good";
    const char * const QUALITY_GOOD_LOCAL_OVERRIDE_STRING				= "Good - Local Override";

    //
    // String presentations of boolean TRUE and FALSE
    //
    const char * const CFG_TRUE					= 	"1";
    const char * const CFG_FALSE				= 	"0";

    //
    // DataPoint Entity Parameters -------------------------------------------------
    //
    const char * const ENT_CFG_KEY			   = "Key";
    const char * const ENT_CFG_NAME            = "Name";
    const char * const ENT_CFG_DESCRIPTION     = "Description";
    const char * const ENT_CFG_AREAKEY         = "AreaKey";
    const char * const ENT_CFG_AREANAME        = "AreaName";
    const char * const ENT_CFG_TYPEKEY         = "TypeKey";
    const char * const ENT_CFG_TYPENAME        = "TypeName";
    const char * const ENT_CFG_SUBSYSTEMKEY    = "SubSystemKey";
    const char * const ENT_CFG_SUBSYSTEMNAME   = "SubSystemName";
    const char * const ENT_CFG_ADDRESS         = "Address";
    const char * const ENT_CFG_PARENTKEY       = "ParentKey";
    const char * const ENT_CFG_PARENTNAME      = "ParentName";

    //
    // String representations of the DataPoint types
    //
    const char * const DPT_CFG_ENTITYTYPE = "DataPoint";
    const char * const DPT_NULL_TYPE_STRING = "Unknown type";
    const char * const DPT_BOOLEAN_TYPE_STRING = "Boolean";
    const char * const DPT_TEXT_TYPE_STRING = "Text";
    const char * const DPT_ANALOGUE_TYPE_STRING = "Analogue";
    const char * const DPT_DERIVED_TYPE_STRING = "Derived";
    const char * const DPT_COMBINED_OUTPUT_TYPE_STRING = "CombinedOutput";
    const char * const DPT_SAFETY_OUTPUT_TYPE_STRING = "SafetyOutput";
    const char * const DPT_METERED_ANALOGUE_TYPE_STRING = "Metered";
    const char * const DPT_CALCULATED_TYPE_STRING = "Calculated";
    const char * const DPT_ENUM_TYPE_STRING = "Enum";
	const char * const DPT_MULTI_BIT_TYPE_STRING = "MultiBit";

    //
    // String representations of the DataPoint data types
    //
    const char * const DPT_CFG_BOOL			= 	"BOOL";
    const char * const DPT_CFG_INT16		=	"INT_16";
    const char * const DPT_CFG_INT32		=	"INT_32";
    const char * const DPT_CFG_INT16S		=	"INT_16S";
    const char * const DPT_CFG_INT32S		=	"INT_32S";
    const char * const DPT_CFG_UINT16		=	"UINT_16";
    const char * const DPT_CFG_UINT32		=	"UINT_32";
    const char * const DPT_CFG_IEEE32		=	"IEEE_32";
    const char * const DPT_CFG_TEXT			=	"TEXT";
    const char * const DPT_CFG_DERIVED		=	"DERIVED";
	const char * const DPT_CFG_MULTIBIT		=	"MULTIBIT";

    // 
    // String representations of the DataPoint control state
    //
    const char * const DPT_CONTROL_NOCONTROL			= 	"";
    const char * const DPT_CONTROL_WRITTEN				= 	"命令已发送";
    const char * const DPT_CONTROL_PENDING				= 	"处理中";
    const char * const DPT_CONTROL_CONDITIONALSUCCESS	= 	"";
    const char * const DPT_CONTROL_SUCCESS				= 	"";
    const char * const DPT_CONTROL_FAILED				= 	"失败";
    const char * const DPT_CONTROL_LCC_FAILED			= 	"启动校验失败";
    const char * const DPT_CONTROL_RCC_FAILED			= 	"结果校验失败";
    const char * const DPT_CONTROL_COMMS_FAILED			= 	"通信失败";

	//Using for IEC104 Protocol control
	const char * const DPT_CONTROL_SELECT_SENT			= 	"处理中";
	const char * const DPT_CONTROL_SELECT_CONFIRM		= 	"处理中";
	const char * const DPT_CONTROL_SELECT_TIMEOUT		= 	"";
	const char * const DPT_CONTROL_OPERATOR_TIMEOUT		= 	"";
	const char * const DPT_CONTROL_EXECUTE_SENT			= 	"处理中";
	const char * const DPT_CONTROL_EXECUTE_CONFIRM		= 	"处理中";
	const char * const DPT_CONTROL_EXECUTE_TIMEOUT		= 	"";
	const char * const DPT_CONTROL_EXECUTE_SUCCESS		= 	"";
	const char * const DPT_CONTROL_EXECUTE_SUC_TIMEOUT	= 	"";
	const char * const DPT_CONTROL_CANCEL_SENT			= 	"处理中";
	const char * const DPT_CONTROL_CANCEL_CONFIRM		= 	"";
	const char * const DPT_CONTROL_CANCEL_TIMEOUT		= 	"";
	
    // DataPoint ServiceState keywords
    const char * const DPT_CFG_INSERVICE	= 	"IN";
    const char * const DPT_CFG_OUTSERVICE	= 	"OUT";

    // DataPoint Address keywords
    const char * const DPT_CFG_VIRTUAL		= 	"VIRTUAL";

    // Alarm Severity keywords
    const char * const DPT_CFG_ALMSEVERITY_SYSTEM	= "SYSTEM";
    const char * const DPT_CFG_ALMSEVERITY_MINOR	= "MINOR";
    const char * const DPT_CFG_ALMSEVERITY_MAJOR	= "MAJOR";
    const char * const DPT_CFG_ALMSEVERITY_EXTREME	= "EXTREME";

    // DataPoint parameter names (Not modifiable online)
    const char * const DPT_CFG_TYPE 		=	"DataPointType";

    //
    // Indicate one of the following attributes of the data point has been changed
    //
    const char * const DPT_CFG_ALARM_PROPERTIES = "AlarmPropertiesChanged";
    const char * const DPT_CFG_OPERATOR_INFO = "OperatorInfoChanged";

    //
    // These strings are used for the AlarmType field of an Alarm
    //
    const char * const DPTALM_NONE			= "DataPointNoAlarm";
    const char * const DPTALM_NONE_SHORT	= "";
    const char * const DPTALM_HIHI			= "VH";
    const char * const DPTALM_HIHI_SHORT	= "VH";
    const char * const DPTALM_HI			= "HI";
    const char * const DPTALM_HI_SHORT		= "HI";
    const char * const DPTALM_LO  			= "LO";
    const char * const DPTALM_LO_SHORT  	= "LO";
    const char * const DPTALM_LOLO			= "VL";
    const char * const DPTALM_LOLO_SHORT	= "VL";
    const char * const DPTALM_TRUE			= "True Alarm";
    const char * const DPTALM_TRUE_SHORT	= "TA";
    const char * const DPTALM_FALSE			= "False Alarm";
    const char * const DPTALM_FALSE_SHORT	= "FA";
    const char * const DPTALM_GENERAL		= "DataPointAlarm";
    const char * const DPTALM_GENERAL_SHORT	= "GA";
    const char * const DPTALM_UNKNWN		= "DataPointUnknownAlarm";
    const char * const DPTALM_UNKNWN_SHORT	= "UA";
    const char * const DPTALM_OH			= "OH";
    const char * const DPTALM_OH_SHORT		= "OH";
    const char * const DPTALM_OL			= "OL";
    const char * const DPTALM_OL_SHORT		= "OL";

    //
    // String to indicate that the output data point has been set by the data point internal process
    //
    const char * const DPT_INTERNAL_SESSION_ID = "InternalProcessing";

	//************************************
    //
    //  Datapoint update use 
    //

	// the actual settings of each alarm will fit in a struct like this
	struct AlarmProperty
	{
		EDataPointAlarms    	type;		// the type of the alarm, eg HIHI, HI, LOLO, LO, TRUE, FALSE...
		bool					enabled;	// true to indicated the alarm is enabled, false otherwise
		double					threshold;	// this value is the alarm threshold (only used for analogue alarms)
		unsigned long			enumValue;	// this is the enum value for General Alarms
	};
	// A vector of alarm properties
	typedef std::vector< AlarmProperty > AlarmPropertyVect;

	struct LastOperatorInfo
    {
		std::string		sessionID;		// the operator session ID
		timeb			timeStamp;		// the timestamp when this datapoint was
		// last changed by the operator
    };

	enum EInputInhibitState
	{
		NoInputInhibit,
		InhibitScan,
		InhibitAlarm,
		InhibitMmsAlarm
	};
	
	enum EOutputInhibitState
	{
		NoOutputInhibit,
		InhibitControl
	};
	
	struct InhibitState
	{
		EInputInhibitState	inputInhibit;
		EOutputInhibitState	outputInhibit;
	};

	struct DpCompleteDynamicSyncState
	{
		DataPointValue			value;
		EDataPointAlarms		alarmState;
        EScadaAlarmAckState     alarmAckState;
		AlarmPropertyVect		alarmProperties;
		DataPointControl		controlState;			
		InhibitState			inhibitState;
		LastOperatorInfo		lastOperatorInfo;
	};

	enum EDpDynamicSyncUpdate
	{
		CompleteDynamicSyncStateUpdate,
		ValueUpdate,
		AlarmStateUpdate,
        AlarmAckStateUpdate,
		AlarmPropertiesUpdate,
		ControlUpdate,
		InhibitUpdate,
		LastOperatorUpdate
	};

	struct DpDynamicSyncUpdate
	{
		EDpDynamicSyncUpdate				updateType;
		DpCompleteDynamicSyncState			syncCompleteState;
		DataPointValue						value;
		EDataPointAlarms					alarmState;
        EScadaAlarmAckState                 alarmAckState;
		AlarmPropertyVect					alarmProperties;
		DataPointControl					controlState;
		InhibitState						inhibitState;
		LastOperatorInfo					lastOperatorInfo;
	};

    //Static configuration for the datapoint
    typedef std::map<unsigned long, std::string> DpvEnumLabelsMap;
	typedef std::pair<std::string, std::string> DpvBooleanLabels;
    struct DatapointCompleteStaticConfig
    {
        //static member variable
		unsigned long				entityKey;            
		std::string 				name;   
		unsigned long				entityTypeKey;
		unsigned long				subsystemKey;
		unsigned long				locationKey;
		unsigned long				parentKey;
		std::string					description;
		std::string					helpURL;
		std::string					typeShortName;//no set/get method provided, since not necessary
		long						displayOrder;
		long						precision;
		bool						writable;
		bool						pscadaFlag;				//Using for IEC104 Protocol
		unsigned long				relatedInputDataPoint;	//Using for IEC104 Protocol
        unsigned long               relatedWfDataPoint;     // Using for IEC104 WF
		TA_Base_Bus::EDataPointType                 type;
		TA_Base_Bus::EDataPointDataType             dataType;
        TA_Base_Bus::AlarmPropertyVect              alarmPropertyVect;
        TA_Base_Bus::DataPointEngineeringUnits      engUnits;
		DpvEnumLabelsMap			                enumLabelsMap;
		DpvBooleanLabels			                booleanLabels;
    };

    //Display information used by the OPC bridge
    struct DisplayDataPointValue
    {
        //indicate the datapoint type;
        EDataPointDataType  dpType;

        double				floatValue;//float datapoint value
        unsigned long		enumValue;//enum datapoint value
        bool				booleanValue;//boolean datapoint
        ByteSequence		stringValue;//text datapoint
    };

    struct DataPointDisplayState
    {
        unsigned long		entityKey;			// current entitykey of DataPoint
        DisplayDataPointValue	value;				// current value of DataPoint
        unsigned long		dataPointStatus;	// quality status of the current value
        bool				alarmState;			// Alarm state of the current DataPoint
        EScadaAlarmAckState alarmAckState;
    };

	// Update sequence
    typedef std::vector< DataPointDisplayState > DisplayStateSequence;
    
    //
    //datanode related 
    //

	struct BooleanTag
	{
		bool			value;
		EQualityStatus	quality;
	};
	
	struct AnalogueTag
	{
		double			value;
		EQualityStatus	quality;
	};
	
	struct EnumTag
	{
		unsigned long	value;
		EQualityStatus	quality;
	};
	
	struct TextTag
	{
		ByteSequence	value;
		EQualityStatus	quality;
	};

    enum SummaryCalculationType
    {
        noCalculation                       = 0,
        recalculateQualitySummary           = 1,
        recalculateAlarmSummary             = 2,
        recalculateOverrideSummary          = 3,
        recalculateControlPendingSummary    = 4,
        recalculateScanInhibitSummary       = 5,
        recalculateAlarmInhibitSummary      = 6,
        recalculateMmsAlarmInhibitSummary   = 7,
        recalculateControlInhibitSummary    = 8,
        recalculateAlarmAckSummary          = 9, 
        recalculateLastOperatorInfo         = 10 
    };

    struct DataNodeStaticConfiguration
    {
        std::string                         name;
        std::string                         description;
        unsigned long                       entityKey;
        unsigned long                       subsystemKey;
        unsigned long                       locationKey;
        std::string                         agentName;
        std::string                         label;
        std::string                         shortLabel;
        std::string                         assetName;
        int                                 displayOrder;
		bool								pscadaFlag;	//Using for IEC104 Protocol

        TA_Base_Core::SharedIEntityDataList childDataNodeEntities;
        TA_Base_Core::SharedIEntityDataList childDataPointEntities;
    };

	enum ETagState
	{
		NotTagged,
		Tagged,
		TaggedPtw
	};

    struct TagState
    {
	    ETagState							tagState;
	    InhibitState		                tagInhibitState;
        std::string							ptwId;
        std::string							comment;
    };

    struct DnCompleteDynamicSyncState
    {
		TagState	tagState;
		BooleanTag	alarmSummary;
		EnumTag		ackSummary;
		BooleanTag	scanInhibitSummary;
		BooleanTag	alarmInhibitSummary;
		BooleanTag	mmsAlarmInhibitSummary;
		BooleanTag	controlInhibitSummary;
		BooleanTag	overrideSummary;
		EnumTag		qualitySummary;
		BooleanTag	controlPendingSummary;
		BooleanTag  combinedInhibitMMSState;
        std::string		notes;
		bool			inhibitMMSState;
        LastOperatorInfo	lastOperator;
    };
 
	enum EDnSyncUpdate
	{
		DnCompleteSyncState,
		DnTagStateUpdate,
		DnAlarmSummaryUpdate,
		DnAlarmAckSummaryUpdate,
		DnScanInhibitSummaryUpdate,
		DnAlarmInhibitSummaryUpdate,
		DnMmsAlarmInhibitSummaryUpdate,
		DnControlInhibitSummaryUpdate,
		DnOverrideSummaryUpdate,
		DnQualitySummaryUpdate,
		DnControlPendingSummaryUpdate,
		DnNotesUpdate,
		DnInhibitMMSUpdate,
		DnCombinedInhibitMMSUpdate,
		DnLastOperatorUpdate
	};

    
	struct DnSyncUpdate 
	{
        EDnSyncUpdate updateType;

        DnCompleteDynamicSyncState completeUpdate;
		TagState	tagState;
		BooleanTag	alarmSummary;
		EnumTag		ackSummary;
		BooleanTag	scanInhibitSummary;
		BooleanTag	alarmInhibitSummary;
		BooleanTag	mmsAlarmInhibitSummary;
		BooleanTag	controlInhibitSummary;
		BooleanTag	overrideSummary;
		EnumTag		qualitySummary;
		BooleanTag	controlPendingSummary;
		BooleanTag  combinedInhibitMMSState;
        std::string		notes;
		bool			inhibitMMSState;
        LastOperatorInfo	lastOperator;
	};


	struct DataNodeDisplayState
	{
		unsigned long		entityKey;			// current entitykey of DataPoint
        std::string				ONITP;				//the 'Override', 'Note', 'Tag', 'Inhibit' and 'Ptw' statuses
	    BooleanTag	alarmState;			// alarm state boolean tag
		EnumTag		alarmAckState;		// alarm ack state tag
		EnumTag		quality;			// quality tag
	};
    
    // Update notification types
    typedef enum
    {
        NotApplicable,  // Raymond Pau++ TD14844
//dp
        ValueStateUpdate,
        ControlStateUpdate,
        ConfigStateLastOperatorUpdate,
        
//dn
        ControlSummaryUpdate,
        AlarmSummaryUpdate,
        OverrideSummaryUpdate,
        QualitySummaryUpdate,
        ScanInhibitSummaryUpdate,
        AlarmInhibitSummaryUpdate,
        MmsAlarmInhibitSummaryUpdate,
        ControlInhibitSummaryUpdate,
		CombinedInhibitMMSUpdate,
        AckSummaryUpdate,
        TagStateUpdate,
        NotesStateUpdate,

//both
        InhibitStateUpdate,
        ConfigStateUpdate,
        ConfigAvailable,

		//zhiqiang++ TD_20597
		AgentConnected

    } ScadaEntityUpdateType;

    // TD18575, Added for control state synchronization
    enum ECommandError
    {
        PermisionDenied,
        OtherError
    };
    
    enum EControlCommandState	//This enum 
    {
        JustReceived,			
        BeingExecuted,
        CompletedWithError,
        CompletedWithSuccess
    };
    
    struct DataPointStateUpdate
    {
        bool                    auditEventLoggingEnabled;
        unsigned long           commandId;
        EControlCommandState    controlCommandState;
        EDataPointControlState  controlState;
        unsigned long           dpKey;
        DataPointValue          dpValue;
        std::string             sessionId;
        unsigned long           triggeringDpKey;
        std::string             triggeringDpValue;
    };
}

#endif
