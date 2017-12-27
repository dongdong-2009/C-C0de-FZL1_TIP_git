/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/proxy_library/src/DataNodeRemoteAccess.cpp $
  * @author:  Darren Sampson
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/02/09 18:11:39 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the interface to a DataNode
  */

#include <sstream>

#include "bus/scada/proxy_library/src/DataNodeProxy.h"
#include "bus/scada/proxy_library/src/DataNodeRemoteAccess.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	const unsigned long DataNodeRemoteAccess::DATANODE_AGENT_CORABA_TIMEOUT = 5;

    DataNodeRemoteAccess::DataNodeRemoteAccess(TA_Base_Core::DataNodeEntityDataPtr dnEntityData)
    :
        m_dnAccessInterface(NULL),
        m_entityData(dnEntityData)
    {
        std::string interfaceName(DN_ACCESS_INTERFACE);
        
        m_dnAccessInterface = new DnAccessInterface();
        m_dnAccessInterface->setCorbaName(m_entityData->getAgentName(), m_entityData->getAgentName() + interfaceName);
        //set timeout to 5 seconds.
        m_dnAccessInterface->setObjectTimeout(DATANODE_AGENT_CORABA_TIMEOUT);
    };

    DataNodeRemoteAccess::~DataNodeRemoteAccess()
    {
        delete m_dnAccessInterface;
        m_dnAccessInterface = NULL;
    };


    /**
      * getConfiguration
      *
      * This method gets the configuration of the DataNode
      *
      * @return The configuration of the DataNode
      */
    boost::shared_ptr<DataNodeStaticConfiguration> DataNodeRemoteAccess::getConfiguration()
    {
        //load from database
        DataNodeStaticConfiguration* configuration = new DataNodeStaticConfiguration();
        if (m_entityData.get() != NULL)
        {
            configuration->agentName = m_entityData->getAgentName();
            configuration->assetName = m_entityData->getAssetName();
            configuration->description = m_entityData->getDescription();
            configuration->displayOrder = m_entityData->getDisplayOrder();
            configuration->entityKey = m_entityData->getKey();
            configuration->label = m_entityData->getLabel();
            configuration->locationKey = m_entityData->getLocation();
            configuration->name = m_entityData->getName();
            configuration->shortLabel = m_entityData->getShortLabel();
            configuration->subsystemKey = m_entityData->getSubsystem();
			/*
			 *getChildDataNode and getChildDataPoints cost much time, this will slow down
			 *creating DataNodeProxy, we will delay these two processes to the moment that 
			 *user really want to get datanode's children, most of time, user does not care 
			 *data node's child at all.
			 */
			//configuration->childDataNodeEntities = m_entityData->getChildDataNodes();
			//configuration->childDataPointEntities = m_entityData->getChildDataPoints();
			configuration->pscadaFlag = m_entityData->getPScadaFlag(); //Using for IEC104 Protocol
        }

        return boost::shared_ptr<DataNodeStaticConfiguration>(configuration);

        //return (*m_dnAccessInterface)->getCompleteStaticConfigurations();
    };


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
    void DataNodeRemoteAccess::changeDataNodeTag(const ETagState tagState, 
        const EOutputInhibitState outputInhibitState, 
        const EInputInhibitState inputInhibitState, 
        const std::string& ptwId, 
        const std::string& tagComment,
        const std::string& sessionId)
    {

        DataNodeCorbaTypes::ETagState		corbaTagState;
        ScadaCorbaTypes::EOutputInhibitState	corbaOutputInhibitState;
        ScadaCorbaTypes::EInputInhibitState	corbaInputInhibitState;

        convertETagState(tagState, corbaTagState);
        convertOutputInhibitState(outputInhibitState, corbaOutputInhibitState);
        convertInputInhibitState(inputInhibitState, corbaInputInhibitState);
        
		//we expand timeout 3 times, change tag really cost time
		m_dnAccessInterface->setObjectTimeout(DATANODE_AGENT_CORABA_TIMEOUT * 3);

        CORBA_CALL( (*m_dnAccessInterface),
                    changeDataNodeTag,
                    ( m_entityData->getKey(), 
                      sessionId.c_str(), 
                      corbaTagState, 
                      corbaOutputInhibitState, 
                      corbaInputInhibitState,
                      ptwId.c_str(), 
                      tagComment.c_str() ) );
    };

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
    void DataNodeRemoteAccess::setDataNodeNotes( const std::string&  sessionId,
                                             const std::string&  notes )
    {
		m_dnAccessInterface->setObjectTimeout(DATANODE_AGENT_CORABA_TIMEOUT);

        CORBA_CALL( (*m_dnAccessInterface),
                    setNotes,
                    ( m_entityData->getKey(), sessionId.c_str(), notes.c_str() ) );
    };

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
    void DataNodeRemoteAccess::setInhibitMms( const std::string&  sessionId, bool inhibitMms )
    {
		m_dnAccessInterface->setObjectTimeout(DATANODE_AGENT_CORABA_TIMEOUT);

        CORBA_CALL( (*m_dnAccessInterface),
                    setInhibitMMS,
                    ( m_entityData->getKey(), sessionId.c_str(), inhibitMms ) );

    } 
	
	//wenching++ (TD13621)
    /**
      * getFlexibleTimeOutValue
      *
      * This method gets the DataNode's flexible timeout
      *
      * @return The DataNode's flexible timeout
      */
    unsigned short DataNodeRemoteAccess::getFlexibleTimeOutValue()
    {
        unsigned short ret;
        CORBA_CALL_RETURN( ret, (*m_dnAccessInterface), getFlexibleTimeOutValue, () );
        return ret;
    };
    //++wenching (TD13621)
};  


