/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeLocalSynchronizer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by:  $Author: yong.liu $
  *
  */
///////////////////////////////////////////////////////////
//  DataNodeLocalSynchronizer.h
//  Implementation of the Class DataNodeLocalSynchronizer
//  Created on:      28-Jun-2004 03:44:08 PM
///////////////////////////////////////////////////////////

#if !defined(DataNodeEventDrivenAgentSyncIf_C8B57EB2_D403_431e_B32A_3278BD7772B7__INCLUDED_)
#define DataNodeEventDrivenAgentSyncIf_C8B57EB2_D403_431e_B32A_3278BD7772B7__INCLUDED_

#include <map>
#include <set>

#include "bus/scada/proxy_library/src/AgentSynchronisationInterface.h"
#include "bus/scada/datanode_library/src/IDataNodeObserverNotification.h"

/**
 * This class is used to synchronise datapoints that are managed in the same
 * process context.  There will only be one instance of this type.
 * @version 1.0
 * @created 28-Jun-2004 03:44:08 PM
 */

namespace TA_Base_Bus
{
    class DataNodeSyncManager;
    class DataNodeProxyManager;
    
    class DataNodeLocalSynchronizer : public AgentSynchronisationInterface,
                                            public TA_Base_Bus::IDataNodeObserverNotification
    {
    public:
        DataNodeLocalSynchronizer(const std::string& agentName,
                                       unsigned long locationKey,
									   DataNodeSyncManager& manager,
									   DataNodeProxyManager& callback);
        
        virtual ~DataNodeLocalSynchronizer();

		/**
		 * processDataNodeUpdate
		 *
		 * Call back for processing of the new data node update details
		 *
		 * @param 	pkey			The pkey of the data node
		 * @param 	updateDetails	The new updates of the data node specified by the pkey
		 *
		 */
		virtual void processDataNodeUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails );

        /**
         * Define a run() method to do your work. This should block until terminate() is
         * called eg: CorbaUtil::GetInstance().Run() or m_dialog->DoModal()  NOTE: do not
         * call this method directly. It needs to be public so that it can be called from
         * the thread-spawning function
         */
        virtual void run();

		virtual void processRequest(RequestItem requestItem);   
      protected:

        
      private:
          DataNodeSyncManager& m_manager;
          DataNodeProxyManager& m_callback;
          
    };
};
#endif // !defined(DataNodeEventDrivenAgentSyncIf_C8B57EB2_D403_431e_B32A_3278BD7772B7__INCLUDED_)
