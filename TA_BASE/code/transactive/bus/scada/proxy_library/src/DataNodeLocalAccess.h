/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeLocalAccess.h $
  * @author:  Darren Sampson
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 14:56:06 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the interface to a DataNode
  */

#ifndef DNLOCALOBJACCESS_H
#define DNLOCALOBJACCESS_H

#include "bus/scada/proxy_library/src/IDataNodeAccess.h"

namespace TA_Base_Bus
{
    class DataNode;
    class DataNodeLocalAccess : public IDataNodeAccess
    {

    public:

        DataNodeLocalAccess(TA_Base_Bus::DataNode* dataNode);

        virtual ~DataNodeLocalAccess();

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

		virtual unsigned short getFlexibleTimeOutValue();
    
    private:

		DataNodeLocalAccess():m_dataNodeLocalObject(NULL)
		{
		};

    protected:

        // Interface to DataNode
        TA_Base_Bus::DataNode*  m_dataNodeLocalObject;
    };
};  

#endif

