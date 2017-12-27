#ifndef DATANODEPROXY_H
#define DATANODEPROXY_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeProxy.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class provides access to a datapoint
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <map>
#include <vector>

#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/DataNodeSyncManager.h"
#include "bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.h"
#include "bus/scada/proxy_library/src/ProxyObserverNotifier.h"
#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

namespace TA_Base_Bus
{
    class IDataNodeAccess;
    
	class DataNodeProxy : public IDataNodeProxy, public ProxyObserverNotifier
	{
		public:

			DataNodeProxy(TA_Base_Core::DataNodeEntityDataPtr dnEntityData,
                DataNodeSyncManager& dataNodeManager);

			virtual ~DataNodeProxy();

			// Accessors for Tag State
			bool						isTagged() const;	
			ETagState				    getTaggingState() const;
			InhibitState				getTagInhibitState() const;
			std::string					getPtwId() const;
			std::string					getTagComment() const;
			void						setInhibitMms(bool inhibitMms,
													  const std::string& sessionId);
			void						setTagState(const ETagState tagState,
													const EOutputInhibitState outputInhibitState,
													const EInputInhibitState inputInhibitState,
													const std::string& ptwId,
													const std::string& tagComment,
													const std::string& sessionId);

			// Accessors for Alarm Summary
			TA_Base_Bus::BooleanTag					getAlarmSummary() const;
			TA_Base_Bus::EnumTag					getAlarmAckStateSummary() const;

			// Accessors for Inhibit summary
			TA_Base_Bus::BooleanTag					getControlInhibitSummary() const;
			TA_Base_Bus::BooleanTag					getAlarmInhibitSummary() const;
			TA_Base_Bus::BooleanTag					getScanInhibitSummary() const;
			TA_Base_Bus::BooleanTag					getMmsAlarmInhibitSummary() const;
			
			// Accessor for Override Summary
			TA_Base_Bus::BooleanTag					getOverrideSummary() const;

			// Accessor for Quality Summary
			TA_Base_Bus::EnumTag					getQualitySummary() const;

			// is this node an EQUIPMENT node
			bool					isEquipmentNode() const;
			
			std::string				getLastStatusChangeSessionId() const;
			unsigned long			getLastStatusChangeTimestamp() const;

			// Accessor for Control Pending Summary
			TA_Base_Bus::BooleanTag					getControlPendingSummary() const;

			// Accessors for Notes
			void					setNotes(const std::string& notes, std::string& sessionId);
			std::string				getNotes() const;
							
			// Accessors for Data node configuration
			virtual bool			isConfigValid() const;
			unsigned long			getEntityKey() const;
            unsigned long	getSubSystemKey() const;
            unsigned long	getParentKey() const;
            unsigned long	getPhysicalSubSystemKey() const;
            std::string 	        getAgentName() const;
            std::string 	        getEntityName() const;
            unsigned long			getLocationKey() const;
			const std::string&		getDescription() const;
			const std::string&		getLabel() const;
			const std::string&		getShortLabel() const;
			const std::string&		getAssetName() const;
			bool					getInhibitMms() const;
			BooleanTag				getCombinedInhibitMms() const;
            virtual TA_Base_Core::SharedIEntityDataList		getChildDataPointEntities() const;
			virtual TA_Base_Core::SharedIEntityDataList		getChildDataNodeEntities() const;

			// Tag State
			void			updateTagState(const TA_Base_Bus::TagState& tagState);
			// Summary State
			void			updateAlarmSummaryState(const TA_Base_Bus::BooleanTag& alarmSummary);
			void			updateAlarmAckSummaryState(const TA_Base_Bus::EnumTag& alarmAckSummary);
			void			updateScanInhibitSummaryState(const TA_Base_Bus::BooleanTag& scanInhbitSummary);
			void 			updateAlarmInhibitSummaryState(const TA_Base_Bus::BooleanTag& alarmInhbitSummary);
			void 			updateControlInhibitSummaryState(const TA_Base_Bus::BooleanTag& controlInhbitSummary);
			void 			updateMmsAlarmInhibitSummaryState(const TA_Base_Bus::BooleanTag& mmsAlarmInhbitSummary);
			void 			updateOverrideSummaryState(const TA_Base_Bus::BooleanTag& overrideSummary);
			void 			updateQualitySummaryState(const TA_Base_Bus::EnumTag& qualitySummary);
			void 			updateControlPendingSummaryState(const TA_Base_Bus::BooleanTag& controlPendingSummary);
            void 			updateLastOperator(const TA_Base_Bus::LastOperatorInfo& lastOperator);
			void            updateCombinedInhibitMMSState(const TA_Base_Bus::BooleanTag& combinedMMS);
			void			inhibitSummaryQuality() const;
			// Notes
			void			updateNotesState(const std::string& notes);
			// Configuration
			void			loadConfiguration();
			void			updateState(const TA_Base_Bus::DnCompleteDynamicSyncState& state);

			//inhibit
			void			updateInhibitState(const bool inhibitState);

			void			updateAgentNotAvailableStatus();

			virtual bool	getPScadaFlag() const;
		protected:
	
            // weimin: to initialise and access datapoint access object
            IDataNodeAccess*   getObjAccess();
            
        protected: 

			mutable TA_Base_Core::ReEntrantThreadLockable			
											m_updateLock;

            struct DataPointProxyData
            {
               bool isConfigValid;
               bool isSynchronized;

               DnCompleteDynamicSyncState      dynamicData;
               DataNodeStaticConfiguration     configData;
            } m_proxyData;

            TA_Base_Core::DataNodeEntityDataPtr m_entityData;
            DataNodeSyncManager&				m_syncManager;
            IDataNodeAccess*					m_objAccess;
			bool								m_isLocal;
	};
}

#endif // #ifndef DATANODEPROXY_H
