/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source:  $
 * @author:  
 * @version: $Revision:  $
 *
 * Last modification: $Date: $
 * Last modified by:  $Author: $
 * 
 * The DataPointFactory creates this servant. The activation of servant will
 * happen when setToMonitor method is executed. The servant will be deactivated
 * when setToControl method is executed. On startup, the servant will not be
 * activated till setToMonitor method is executed.
 * 
 * This class will get the reference to the datapoint used in the update, get  the
 * CommandProcessor from the DataPoint and then execute the method
 * processDataPointStateUpdate.
 * 
 * This class should translate CORBA structs to internal ones. CORBA structs
 * should not be used else where in the code.
 */

#if !defined DataPointStateUpdateServant__INCLUDED_
#define DataPointStateUpdateServant__INCLUDED_

#include "core/corba/src/ServantBase.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/IDataPointStateUpdateCorbaDef.h"
//#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Bus
{
	class CommandProcessor;
    class DataPointStateUpdateServant : public virtual POA_TA_Base_Bus::IDataPointStateUpdateCorbaDef,
                                        public virtual TA_Base_Core::ServantBase
    {

    public:
	    virtual ~DataPointStateUpdateServant();

    public:
	    /**
	     * 
	     * @param servantName
	     * @param commandProcessor
	     */
	    DataPointStateUpdateServant(const std::string& servantName, CommandProcessor* commandProcessor);
	    
        /**
	     * 
	     * @param dpWriteStateUpdate
	     */
        void sendDataPointStateUpdate(const DataPointCorbaTypes::DataPointWriteStateUpdate& dpWriteStateUpdate);
	    
        /**
	     * 
	     * @param dpWriteStateUpdateSequence
	     */
        void sendDataPointStateUpdates(const DataPointCorbaTypes::DataPointWriteStateUpdateSequence& dpWriteStateUpdates);
	    
        /**
	     * 
	     * @param 
	     */
        TA_Base_Bus::IDataPointStateUpdateCorbaDef_ptr getDataPointStateUpdateServant();

        void setToControl();
	    void setToMonitor();
		void setOperationMode(TA_Base_Core::EOperationMode mode);

    private:
        //DataPointStateUpdateServant(){};
        //DataPointStateUpdateServant(DataPointStateUpdateServant&){};

        std::string m_servantName;
        CommandProcessor* m_commandProcessor;

        bool m_isControl;
        bool m_isActivated;
    };
}
#endif