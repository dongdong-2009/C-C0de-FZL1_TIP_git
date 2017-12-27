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
// DispalyDataPointProxy.h: interface for the DispalyDataPointProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DISPLAY_DATAPOINT_PROXY_H
#define DISPLAY_DATAPOINT_PROXY_H

#include <string>
#include <comdef.h>

#include "bus/scada/common_library/src/DpValue.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_App
{
	class AbstractDataObjectAttributes;
    
    class DisplayDataPointProxy  
    {
    public:

        DisplayDataPointProxy(unsigned long entityKey, AbstractDataObjectAttributes * eventProcessor);
        virtual ~DisplayDataPointProxy();

        bool updateState(TA_Base_Bus::DataPointDisplayState& newState);

        TA_Base_Bus::DisplayDataPointValue getValue();
	    _variant_t getVariantValue();

		bool isInAlarmState();
        TA_Base_Bus::EScadaAlarmAckState getAlarmAckState();
        WORD getStatus();
        void setStatus(unsigned long newStatus);

		AbstractDataObjectAttributes * getAttribute();
		void setAttribute(AbstractDataObjectAttributes * );
            
    private:    

        DisplayDataPointProxy();
        DisplayDataPointProxy(DisplayDataPointProxy&){};
    
    private:
		AbstractDataObjectAttributes * m_attribute;
		TA_Base_Core::ReEntrantThreadLockable m_lock;

        TA_Base_Bus::DataPointDisplayState m_state;

    };
}
#endif // !defined(DISPLAY_DATAPOINT_PROXY_H)
