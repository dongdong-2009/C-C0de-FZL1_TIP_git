#ifndef IDATANODEPROXY_H
#define IDATANODEPROXY_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/IDataNodeProxy.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  *
  * Last modified by:  $Author: haijun.li $
  * 
  * This class provides access to a datapoint
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/naming/src/INamedObject.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Bus
{
	class IDataNodeProxy
	{
		public:

			//
			// DataNode State/Properties
			//

			// Accessors for Data node configuration
			virtual bool							isConfigValid()	const 							= 0;
			virtual unsigned long					getEntityKey() const							= 0;
            virtual std::string 	                getAgentName() const                            = 0;
            virtual std::string 	                getEntityName() const                            = 0;
            virtual unsigned long			        getLocationKey() const                           = 0;
            virtual unsigned long	                getSubSystemKey() const                         = 0;
            virtual unsigned long	                getParentKey() const                            = 0;
            virtual unsigned long	                getPhysicalSubSystemKey() const                 = 0;
            virtual const std::string&				getDescription() const							= 0;
			virtual const std::string&				getLabel() const								= 0;
			virtual const std::string&				getShortLabel() const							= 0;
			virtual const std::string&				getAssetName() const							= 0;
            virtual TA_Base_Core::SharedIEntityDataList           getChildDataPointEntities() const			= 0;
			virtual TA_Base_Core::SharedIEntityDataList           getChildDataNodeEntities() const			= 0;
			virtual bool							isEquipmentNode() const							= 0;

			// Accessors for Tag State
			virtual bool							isTagged() const								= 0;	
			virtual ETagState					    getTaggingState() const							= 0;
			virtual InhibitState					getTagInhibitState() const						= 0;
			virtual std::string						getPtwId() const								= 0;
			virtual std::string						getTagComment() const							= 0;
			virtual void							setTagState(const ETagState tagState, 
																const EOutputInhibitState outputInhibitState,
																const EInputInhibitState inputInhibitState,
																const std::string& ptwId,
																const std::string& tagComment,
																const std::string& sessionId)			= 0;

			virtual std::string						getLastStatusChangeSessionId()  const			= 0;
			virtual unsigned long					getLastStatusChangeTimestamp() const			= 0;

			// Accessors for Notes
			virtual void							setNotes(const std::string& notes, 
															 std::string& sessionId)				= 0;
			virtual std::string						getNotes() const								= 0;

			// Accessors for MMS Inhibit
			virtual void							setInhibitMms(bool inhibitMms,
																  const std::string& sessionId)		= 0;
			virtual bool							getInhibitMms() const							= 0;
			virtual TA_Base_Bus::BooleanTag			getCombinedInhibitMms() const					= 0;
			//
			// DataNode Summaries
			//

			// Accessors for Alarm Summary
			virtual TA_Base_Bus::BooleanTag			getAlarmSummary() const							= 0;
			virtual TA_Base_Bus::EnumTag			getAlarmAckStateSummary() const					= 0;

			// Accessors for Inhibit summary
			virtual TA_Base_Bus::BooleanTag			getControlInhibitSummary() const				= 0;
			virtual TA_Base_Bus::BooleanTag			getAlarmInhibitSummary() const					= 0;
			virtual TA_Base_Bus::BooleanTag			getScanInhibitSummary() const					= 0;
			virtual TA_Base_Bus::BooleanTag			getMmsAlarmInhibitSummary() const				= 0;

			// Accessor for Override Summary
			virtual TA_Base_Bus::BooleanTag			getOverrideSummary() const						= 0;

			// Accessor for Quality Summary
			virtual TA_Base_Bus::EnumTag			getQualitySummary() const						= 0;
			
			// Accessor for Control Pending Summary
			virtual TA_Base_Bus::BooleanTag			getControlPendingSummary() const				= 0;

			//Using for IEC104 Protocol
			/**
			* DataPoint::getPScadaFlag()
			*
			* Returns the PScada Flag of this Data Point
			*
			* @return	true indicate this is a data point used in PSCADA.
			*
			*/
			virtual bool getPScadaFlag() const														= 0;
			IDataNodeProxy()
			{
			};
			virtual ~IDataNodeProxy()
			{
			};
	};
}

#endif // #ifndef IDATANODEPROXY_H
