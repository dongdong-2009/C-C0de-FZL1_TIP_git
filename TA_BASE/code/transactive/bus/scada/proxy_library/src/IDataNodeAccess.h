/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/IDataNodeAccess.h $
  * @author:  Darren Sampson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is the interface to a DataNode
  */

#ifndef IDNOBJACCESS_H
#define IDNOBJACCESS_H

#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/naming/src/INamedObject.h"

namespace TA_Base_Bus
{
    class DataNodeProxy;

    class IDataNodeAccess
    {
    
    public:

        IDataNodeAccess() {};

        virtual ~IDataNodeAccess() {};

        /**
          * getConfiguration
          *
          * This method gets the configuration of the DataNode
          *
          * @return The configuration of the DataNode
          */
        virtual boost::shared_ptr<DataNodeStaticConfiguration> getConfiguration() = 0;

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
            const std::string& sessionId) = 0;


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
        virtual void setInhibitMms(const std::string& sessionId, bool inhibitMms) = 0;

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
        virtual void setDataNodeNotes( const std::string& sessionId,
                                       const std::string& notes) = 0;

		virtual unsigned short getFlexibleTimeOutValue() = 0;
    };
};  

#endif
