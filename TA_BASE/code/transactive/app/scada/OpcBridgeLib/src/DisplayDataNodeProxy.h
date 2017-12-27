/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// DispalyDataNodeProxy.h: interface for the DispalyDataNodeProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DISPLAY_DATANODE_PROXY_H
#define DISPLAY_DATANODE_PROXY_H

#include <string>
#include <comdef.h>

#include "bus/scada/common_library/src/Commondefs.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_App
{
	class AbstractDataObjectAttributes;
	
    class DisplayDataNodeProxy  
    {
    public:

        DisplayDataNodeProxy(unsigned long entityKey, AbstractDataObjectAttributes * eventProcessor);
        virtual ~DisplayDataNodeProxy();

        bool updateState(TA_Base_Bus::DataNodeDisplayState& newState);

        std::string getONITP();
		_variant_t getVariantONITP();
        TA_Base_Bus::BooleanTag getAlarmSummary();
        TA_Base_Bus::EnumTag getAlarmAckStateSummary();
        TA_Base_Bus::EnumTag getQualitySummary();
		unsigned long getQualityValue();

        void setStatus(unsigned long newStatus);

		AbstractDataObjectAttributes * getAttribute();
		void setAttribute(AbstractDataObjectAttributes * );
            
    private:    
        DisplayDataNodeProxy();
        DisplayDataNodeProxy(DisplayDataNodeProxy&){};
    
    private:
		AbstractDataObjectAttributes * m_attribute;
		TA_Base_Core::ReEntrantThreadLockable m_lock;

        TA_Base_Bus::DataNodeDisplayState m_state;
    };
}
#endif // !defined(DISPLAY_DATANODE_PROXY_H)
