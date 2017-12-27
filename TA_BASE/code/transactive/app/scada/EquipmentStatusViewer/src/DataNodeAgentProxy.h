/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataNodeAgentProxy.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// DataNodeAgentProxy.h: interface for the DataNodeAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DATANODE_AGENT_PROXY_H
#define DATANODE_AGENT_PROXY_H

#include <set>
#include <string>
#include <sys/timeb.h>

#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/Condition.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "bus/scada/common_library/IDL/src/IDataNodeAgentCorbaDef.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

namespace TA_Base_App
{
    class CEquipmentStatusViewerDlg;
    
    class DataNodeAgentProxy : public TA_Base_Core::Thread
								//public TA_Base_Core::ITimeoutCallback //CL-20798 zhangjunsheng
        
    {
    public:
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataNodeAgentCorbaDef,
                                           TA_Base_Bus::IDataNodeAgentCorbaDef_ptr,
                                           TA_Base_Bus::IDataNodeAgentCorbaDef_var >   DataNodeAgentInterface;	
        DataNodeAgentProxy(TA_Base_Core::CorbaNameList corbaNames, CEquipmentStatusViewerDlg* dlg);

	    virtual ~DataNodeAgentProxy();

        virtual void run();
        
        virtual void terminate();

		//CL-20798 zhangjunsheng
		/*virtual void timerExpired(long timerId, void* userData);
		
        std::vector<unsigned long> getCurrentEntitiesToShow();*/

		void setToBadNotConnectedQuality();
                
    private:

        volatile bool m_toBeTerminated;
		//unsigned long m_pollPeriod; //CL-20798 zhangjunsheng
        CEquipmentStatusViewerDlg* m_dlg;
        std::map<DataNodeAgentInterface *, unsigned long > m_agentMap;
		
		//CL-20798 zhangjunsheng
        //TA_Base_Core::Condition					m_timerCondition;
        //TA_Base_Core::SingletonTimerUtil&		m_timerUtility;
        
    };
}
#endif // DATANODE_AGENT_PROXY_H
