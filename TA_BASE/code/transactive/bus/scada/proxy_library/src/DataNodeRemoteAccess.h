/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeRemoteAccess.h $
  * @author:  Darren Sampson
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/02/09 18:11:39 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the interface to a DataNode
  */

#ifndef DNNAMEDOBJACCESS_H
#define DNNAMEDOBJACCESS_H

#include "bus/scada/proxy_library/src/IDataNodeAccess.h"
#include "bus/scada/common_library/IDL/src/IDataNodeAccessCorbaDef.h"

#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/naming/src/NamedObject.h"

#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class DataNodeEntityData;
}

namespace TA_Base_Bus
{
    class DataNodeRemoteAccess : public IDataNodeAccess
    {

    public:

        DataNodeRemoteAccess(TA_Base_Core::DataNodeEntityDataPtr dnEntityData);

        virtual ~DataNodeRemoteAccess();


        /**
          * getConfiguration
          *
          * This method gets the configuration of the DataNode
          *
          * @return The configuration of the DataNode
          */
        virtual boost::shared_ptr<DataNodeStaticConfiguration> getConfiguration();

        // Change the Tag on a DataNode 
        /**
          * changeDataNodeTag
          *
          * This method changes the Tag on a DataNode   
          *
          * @param The operator session id
          * @param The new tag state for the DataNode
          * @param The new PTW ID for the DataNode (only if the tag state is TAGGED_PTW)
          * @param The new PTW Comment for the DataNode (only if the tag state is TAGGED_PTW)
          *
          * @exception TA_Base_Core::BadParameterException raised if tag state is TAGGED_PTW and no PTW ID or Comment is provided.
          * @exception TA_Base_Core::BadParameterException raised if tag state is not TAGGED_PTW and PTW ID or Comment is provided.
          * @exception TA_Base_Core::AccessDeniedException raised if the operator does not have permission to tag.
          */    
        virtual void changeDataNodeTag(const ETagState tagState, 
            const EOutputInhibitState outputInhibitState, 
            const EInputInhibitState inputInhibitState, 
            const std::string& ptwId, 
            const std::string& tagComment,
            const std::string& sessionId);


        /**
          * setInhibitMms
          *
          * This method sets the DataNode's Mms Inhibit state
          *
          * @param The operator session id
          * @param The new state for the DataNode Mms inihibit state
          *
          * @exception TA_Base_Core::AccessDeniedException raised if the operator does not have permission to modify notes.
          */
        virtual void setInhibitMms(const std::string&  sessionId, bool inhibitMms);

        /**
          * setDataNodeNotes
          *
          * This method sets the DataNode's notes
          *
          * @param The operator session id
          * @param The new notes for the DataNode
          *
          * @exception TA_Base_Core::AccessDeniedException raised if the operator does not have permission to modify notes.
          */
        virtual void setDataNodeNotes(  const std::string&  sessionId,
                                        const std::string&  notes);

		//wenching++ (TD13621)
        /**
          * getFlexibleTimeOutValue
          *
          * This method gets the DataNode's flexible timeout
          *
          * @return The DataNode's flexible timeout
          */
        virtual unsigned short getFlexibleTimeOutValue();
        //++wenching (TD13621)

    private:

        // Disable default constructor
        DataNodeRemoteAccess();
        
        static const unsigned long DATANODE_AGENT_CORABA_TIMEOUT;
    protected:
            
        // Corba Interface to DataNode
        typedef TA_Base_Core::FlexibleTimeoutNamedObject< TA_Base_Bus::IDataNodeAccessCorbaDef,
                                                  TA_Base_Bus::IDataNodeAccessCorbaDef_ptr,
                                                  TA_Base_Bus::IDataNodeAccessCorbaDef_var > 
                                                  DnAccessInterface;
        
        DnAccessInterface* m_dnAccessInterface;

        TA_Base_Core::DataNodeEntityDataPtr m_entityData;
    };
};  

#endif

